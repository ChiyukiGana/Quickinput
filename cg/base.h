#pragma once

#include <stdint.h>
#include <utility>
#include <time.h>
#include <windows.h>

namespace CG
{
	typedef std::uint8_t byte;
	typedef std::int8_t int8;
	typedef std::uint8_t uint8;
	typedef std::int16_t int16;
	typedef std::uint16_t uint16;
	typedef std::int32_t int32;
	typedef std::uint32_t uint32;
	typedef std::int64_t int64;
	typedef std::uint64_t uint64;

#ifdef _M_AMD64
	typedef std::uint64_t pointer;
#else _M_IX86
	typedef std::uint32_t pointer;
#endif // _M_AMD64 

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

#define MAX_PATH_NEW 0x7FFF

#define AlignmentSize(num, size) ((num%size)?(num+(size-(num%size))):(num))

#define forxy(xmax, ymax) for (size_t y = 0, ym = ymax; y < ym; y++) for (size_t x = 0, xm = xmax; x < xm; x++)
#define forxyx(xnam, ynam, xmin, ymin, xmax, ymax) for (size_t ynam = ymin, ym = ymax; ynam < ym; ynam++) for (size_t xnam = xmin, xm = xmax; xnam < xm; xnam++)
#define forlt(xmax, ymax) for (int32 x = 0, y = 0, xs = 0, ys = 0, xm = xmax, ym = ymax; x <= xm && y <= ym; xs < xm -1?xs++:ys++) for (x = xs, y = ys; x >= 0 && x < xm && y < ym; x--, y++)
#define forltx(xnam, ynam, xmin, ymin, xmax, ymax) for (int32 xnam = 0, ynam = 0, xs = xmin, ys = ymin, xm = xmax, ym = ymax; xnam <= xm && ynam <= ym; xs < xm -1?xs++:ys++) for (xnam = xs, ynam = ys; xnam >= 0 && xnam < xm && ynam < ym; xnam--, ynam++)

#define RGBA(r,g,b,a) ((COLORREF)((BYTE)(r)|((BYTE)(g)<<8)|((BYTE)(b)<<16)|((BYTE)(a)<<24)))
#define GetAValue(rgba) ((BYTE)((rgba)>>24))

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

	static int Distance(int left, int right)
	{
		if (left > right) return left - right;
		else return right - left;
	}

	static bool InRange(const int& num, const int& _min, const int& _max, const int& extent) { return ((num >= _min - extent) && (num <= _max + extent)); }
	static bool InRange(const int& num, const int& refer, const int& extend) { return (num <= (refer + extend)) && (num >= (refer - extend)); }
	static bool InRect(const RECT& rect, const int& x, const int& y) { if (x < rect.left || x > rect.right || y < rect.top || y > rect.bottom) return false; return true; }
	static bool InRect(const RECT& rect, const POINT& pt) { if (pt.x < rect.left || pt.x > rect.right && pt.y < rect.top || pt.y > rect.bottom) return false; return true; }
	static bool InRect(const RECT& rectParent, const RECT& rectChild) { if (rectChild.left < rectParent.left || rectChild.top < rectParent.top || rectChild.right > rectParent.right || rectChild.bottom > rectParent.bottom) return false; return true; }
	static POINT InRectPos(const RECT& rect, const POINT& pt) { if (pt.x >= rect.left && pt.x <= rect.right && pt.y >= rect.top && pt.y <= rect.bottom) return { pt.x - rect.left, pt.y - rect.top }; return { -1, -1 }; }

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
		return { rect.right + 1, rect.bottom + 1 };
	}

	static uint32 RectArea(const RECT& rect)
	{
		SIZE size = RectSize(rect);
		return size.cx * size.cy;
	}

	static int Rand(int max, int min = 0) { return min + (rand() % (max - min + 1)); }
}