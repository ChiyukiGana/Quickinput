#pragma once
#include "inc_header.h"
namespace QiFn
{
	// Pos convert
	POINT P_ATA(const QPointF& abs);
	QPointF P_ATA(const POINT& abs);

	POINT P_SRTA(const POINT& rel);
	POINT P_SATR(const POINT& abs);
	QPointF PF_SRTA(const POINT& abs);
	POINT PF_SATR(const QPointF& abs);
	RECT R_SRTA(const RECT& rel);
	RECT R_SATR(const RECT& abs);
	QRectF RF_SRTA(const RECT& rel);
	RECT RF_SATR(const QRectF& abs);

	POINT P_WRTA(const POINT& rel, const HWND& wnd);
	POINT P_WATR(const POINT& abs, const HWND& wnd);
	QPointF PF_WATR(const POINT& rel, const HWND& wnd);
	POINT PF_WATR(const QPointF& abs, const HWND& wnd);
	RECT R_WRTA(const RECT& rel, const HWND& wnd);
	RECT R_WATR(const RECT& abs, const HWND& wnd);
	QRectF RF_WRTA(const RECT& rel, const HWND& wnd);
	RECT RF_WATR(const QRectF& abs, const HWND& wnd);
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

	void SmoothMove(const int sx, const int sy, const int dx, const int dy, const int speed, std::function<void(int x, int y, int stepx, int stepy)> CallBack);
	WndInfo WindowSelection();
	QiBlock* FindBlock(Actions& actions, int id);
	const QiBlock* FindBlock(const Actions& actions, int id);
};