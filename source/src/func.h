#pragma execution_character_set("utf-8")
#pragma once
#include "minc.h"

namespace QiFn
{
	// Pos convert
	POINT RTA(POINT rel);
	RECT RTAR(RECT rel);
	POINT ATR(POINT abs);
	RECT ATRR(RECT abs);
	POINT WRTA(POINT rel, HWND wnd);
	RECT WRTAR(RECT rel, HWND wnd);
	POINT WATR(POINT abs, HWND wnd);
	RECT WATRR(RECT abs, HWND wnd);

	std::wstring ParseQPop(std::wstring text);
	std::wstring ParseWPop(std::wstring text, std::wstring window = L"(窗口名)");
	std::wstring ParseMacroPop(std::wstring text, std::wstring macro = L"(示例宏)", size_t count = 123);
	void QPop(QiUi::PopBoxBase pop);
	void WPop(std::wstring window, QiUi::PopBoxBase pop);
	void QcPop(QiUi::PopBoxBase pop);
	void MacroPop(Macro* macro, QiUi::PopBoxBase pop);

	bool SelfActive();
	std::wstring NameFilter(std::wstring name);
	bool TransMove(int sx, int sy, int dx, int dy, int step, POINT& pt);
	WndInfo WindowSelection();

	uint8 ActionExecute(const Action& action, POINT& cursor, WndInput* wi);
	void Trigger(short vk);

	void QiHook(bool state);
	void QiState(bool state);
};