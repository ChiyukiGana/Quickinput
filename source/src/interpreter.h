#pragma execution_character_set("utf-8")
#pragma once
#include "minc.h"

namespace QiInterpreter
{
	int ActionInterpreter(const Actions& parent, const Actions& current, POINT& cursor, WndInput* wp, int& jumpId);
}