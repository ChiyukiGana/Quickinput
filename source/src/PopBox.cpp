#include "PopBox.h"

namespace PopBox
{
	bool init = false;
	HWND wnd = 0;
	HFONT font = Window::Font(28, 900);
	UINT time = 0;
	UINT begin = 0;
	std::wstring text;
	COLORREF color = 0;

	LRESULT _stdcall PopBoxProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		if (msg == WM_TIMER) // reduce transparency
		{
			UINT now = clock() - PopBox::begin;
			if (now < time)
			{
				UINT alpha = (UINT)(0.255f * (float)(PopBox::time - now));
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
			SIZE screen = System::screenVSize();
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(wnd, &ps); DeleteObject(SelectObject(hdc, PopBox::font));
			SIZE size; GetTextExtentExPointW(hdc, PopBox::text.c_str(), PopBox::text.length(), 0, 0, 0, &size); size.cx += 24, size.cy += 16;

			POINT pt = QiFn::ATR(qis.ui.pop.p);
			if (pt.x < (size.cx >> 1)) pt.x = size.cx >> 1; else if (pt.x > (screen.cx - (size.cx >> 1))) pt.x = (screen.cx - (size.cx >> 1));
			if (pt.y < (size.cy >> 1)) pt.y = size.cy >> 1; else if (pt.y > (screen.cy - (size.cy >> 1))) pt.y = (screen.cy - (size.cy >> 1));
			RECT rect = { pt.x - (size.cx >> 1), pt.y - (size.cy >> 1), pt.x + (size.cx >> 1), pt.y + (size.cy >> 1) };
			Window::Rect(wnd, rect);

			GetClientRect(wnd, &rect);
			FillRect(hdc, &rect, CreateSolidBrush(PopBox::color));
			SetBkMode(hdc, TRANSPARENT);
			DrawTextW(hdc, PopBox::text.c_str(), PopBox::text.length(), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			EndPaint(wnd, &ps);
		}
		return DefWindowProcW(wnd, msg, wp, lp);
	}
	DWORD _stdcall PopBoxThread(LPVOID)
	{
		Window::Register(L"MsgPopClass", PopBoxProc, 1);
		PopBox::wnd = Window::Create(L"MsgPop", L"MsgPopClass", WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOPMOST, 1, 1, 0, 0);
		SetLayeredWindowAttributes(PopBox::wnd, 0, 0, LWA_ALPHA);
		MSG msg; while (GetMessageW(&msg, PopBox::wnd, 0, 0)) DispatchMessageW(&msg);
		return 0;
	}

	void Init()
	{
		if (!PopBox::init)
		{
			PopBox::init = true;
			Thread::Start(PopBoxThread);
		}
	}
	void Popup(std::wstring text, COLORREF color, UINT time)
	{
		KillTimer(PopBox::wnd, 0);
		PopBox::begin = clock();
		PopBox::time = time;
		PopBox::text = text;
		PopBox::color = color;
		Window::Redraw(PopBox::wnd);
		SetTimer(PopBox::wnd, 0, 20, 0);
	}
	void Show(std::wstring text, COLORREF color)
	{
		KillTimer(PopBox::wnd, 0);
		PopBox::text = text;
		PopBox::color = color;
		Window::Redraw(PopBox::wnd);
		SetLayeredWindowAttributes(PopBox::wnd, 0, 255, LWA_ALPHA);
	}
	void Hide()
	{
		KillTimer(PopBox::wnd, 0);
		SetLayeredWindowAttributes(PopBox::wnd, 0, 0, LWA_ALPHA);
	}
};