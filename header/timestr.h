#pragma once
#include "string.h"
using namespace CG;
class TimeString
{
	Time::DATETIME ts = Time::toDATETIME();
	std::wstring ParseArgv(std::wstring argv)
	{
		std::wstring timeString;
		if (argv.length())
		{
			if (argv[0] == L'/' && argv.length() > 1)
			{
				switch (argv[1])
				{
				case 'Y': timeString += String::toWString(ts.year); break;
				case 'M': timeString += String::toWString(ts.mon); break;
				case 'D': timeString += String::toWString(ts.day); break;
				case 'h': timeString += String::toWString(ts.hour); break;
				case 'm': timeString += String::toWString(ts.min); break;
				case 's': timeString += String::toWString(ts.sec); break;
				}
			}
			else timeString += argv;
		}
		return timeString;
	}
public:
	TimeString() {}
	std::wstring Parse(std::wstring src)
	{
		std::wstring timeString;
		uint32 p = 0;
		for (uint32 u = 0; u < src.length(); u++)
		{
			if (src[u] == L'+')
			{
				timeString += ParseArgv(src.substr(p, u - p));
				p = u + 1;
			}
		}
		timeString += ParseArgv(src.substr(p));
		return timeString;
	}
};