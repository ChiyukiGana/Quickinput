#pragma once

#include <windows.h>
#include <atlimage.h>
#include "base.h"
#include "string.h"

namespace CG {
	struct Rgb {
		Rgb() {}
		Rgb(byte r, byte g, byte b) { set(r, g, b); }
		Rgb(COLORREF rgb) { set(rgb); }
		void set(byte r, byte g, byte b) { this->r = r, this->g = g, this->b = b; }
		void set(COLORREF rgb) { this->r = GetRValue(rgb), this->g = GetGValue(rgb), this->b = GetBValue(rgb); }
		COLORREF toCOLORREF() const { return RGB(r, g, b); }
		byte b = 0;
		byte g = 0;
		byte r = 0;
	};

	struct Rgba {
		Rgba() {}
		Rgba(Rgb rgb) { set(rgb); }
		Rgba(byte r, byte g, byte b, byte a) { set(r, g, b, a); }
		Rgba(COLORREF rgba) { set(rgba); }
		void set(Rgb rgb) { this->r = rgb.r, this->g = rgb.g, this->b = rgb.b, this->a = 0; }
		void set(byte r, byte g, byte b, byte a) { this->r = r, this->g = g, this->b = b, this->a = a; }
		void set(COLORREF rgba) { this->r = GetRValue(rgba), this->g = GetGValue(rgba), this->b = GetBValue(rgba), this->a = GetAValue(rgba); }
		Rgb toRgb() const { return { r, g, b }; }
		COLORREF toCOLORREF() const { return RGBA(r, g, b, a); }
		byte a = 0;
		byte b = 0;
		byte g = 0;
		byte r = 0;
	};

	template<class T>
	class PointMap
	{
		T* _map = 0;
		uint32 _width = 0;
		uint32 _height = 0;

	public:
		PointMap() { }
		PointMap(uint32 width, uint32 height) { create(width, height); }
		~PointMap() { release(); }
		void operator=(const PointMap& pointMap) { copy_s(pointMap); }
		T* operator[](uint32 row) { return _map + (_width * row); }
		T* accessMap() { return _map; }
		T& accessPoint(uint32 row, uint32 col) { return _map[((_width * row) + col)]; }
		uint32 bytes() const { return ((_width * _height) * sizeof(T)); }
		uint32 width() const { return _width; }
		uint32 height() const { return _height; }
		void create(uint32 width = 1, uint32 height = 1) { release(); if (width && height) { _width = width; _height = height; _map = new T[_width * _height]; } }
		void copy(const PointMap& pointMap) { release(); create(pointMap._width, pointMap._height); memcpy_s(_map, _width * _height, pointMap._map, pointMap._width * pointMap._height); }
		void copy_s(const PointMap& pointMap) { release(); create(pointMap._width, pointMap._height); uint32 size = _width * _height; for (uint32 u = 0; u < size; u++) _map[u] = pointMap._map[u]; }
		void fill(const T& point) { for (uint32 u = 0; u < (_width * _height); u++) _map[u] = point; }
		void release() { if (_map) { delete[] _map; _map = 0; } _width = 0; _height = 0; }
	};

	typedef PointMap<Rgb> RgbMap;
	typedef PointMap<Rgba> RgbaMap;

	class Color
	{
	public:
		struct FindOrStatus { bool find = 0; POINT pt = { 0 }; };

		static bool Equal(Rgb rgb1, Rgb rgb2, byte extend) { return (InRange(rgb1.r, rgb2.r, extend) && InRange(rgb1.g, rgb2.g, extend) && InRange(rgb1.b, rgb2.b, extend)); }
		static bool Equal(COLORREF rgb, COLORREF refer, byte extend) { return (InRange(GetRValue(rgb), GetRValue(refer), extend) && InRange(GetGValue(rgb), GetGValue(refer), extend) && InRange(GetBValue(rgb), GetBValue(refer), extend)); }

