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
		static unsigned int toTimeStamp(unsigned int h, unsigned int m, unsigned int s)
		{
			if (h > 23) h = 23;
			if (m > 59) m = 59;
			if (s > 59) s = 59;
			return h * 3600 + m * 60 + s;
		}
		static unsigned int h(unsigned int timestamp) { return timestamp / 3600; }
		static unsigned int m(unsigned int timestamp) { return (timestamp % 3600) / 60; }
		static unsigned int s(unsigned int timestamp) { return timestamp % 60; }
		static std::string toString(unsigned int timestamp)
		{
			std::ostringstream oss;
			oss << std::setw(2) << std::setfill('0') << h(timestamp) << ":"
				<< std::setw(2) << std::setfill('0') << m(timestamp) << ":"
				<< std::setw(2) << std::setfill('0') << s(timestamp);
			return oss.str();
		}
		static unsigned int current()
		{
			std::time_t now = std::time(nullptr);
			std::tm* localTime = std::localtime(&now);
			return localTime->tm_hour * 3600 + localTime->tm_min * 60 + localTime->tm_sec;
		}
		static int compare(unsigned int timestamp)
		{
			int currentTimestamp = current();
			return timestamp - currentTimestamp;
		}
	};
}