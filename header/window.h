#pragma once

#include <windows.h>
#include <dwmapi.h> //DwmGetWindowAttribute
#pragma comment(lib, "dwmapi.lib") //DwmGetWindowAttribute

#include "base.h"

#define WndMain() int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE, _In_ LPWSTR cmdLine, _In_ INT cmdShow)
#define PeekMsg(wnd) MSG msg; while(1) { if (PeekMessageW(&msg, wnd, 0, 0, PM_REMOVE)) TranslateMessage(&msg), DispatchMessageW(&msg); else
#define PeekMsgEnd() };

#define GetMod(wParam) (LOWORD(wParam))
#define GetVk(wParam) (HIWORD(wParam))

namespace CG {
	class Window
	{
	public:

		static POINT clientAbsPos(HWND wnd) {
			POINT client = { 0 };
			RECT window = { 0 };
			ClientToScreen(wnd, &client);
			GetWindowRect(wnd, &window);
			return { client.x - window.left - 8, client.y - window.top };
		}

		static LRESULT CALLBACK DefWndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) { return DefWindowProcW(wnd, msg, wp, lp); }

		static void Register(LPCWSTR className, WNDPROC wndProc, bool transparent = 0, COLORREF color = RGB(255, 255, 255), HICON icon = 0, INT menu = 0) {
			WNDCLASSW wndClass = { 0 };
			wndClass.lpszClassName = className;
			wndClass.lpfnWndProc = wndProc;
			wndClass.hIcon = icon;
			wndClass.lpszMenuName = MAKEINTRESOURCEW(menu);
			wndClass.style = CS_HREDRAW | CS_VREDRAW;
			if (!transparent) wndClass.hbrBackground = CreateSolidBrush(color);
			RegisterClassW(&wndClass);
		}

		static HWND Create(
			LPCWSTR wndName, LPCWSTR className, DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0,
			int width = CW_USEDEFAULT, int height = CW_USEDEFAULT, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
			HWND parent = 0, HMENU menu = 0, HINSTANCE instance = GetModuleHandleW(0)
		) {
			return CreateWindowExW(exStyle, className, wndName, style, x, y, width, height, parent, menu, instance, 0);
		}


		//size: pixel, weight: 0 ~ 900, font: name
		static HFONT setFont(UINT size, USHORT weight = 0, LPCWSTR font = L"Microsoft YaHei") {
			return CreateFontW(size, 0, 0, 0, weight, 0, 0, 0, UNICODE, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, font);
		}

		static bool Redraw(HWND wnd) { return RedrawWindow(wnd, 0, 0, RDW_ERASE | RDW_INVALIDATE); }

		static bool Size(HWND wnd, int cx, int cy) { return SetWindowPos(wnd, 0, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE); }

		static bool Pos(HWND wnd, int x, int y) { return SetWindowPos(wnd, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE); }

