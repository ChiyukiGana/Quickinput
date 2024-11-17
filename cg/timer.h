#pragma once
#include <windows.h>
#include "string.h"
#define BeginSpeedTest() { LARGE_INTEGER __f, __b, __a; QueryPerformanceFrequency(&__f); QueryPerformanceCounter(&__b);
#define EndSpeedTest() QueryPerformanceCounter(&__a); MessageBoxW(nullptr, String::markNumber((__a.QuadPart - __b.QuadPart) / (__f.QuadPart / 1000000)).c_str(), L"time (us)", MB_OK); }

namespace CG
{
	class Timer
	{
		LARGE_INTEGER f;
		LARGE_INTEGER ca;
		LARGE_INTEGER cb;

	public:
		void wait_s(LONGLONG s)
		{
			QueryPerformanceFrequency(&f);
			QueryPerformanceCounter(&ca);
			cb.QuadPart = ca.QuadPart;
			ca.QuadPart += s * f.QuadPart;
			while (cb.QuadPart < ca.QuadPart)
			{
				QueryPerformanceCounter(&cb);
			}
		}
		void wait_s(LONGLONG s, bool(&Fn)())
		{
			QueryPerformanceFrequency(&f);
			QueryPerformanceCounter(&ca);
			cb.QuadPart = ca.QuadPart;
			ca.QuadPart += s * f.QuadPart;
			while (cb.QuadPart < ca.QuadPart)
			{
				QueryPerformanceCounter(&cb);
				if (Fn()) break;
			}
		}

		void wait_m(LONGLONG ms)
		{
			QueryPerformanceFrequency(&f);
			QueryPerformanceCounter(&ca);
			cb.QuadPart = ca.QuadPart;
			ca.QuadPart += ms * (f.QuadPart / 1000);
			while (cb.QuadPart < ca.QuadPart)
			{
				QueryPerformanceCounter(&cb);
			}
		}
		void wait_m(LONGLONG ms, bool(&Fn)())
		{
			QueryPerformanceFrequency(&f);
			QueryPerformanceCounter(&ca);
			cb.QuadPart = ca.QuadPart;
			ca.QuadPart += ms * (f.QuadPart / 1000);
			while (cb.QuadPart < ca.QuadPart)
			{
				QueryPerformanceCounter(&cb);
				if (Fn()) break;
			}
		}

		void wait_u(LONGLONG us)
		{
			QueryPerformanceFrequency(&f);
			QueryPerformanceCounter(&ca);
			cb.QuadPart = ca.QuadPart;
			ca.QuadPart += us * (f.QuadPart / 1000000);
			while (cb.QuadPart < ca.QuadPart)
			{
				QueryPerformanceCounter(&cb);
			}
		}
		void wait_u(LONGLONG us, bool(&Fn)())
		{
			QueryPerformanceFrequency(&f);
			QueryPerformanceCounter(&ca);
			cb.QuadPart = ca.QuadPart;
			ca.QuadPart += us * (f.QuadPart / 1000000);
			while (cb.QuadPart < ca.QuadPart)
			{
				QueryPerformanceCounter(&cb);
				if (Fn()) break;
			}
		}

		void wait_n(LONGLONG ns)
		{
			QueryPerformanceFrequency(&f);
			QueryPerformanceCounter(&ca);
			cb.QuadPart = ca.QuadPart;
			ca.QuadPart += (LONGLONG)((double)ns * ((double)f.QuadPart / 1000000000.0));
			while (cb.QuadPart < ca.QuadPart)
			{
				QueryPerformanceCounter(&cb);
			}
		}
		void wait_n(LONGLONG ns, bool(&Fn)())
		{
			QueryPerformanceFrequency(&f);
			QueryPerformanceCounter(&ca);
			cb.QuadPart = ca.QuadPart;
			ca.QuadPart += (LONGLONG)((double)ns * ((double)f.QuadPart / 1000000000.0));
			while (cb.QuadPart < ca.QuadPart)
			{
				QueryPerformanceCounter(&cb);
				if (Fn()) break;
			}
		}

		void wait_f(LONGLONG frequency)
		{
			QueryPerformanceFrequency(&f);
			QueryPerformanceCounter(&ca);
			cb.QuadPart = ca.QuadPart;

			if (frequency <= 1000)
			{
				ca.QuadPart += (1000 / frequency) * (f.QuadPart / 1000);
			}
			else if (frequency <= 1000000)
			{
				ca.QuadPart += (1000000 / frequency) * (f.QuadPart / 1000000);
			}
			else if (frequency <= 1000000000)
			{
				ca.QuadPart += (LONGLONG)((1000000000.0 / (double)frequency) * ((double)f.QuadPart / 1000000000.0));
			}

			while (cb.QuadPart < ca.QuadPart)
			{
				QueryPerformanceCounter(&cb);
			}
		}
		void wait_f(LONGLONG frequency, bool(&Fn)())
		{
			QueryPerformanceFrequency(&f);
			QueryPerformanceCounter(&ca);
			cb.QuadPart = ca.QuadPart;

			if (frequency <= 1000)
			{
				ca.QuadPart += (1000 / frequency) * (f.QuadPart / 1000);
			}
			else if (frequency <= 1000000)
			{
				ca.QuadPart += (1000000 / frequency) * (f.QuadPart / 1000000);
			}
			else if (frequency <= 1000000000)
			{
				ca.QuadPart += (LONGLONG)((1000000000.0 / (double)frequency) * ((double)f.QuadPart / 1000000000.0));
			}

			while (cb.QuadPart < ca.QuadPart)
			{
				QueryPerformanceCounter(&cb);
				if (Fn()) break;
			}
		}
	};

	template<class Fn, class... Args>
	static void TaskTimer(clock_t time, clock_t interval, Fn fn, Args ...args)
	{
		clock_t begin = clock();
		while ((begin + time) > clock())
		{
			fn(args...);
			Sleep(interval);
		}
	}
}