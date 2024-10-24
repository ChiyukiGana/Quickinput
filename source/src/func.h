﻿#pragma execution_character_set("utf-8")
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

	// Pop text
	QString ParseCustom(QString text, QString name, QString number);
	QString ParseState(QString text);
	QString ParseWindow(QString text, QString window);
	QString ParseQuickClick(QString text, short key);
	QString ParseMacro(QString text, QString macro, int count);
	void StatePop(bool state);
	void WindowPop(std::wstring window, bool state);
	void QuickClickPop(bool state);
	void MacroPop(Macro* macro, bool state);
	// ~Pop text

	bool SelfActive();
	bool TransMove(int sx, int sy, int dx, int dy, int step, POINT& pt);
	WndInfo WindowSelection();
	std::wstring AllocName(std::wstring name);

	int ActionExecute(const Action& action, POINT& cursor, WndInput* wi);
	void Trigger(short vk, const bool* state);

	void QiHook(bool state);
	void QiState(bool state);
};