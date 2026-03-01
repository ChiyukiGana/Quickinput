#pragma once
#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>
#include <sstream>

namespace QiTools
{
	class QiTime
	{
	public:
		static time_t toTimeStamp(time_t h, time_t m, time_t s)
		{
			if (h > 23) h = 23;
			if (m > 59) m = 59;
			if (s > 59) s = 59;
			return h * 3600 + m * 60 + s;
		}
		static time_t h(time_t timestamp) { return timestamp / 3600; }
		static time_t m(time_t timestamp) { return (timestamp % 3600) / 60; }
		static time_t s(time_t timestamp) { return timestamp % 60; }
		static std::string toString(time_t timestamp)
		{
			std::ostringstream oss;
			oss << std::setw(2) << std::setfill('0') << h(timestamp) << ":"
				<< std::setw(2) << std::setfill('0') << m(timestamp) << ":"
				<< std::setw(2) << std::setfill('0') << s(timestamp);
			return oss.str();
		}
		static time_t current()
		{
			std::time_t now = std::time(nullptr);
			std::tm* localTime = std::localtime(&now);
			return localTime->tm_hour * 3600 + localTime->tm_min * 60 + localTime->tm_sec;
		}
		static time_t compare(time_t timestamp)
		{
			time_t currentTimestamp = current();
			return timestamp - currentTimestamp;
		}
		static int compare(time_t start, time_t end)
		{
			time_t currentTimestamp = current();
			if (currentTimestamp < start) return -1;
			if (currentTimestamp > end) return 1;
			return 0;
		}
		static bool in(time_t start, time_t end)
		{
			time_t currentTimestamp = current();
			return currentTimestamp >= start && currentTimestamp <= end;
		}
	};
}