#pragma once

#include <windows.h>
#include <atlimage.h>
#include "base.h"
#include "string.h"

namespace CG {
	struct Rgb {
		byte b = 0;
		byte g = 0;
		byte r = 0;
		Rgb() {}
		Rgb(const byte& red, const byte& green, const byte& blue) { set(red, green, blue); }
		Rgb(const COLORREF& rgb) { set(rgb); }
		void set(const byte& red, const byte& green, const byte& blue) { r = red, g = green, b = blue; }
		void set(const COLORREF& rgb) { r = GetRValue(rgb), g = GetGValue(rgb), b = GetBValue(rgb); }
		bool equal(const Rgb& rgb, const byte& extend = 10) const { return (InRange(r, rgb.r, extend) && InRange(g, rgb.g, extend) && InRange(b, rgb.b, extend)); }
		bool equal(const COLORREF& rgb, const byte& extend = 10) const { return (InRange(r, GetRValue(rgb), extend) && InRange(g, GetGValue(rgb), extend) && InRange(b, GetBValue(rgb), extend)); }
		COLORREF toCOLORREF() const { return RGB(r, g, b); }
	};

	struct Rgba {
		byte a = 0;
		byte b = 0;
		byte g = 0;
		byte r = 0;
		Rgba() {}
		Rgba(const Rgb& rgb) { set(rgb); }
		Rgba(const byte& red, const byte& green, const byte& blue, const byte& alpha) { set(red, green, blue, alpha); }
		Rgba(const COLORREF& rgba) { set(rgba); }
		void set(const Rgb& rgb) { r = rgb.r, g = rgb.g, b = rgb.b, a = 0; }
		void set(const byte& red, const byte& green, const byte& blue, const byte& alpha) { r = red, g = green, b = blue, a = alpha; }
		void set(const COLORREF& rgba) { r = GetRValue(rgba), g = GetGValue(rgba), b = GetBValue(rgba), a = GetAValue(rgba); }
		bool equal(const Rgba& rgba, const byte& extend = 10) const { return (InRange(r, rgba.r, extend) && InRange(g, rgba.g, extend) && InRange(b, rgba.b, extend) && InRange(a, rgba.a, extend)); }
		bool equal(const COLORREF& rgba, const byte& extend = 10) const { return (InRange(r, GetRValue(rgba), extend) && InRange(g, GetRValue(rgba), extend) && InRange(b, GetBValue(rgba), extend) && InRange(a, GetAValue(rgba), extend)); }
		Rgb toRgb() const { return { r, g, b }; }
		COLORREF toCOLORREF() const { return RGBA(a, b, g, r); }
	};

	template<class T>
	class PointMap
	{
		T* _map = 0;
		uint32 _positon = 0;
		uint32 _width = 0;
		uint32 _height = 0;
		uint32 _count = 0;
	public:
		PointMap() { }
		PointMap(const PointMap& pointMap) { copy(pointMap); }
		PointMap(uint32 width, uint32 height) { create(width, height); }
		~PointMap() { release(); }

		void operator=(const PointMap& pointMap) { copy(pointMap); }
		T* operator[](uint32 row) { return _map + (_width * row); }
		const T* operator[](uint32 row) const { return _map + (_width * row); }

		void* data() { return _map; }
		const void* data() const { return _map; }

		T& map(uint32 positon = 0) { return _map[positon]; }
		const T& map(uint32 positon = 0) const { return _map[positon]; }

		T& point(uint32 row, uint32 col) { return _map[((_width * row) + col)]; }
		const T& point(uint32 row, uint32 col) const { return _map[((_width * row) + col)]; }

		T* Iterate() { if (_count) { if (_positon < _count) { _positon++; return _map + _positon - 1; } IterateReset(), Iterate(); } return 0; }
		void IterateReset() { _positon = 0; }
		uint32 width() const { return _width; }
		uint32 height() const { return _height; }
		uint32 count() const { return _count; }
		uint32 bytes() const { return (_count * sizeof(T)); }
		void create(uint32 width = 1, uint32 height = 1) { release(); if (width && height) { _width = width; _height = height; _count = width * height; _map = new T[_count]; } }
		void copy(const PointMap& pointMap) { release(); create(pointMap._width, pointMap._height); memcpy_s(_map, bytes(), pointMap._map, pointMap.bytes()); }
		void copy_s(const PointMap& pointMap) { release(); create(pointMap._width, pointMap._height); for (uint32 u = 0; u < _count; u++) _map[u] = pointMap._map[u]; }
		void fill(const T& point) { for (uint32 u = 0; u < (_count); u++) _map[u] = point; }
		void release() { if (_map) { delete[] _map; _map = nullptr; } _width = 0; _height = 0; _count = 0; }
	};

