#pragma once
#include "inc_header.h"

class QiInterpreter
{
	QiVarMap& varMap;
	const Actions& actions;
	const float speed;
	WndInput* wndInput;
	POINT& cursor;

	std::mutex debug_mutex;
	std::condition_variable debug;
	int jumpId = 0;
	bool debug_entry = false;
public:
	QiInterpreter(QiVarMap& varMap, const Actions& actions, float speed, WndInput* wndInput, POINT& cursor);
	void DebugContinue();
	bool PeekSleep(clock_t ms);
	int ActionInterpreter(const Actions& current);
};