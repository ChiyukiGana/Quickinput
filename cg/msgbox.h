#pragma once

#include <windows.h>
#include "string.h"

namespace CG {
	class MsgBox
	{
	public:
		MsgBox() { MessageBoxW(0, L"MessageBox", L"Message", MB_OK); };
		template<typename T>
		MsgBox(T text = L"MessageBox", LPCWSTR title = L"Message", LONG style = MB_OK) { MessageBoxW(0, String::toWString(text).c_str(), title, style); }

		template<typename T>
		static int Message(T text = L"MessageBox", std::wstring title = L"Message", LONG style = MB_OK) { return MessageBoxW(0, String::toWString(text).c_str(), title.c_str(), style); }
		static int Warning(std::wstring text, std::wstring title = L"Warning", LONG style = MB_OK | MB_ICONWARNING | MB_TOPMOST) { return MessageBoxW(0, text.c_str(), title.c_str(), style); }
		static int Error(std::wstring text, std::wstring title = L"Error", LONG style = MB_OK | MB_ICONERROR | MB_TOPMOST) { return MessageBoxW(0, text.c_str(), title.c_str(), style); }
		static void Size(const SIZE& size) { MessageBoxW(0, (std::to_wstring(size.cx) + std::wstring(L", ") + std::to_wstring(size.cy)).c_str(), L"Size", MB_TOPMOST); }
		static void Size(const int& cx, const int& cy) { MessageBoxW(0, (std::to_wstring(cx) + std::wstring(L", ") + std::to_wstring(cy)).c_str(), L"Size", MB_TOPMOST); }
		static void Point(const POINT& point) { MessageBoxW(0, (std::to_wstring(point.x) + std::wstring(L", ") + std::to_wstring(point.y)).c_str(), L"Point", MB_TOPMOST); }
		static void Point(const int& x, const int& y) { MessageBoxW(0, (std::to_wstring(x) + std::wstring(L", ") + std::to_wstring(y)).c_str(), L"Point", MB_TOPMOST); }
		static void Rect(const RECT& rect) { MessageBoxW(0, (std::to_wstring(rect.left) + std::wstring(L", ") + std::to_wstring(rect.top) + std::wstring(L", ") + std::to_wstring(rect.right) + std::wstring(L", ") + std::to_wstring(rect.bottom)).c_str(), L"Rect", MB_TOPMOST); }
		static void Color(const COLORREF& color) { MessageBoxW(0, (std::to_wstring(GetRValue(color)) + std::wstring(L", ") + std::to_wstring(GetGValue(color)) + std::wstring(L", ") + std::to_wstring(GetBValue(color)) + std::wstring(L", ") + std::to_wstring(GetAValue(color))).c_str(), L"Color", MB_TOPMOST); }
	};
}