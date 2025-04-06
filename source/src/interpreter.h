#pragma once
#include "inc_header.h"

class QiInterpreter
{
	QiVarMap& varMap;
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

	std::mutex debug_mutex;
	std::condition_variable debug_condition;
	int jumpId = 0;
	int layer = 0;
	bool debug_entry = false;
public:
	QiInterpreter(Macro& macro, bool isRunning);
	bool isInvalid();
	void DebugContinue();
	bool PeekSleep(clock_t ms);
	int ActionInterpreter(const Actions& current);
};