		static RECT rect(HWND wnd) { RECT rect = { 0 }; DwmGetWindowAttribute(wnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT)); return rect; }
		static bool Rect(HWND wnd, RECT rect) { return SetWindowPos(wnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER); }

		static bool UnTop(HWND wnd) { return SetWindowPos(wnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE); }
		static bool Top(HWND wnd) { return SetWindowPos(wnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE); }
		static bool TopMost(HWND wnd) { if (SetWindowPos(wnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE)) return SetWindowPos(wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE); return 0; }

		static long style(HWND wnd) { return GetWindowLongW(wnd, GWL_STYLE); }
		static void Style(HWND wnd, long style, bool remove = 0) { if (remove) SetWindowLongW(wnd, GWL_STYLE, GetWindowLongW(wnd, GWL_STYLE) & ~(style)); else SetWindowLongW(wnd, GWL_STYLE, GetWindowLongW(wnd, GWL_STYLE) | (style)); }

		static long exStyle(HWND wnd) { return GetWindowLongW(wnd, GWL_EXSTYLE); }
		static void ExStyle(HWND wnd, long exStyle, bool remove = 0) { if (remove) SetWindowLongW(wnd, GWL_EXSTYLE, GetWindowLongW(wnd, GWL_EXSTYLE) & ~(exStyle)); else SetWindowLongW(wnd, GWL_EXSTYLE, GetWindowLongW(wnd, GWL_EXSTYLE) | (exStyle)); }

	public:
		typedef List<HWND> WNDS;
	private:
		struct _EnumWindowsStruct { WNDS wnds; DWORD pid; };

		static BOOL _stdcall _EnumWindowsProc(HWND wnd, LPARAM lParam) {
			_EnumWindowsStruct* ews = (_EnumWindowsStruct*)lParam;
			DWORD pid; GetWindowThreadProcessId(wnd, &pid);
			if (pid == ews->pid) ews->wnds.Add(wnd);
			return 1;
		}
	public:

		static WNDS ProcessWindows(DWORD pid) {
			_EnumWindowsStruct ews;
			ews.pid = pid;
			EnumWindows(_EnumWindowsProc, (LPARAM)&ews);
			return ews.wnds;
		}

		static WNDS ProcessWindows(HANDLE proc) { return ProcessWindows(GetProcessId(proc)); }

		static LPCWSTR StyleName(long style) {
			switch (style) {
			case WS_OVERLAPPED: return L"WS_OVERLAPPED";
			case WS_POPUP: return L"WS_POPUP";
			case WS_CHILD: return L"WS_CHILD";
			case WS_MINIMIZE: return L"WS_MINIMIZE";
			case WS_VISIBLE: return L"WS_VISIBLE";
			case WS_DISABLED: return L"WS_DISABLED";
			case WS_CLIPSIBLINGS: return L"WS_CLIPSIBLINGS";
			case WS_CLIPCHILDREN: return L"WS_CLIPCHILDREN";
			case WS_MAXIMIZE: return L"WS_MAXIMIZE";
			case WS_BORDER: return L"WS_BORDER";
			case WS_DLGFRAME: return L"WS_DLGFRAME";
			case WS_VSCROLL: return L"WS_VSCROLL";
			case WS_HSCROLL: return L"WS_HSCROLL";
			case WS_SYSMENU: return L"WS_SYSMENU";
			case WS_THICKFRAME: return L"WS_THICKFRAME";
			case WS_GROUP: return L"WS_GROUP";
			case WS_TABSTOP: return L"WS_TABSTOP";
			}
			return 0;
		}

		static LPCWSTR ExStyleName(long exStyle) {
			switch (exStyle) {
			case WS_EX_DLGMODALFRAME: return L"WS_EX_DLGMODALFRAME";
			case WS_EX_NOPARENTNOTIFY: return L"WS_EX_NOPARENTNOTIFY";
			case WS_EX_TOPMOST: return L"WS_EX_TOPMOST";
			case WS_EX_ACCEPTFILES: return L"WS_EX_ACCEPTFILES";
			case WS_EX_TRANSPARENT: return L"WS_EX_TRANSPARENT";
			case WS_EX_MDICHILD: return L"WS_EX_MDICHILD";
			case WS_EX_TOOLWINDOW: return L"WS_EX_TOOLWINDOW";
			case WS_EX_WINDOWEDGE: return L"WS_EX_WINDOWEDGE";
			case WS_EX_CLIENTEDGE: return L"WS_EX_CLIENTEDGE";
			case WS_EX_CONTEXTHELP: return L"WS_EX_CONTEXTHELP";
			case WS_EX_RIGHT: return L"WS_EX_RIGHT";
			case WS_EX_LEFT: return L"WS_EX_LEFT";
			case WS_EX_RTLREADING: return L"WS_EX_RTLREADING";
			case WS_EX_LEFTSCROLLBAR: return L"WS_EX_LEFTSCROLLBAR";
			case WS_EX_CONTROLPARENT: return L"WS_EX_CONTROLPARENT";
			case WS_EX_STATICEDGE: return L"WS_EX_STATICEDGE";
			case WS_EX_LAYERED: return L"WS_EX_LAYERED";
			case WS_EX_NOINHERITLAYOUT: return L"WS_EX_NOINHERITLAYOUT";
			case WS_EX_NOREDIRECTIONBITMAP: return L"WS_EX_NOREDIRECTIONBITMAP";
			case WS_EX_LAYOUTRTL: return L"WS_EX_LAYOUTRTL";
			case WS_EX_COMPOSITED: return L"WS_EX_COMPOSITED";
			case WS_EX_NOACTIVATE: return L"WS_EX_NOACTIVATE";
			}
			return 0;
		}
	};
}
