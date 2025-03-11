#pragma once
#include "inc_header.h"

class QiInterpreter
{
	QiVarMap& varMap;
	const Actions& actions;
	const float speed;
	WndInput* wndInput;
	POINT& cursor;
public:
	QiInterpreter(QiVarMap& varMap, const Actions& actions, float speed, WndInput* wndInput, POINT& cursor) : varMap(varMap), actions(actions), speed(speed), wndInput(wndInput), cursor(cursor) {}
	bool PeekSleep(clock_t ms);
	int ActionInterpreter(const Actions& current, int& jumpId);
};