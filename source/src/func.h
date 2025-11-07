#pragma once
#include "inc_header.h"
namespace QiFn
{
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