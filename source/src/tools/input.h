#pragma once
#include <windows.h>
#ifdef Q_KEYEDIT_PAD_ENABLED
#include "xboxpad.h"
#endif
#ifndef VK_WHEELUP
#define VK_WHEELUP 0x0A
#endif
#ifndef VK_WHEELDOWN
#define VK_WHEELDOWN 0x0B
#endif
namespace QiTools {
	class Input
	{
	public:
		static BYTE ScanCode(BYTE vk) { return MapVirtualKeyW(vk, MAPVK_VK_TO_VSC); }
		static BYTE Convert(BYTE vk)
		{
			if (vk == VK_LCONTROL || vk == VK_RCONTROL) return VK_CONTROL;
			if (vk == VK_LSHIFT || vk == VK_RSHIFT) return VK_SHIFT;
			if (vk == VK_LMENU || vk == VK_RMENU) return VK_MENU;
			return vk;
		}
		static bool isMouse(int keyCode) { return ((keyCode >= VK_LBUTTON && keyCode <= VK_XBUTTON2) || keyCode == VK_WHEELUP || keyCode == VK_WHEELDOWN); }
		static bool isKeyboard(int keyCode) { return ((keyCode >= VK_CLEAR && keyCode <= VK_OEM_CLEAR) || keyCode == VK_BACK || keyCode == VK_TAB); }
		static bool isPress(BYTE vk) { return GetAsyncKeyState(vk) & 0x8000; }
		static POINT pos() { POINT pt; GetCursorPos(&pt); return pt; }
		static void LoopDown(BYTE vk, UINT delay = 10) { while (isPress(vk)) Sleep(delay); }
		static void LoopUp(BYTE vk, UINT delay = 10) { while (!isPress(vk)) Sleep(delay); }
		static void State(BYTE vk, bool press = true, ULONG_PTR info = 0) {
			if (vk)
			{
				INPUT input = {};
				if (vk < 0x07 || vk == VK_WHEELUP || vk == VK_WHEELDOWN)
				{
					input.type = INPUT_MOUSE;
					if (vk == VK_LBUTTON) input.mi = press ? MOUSEINPUT({ 0, 0, 0, MOUSEEVENTF_LEFTDOWN, 0, info }) : MOUSEINPUT({ 0, 0, 0, MOUSEEVENTF_LEFTUP, 0, info });
					else if (vk == VK_RBUTTON) input.mi = press ? MOUSEINPUT({ 0, 0, 0, MOUSEEVENTF_RIGHTDOWN, 0, info }) : input.mi = MOUSEINPUT({ 0, 0, 0, MOUSEEVENTF_RIGHTUP, 0, info });
					else if (vk == VK_MBUTTON) input.mi = press ? MOUSEINPUT({ 0, 0, 0, MOUSEEVENTF_MIDDLEDOWN, 0, info }) : input.mi = MOUSEINPUT({ 0, 0, 0, MOUSEEVENTF_MIDDLEUP, 0, info });
					else if (vk == VK_XBUTTON1) input.mi = press ? MOUSEINPUT({ 0, 0, XBUTTON1, MOUSEEVENTF_XDOWN, 0, info }) : input.mi = MOUSEINPUT({ 0, 0, XBUTTON1, MOUSEEVENTF_XUP, 0, info });
					else if (vk == VK_XBUTTON2) input.mi = press ? MOUSEINPUT({ 0, 0, XBUTTON2, MOUSEEVENTF_XDOWN, 0, info }) : input.mi = MOUSEINPUT({ 0, 0, XBUTTON2, MOUSEEVENTF_XUP, 0, info });
					else if (vk == VK_WHEELUP) input.mi = { 0, 0, (DWORD)(WHEEL_DELTA), MOUSEEVENTF_WHEEL, 0, info };
					else if (vk == VK_WHEELDOWN) input.mi = { 0, 0, (DWORD)(-WHEEL_DELTA), MOUSEEVENTF_WHEEL, 0, info };
				}
				else
				{
					input.type = INPUT_KEYBOARD;
					input.ki = press ? KEYBDINPUT({ static_cast<WORD>(vk), static_cast<WORD>(ScanCode(vk)), 0, 0, info }) : KEYBDINPUT({ static_cast<WORD>(vk), static_cast<WORD>(MapVirtualKeyW(vk, MAPVK_VK_TO_VSC)), KEYEVENTF_KEYUP, 0, info });
				}
				SendInput(1, &input, sizeof(input));
			}
		}
		static void State(HWND wnd, BYTE vk, POINT pos = { 0 }, bool press = true)
		{
			if (isMouse(vk))
			{
				if (vk == VK_LBUTTON) PostMessageW(wnd, press ? WM_LBUTTONDOWN : WM_LBUTTONUP, 0, (LPARAM)pos.y << (LPARAM)16 | (LPARAM)pos.x);
				else if (vk == VK_RBUTTON) PostMessageW(wnd, press ? WM_RBUTTONDOWN : WM_RBUTTONUP, 0, ((LPARAM)pos.y) << ((LPARAM)16) | ((LPARAM)pos.x));
				else if (vk == VK_MBUTTON) PostMessageW(wnd, press ? WM_MBUTTONDOWN : WM_MBUTTONUP, 0, ((LPARAM)pos.y) << ((LPARAM)16) | ((LPARAM)pos.x));
				else if (vk == VK_XBUTTON1) PostMessageW(wnd, press ? WM_XBUTTONDOWN : WM_XBUTTONUP, MK_XBUTTON1 << 16, ((LPARAM)pos.y) << ((LPARAM)16) | ((LPARAM)pos.x));
				else if (vk == VK_XBUTTON2) PostMessageW(wnd, press ? WM_XBUTTONDOWN : WM_XBUTTONUP, MK_XBUTTON2 << 16, ((LPARAM)pos.y) << ((LPARAM)16) | ((LPARAM)pos.x));
			}
			else
			{
				PostMessageW(wnd, press ? WM_KEYDOWN : WM_KEYUP, vk, static_cast<LPARAM>(ScanCode(vk)) << static_cast<LPARAM>(16));
			}
		}
		static void Down(BYTE vk, ULONG_PTR info = 0) { State(vk, true, info); }
		static void Up(BYTE vk, ULONG_PTR info = 0) { State(vk, false, info); }
		static void Click(BYTE vk, UINT delay = 10, ULONG_PTR info = 0) { State(vk, 1, info); Sleep(delay); State(vk, 0, info); Sleep(delay); }
		static void Click(HWND wnd, BYTE vk, POINT pos = { 0 }, UINT delay = 10) { State(wnd, vk, pos, 1); Sleep(delay); State(wnd, vk, pos, 0); Sleep(delay); }
		static void String(const std::wstring& str)
		{
			if (str.empty()) return;
			std::vector<INPUT> inputs;
			inputs.reserve(str.size() * 2);
			for (wchar_t c : str)
			{
				INPUT i;
				i.type = INPUT_KEYBOARD;
				i.ki.wVk = 0;
				i.ki.wScan = c;
				i.ki.dwFlags = KEYEVENTF_UNICODE;
				inputs.push_back(i);
				i.ki.dwFlags |= KEYEVENTF_KEYUP;
				inputs.push_back(i);
			}
			SendInput(inputs.size(), inputs.data(), sizeof(INPUT));
		}
		static void String(HWND wnd, const std::wstring& str)
		{
			if (str.empty()) return;
			for (wchar_t c : str)
			{
				PostMessageW(wnd, WM_KEYDOWN, c, 0);
				PostMessageW(wnd, WM_KEYUP, c, 0);
			}
		}
		// param: x, y = Pixel
		static void Move(LONG x, LONG y, ULONG_PTR info = 0) { INPUT input = { 0 }; input.type = INPUT_MOUSE; input.mi = { x, y, 0, MOUSEEVENTF_MOVE, 0, info }; SendInput(1, &input, sizeof(input)); }
		// param: x, y = 0~ScreenPixel-1
		static void MoveTo(int x, int y, ULONG_PTR info = 0)
		{
			HMONITOR hmt = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);
			MONITORINFOEXW mti = {}; mti.cbSize = sizeof(MONITORINFOEXW); GetMonitorInfoW(hmt, &mti);
			DEVMODEW dm = { sizeof(DEVMODEW) }; EnumDisplaySettingsW(mti.szDevice, ENUM_CURRENT_SETTINGS, &dm);
			dm.dmPelsWidth -= 1, dm.dmPelsHeight -= 1; MoveToA((65536.0f / (float)dm.dmPelsWidth) * (float)x, 65536.0f / (float)dm.dmPelsHeight * (float)y, info);
		}
		// param: x, y = WindowPixel
		static void MoveTo(HWND wnd, int x, int y, WORD mk = 0) { PostMessageW(wnd, WM_MOUSEMOVE, mk, (LONGLONG)(y << 16 | x)); }
		// param: x, y = 0~65535
		static void MoveToA(LONG x, LONG y, ULONG_PTR info = 0) { INPUT input = { 0 }; input.type = INPUT_MOUSE; input.mi = { x, y, 0, MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, 0, info }; SendInput(1, &input, sizeof(input)); }
		// param: x, y = 0~ScreenPixel
		static void MoveClick(BYTE vk, int x, int y, UINT delay = 10) { MoveTo(x, y); MoveLock(); Click(vk, delay); MoveLock(0); }
		// param: x, y = 0~65535
		static void MoveClickA(BYTE vk, LONG x, LONG y, UINT delay = 10) { MoveToA(x, y); MoveLock(); Click(vk, delay); MoveLock(0); }
		static void MoveLock(bool block = 1) { POINT point; GetCursorPos(&point); RECT rect = { point.x, point.y, point.x + 1, point.y + 1 }; if (block) ClipCursor(&rect); else ClipCursor(0); }
		// support xboxpad
		static bool isPad(int keyCode)
		{
#ifdef Q_KEYEDIT_PAD_ENABLED
			return ((keyCode >= XBoxPad::key_begin) && (keyCode <= XBoxPad::key_end));
#endif
			return false;
		}
		static bool isPressEx(SHORT vk)
		{
#ifdef Q_KEYEDIT_PAD_ENABLED
			if (isPad(vk)) return XBoxPad::isPress(vk);
			else
#endif
			if (vk < 256) return isPress(vk);
			return false;
		}
	};
}