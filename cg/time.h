#pragma once

#include "base.h"

namespace CG
{
	class Time
	{
	public:
		struct TIME
		{
			UINT hour;
			UINT min;
			UINT sec;
		};

		struct DATE
		{
			UINT year;
			UINT mon;
			UINT day;
		};

		struct DATETIME : public DATE, public TIME {};

		static TIME toTIME(time_t t)
		{
			double c = 0.0;
			uint32 h = 0, m = 0, s = 0;

			c = (double)t / 3600.0;
			h = (uint32)c;

			if (c > 0.0) c = c - (double)h;

			c = (c + 0.0001) * 60;
			m = (uint32)c;

			if (c > 0.0) c = c - (double)m;

			c = (c + 0.01) * 60;
			s = (uint32)c;
			return { h, m, s };
		}

		static DATETIME toDATETIME(time_t t = time(0))
		{
			tm tf;
			localtime_s(&tf, &t);

			DATETIME dt;
			dt.year = tf.tm_year + 1900;
			dt.mon = tf.tm_mon + 1;
			dt.day = tf.tm_mday;
			dt.hour = tf.tm_hour;
			dt.min = tf.tm_min;
			dt.sec = tf.tm_sec;

			return dt;
		}

		static std::wstring toWStringD(const DATETIME& dt = toDATETIME())
		{
			return std::to_wstring(dt.year) + std::wstring(L"-") + std::to_wstring(dt.mon) + std::wstring(L"-") + std::to_wstring(dt.day);
		}

		static std::wstring toWStringT(const DATETIME& dt = toDATETIME())
		{
			return std::to_wstring(dt.hour) + std::wstring(L":") + std::to_wstring(dt.min) + std::wstring(L":") + std::to_wstring(dt.sec);
		}

		static std::wstring toWStringDT(const DATETIME& dt = toDATETIME())
		{
			return std::to_wstring(dt.year) + std::wstring(L"-") + std::to_wstring(dt.mon) + std::wstring(L"-") + std::to_wstring(dt.day) + std::wstring(L" ") +
				std::to_wstring(dt.hour) + std::wstring(L":") + std::to_wstring(dt.min) + std::wstring(L":") + std::to_wstring(dt.sec);
		}
	};
}