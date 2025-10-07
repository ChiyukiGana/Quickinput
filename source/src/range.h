#pragma once
#include <qstring.h>
#include <algorithm>
using QiIntRange = std::pair<int, int>;
using QiLongRange = std::pair<long long, long long>;
using QiFloatRange = std::pair<float, float>;
namespace QiRange
{
	inline int Restricted(int value, QiIntRange range)
	{
		return std::clamp(value, range.first, range.second);
	}
	inline long long Restricted(long long value, QiLongRange range)
	{
		return std::clamp(value, range.first, range.second);
	}
	inline float Restricted(float value, QiFloatRange range)
	{
		return std::clamp(value, range.first, range.second);
	}
	inline int Restricted(int value, int max, int min = 0)
	{
		return std::clamp(value, max, min);
	}
	inline long long Restricted(long long value, long long max, long long min = 0ll)
	{
		return std::clamp(value, max, min);
	}
	inline float Restricted(float value, float max, float min = 0.0f)
	{
		return std::clamp(value, max, min);
	}
}