#pragma once

#include <stdint.h>
#include <utility>
#include <time.h>
#include <windows.h>

typedef unsigned __int8 byte;
typedef signed __int8 int8;
typedef unsigned __int8 uint8;
typedef signed __int16 int16;
typedef unsigned __int16 uint16;
typedef signed __int32 int32;
typedef unsigned __int32 uint32;
#ifdef _WIN64
typedef signed __int64 int64;
typedef unsigned __int64 uint64;
#endif

#define RGBA(r,g,b,a) ((COLORREF)((BYTE)(r)|((BYTE)(g)<<8)|((BYTE)(b)<<16)|((BYTE)(a)<<24)))
#define GetAValue(rgba) ((BYTE)((rgba)>>24))

typedef struct _FRECT {
	double left;
	double top;
	double right;
	double bottom;
} FRECT, * PFRECT;

typedef struct _FPOINT {
	double x;
	double y;
} FPOINT, * PFPOINT;

namespace CG {
	template <typename T>
	static void ArrLeftMove(T arr, uint32 end, uint32 pos, uint32 len) {
		for (uint32 u = pos; u + len - 1 < end; u++) {
			arr[u] = arr[u + len];
		}
		for (uint32 u = end; u > end - len - 1; u--) {
			arr[u] = { 0 };
		}
	}

	template <typename T>
	static void ArrRightMove(T arr, uint32 end, uint32 pos, uint32 len) {
		for (uint32 u = end; u - len + 1 > pos; u--) {
			arr[u] = arr[u - len];
		}
		for (uint32 u = pos; u < pos + len; u++) {
			arr[u] = { 0 };
		}
	}

	static void ArrarReverse(int* arr, unsigned int len) {
		unsigned int l = 0, r = len - 1, e = (len - 1) / 2;
		while (l <= e) {
			std::swap(arr[l], arr[r]);
			l++, r--;
		}
	}

	static bool InRange(int num, int min, int max, int extend) { return ((num >= (min - extend)) && (num <= (max + extend))); }
	static bool InRange(int num, int refer, int extend) { return (num <= (refer + extend)) && (num >= (refer - extend)); }
	static bool InRect(RECT rect, int x, int y, int extend = 0) { if (x >= rect.left - extend && x <= rect.right + extend && y >= rect.top - extend && y <= rect.bottom + extend) return 1; return 0; }

	static RECT RectAbs(RECT rect) {
		if (rect.left > rect.right) std::swap(rect.left, rect.right);
		if (rect.top > rect.bottom) std::swap(rect.top, rect.bottom);
		return rect;
	}

	static RECT RectSize(RECT rect) {
		if (rect.left > rect.right) rect.right = rect.left - rect.right;
		else rect.right -= rect.left;
		if (rect.top > rect.bottom) rect.bottom = rect.top - rect.bottom;
		else rect.bottom -= rect.top;
		rect.left = 0;
		rect.top = 0;
		return rect;
	}

	static int Rand(int max, int min = 0) { return min + (rand() % (max - min + 1)); }

	class TimeOut {
	private:

		long timet = 0;

	public:

		bool timeOut = 0;

		TimeOut() { Set(clock()); }
		TimeOut(long ms) { Set(ms); }

		void Set(long ms) { timeOut = 0; timet = clock() + ms; }

		bool state() { if (timet > clock()) return 1; timeOut = 1; return 0; }
	};

	class AbsPos {
	private:

		double cx = 0, cy = 0;

	public:

		AbsPos() {}
		AbsPos(long cx, long cy) { Size(cx, cy); }

		// cx, cy: pixel
		void Size(long cx, long cy) { this->cx = (double)cx; this->cy = (double)cy; }

		// rect: 0.0 ~ 1.0
		RECT relative(FRECT& rect) {
			return { (long)(cx * rect.left), (long)(cy * rect.top), (long)(cx * rect.right), (long)(cy * rect.bottom), };
		}
		// rect: 0.0 ~ 1.0
		RECT relative(double left, double top, double right, double bottom) {
			return { (long)(cx * left), (long)(cy * top), (long)(cx * right), (long)(cy * bottom), };
		}
		// rect: 0.0 ~ 1.0
		POINT relative(double x, double y) {
			return { (long)(cx * x), (long)(cy * y), };
		}

		// rect: pixel
		FRECT absolute(RECT& rect) {
			return { (double)rect.left / cx, (double)rect.top / cy, (double)rect.right / cx, (double)rect.bottom / cy };
		}
		// rect: pixel
		FRECT absolute(long left, long top, long right, long bottom) {
			return { (double)left / cx, (double)top / cy, (double)right / cx, (double)bottom / cy };
		}
		// rect: pixel
		FPOINT absolute(long x, long y) {
			return { (double)x / cx, (double)y / cy, };
		}
	};

