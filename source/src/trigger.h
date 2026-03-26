#pragma once
#include "inc_header.h"
namespace QiTr
{
	void Trigger(short vk, const bool* state);
	void QiHook(bool state);
	void QiState(bool state);
	void UnBlock();
};