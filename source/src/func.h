#pragma once
#include "inc_header.h"
namespace QiFn
{
	// Pos convert
	int I_RTA(int rel, int size);
	int I_ATR(int abs, int size);
	float IF_RTA(int rel, int size);
	int IF_ATR(float abs, int size);

	POINT P_ATA(const POINTF& abs);
	POINTF P_ATA(const POINT& abs);

	POINT P_SRTA(const POINT& rel);
	POINT P_SATR(const POINT& abs);
	POINTF PF_SRTA(const POINT& abs);
	POINT PF_SATR(const POINTF& abs);
	RECT R_SRTA(const RECT& rel);
	RECT R_SATR(const RECT& abs);
	RECTF RF_SRTA(const RECT& rel);
	RECT RF_SATR(const RECTF& abs);

	POINT P_WRTA(const POINT& rel, const HWND& wnd);
	POINT P_WATR(const POINT& abs, const HWND& wnd);
	POINTF PF_WATR(const POINT& rel, const HWND& wnd);
	POINT PF_WATR(const POINTF& abs, const HWND& wnd);
	RECT R_WRTA(const RECT& rel, const HWND& wnd);
	RECT R_WATR(const RECT& abs, const HWND& wnd);
	RECTF RF_WRTA(const RECT& rel, const HWND& wnd);
	RECT RF_WATR(const RECTF& abs, const HWND& wnd);

	void Key(char key, bool press);
	void Move(int x, int y); // pixel
	void MoveTo(int x, int y); // screen size
	void MoveToA(int x, int y); // 0~10000

	QString FoldText(QString str, int len = 20, bool back = false);

	// Pop text
	QString ParseCustom(QString text, QString name, QString number);
	QString ParseState(QString text);
	QString ParseWindow(QString text, QString window);
	QString ParseQuickClick(QString text, short key);
	QString ParseMacro(QString text, QString macro, int count);
	void StatePop(bool state);
	void WindowPop(const QString& window, bool state);
	void QuickClickPop(bool state);
	void MacroPop(Macro* macro, bool state);
	void SoundPlay(const QString& sound, bool sync);
	// State
	void Trigger(short vk, const bool* state);
	void QiHook(bool state);
	void QiState(bool state);
	void UnBlock();

	void InitOcr(bool warning = true);
#ifdef Q_RAWINPUT
	void InitRawInput(bool warning = true);
#endif

	void SmoothMove(const int sx, const int sy, const int dx, const int dy, const int speed, std::function<void(int x, int y, int stepx, int stepy)> CallBack);
	WndInfo WindowSelection();

	Macro* FindMacro(const QString& name);
};