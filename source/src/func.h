#pragma once
#include "inc_header.h"
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
	void WindowPop(QString window, bool state);
	void QuickClickPop(bool state);
	void MacroPop(Macro* macro, bool state);
	// State
	void Trigger(short vk, const bool* state);
	void QiHook(bool state);
	void QiState(bool state);

	bool SelfActive();
	void SmoothMove(const int sx, const int sy, const int dx, const int dy, const int speed, std::function<void(int x, int y, int stepx, int stepy)> CallBack);
	WndInfo WindowSelection();
	QiBlock* FindBlock(Actions& actions, int id);
	const QiBlock* FindBlock(const Actions& actions, int id);
};