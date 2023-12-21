#pragma once

#include <windows.h>

#ifndef sleep
#define sleep(ms) Sleep(ms)
#endif
#ifndef VK_WHEELUP
#define VK_WHEELUP 0x0A
#endif
#ifndef VK_WHEELDOWN
#define VK_WHEELDOWN 0x0B
#endif

namespace CG {
	class Input
	{
	public:

		static bool state(BYTE vk) { return GetAsyncKeyState(vk) & 0x8000; }

		static void Loop(BYTE vk, UINT delay = 10) { while (GetAsyncKeyState(vk)) sleep(delay); }

		// flags: 1 = down, 0 = up
		static void State(BYTE vk, bool state = 1, ULONG_PTR ex = 0) {
			INPUT input = { 0 };
			if (vk < 0x07 || vk == VK_WHEELUP || vk == VK_WHEELDOWN) {
				input.type = INPUT_MOUSE;

				if (vk == VK_LBUTTON)
				{
					if (state) input.mi = { 0, 0, 0, MOUSEEVENTF_LEFTDOWN, 0, ex }; else input.mi = { 0, 0, 0, MOUSEEVENTF_LEFTUP, 0, ex };
				}
				else if (vk == VK_RBUTTON)
				{
					if (state) input.mi = { 0, 0, 0, MOUSEEVENTF_RIGHTDOWN, 0, ex }; else input.mi = { 0, 0, 0, MOUSEEVENTF_RIGHTUP, 0, ex };
				}
				else if (vk == VK_MBUTTON)
				{
					if (state) input.mi = { 0, 0, 0, MOUSEEVENTF_MIDDLEDOWN, 0, ex }; else input.mi = { 0, 0, 0, MOUSEEVENTF_MIDDLEUP, 0, ex };
				}
				else if (vk == VK_XBUTTON1)
				{
					if (state) input.mi = { 0, 0, XBUTTON2, MOUSEEVENTF_XDOWN, 0, ex }; else input.mi = { 0, 0, XBUTTON2, MOUSEEVENTF_XUP, 0, ex };
				}
				else if (vk == VK_XBUTTON2)
				{
					if (state) input.mi = { 0, 0, XBUTTON2, MOUSEEVENTF_XDOWN, 0, ex }; else input.mi = { 0, 0, XBUTTON2, MOUSEEVENTF_XUP, 0, ex };
				}
				else if (vk == VK_WHEELUP)
				{
					input.mi = { 0, 0, (DWORD)(WHEEL_DELTA), MOUSEEVENTF_WHEEL, 0, ex };
				}
				else if (vk == VK_WHEELDOWN)
				{
					input.mi = { 0, 0, (DWORD)(-WHEEL_DELTA), MOUSEEVENTF_WHEEL, 0, ex };
				}
			}
			else {
				input.type = INPUT_KEYBOARD;
				if (state) input.ki = { vk, ScanCode(vk), 0, 0, ex }; else input.ki = { vk, ScanCode(vk), KEYEVENTF_KEYUP, 0, ex };
			}
			SendInput(1, &input, sizeof(input));
		}