	typedef PointMap<Rgb> RgbMap;
	typedef PointMap<Rgba> RgbaMap;

	class Color
	{
	public:
		struct FindResult { bool find = 0; POINT pt = { 0 }; };

		static bool Equal(const Rgb& rgb1, const Rgb& rgb2, byte extend) { return (InRange(rgb1.r, rgb2.r, extend) && InRange(rgb1.g, rgb2.g, extend) && InRange(rgb1.b, rgb2.b, extend)); }
		static bool Equal(const Rgb& rgb1, const Rgb& _min, const Rgb& _max) { return (InRange(rgb1.r, _min.r, _max.r, 0) && InRange(rgb1.g, _min.g, _max.g, 0) && InRange(rgb1.b, _min.b, _max.b, 0)); 		}
		static bool Equal(COLORREF rgb, COLORREF refer, byte extend) { return (InRange(GetRValue(rgb), GetRValue(refer), extend) && InRange(GetGValue(rgb), GetGValue(refer), extend) && InRange(GetBValue(rgb), GetBValue(refer), extend)); }
		static bool Equal(COLORREF rgb, COLORREF _min, COLORREF _max) { return (InRange(GetRValue(rgb), GetRValue(_min), GetRValue(_max), 0) && InRange(GetGValue(rgb), GetGValue(_min), GetGValue(_max), 0) && InRange(GetBValue(rgb), GetBValue(_min), GetBValue(_max), 0)); }

		// POINT: x/y >= 0;
		static bool Find(const RgbMap& map, POINT pt, const Rgb& rgb, byte extend = 10) { if (pt.x < 0 || pt.y < 0) return 0; if (pt.x >= map.width()) pt.x = map.width() - 1; if (pt.y >= map.width()) pt.y = map.width() - 1; return Equal(map[pt.y][pt.x], rgb, extend); }
		// POINT: x/y >= 0;
		static bool Find(HDC hdc, POINT pt, COLORREF color, byte extend = 10) { if (pt.x < 0 || pt.y < 0) return 0; return Equal(color, GetPixel(hdc, pt.x, pt.y), extend); }

		// RECT: left/top >= 0, right/bottom >= left/top;
		static FindResult FindOr(const RgbMap& rgbMap, Rgb rgb, byte extend = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX }) {
			if (!rgbMap.count()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.left >= rgbMap.width()) rect.left = rgbMap.width();
			if (rect.top >= rgbMap.height()) rect.top = rgbMap.height();
			if (rect.right >= rgbMap.width()) rect.right = rgbMap.width();
			if (rect.bottom >= rgbMap.height()) rect.bottom = rgbMap.height();
			if (rect.right <= rect.left || rect.bottom <= rect.top) return {};
			uint32 xmax = rect.right - rect.left;
			uint32 ymax = rect.bottom - rect.top;
			for (uint32 y = 0; y < ymax; y++) for (uint32 x = 0; x < xmax; x++) if (Equal(rgbMap[rect.top + y][rect.left + x], rgb, extend)) return { true, rect.left + (LONG)x, rect.top + (LONG)y };
			return { 0 };
		}
		// RECT: left/top >= 0, right/bottom >= left/top;
		static FindResult FindOr(const RgbMap& rgbMap, Rgb min, Rgb max, RECT rect = { 0, 0, LONG_MAX, LONG_MAX }) {
			if (!rgbMap.count()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.left >= rgbMap.width()) rect.left = rgbMap.width();
			if (rect.top >= rgbMap.height()) rect.top = rgbMap.height();
			if (rect.right >= rgbMap.width()) rect.right = rgbMap.width();
			if (rect.bottom >= rgbMap.height()) rect.bottom = rgbMap.height();
			if (rect.right <= rect.left || rect.bottom <= rect.top) return {};
			uint32 xmax = rect.right - rect.left;
			uint32 ymax = rect.bottom - rect.top;
			for (uint32 y = 0; y < ymax; y++) for (uint32 x = 0; x < xmax; x++) if (Equal(rgbMap[rect.top + y][rect.left + x], min, max)) return { true, rect.left + (LONG)x, rect.top + (LONG)y };
			return { 0 };
		}
		// RECT: left/top >= 0, right/bottom >= left/top;
		static FindResult FindOr(HDC hdc, RECT rect, COLORREF rgb, byte extend = 10) {
			if (rect.left < 0 || rect.top < 0 || rect.right <= rect.left || rect.bottom <= rect.top) return { 0 };
			CImage image;
			image.Create(rect.right - rect.left, rect.bottom - rect.top, 24);
			BitBlt(image.GetDC(), 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, rect.left, rect.top, SRCCOPY);
			uint32 xmax = rect.right - rect.left;
			uint32 ymax = rect.bottom - rect.top;
			for (uint32 y = 0; y < ymax; y++) for (uint32 x = 0; x < xmax; x++) if (Equal(image.GetPixel(x, y), rgb, extend)) { image.ReleaseDC(); return { true, rect.left + (LONG)x, rect.top + (LONG)y }; }
			image.ReleaseDC();
			return { 0 };
		}
		// RECT: left/top >= 0, right/bottom >= left/top;
		static FindResult FindOr(HDC hdc, RECT rect, COLORREF min, COLORREF max) {
			if (rect.left < 0 || rect.top < 0 || rect.right <= rect.left || rect.bottom <= rect.top) return { 0 };
			CImage image;
			image.Create(rect.right - rect.left, rect.bottom - rect.top, 24);
			BitBlt(image.GetDC(), 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, rect.left, rect.top, SRCCOPY);
			uint32 xmax = rect.right - rect.left;
			uint32 ymax = rect.bottom - rect.top;
			for (uint32 y = 0; y < ymax; y++) for (uint32 x = 0; x < xmax; x++) if (Equal(image.GetPixel(x, y), min, max)) { image.ReleaseDC(); return { true, rect.left + (LONG)x, rect.top + (LONG)y }; }
			image.ReleaseDC();
			return { 0 };
		}

