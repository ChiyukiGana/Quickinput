#pragma execution_character_set("utf-8")
#pragma once
#include "minc.h"

namespace QiInterpreter
{
	int ActionInterpreter(const Actions& actions, POINT& cursor, WndInput* wi, int& jumpId, bool parent);
}