		static void State(HWND wnd, BYTE vk, POINT pos = { 0 }, bool state = 1)
		{
			if (Type(vk))
			{
				if (vk == VK_LBUTTON)
				{
					if (state) SendMessageW(wnd, WM_LBUTTONDOWN, 0, (LONGLONG)(pos.y << 16 | pos.x));
					else SendMessageW(wnd, WM_LBUTTONUP, 0, (LONGLONG)(pos.y << 16 | pos.x));
				}
				else if (vk == VK_RBUTTON)
				{
					if (state) SendMessageW(wnd, WM_RBUTTONDOWN, 0, (LONGLONG)(pos.y << 16 | pos.x));
					else SendMessageW(wnd, WM_RBUTTONUP, 0, (LONGLONG)(pos.y << 16 | pos.x));
				}
				else if (vk == VK_MBUTTON)
				{
					if (state) SendMessageW(wnd, WM_MBUTTONDOWN, 0, (LONGLONG)(pos.y << 16 | pos.x));
					else SendMessageW(wnd, WM_MBUTTONUP, 0, (LONGLONG)(pos.y << 16 | pos.x));
				}
				else if (vk == VK_XBUTTON1)
				{
					if (state) SendMessageW(wnd, WM_XBUTTONDOWN, MK_XBUTTON1 << 16, (LONGLONG)(pos.y << 16 | pos.x));
					else SendMessageW(wnd, WM_XBUTTONUP, MK_XBUTTON1 << 16, (LONGLONG)(pos.y << 16 | pos.x));
				}
				else if (vk == VK_XBUTTON2)
				{
					if (state) SendMessageW(wnd, WM_XBUTTONDOWN, MK_XBUTTON2 << 16, (LONGLONG)(pos.y << 16 | pos.x));
					else SendMessageW(wnd, WM_XBUTTONUP, MK_XBUTTON2 << 16, (LONGLONG)(pos.y << 16 | pos.x));
				}
			}
			else
			{
				if (state)
				{
					SendMessageW(wnd, WM_KEYDOWN, vk, (LONGLONG)Input::ScanCode(vk) << 15ll);
				}
				else
				{
					SendMessageW(wnd, WM_KEYUP, vk, (LONGLONG)Input::ScanCode(vk) << 15ll);
				}
			}
		}

		static void Click(BYTE vk, UINT delay = 10, ULONG_PTR ex = 0) {
			State(vk, 1, ex); sleep(delay); State(vk, 0, ex); sleep(delay);
		}

		static void Click(HWND wnd, BYTE vk, POINT pos = { 0 }, UINT delay = 10) {
			State(wnd, vk, pos, 1); sleep(delay); State(wnd, vk, pos, 0); sleep(delay);
		}

		// param: x, y = Pixel
		static void Move(LONG x, LONG y) { INPUT input = { 0 }; MOUSEINPUT mouseInput = { x, y, 0, MOUSEEVENTF_MOVE, 0, 0 }; input.type = INPUT_MOUSE; input.mi = mouseInput; SendInput(1, &input, sizeof(input)); }

		// param: x, y = 0~ScreenPixel
		static void MoveTo(int x, int y) { SetCursorPos(x - 1, y - 1); }

		// param: x, y = 0~65535
		static void MoveToA(LONG x, LONG y) { INPUT input = { 0 }; MOUSEINPUT mouseInput = { x, y, 0, MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, 0, 0 }; input.type = INPUT_MOUSE; input.mi = mouseInput; SendInput(1, &input, sizeof(input)); }

		// param: x, y = 0~ScreenPixel
		static void MoveClick(BYTE vk, int x, int y, UINT delay = 10) { MoveTo(x, y); MoveLock(); Click(vk, delay); MoveLock(0); }

		// param: x, y = 0~65535
		static void MoveClickA(BYTE vk, LONG x, LONG y, UINT delay = 10) { MoveToA(x, y); MoveLock(); Click(vk, delay); MoveLock(0); }

		static void MoveLock(bool block = 1) { POINT point; GetCursorPos(&point); RECT rect = { point.x, point.y, point.x + 1, point.y + 1 }; if (block) ClipCursor(&rect); else ClipCursor(0); }

		// 0: Keyboard, 1: Mouse
		static bool Type(BYTE vk) { if ((vk >= VK_LBUTTON && vk <= VK_XBUTTON2) || (vk == VK_WHEELUP || vk == VK_WHEELDOWN)) return 1; return 0; }

		static BYTE ScanCode(BYTE vk) { return MapVirtualKeyW(vk, MAPVK_VK_TO_VSC); }

		static BYTE Convert(BYTE vk)
		{
			if (vk == VK_LCONTROL || vk == VK_RCONTROL) return VK_CONTROL;
			if (vk == VK_LSHIFT || vk == VK_RSHIFT) return VK_SHIFT;
			if (vk == VK_LMENU || vk == VK_RMENU) return VK_MENU;
			return vk;
		}

