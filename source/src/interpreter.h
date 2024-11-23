#pragma once
#include "inc_header.h"
namespace QiInterpreter
{
	int ActionInterpreter(const Actions& parent, const Actions& current, POINT& cursor, WndInput* wp, int& jumpId);
}