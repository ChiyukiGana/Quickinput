#pragma once

#include <stdint.h>
#include <utility>
#include <time.h>
#include <windows.h>

#define DeclareBytes(name, size) struct name { byte bytes[size]; }

#define cfor(count) for (uint32 c = 0; c < count; c++)

#define RGBA(r,g,b,a) ((COLORREF)((BYTE)(r)|((BYTE)(g)<<8)|((BYTE)(b)<<16)|((BYTE)(a)<<24)))
#define GetAValue(rgba) ((BYTE)((rgba)>>24))

typedef unsigned __int8 byte;
typedef signed __int8 int8;
typedef unsigned __int8 uint8;
typedef signed __int16 int16;
typedef unsigned __int16 uint16;
typedef signed __int32 int32;
typedef unsigned __int32 uint32;
typedef signed __int64 int64;
typedef unsigned __int64 uint64;

#define int8Max ((int8)0x7F)
#define int8Min ((int8)0x80)
#define uint8Max ((uint8)0xFF)
#define int16Max ((int16)0x7FFF)
#define int16Min ((int16)0x8000)
#define uint16Max ((uint16)0xFFFF)
#define int32Max ((int32)0x7FFFFFFF)
#define int32Min ((int32)0x80000000)
#define uint32Max ((uint32)0xFFFFFFFF)
#define int64Max ((int64)0x7FFFFFFFFFFFFFFF)
#define int64Min ((int64)0x8000000000000000)
#define uint64Max ((uint64)0xFFFFFFFFFFFFFFFF)

template<typename T>
struct TPOINT {
	T x;
	T y;
};

template<typename T>
struct TRECT {
	T left;
	T top;
	T right;
	T bottom;
};

typedef TRECT<float> FRECT, * PFRECT;
typedef TPOINT<float> FPOINT, * PFPOINT;

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

	static bool InRange(int num, int _min, int _max, int extent) { return ((num >= _min - extent) && (num <= _max + extent)); }
	static bool InRange(int num, int refer, int extend) { return (num <= (refer + extend)) && (num >= (refer - extend)); }
	static bool InRect(RECT rect, int x, int y, int extend = 0) { if (x >= rect.left - extend && x <= rect.right + extend && y >= rect.top - extend && y <= rect.bottom + extend) return true; return false; }
	static bool InRect(RECT rect, POINT pt, int extend = 0) { if (pt.x >= rect.left - extend && pt.x <= rect.right + extend && pt.y >= rect.top - extend && pt.y <= rect.bottom + extend) return true; return false; }
	static POINT PointInRect(RECT rect, POINT pt, int extend = 0) {
		if (pt.x >= rect.left - extend && pt.x <= rect.right + extend && pt.y >= rect.top - extend && pt.y <= rect.bottom + extend)
			return { pt.x - rect.left, pt.y - rect.top };
		return { -1, -1 };
	}
	
	static RECT RectAbs(RECT rect) {
		if (rect.left > rect.right) std::swap(rect.left, rect.right);
		if (rect.top > rect.bottom) std::swap(rect.top, rect.bottom);
		return rect;
	}

	static SIZE RectSize(RECT rect) {
		if (rect.left > rect.right) rect.right = rect.left - rect.right;
		else rect.right -= rect.left;
		if (rect.top > rect.bottom) rect.bottom = rect.top - rect.bottom;
		else rect.bottom -= rect.top;
		return { rect.right, rect.bottom };
	}

	static uint32 RectArea(RECT rect)
	{
		SIZE size = RectSize(rect);
		return size.cx * size.cy;
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
			return { (float)(rect.left / cx), (float)(rect.top / cy), (float)(rect.right / cx), (float)(rect.bottom / cy) };
		}
		// rect: pixel
		FRECT absolute(long left, long top, long right, long bottom) {
			return { (float)(left / cx), (float)(top / cy), (float)(right / cx), (float)(bottom / cy) };
		}
		// rect: pixel
		FPOINT absolute(long x, long y) {
			return { (float)(x / cx), (float)(y / cy), };
		}
	};
}

#define SleepRun(ms) CG::TimeOut timeOut(ms); while (timeOut.state())
#define SleepSet(ms) timeOut.set(ms);  //### Set in SleepRun inner ###
#define SleepOut() if (timeOut.timeOut)  //### Set in SleepRun outer ###