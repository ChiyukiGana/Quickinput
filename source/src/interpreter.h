#pragma once
#include "inc_header.h"

class QiInterpreter
{
	QiWorker& worker;
	Macro& macro;
	POINT& cursor;
	const Actions& actions;
	const bool timer;
	const time_t timerStart;
	const time_t timerEnd;
	const float speed;
	const float moveScaleX;
	const float moveScaleY;
	const float posScaleX;
	const float posScaleY;
	WndInput* wndInput;

	QiVector<QString> path;
	std::mutex debug_mutex;
	std::condition_variable debug_condition;
	int jumpId = 0;
	bool debug_entry = false;
public:
	QiInterpreter(Macro& macro, bool isRunning, QiWorker& worker);
	int rand(int max, int min = 0);
	void setLastPos(int x, int y);
	bool isInvalid();
	void DebugContinue();
	bool PeekSleep(clock_t ms);
	int ActionInterpreter(const Actions& current);
	QString makePath();
};