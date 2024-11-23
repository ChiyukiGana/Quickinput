#pragma once
#include <windows.h>
#define VK_WHEELUP 0x0A
#define VK_WHEELDOWN 0x0B
#define DEFINE_INPUTHOOK()\
HANDLE InputHook::thread = 0;\
HHOOK InputHook::mouseHook = 0;\
HHOOK InputHook::keybdHook = 0;\
bool InputHook::createFlag = false;\
bool InputHook::mouseState = false;\
bool InputHook::keybdState = false;\
bool InputHook::blockRep = false;\
bool InputHook::keys[255] = {};
namespace QiTools
{
class InputHook
{
	static HANDLE thread;
	static HHOOK mouseHook;
	static HHOOK keybdHook;
	static bool createFlag;
	static bool mouseState;
	static bool keybdState;
	static bool blockRep;
	static bool keys[0xFF];
	// return true: block
	static bool _stdcall InputProc(BYTE key, bool press, POINT corsor, PULONG_PTR param);
	static LRESULT _stdcall MouseHook(int code, WPARAM msg, LPARAM param)
	{
		PMSLLHOOKSTRUCT ms = (PMSLLHOOKSTRUCT)param;
		if (mouseState)
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
		return CallNextHookEx(mouseHook, code, msg, param);
	}
	static LRESULT _stdcall KeybdHook(int code, WPARAM msg, LPARAM param)
	{
		PKBDLLHOOKSTRUCT kb = (PKBDLLHOOKSTRUCT)param;
		if (keybdState)
		{
			if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)
			{
				if (blockRep && keys[kb->vkCode]);
				else
				{
					if (InputProc((BYTE)kb->vkCode, true, {}, &kb->dwExtraInfo)) return 1;
					if (blockRep) keys[kb->vkCode] = true;
				}
			}
			else if (msg == WM_KEYUP || msg == WM_SYSKEYUP)
			{
				if (InputProc((BYTE)kb->vkCode, false, {}, &kb->dwExtraInfo)) return 1;
				if (blockRep) keys[kb->vkCode] = false;
			}
		}
		return CallNextHookEx(keybdHook, code, msg, param);
	}
	static DWORD _stdcall HookThread(void*)
	{
		mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHook, GetModuleHandleW(0), 0);
		keybdHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeybdHook, GetModuleHandleW(0), 0);
		createFlag = true;
		MSG msg; while (GetMessageW(&msg, 0, WM_KEYFIRST, WM_MOUSELAST)) DispatchMessageW(&msg);
		return 0;
	}

public:
	typedef unsigned __int8 Type;

	enum Types
	{
		none = (unsigned __int8)0,
		mouse = (unsigned __int8)1,
		keybd = (unsigned __int8)2,
		all = mouse | keybd
	};

	static void BlockRepeat(bool blockRepeat = true)
	{
		blockRep = blockRepeat;
	}

	static bool IsRunning() { return thread; }

	static bool State() { if (thread && (mouseState || keybdState)) return true; return false; }

	static bool Start(Type flags = Types::all, bool blockRepeat = false)
	{
		if (!thread)
		{
			thread = CreateThread(0, 0, HookThread, 0, 0, 0);
			if (thread)
			{
				while (!createFlag) Sleep(0);
				if (mouseHook && keybdHook);
				else return false;
			}
			else return false;
		}

		BlockRepeat(blockRepeat);
		memset(keys, 0, sizeof(keys));
		mouseState = (flags & mouse);
		keybdState = (flags & keybd);
		return true;
	}

	static void Stop(Type flags = Types::all)
	{
		mouseState = !(flags & mouse);
		keybdState = !(flags & keybd);
		memset(keys, 0, sizeof(keys));
	}

	static void Close()
	{
		if (thread)
		{
			TerminateThread(thread, 0);
			{
				thread = 0;
				mouseHook = 0;
				keybdHook = 0;
				createFlag = false;
				mouseState = false;
				keybdState = false;
				blockRep = false;
				memset(keys, 0, sizeof(keys));
			}
		}
	}

	static void Wait() { if (!thread) return; WaitForSingleObject(thread, INFINITE); }
};
}