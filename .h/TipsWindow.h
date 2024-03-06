#pragma once
#include "QuickInputDef.h"

struct TipsWindow
{
	struct Param
	{
		HWND wnd = 0;
		HFONT font = Window::setFont(28, 900);
		UINT time = 0, begin = 0;
		COLORREF color = 0;
		std::wstring text;
	};
	static Param p;

	static LRESULT CALLBACK TipsWndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		if (msg == WM_TIMER) // reduce transparency
		{
			UINT now = clock() - p.begin;
			if (now < p.time)
			{
				UINT alpha = (UINT)(0.255f * (float)(p.time - now));
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
			SIZE size = System::screenSize();
			RECT rect = { size.cx >> 1, 0, size.cx >> 1, 0 };
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(wnd, &ps);
			SelectObject(hdc, p.font);
			GetTextExtentExPointW(hdc, p.text.c_str(), p.text.length(), 0, 0, 0, &size);
			size.cx += 24, size.cy += 16;
			rect = { rect.left - (size.cx >> 1), 0, rect.right + (size.cx >> 1), size.cy };
			Window::Rect(wnd, rect);
			rect = { 0, 0, size.cx, size.cy };
			FillRect(hdc, &rect, CreateSolidBrush(p.color));
			SetBkMode(hdc, TRANSPARENT);
			TextOutW(hdc, 12, 8, p.text.c_str(), p.text.length());
			EndPaint(wnd, &ps);
		}
		return DefWindowProcW(wnd, msg, wp, lp);
	}

	static DWORD CALLBACK TipsWindowThread(LPVOID)
	{
		Window::Register(L"TipsWnd", TipsWndProc, 1);
		p.wnd = Window::Create(L"TipsWnd", L"TipsWnd", WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOPMOST, 1, 1, 0, 0);
		SetLayeredWindowAttributes(p.wnd, 0, 0, LWA_ALPHA);
		MSG msg; while (GetMessageW(&msg, p.wnd, 0, 0)) DispatchMessageW(&msg);
		return 0;
	}

	static void Popup(std::wstring text, COLORREF color = RGB(0xCC, 0xEE, 0xFF), UINT time = 1000)
	{
		KillTimer(p.wnd, 0);
		p.begin = clock();
		p.time = time;
		p.text = text;
		p.color = color;
		Window::Redraw(p.wnd);
		SetTimer(p.wnd, 0, 20, 0);
	}
	static void Show(std::wstring text, COLORREF color = RGB(0xCC, 0xEE, 0xFF))
	{
		KillTimer(p.wnd, 0);
		p.text = text;
		p.color = color;
		Window::Redraw(p.wnd);
		SetLayeredWindowAttributes(p.wnd, 0, 255, LWA_ALPHA);
	}
	static void Hide()
	{
		KillTimer(p.wnd, 0);
		SetLayeredWindowAttributes(p.wnd, 0, 0, LWA_ALPHA);
	}
};