		// RECT: left/top >= 0, right/bottom >= left/top
		static bool FindAnd(const RgbMap& rgbMap, Rgb rgb, byte extend = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (!rgbMap.count()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.left >= rgbMap.width()) rect.left = rgbMap.width();
			if (rect.top >= rgbMap.height()) rect.top = rgbMap.height();
			if (rect.right >= rgbMap.width()) rect.right = rgbMap.width();
			if (rect.bottom >= rgbMap.height()) rect.bottom = rgbMap.height();
			if (rect.right <= rect.left || rect.bottom <= rect.top) return false;
			uint32 xmax = rect.right - rect.left;
			uint32 ymax = rect.bottom - rect.top;
			for (uint32 y = 0; y < ymax; y++) for (uint32 x = 0; x < xmax; x++) if (!Equal(rgbMap[rect.top + y][rect.left + x], rgb, extend)) return { 0 };
			return 1;
		}
		// RECT: left/top >= 0, right/bottom >= left/top
		static bool FindAnd(const RgbMap& rgbMap, Rgb min, Rgb max, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (!rgbMap.count()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.left >= rgbMap.width()) rect.left = rgbMap.width();
			if (rect.top >= rgbMap.height()) rect.top = rgbMap.height();
			if (rect.right >= rgbMap.width()) rect.right = rgbMap.width();
			if (rect.bottom >= rgbMap.height()) rect.bottom = rgbMap.height();
			if (rect.right <= rect.left || rect.bottom <= rect.top) return false;
			uint32 xmax = rect.right - rect.left;
			uint32 ymax = rect.bottom - rect.top;
			for (uint32 y = 0; y < ymax; y++) for (uint32 x = 0; x < xmax; x++) if (!Equal(rgbMap[rect.top + y][rect.left + x], min, max)) return { 0 };
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
			for (uint32 y = 0; y < rect.bottom - rect.top; y++) for (uint32 x = 0; x < rect.right - rect.left; x++) if (!Equal(image.GetPixel(x, y), rgb, extend)) { image.ReleaseDC(); return 0; }
			image.ReleaseDC();
			return 1;
		}
		// RECT: left/top >= 0, right/bottom >= left/top;
		static bool FindAnd(HDC hdc, RECT rect, COLORREF min, COLORREF max) {
			if (rect.left < 0 || rect.top < 0 || rect.right <= rect.left || rect.bottom <= rect.top) return { 0 };
			CImage image;
			image.Create(rect.right - rect.left, rect.bottom - rect.top, 24);
			BitBlt(image.GetDC(), 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, rect.left, rect.top, SRCCOPY);
			uint32 xmax = rect.right - rect.left;
			uint32 ymax = rect.bottom - rect.top;
			for (uint32 y = 0; y < rect.bottom - rect.top; y++) for (uint32 x = 0; x < rect.right - rect.left; x++) if (!Equal(image.GetPixel(x, y), min, max)) { image.ReleaseDC(); return 0; }
			image.ReleaseDC();
			return 1;
		}
	};
}