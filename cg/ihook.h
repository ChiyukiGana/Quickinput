#pragma once

#include <windows.h>

#define VK_WHEELUP 0x0A
#define VK_WHEELDOWN 0x0B
#define InputHookProc() InputHook::Param InputHook::p; bool InputHook::InputProc(BYTE vk, bool state, POINT msPt, PULONG_PTR exInfo)

class InputHook
{
public:
	typedef unsigned __int8 HookType;

	enum HookTypes
	{
		none = (unsigned __int8)0,
		mouse = (unsigned __int8)1,
		keybd = (unsigned __int8)2,
		all = mouse | keybd
	};

	static void BlockRep(bool blockRep = true)
	{
		p.blockRep = blockRep;
	}

	static bool Start(HookType flags = HookTypes::all, bool blockRep = false)
	{
		if (!p.thread)
		{
			p.thread = CreateThread(0, 0, HookThread, 0, 0, 0);
			if (p.thread)
			{
				while (!p.createFlag) Sleep(0);
				if (p.mouse && p.keybd);
				else return false;
			}
			else return false;
		}

		BlockRep(blockRep);
		memset(p.keys, 0, sizeof(p.keys));
		p.mouseState = (flags & mouse);
		p.keybdState = (flags & keybd);
		return true;
	}

	static void Stop(HookType flags = HookTypes::all)
	{
		p.mouseState = !(flags & mouse);
		p.keybdState = !(flags & keybd);
		memset(p.keys, 0, sizeof(p.keys));
	}

	static void Close()
	{
		if (p.thread)
		{
			TerminateThread(p.thread, 0);
			p = {};
		}
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
		bool createFlag = false;
		bool mouseState = false;
		bool keybdState = false;
		bool blockRep = false;
		bool keys[255] = {};
	};

	static Param p;

private:

	//return false: call next hook, return true: block
	static bool CALLBACK InputProc(BYTE vk, bool state, POINT msPt, PULONG_PTR exInfo);

	static LRESULT CALLBACK MouseHook(int code, WPARAM msg, LPARAM param)
	{
		PMSLLHOOKSTRUCT ms = (PMSLLHOOKSTRUCT)param;
		if (p.mouseState)
		{
			switch (msg)
			{
			case WM_MOUSEMOVE:
				if (InputProc(0, false, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_LBUTTONDOWN:
				if (InputProc(VK_LBUTTON, true, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_LBUTTONUP:
				if (InputProc(VK_LBUTTON, false, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_RBUTTONDOWN:
				if (InputProc(VK_RBUTTON, true, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_RBUTTONUP:
				if (InputProc(VK_RBUTTON, false, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_MBUTTONDOWN:
				if (InputProc(VK_MBUTTON, true, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_MBUTTONUP:
				if (InputProc(VK_MBUTTON, false, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_XBUTTONDOWN:
				if ((ms->mouseData >> 16) == XBUTTON1)
				{
					if (InputProc(VK_XBUTTON1, true, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				else if ((ms->mouseData >> 16) == XBUTTON2)
				{
					if (InputProc(VK_XBUTTON2, true, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				break;
			case WM_XBUTTONUP:
				if ((ms->mouseData >> 16) == XBUTTON1)
				{
					if (InputProc(VK_XBUTTON1, false, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				else if ((ms->mouseData >> 16) == XBUTTON2)
				{
					if (InputProc(VK_XBUTTON2, false, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				break;
			case WM_MOUSEWHEEL:
				if ((short)(ms->mouseData >> 16) >= (short)0x78)
				{
					if (InputProc(VK_WHEELUP, true, ms->pt, &ms->dwExtraInfo)) return 1;
					if (InputProc(VK_WHEELUP, false, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				else if ((short)(ms->mouseData >> 16) <= (short)(0xFF88))
				{
					if (InputProc(VK_WHEELDOWN, true, ms->pt, &ms->dwExtraInfo)) return 1;
					if (InputProc(VK_WHEELDOWN, false, ms->pt, &ms->dwExtraInfo)) return 1;
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
			if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)
			{
				if (p.blockRep && p.keys[kb->vkCode]);
				else
				{
					if (InputProc((BYTE)kb->vkCode, true, {}, &kb->dwExtraInfo)) return 1;
					if (p.blockRep) p.keys[kb->vkCode] = true;
				}
			}
			else if (msg == WM_KEYUP || msg == WM_SYSKEYUP)
			{
				if (InputProc((BYTE)kb->vkCode, false, {}, &kb->dwExtraInfo)) return 1;
				if (p.blockRep) p.keys[kb->vkCode] = false;
			}
		}
		return CallNextHookEx(p.keybd, code, msg, param);
	}

	static DWORD CALLBACK HookThread(PVOID)
	{
		p.mouse = SetWindowsHookExW(WH_MOUSE_LL, MouseHook, GetModuleHandleW(0), 0);
		p.keybd = SetWindowsHookExW(WH_KEYBOARD_LL, KeybdHook, GetModuleHandleW(0), 0);
		p.createFlag = true;
		MSG msg; while (GetMessageW(&msg, 0, WM_KEYFIRST, WM_MOUSELAST)) DispatchMessageW(&msg);
		return 0;
	}
};