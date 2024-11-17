#pragma once
#include "type.h"

class MsgPop
{
	static inline bool init = false;
	static inline HWND wnd = 0;
	static inline HFONT font = Window::Font(28, 900);
	static inline UINT time = 0;
	static inline UINT begin = 0;
	static inline COLORREF color = 0;
	static inline std::wstring text;
	static LRESULT _stdcall MsgPopProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		if (msg == WM_TIMER) // reduce transparency
		{
			UINT now = clock() - MsgPop::begin;
			if (now < time)
			{
				UINT alpha = (UINT)(0.255f * (float)(MsgPop::time - now));
				if (alpha > 255) alpha = 255;
				SetLayeredWindowAttributes(wnd, 0, alpha, LWA_ALPHA);
			}
			else
			{
				KillTimer(wnd, 0);
				SetLayeredWindowAttributes(wnd, 0, 0, LWA_ALPHA);
			}
		}
		else if (msg == WM_PAINT)
		{
			SIZE size = System::screenVSize();
			RECT rect = { size.cx >> 1, 0, size.cx >> 1, 0 };
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(wnd, &ps);
			SelectObject(hdc, MsgPop::font);
			GetTextExtentExPointW(hdc, MsgPop::text.c_str(), MsgPop::text.length(), 0, 0, 0, &size);
			size.cx += 24, size.cy += 16;
			rect = { rect.left - (size.cx >> 1), 0, rect.right + (size.cx >> 1), size.cy };
			Window::Rect(wnd, rect);
			rect = { 0, 0, size.cx, size.cy };
			FillRect(hdc, &rect, CreateSolidBrush(MsgPop::color));
			SetBkMode(hdc, TRANSPARENT);
			DrawTextW(hdc, MsgPop::text.c_str(), MsgPop::text.length(), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			EndPaint(wnd, &ps);
		}
		return DefWindowProcW(wnd, msg, wp, lp);
	}
	static DWORD _stdcall MsgPopThread(LPVOID)
	{
		Window::Register(L"MsgPopClass", MsgPopProc, 1);
		MsgPop::wnd = Window::Create(L"MsgPop", L"MsgPopClass", WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOPMOST, 1, 1, 0, 0);
		SetLayeredWindowAttributes(MsgPop::wnd, 0, 0, LWA_ALPHA);
		MSG msg; while (GetMessageW(&msg, MsgPop::wnd, 0, 0)) DispatchMessageW(&msg);
		return 0;
	}
public:
	static void Init()
	{
		if (!MsgPop::init)
		{
			MsgPop::init = true;
			Thread::Start(MsgPopThread);
		}
	}
	static void Popup(std::wstring text, COLORREF color = RGB(0xCC, 0xEE, 0xFF), UINT time = 1000)
	{
		KillTimer(MsgPop::wnd, 0);
		MsgPop::begin = clock();
		MsgPop::time = time;
		MsgPop::text = text;
		MsgPop::color = color;
		Window::Redraw(MsgPop::wnd);
		SetTimer(MsgPop::wnd, 0, 20, 0);
	}
	static void Show(std::wstring text, COLORREF color = RGB(0xCC, 0xEE, 0xFF))
	{
		KillTimer(MsgPop::wnd, 0);
		MsgPop::text = text;
		MsgPop::color = color;
		Window::Redraw(MsgPop::wnd);
		SetLayeredWindowAttributes(MsgPop::wnd, 0, 255, LWA_ALPHA);
	}
	static void Hide()
	{
		KillTimer(MsgPop::wnd, 0);
		SetLayeredWindowAttributes(MsgPop::wnd, 0, 0, LWA_ALPHA);
	}
};