		static LPCWSTR Name(BYTE vk) {
			switch (vk) {
			case VK_LBUTTON: return L"左键";
			case VK_RBUTTON: return L"右键";
			case VK_CANCEL: return L"Cancel";
			case VK_MBUTTON: return L"中键";
			case VK_XBUTTON1: return L"X1键";
			case VK_XBUTTON2: return L"X2键";
			case VK_WHEELUP: return L"滚轮上";
			case VK_WHEELDOWN: return L"滚轮下";
			case VK_BACK: return L"Back";
			case VK_TAB: return L"Tab";
			case VK_CLEAR: return L"Clear";
			case VK_RETURN: return L"Return";
			case VK_SHIFT: return L"Shift";
			case VK_CONTROL: return L"Ctrl";
			case VK_MENU: return L"Alt";
			case VK_PAUSE: return L"Pause";
			case VK_CAPITAL: return L"CapsLock";
			case VK_KANA: return L"Kana";
			case VK_JUNJA: return L"Junja";
			case VK_FINAL: return L"Final";
			case VK_HANJA: return L"Hanja";
			case VK_ESCAPE: return L"Esc";
			case VK_CONVERT: return L"Convert";
			case VK_NONCONVERT: return L"NonConvert";
			case VK_ACCEPT: return L"Accept";
			case VK_MODECHANGE: return L"ModeChange";
			case VK_SPACE: return L"Space";
			case VK_PRIOR: return L"PageUp";
			case VK_NEXT: return L"PageDown";
			case VK_END: return L"End";
			case VK_HOME: return L"Home";
			case VK_LEFT: return L"Left";
			case VK_UP: return L"Up";
			case VK_RIGHT: return L"Right";
			case VK_DOWN: return L"Down";
			case VK_SELECT: return L"Select";
			case VK_PRINT: return L"Print";
			case VK_EXECUTE: return L"Execute";
			case VK_SNAPSHOT: return L"ScreenShot";
			case VK_INSERT: return L"Insert";
			case VK_DELETE: return L"Delete";
			case VK_HELP: return L"Help";
			case 0x30: return L"0";
			case 0x31: return L"1";
			case 0x32: return L"2";
			case 0x33: return L"3";
			case 0x34: return L"4";
			case 0x35: return L"5";
			case 0x36: return L"6";
			case 0x37: return L"7";
			case 0x38: return L"8";
			case 0x39: return L"9";
			case 0x41: return L"A";
			case 0x42: return L"B";
			case 0x43: return L"C";
			case 0x44: return L"D";
			case 0x45: return L"E";
			case 0x46: return L"F";
			case 0x47: return L"G";
			case 0x48: return L"H";
			case 0x49: return L"I";
			case 0x4A: return L"J";
			case 0x4B: return L"K";
			case 0x4C: return L"L";
			case 0x4D: return L"M";
			case 0x4E: return L"N";
			case 0x4F: return L"O";
			case 0x50: return L"P";
			case 0x51: return L"Q";
			case 0x52: return L"R";
			case 0x53: return L"S";
			case 0x54: return L"T";
			case 0x55: return L"U";
			case 0x56: return L"V";
			case 0x57: return L"W";
			case 0x58: return L"X";
			case 0x59: return L"Y";
			case 0x5A: return L"Z";
			case VK_LWIN: return L"LWin";
			case VK_RWIN: return L"RWin";
			case VK_APPS: return L"Apps";
			case VK_SLEEP: return L"Sleep";
			case VK_NUMPAD0: return L"Num0";
			case VK_NUMPAD1: return L"Num1";
			case VK_NUMPAD2: return L"Num2";
			case VK_NUMPAD3: return L"Num3";
			case VK_NUMPAD4: return L"Num4";
			case VK_NUMPAD5: return L"Num5";
			case VK_NUMPAD6: return L"Num6";
			case VK_NUMPAD7: return L"Num7";
			case VK_NUMPAD8: return L"Num8";
			case VK_NUMPAD9: return L"Num9";
			case VK_MULTIPLY: return L"Num*";
			case VK_ADD: return L"Num+";
			case VK_SEPARATOR: return L"NumEnter";
			case VK_SUBTRACT: return L"Num-";
			case VK_DECIMAL: return L"Num.";
			case VK_DIVIDE: return L"Num/";
			case VK_F1: return L"F1";
			case VK_F2: return L"F2";
			case VK_F3: return L"F3";
			case VK_F4: return L"F4";
			case VK_F5: return L"F5";
			case VK_F6: return L"F6";
			case VK_F7: return L"F7";
			case VK_F8: return L"F8";
			case VK_F9: return L"F9";
			case VK_F10: return L"F10";
			case VK_F11: return L"F11";
			case VK_F12: return L"F12";
			case VK_F13: return L"F13";
			case VK_F14: return L"F14";
			case VK_F15: return L"F15";
			case VK_F16: return L"F16";
			case VK_F17: return L"F17";
			case VK_F18: return L"F18";
			case VK_F19: return L"F19";
			case VK_F20: return L"F20";
			case VK_F21: return L"F21";
			case VK_F22: return L"F22";
			case VK_F23: return L"F23";
			case VK_F24: return L"F24";
			case VK_NUMLOCK: return L"NumLock";
			case VK_SCROLL: return L"ScrollLock";
			case VK_LSHIFT: return L"LShift";
			case VK_RSHIFT: return L"RShift";
			case VK_LCONTROL: return L"LCtrl";
			case VK_RCONTROL: return L"RCtrl";
			case VK_LMENU: return L"LAlt";
			case VK_RMENU: return L"RAlt";
			case VK_BROWSER_BACK: return L"BrowserBack";
			case VK_BROWSER_FORWARD: return L"BrowserForward";
			case VK_BROWSER_REFRESH: return L"BrowserRefresh";
			case VK_BROWSER_STOP: return L"BrowserStop";
			case VK_BROWSER_SEARCH: return L"BrowserSearch";
			case VK_BROWSER_FAVORITES: return L"BrowserFavorites";
			case VK_BROWSER_HOME: return L"BrowserHome";
			case VK_VOLUME_MUTE: return L"VolumeMute";
			case VK_VOLUME_DOWN: return L"VolumeDown";
			case VK_VOLUME_UP: return L"VolumeUp";
			case VK_MEDIA_NEXT_TRACK: return L"MediaNext";
			case VK_MEDIA_PREV_TRACK: return L"MediaPrev";
			case VK_MEDIA_STOP: return L"MediaStop";
			case VK_MEDIA_PLAY_PAUSE: return L"MediaPlay";
			case VK_LAUNCH_MAIL: return L"Mail";
			case VK_LAUNCH_MEDIA_SELECT: return L"MediaSelect";
			case VK_LAUNCH_APP1: return L"RunApp1";
			case VK_LAUNCH_APP2: return L"RunApp2";
			case VK_OEM_1: return L";:";
			case VK_OEM_PLUS: return L"=+";
			case VK_OEM_COMMA: return L",<";
			case VK_OEM_MINUS: return L"-_";
			case VK_OEM_PERIOD: return L".>";
			case VK_OEM_2: return L"/?";
			case VK_OEM_3: return L"`~";
			case VK_OEM_4: return L"[{";
			case VK_OEM_5: return L"\\|";
			case VK_OEM_6: return L"]}";
			case VK_OEM_7: return L"\'\"";
			case VK_OEM_8: return L"Oem8";
			case VK_OEM_102: return L"Oem102";
			case VK_PROCESSKEY: return L"Process";
			case VK_PACKET: return L"Packet";
			case VK_ATTN: return L"Attn";
			case VK_CRSEL: return L"CrSel";
			case VK_EXSEL: return L"ExSel";
			case VK_EREOF: return L"ErEof";
			case VK_PLAY: return L"Play";
			case VK_ZOOM: return L"Zoom";
			case VK_NONAME: return L"NoName";
			case VK_PA1: return L"Pa1";
			case VK_OEM_CLEAR: return L"OemClear";
			}
			return L"";
		}
	};
}