	template <class ITEM>
	class List
	{
	private:

		ITEM* item = 0;
		uint32 length = 0;

	public:

		ITEM& operator[](uint32 u) { return item[u]; }

		void Add()
		{
			ITEM* cache = new ITEM[length + 1];
			for (uint32 u = 0; u < length; u++) { cache[u] = item[u]; }

			if (item) delete[] item;
			item = cache;
			length++;
		}

		void Add(ITEM val)
		{
			ITEM* cache = new ITEM[length + 1];
			for (uint32 u = 0; u < length; u++) { cache[u] = item[u]; }

			if (item) delete[] item;

			cache[length] = val;
			item = cache;
			length++;
		}

		bool Ins(uint32 pos)
		{
			if (pos > length) return 0;
			ITEM* cache = new ITEM[length + 1];
			for (uint32 u = 0; u < pos && u < length; u++)
			{
				cache[u] = item[u];
			}
			for (uint32 u = pos; u < length; u++)
			{
				cache[u + 1] = item[u];
			}

			if (item) delete[] item;

			item = cache;
			length++;
			return 0;
		}

		bool Ins(uint32 pos, ITEM val)
		{
			if (pos > length) return 0;
			ITEM* cache = new ITEM[length + 1];
			for (uint32 u = 0; u < pos && u < length; u++)
			{
				cache[u] = item[u];
			}

			cache[pos] = val;


			for (uint32 u = pos; u < length; u++)
			{
				cache[u + 1] = item[u];
			}

			if (item) delete[] item;

			item = cache;
			length++;
			return 1;
		}

		bool Swp(uint32 p1, uint32 p2)
		{
			if ((p1 < length) && (p2 < length) && (p1 != p2))
			{
				ITEM cache;
				cache = item[p1];
				item[p1] = item[p2];
				item[p2] = cache;
				return 1;
			}
			return 0;
		}

		bool Mov(uint32 before, uint32 after)
		{
			if (before > length || after > length) return 0;

			if (before > after)
			{
				for (uint32 u = before; u > after; u--)
				{
					Swp(u, u - 1);
				}
				return 1;
			}
			else if (before < after)
			{
				for (uint32 u = before; u < after; u++)
				{
					Swp(u, u + 1);
				}
				return 1;
			}
			else return 1;
		}

		void Cop(List list)
		{
			Emp();
			for (uint32 u = 0; u < list.len(); u++) Add(list.item[u]);
		}

		void Cop(List list, int begin, int end)
		{
			Emp();
			for (; begin <= end && begin < list.len(); begin++) Add(list.item[begin]);
		}

		void Cut(List list)
		{
			for (uint32 u = 0; u < list.len(); u++) Add(list.item[u]);
		}

		void Cut(List list, int begin, int end)
		{
			for (; begin <= end && begin < list.len(); begin++) Add(list.item[begin]);
		}

		bool Del(uint32 pos)
		{
			if (length < 1) return 0;
			if (pos > length - 1) return 0;

			length--;

			if (length == 0)
			{
				if (item) delete[] item;
				item = 0;
				return 1;
			}

			ITEM* cache = new ITEM[length];

			for (uint32 u = 0; u < length + 1; u++)
			{
				if (u < pos) {
					cache[u] = item[u];
				}
				else if (u > pos) {
					cache[u - 1] = item[u];
				}
			}

			if (item) delete[] item;
			item = cache;

			return 1;
		}

		void Emp()
		{
			if (item) delete[] item;
			item = 0;
			length = 0;
		}

		uint32 len() { return length; }
	};

	class TimeStamp
	{
	public:
		struct Time
		{
			uint32 h;
			uint32 m;
			uint32 s;
		};

		static Time toTime(ULONG stamp)
		{
			double c;
			uint32 h = 0, m = 0, s = 0;

			c = (double)stamp / 3600.0;
			h = c;

			if (c > 0.0) c = c - (double)h;

			c = (c + 0.0001) * 60;
			m = c;

			if (c > 0.0) c = c - (double)m;

			c = (c + 0.01) * 60;
			s = c;
			return { h, m, s };
		}

		static ULONG toStamp(Time time)
		{
			return time.s + time.m * 60 + time.h * 3600;
		}
	};
}

#define SleepRun(ms) CG::TimeOut timeOut(ms); while (timeOut.state())
#define SleepSet(ms) timeOut.set(ms);  //### Set in SleepRun inner ###
#define SleepOut() if (timeOut.timeOut)  //### Set in SleepRun outer ###