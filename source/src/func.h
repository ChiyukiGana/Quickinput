#pragma once
#include "minc.h"

namespace QiFn
{
	//////////////////////////// Pos convert
	POINT RTA(POINT rel);
	RECT RTAR(RECT rel);
	POINT ATR(POINT abs);
	RECT ATRR(RECT abs);
	POINT WRTA(POINT rel, HWND wnd);
	RECT WRTAR(RECT rel, HWND wnd);
	POINT WATR(POINT abs, HWND wnd);
	RECT WATRR(RECT abs, HWND wnd);
	//////////////////////////// #Pos convert


	bool TransMove(int sx, int sy, int dx, int dy, int step, POINT& pt);
	WndInfo WindowSelection();

	// return 0: next, return 1: stop all, return 2: stop loop;
	uint8 ActionExecute(const Action& action, POINT& cursor, WndInput* wi);

	std::wstring ParseQPop(std::wstring text);
	std::wstring ParseWPop(std::wstring text, std::wstring window = L"(窗口名)");
	std::wstring ParseMacroPop(std::wstring text, std::wstring macro = L"(示例宏)", size_t count = 123);
	void QPop(QiUi::PopBoxBase pop);
	void WPop(std::wstring text, std::wstring window);
	void QcPop(QiUi::PopBoxBase pop);
	void MacroPop(Macro* macro, QiUi::PopBoxBase pop);

	bool SelfActive();

	std::wstring NameFilter(std::wstring name);

	void QiHook(bool state);
	void QiState(bool state);

	void Trigger(byte vk);
};