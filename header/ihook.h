#pragma once

#include <windows.h>

#define VK_WHEELUP 0x0A
#define VK_WHEELDOWN 0x0B
#define InputHookProc() InputHook::Param InputHook::p; bool InputHook::InputProc(BYTE vk, bool state, POINT msPt, PULONG_PTR exInfo)

class InputHook
{
public:
	enum
	{
		none,
		mouse,
		keybd,
		all = mouse | keybd
	};

	static void Stop(int flags)
	{
		if (flags & mouse)
		{
			p.mouseState = 0;
		}
		if (flags & keybd)
		{
			p.keybdState = 0;
		}
		memset(p.keys, 0, sizeof(p.keys));
	}

	static bool Start(int flags, bool blockRep = 0)
	{
		if (!p.thread) p.thread = CreateThread(0, 0, HookThread, 0, 0, 0);
		if (!p.thread) return 0;
		while (!p.createFlag) sleep(1);
		if (!p.mouse) return 0;
		if (!p.keybd) return 0;

		if (flags & mouse)
		{
			p.mouseState = 1;
		}
		if (flags & keybd)
		{
			p.keybdState = 1;
		}
		memset(p.keys, 0, sizeof(p.keys));
		BlockRep(blockRep);
		return 1;
	}

	static void BlockRep(bool blockRep = 1)
	{
		p.blockRep = blockRep;
	}

	static void Wait()
	{
		if (!p.thread) return;
		WaitForSingleObject(p.thread, INFINITE);
	}

	struct Param
	{
		HANDLE thread = 0;
		HHOOK mouse = 0;
		HHOOK keybd = 0;
		bool createFlag = 0;
		bool mouseState = 0;
		bool keybdState = 0;
		bool blockRep = 0;
		bool keys[255] = { 0 };
	};

	static Param p;

private:

	//return 0: call next hook, return 1: block
	static bool CALLBACK InputProc(BYTE vk, bool state, POINT msPt, PULONG_PTR exInfo);

	static LRESULT CALLBACK MouseHook(int code, WPARAM msg, LPARAM param)
	{
		PMSLLHOOKSTRUCT ms = (PMSLLHOOKSTRUCT)param;
		if (p.mouseState)
		{
			switch (msg)
			{
			case WM_MOUSEMOVE:
				if (InputProc(0, 0, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_LBUTTONDOWN:
				if (InputProc(VK_LBUTTON, 1, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_LBUTTONUP:
				if (InputProc(VK_LBUTTON, 0, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_RBUTTONDOWN:
				if (InputProc(VK_RBUTTON, 1, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_RBUTTONUP:
				if (InputProc(VK_RBUTTON, 0, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_MBUTTONDOWN:
				if (InputProc(VK_MBUTTON, 1, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_MBUTTONUP:
				if (InputProc(VK_MBUTTON, 0, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_XBUTTONDOWN:
				if (HIWORD(ms->mouseData) == XBUTTON1)
				{
					if (InputProc(VK_XBUTTON1, 1, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				else if (HIWORD(ms->mouseData) == XBUTTON2)
				{
					if (InputProc(VK_XBUTTON2, 1, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				break;
			case WM_XBUTTONUP:
				if (HIWORD(ms->mouseData) == XBUTTON1)
				{
					if (InputProc(VK_XBUTTON1, 0, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				else if (HIWORD(ms->mouseData) == XBUTTON2)
				{
					if (InputProc(VK_XBUTTON2, 0, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				break;
			case WM_MOUSEWHEEL:
				if ((short)(HIWORD(ms->mouseData)) >= (short)WHEEL_DELTA)
				{
					if (InputProc(VK_WHEELUP, 1, ms->pt, &ms->dwExtraInfo)) return 1;
					if (InputProc(VK_WHEELUP, 0, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				else if ((short)(HIWORD(ms->mouseData)) <= (short)(-WHEEL_DELTA))
				{
					if (InputProc(VK_WHEELDOWN, 1, ms->pt, &ms->dwExtraInfo)) return 1;
					if (InputProc(VK_WHEELDOWN, 0, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				break;
			}
		}
		return CallNextHookEx(p.mouse, code, msg, param);
	}

	static LRESULT CALLBACK KeybdHook(int code, WPARAM msg, LPARAM param)
	{
		PKBDLLHOOKSTRUCT kb = (PKBDLLHOOKSTRUCT)param;
		if (p.keybdState)
		{
			switch (msg)
			{
			case WM_KEYDOWN:
				if (p.keys[kb->vkCode]) break;
				if (InputProc((BYTE)kb->vkCode, 1, { 0 }, &kb->dwExtraInfo)) return 1;
				p.keys[kb->vkCode] = 1;
				break;
			case WM_KEYUP:
				if (InputProc((BYTE)kb->vkCode, 0, { 0 }, &kb->dwExtraInfo)) return 1;
				p.keys[kb->vkCode] = 0;
				break;
			case WM_SYSKEYDOWN:
				if (p.keys[kb->vkCode]) break;
				if (InputProc((BYTE)kb->vkCode, 1, { 0 }, &kb->dwExtraInfo)) return 1;
				p.keys[kb->vkCode] = 1;
				break;
			case WM_SYSKEYUP:
				if (InputProc((BYTE)kb->vkCode, 0, { 0 }, &kb->dwExtraInfo)) return 1;
				p.keys[kb->vkCode] = 0;
				break;
			}
		}
		return CallNextHookEx(p.keybd, code, msg, param);
	}

	static DWORD CALLBACK HookThread(LPVOID)
	{
		MSG msg;
		p.mouse = SetWindowsHookExW(WH_MOUSE_LL, MouseHook, GetModuleHandleW(0), 0);
		p.keybd = SetWindowsHookExW(WH_KEYBOARD_LL, KeybdHook, GetModuleHandleW(0), 0);
		p.createFlag = 1;
		while (GetMessageW(&msg, 0, WM_KEYFIRST, WM_MOUSELAST))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		return 0;
	}
};