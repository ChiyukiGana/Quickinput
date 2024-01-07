#pragma once

#include "string.h"
#include "thread.h"
#include "window.h"

#define ConsoleClose ProcessClose(L"VsDebugConsole.exe");

namespace CG {

	////DMsgBox
	struct _DMsgBoxStruct {
		std::wstring text;
		std::wstring title;
		LONG style;
		bool state;
	};

	static DWORD CALLBACK _DMsgBoxThread(LPVOID lParam) {
		_DMsgBoxStruct dms = { ((_DMsgBoxStruct*)lParam)->text, ((_DMsgBoxStruct*)lParam)->title, ((_DMsgBoxStruct*)lParam)->style, 0 };
		((_DMsgBoxStruct*)lParam)->state = 1;
		MessageBoxW(0, dms.text.c_str(), dms.title.c_str(), dms.style);
		return 0;
	}

	template <typename T>
	static void DMsgBox(T text, std::wstring title = String::toWString(clock()), LONG style = MB_OK) {
		_DMsgBoxStruct dms;
		dms.text = String::toWString(text);
		dms.title = title;
		dms.style = style;
		dms.state = 0;
		Thread::Start(_DMsgBoxThread, &dms);
		while (!dms.state);
	}
	//DMsgBox
}