		// POINT: x/y >= 0;
		static bool Find(RgbMap& map, POINT pt, Rgb rgb, byte extend = 10) { if (pt.x < 0 || pt.y < 0) return 0; if (pt.x >= map.width()) pt.x = map.width() - 1; if (pt.y >= map.width()) pt.y = map.width() - 1; return Equal(map[pt.y][pt.x], rgb, extend); }
		// POINT: x/y >= 0;
		static bool Find(HDC hdc, POINT pt, COLORREF color, byte extend = 10) { if (pt.x < 0 || pt.y < 0) return 0; return Equal(color, GetPixel(hdc, pt.x, pt.y), extend); }

		// RECT: left/top >= 0, right/bottom >= left/top;
		static FindOrStatus FindOr(RgbMap& map, Rgb rgb, byte extend = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX }) {
			if (rect.left < 0 || rect.top < 0 || rect.right <= rect.left || rect.bottom <= rect.top) return { 0 };
			if (rect.right >= map.width()) rect.right = map.width();
			if (rect.bottom >= map.height()) rect.bottom = map.height();
			uint32 xmax = rect.right - rect.left + 1;
			uint32 ymax = rect.bottom - rect.top + 1;
			for (uint32 y = 0; y < ymax; y++) for (uint32 x = 0; x < xmax; x++) if (Equal(map[rect.top + y][rect.left + x], rgb, extend)) return { 1, rect.left + (LONG)x, rect.top + (LONG)y };
			return { 0 };
		}
		// RECT: left/top >= 0, right/bottom >= left/top;
		static FindOrStatus FindOr(HDC hdc, RECT rect, COLORREF rgb, byte extend = 10) {
			if (rect.left < 0 || rect.top < 0 || rect.right <= rect.left || rect.bottom <= rect.top) return { 0 };
			CImage image;
			image.Create(rect.right - rect.left, rect.bottom - rect.top, 24);
			BitBlt(image.GetDC(), 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, rect.left, rect.top, SRCCOPY);
			uint32 xmax = rect.right - rect.left;
			uint32 ymax = rect.bottom - rect.top;
			for (uint32 y = 0; y < ymax; y++)
			{
				for (uint32 x = 0; x < xmax; x++)
				{
					if (Equal(image.GetPixel(x, y), rgb, extend))
					{
						image.ReleaseDC();
						return { 1, rect.left + (LONG)x, rect.top + (LONG)y };
					}
				}
			}
			image.ReleaseDC();
			return { 0 };
		}

		// RECT: left/top >= 0, right/bottom >= left/top
		static bool FindAnd(RgbMap& map, Rgb rgb, byte extend = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (rect.left < 0 || rect.top < 0 || rect.right <= rect.left || rect.bottom <= rect.top) return { 0 };
			if (rect.right > map.width()) rect.right = map.width();
			if (rect.bottom > map.height()) rect.bottom = map.height();
			uint32 xmax = rect.right - rect.left;
			uint32 ymax = rect.bottom - rect.top;
			for (uint32 y = 0; y < ymax; y++) for (uint32 x = 0; x < xmax; x++) if (!Equal(map[rect.top + y][rect.left + x], rgb, extend)) return { 0 };
			return 1;
		}
		// RECT: left/top >= 0, right/bottom >= left/top;
		static bool FindAnd(HDC hdc, RECT rect, COLORREF rgb, BYTE extend = 10) {
			if (rect.left < 0 || rect.top < 0 || rect.right <= rect.left || rect.bottom <= rect.top) return { 0 };
			CImage image;
			image.Create(rect.right - rect.left, rect.bottom - rect.top, 24);
			BitBlt(image.GetDC(), 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, rect.left, rect.top, SRCCOPY);
			uint32 xmax = rect.right - rect.left;
			uint32 ymax = rect.bottom - rect.top;
			for (uint32 y = 0; y < rect.bottom - rect.top; y++)
			{
				for (uint32 x = 0; x < rect.right - rect.left; x++)
				{
					if (!Equal(image.GetPixel(x, y), rgb, extend))
					{
						image.ReleaseDC();
						return 0;
					}
				}
			}
			image.ReleaseDC();
			return 1;
		}
	};
}
