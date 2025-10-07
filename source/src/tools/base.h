#pragma once
#include <windows.h>
#include <cmath>
#define AlignmentSize(num, size) ((num%size)?(num+(size-(num%size))):(num))
#define forxy(xmax, ymax) for (int y = 0, ym = ymax; y < ym; y++) for (int x = 0, xm = xmax; x < xm; x++)
#define forxyx(xnam, ynam, xmin, ymin, xmax, ymax) for (int ynam = ymin, ym = ymax; ynam < ym; ynam++) for (int xnam = xmin, xm = xmax; xnam < xm; xnam++)
#define forlt(xmax, ymax) for (int x = 0, y = 0, xs = 0, ys = 0, xm = xmax, ym = ymax; x <= xm && y <= ym; xs < xm -1?xs++:ys++) for (x = xs, y = ys; x >= 0 && x < xm && y < ym; x--, y++)
#define forltx(xnam, ynam, xmin, ymin, xmax, ymax) for (int xnam = 0, ynam = 0, xs = xmin, ys = ymin, xm = xmax, ym = ymax; xnam <= xm && ynam <= ym; xs < xm -1?xs++:ys++) for (xnam = xs, ynam = ys; xnam >= 0 && xnam < xm && ynam < ym; xnam--, ynam++)
#define RGBA(r,g,b,a) ((COLORREF)((BYTE)(r)|((BYTE)(g)<<8)|((BYTE)(b)<<16)|((BYTE)(a)<<24)))
#define GetAValue(rgba) ((BYTE)((rgba)>>24))
namespace QiTools
{
	template <class num>
	static num Distance(num left, num right)
	{
		return (left > right) ? left - right : right - left;
	}
	template <class num>
	static double Distance(num x1, num y1, num x2, num y2) { return std::sqrt(std::pow((double)x1 - (double)x2, 2.0) + std::pow((double)y1 - (double)y2, 2.0)); }
	static double Distance(POINT pt1, POINT pt2) { return Distance(pt1.x, pt1.y, pt2.x, pt2.y); }
	template <class num>
	static bool InRange(num value, num _min, num _max, num ex) { return ((value >= _min - ex) && (value <= _max + ex)); }
	template <class num>
	static bool InRange(num value, num refer, num ex) { return (value <= (refer + ex)) && (value >= (refer - ex)); }
	static bool InRect(const RECT& rect, LONG x, LONG y) { return !(x < rect.left || x > rect.right || y < rect.top || y > rect.bottom); }
	static bool InRect(const RECT& rect, const POINT& pt) { return InRect(rect, pt.x, pt.y); }
	static bool InRect(const RECT& rectParent, const RECT& rectChild) { return !(rectChild.left < rectParent.left || rectChild.top < rectParent.top || rectChild.right > rectParent.right || rectChild.bottom > rectParent.bottom); }
	static POINT InRectPos(const RECT& rect, const POINT& pt) { return InRect(rect, pt) ? POINT({ pt.x - rect.left, pt.y - rect.top }) : POINT({ -1, -1 }); }
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
	static int RectArea(const RECT& rect)
	{
		SIZE size = RectSize(rect);
		return size.cx * size.cy;
	}
	static int Rand(int max, int min = 0)
	{
		if (min > max) std::swap(max, min);
		return min + (rand() % (max - min + 1));
	}
}