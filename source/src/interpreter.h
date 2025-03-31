#pragma once
#include "inc_header.h"

class QiInterpreter
{
	QiVarMap& varMap;
	POINT& cursor;
	const Actions& actions;
	const float speed;
	const float moveScaleX;
	const float moveScaleY;
	const float posScaleX;
	const float posScaleY;
	WndInput* wndInput;

	std::mutex debug_mutex;
	std::condition_variable debug;
	int jumpId = 0;
	bool debug_entry = false;
public:
	QiInterpreter(Macro& macro, bool isRunning);
	void DebugContinue();
	bool PeekSleep(clock_t ms);
	int ActionInterpreter(const Actions& current);
};