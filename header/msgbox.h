#pragma once

#include <windows.h>
#include "string.h"

namespace CG {
	class MsgBox
	{
	public:
		MsgBox() { MessageBoxW(0, L"MessageBox", L"Message", MB_OK); };
		template<typename T>
		MsgBox(T text = L"MsgBox", LPCWSTR title = L"Message", LONG style = MB_OK) { MessageBoxW(0, String::toWString(text).c_str(), title, style); }

		template<typename T>
		static int Message(T text, LPCWSTR title = L"Message", LONG style = MB_OK) { return MessageBoxW(0, String::toWString(text).c_str(), title, style); }
		static int Warning(LPCWSTR text, LPCWSTR title = L"Warning", LONG style = MB_OK | MB_ICONWARNING | MB_TOPMOST) { return MessageBoxW(0, text, title, style); }
		static int Error(LPCWSTR text, LPCWSTR title = L"Error", LONG style = MB_OK | MB_ICONERROR | MB_TOPMOST) { return MessageBoxW(0, text, title, style); }

		static void Rect(RECT rect) {
			std::wstring str =
				std::to_wstring(rect.left) + (std::wstring)L", " +
				std::to_wstring(rect.top) + (std::wstring)L", " +
				std::to_wstring(rect.right) + (std::wstring)L", " +
				std::to_wstring(rect.bottom);
			MessageBoxW(0, str.c_str(), L"Rect", MB_TOPMOST);
		}
		static void FRect(FRECT frect) {
			std::wstring str =
				std::to_wstring(frect.left) + (std::wstring)L", " +
				std::to_wstring(frect.top) + (std::wstring)L", " +
				std::to_wstring(frect.right) + (std::wstring)L", " +
				std::to_wstring(frect.bottom);
			MessageBoxW(0, str.c_str(), L"FRect", MB_TOPMOST);
		}
		static void Point(POINT point) {
			std::wstring str = std::to_wstring(point.x) + (std::wstring)L", " + std::to_wstring(point.y);
			MessageBoxW(0, str.c_str(), L"Point", MB_TOPMOST);
		}
		static void FPoint(FPOINT fpoint) {
			std::wstring str = std::to_wstring(fpoint.x) + (std::wstring)L", " + std::to_wstring(fpoint.y);
			MessageBoxW(0, str.c_str(), L"FPoint", MB_TOPMOST);
		}
	};
}