#pragma once
#include "string.h"

#define DMsgWnd() MsgWnd::Param MsgWnd::p;

namespace CG
{
	class MsgWnd
	{
		struct Param
		{
			HWND wnd = 0;
			HANDLE thread = 0;
			std::wstring text;
		};
		static Param p;

		static DWORD _stdcall MsgWndThread(PVOID)
		{
			p.wnd = CreateWindowExW(0, L"EDIT", L"MessageWindow", WS_THICKFRAME | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE | ES_READONLY | ES_MULTILINE, 0, 0, 300, 500, 0, 0, GetModuleHandleW(0), 0);
			MSG msg; while (GetMessageW(&msg, p.wnd, 0, 0)) TranslateMessage(&msg), DispatchMessageW(&msg);
			return 0;
		}

	public:
		template <typename T>
		MsgWnd(T t = L"MessageWindow")
		{
			log(t);
		}

		template <typename T>
		static void msg(T text, size_t blank = 0)
		{
			if (!p.thread)
			{
				p.thread = CreateThread(0, 0, MsgWndThread, 0, 0, 0);
				while (!IsWindowVisible(p.wnd)) Sleep(16);
			}
			p.text = std::wstring(blank, L' ');
			p.text += String::toWString(text);
			p.text += L"\r\n";
			SetWindowTextW(p.wnd, p.text.c_str());
		}

		template <typename T>
		static void log(T text, size_t blank = 0)
		{
			if (!p.thread)
			{
				p.thread = CreateThread(0, 0, MsgWndThread, 0, 0, 0);
				while (!IsWindowVisible(p.wnd)) Sleep(16);
			}
			p.text += std::wstring(blank, L' ');
			p.text += String::toWString(text);
			p.text += L"\r\n";
			SetWindowTextW(p.wnd, p.text.c_str());
		}

		template <typename T>
		static void str(T text, size_t blank = 0)
		{
			if (!p.thread)
			{
				p.thread = CreateThread(0, 0, MsgWndThread, 0, 0, 0);
				while (!IsWindowVisible(p.wnd)) Sleep(16);
			}
			p.text += std::wstring(blank, L' ');
			p.text += String::toWString(text);
			SetWindowTextW(p.wnd, p.text.c_str());
		}

		static void emp()
		{
			if (!p.thread)
			{
				p.thread = CreateThread(0, 0, MsgWndThread, 0, 0, 0);
				while (!IsWindowVisible(p.wnd)) Sleep(16);
			}
			p.text = L"";
			SetWindowTextW(p.wnd, p.text.c_str());
		}
	};
}