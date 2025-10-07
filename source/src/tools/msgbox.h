#pragma once
#include <windows.h>
#include "string.h"
namespace QiTools
{
	class MsgBox
	{
	public:
		template<typename Ty>
		static int Message(Ty text = L"MessageBox", std::wstring title = L"Message", LONG style = MB_OK | MB_TOPMOST)
		{
			return MessageBoxW(0, String::toWString(text).c_str(), title.c_str(), style);
		}
		static int Warning(std::wstring text, std::wstring title = L"Warning", LONG style = MB_OK | MB_ICONWARNING | MB_TOPMOST)
		{
			return MessageBoxW(0, text.c_str(), title.c_str(), style);
		}
		static int Error(std::wstring text, std::wstring title = L"Error", LONG style = MB_OK | MB_ICONERROR | MB_TOPMOST)
		{
			return MessageBoxW(0, text.c_str(), title.c_str(), style);
		}
	};
}