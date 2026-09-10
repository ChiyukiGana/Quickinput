#pragma once
#include "inc_header.h"

class QiInterpreter
{
	QiWorker& worker;
	Macro& macro;
	POINT& cursor;
	Actions* actions;
	const bool force_stop;
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
	double rand(double max, double min = 0);
	void setActions(Actions& actions);
	void setValue(const std::string& var, const QiVar& val);
	void setCount(int i);
	void setIndex(int i);
	void setLastPos(int x, int y);
	bool isTimrout();
	bool isInvalid(bool check_timer = true);
	void DebugContinue();
	bool Sleep(double ms);
	InterpreterResult ActionInterpreter(Actions& current);
	QString makePath();
	std::string errPath();
	std::wstring werrPath();
};