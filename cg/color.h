#pragma once

#include <windows.h>
#include <atlimage.h>
#include "base.h"
#include "string.h"

namespace CG {
	struct _RGB_ { byte r; byte g; byte b; };
	struct _BGR_ { byte b; byte g; byte r; };
	struct _ARGB_ { byte a; byte r; byte g; byte b; };
	struct _RGBA_ { byte r; byte g; byte b; byte a; };
	struct _ABGR_ { byte a; byte b; byte g; byte r; };
	struct _BGRA_ { byte b; byte g; byte r; byte a; };

	struct Rgb
	{
		byte r = 0;
		byte g = 0;
		byte b = 0;
		Rgb() {}
		Rgb(const byte& red, const byte& green, const byte& blue) { set(red, green, blue); }
		Rgb(const COLORREF& rgb) { set(rgb); }
		void set(const byte& red, const byte& green, const byte& blue) { r = red, g = green, b = blue; }
		void set(const COLORREF& rgb) { r = GetRValue(rgb), g = GetGValue(rgb), b = GetBValue(rgb); }
		bool equal(const Rgb& rgb, const byte& extend = 10) const { return (InRange(r, rgb.r, extend) && InRange(g, rgb.g, extend) && InRange(b, rgb.b, extend)); }
		bool equal(const COLORREF& rgb, const byte& extend = 10) const { return (InRange(r, GetRValue(rgb), extend) && InRange(g, GetGValue(rgb), extend) && InRange(b, GetBValue(rgb), extend)); }
		COLORREF toCOLORREF() const { return RGB(r, g, b); }
	};

	struct Rgba
	{
		byte r = 0;
		byte g = 0;
		byte b = 0;
		byte a = 0;
		Rgba() {}
		Rgba(const Rgb& rgb) { set(rgb); }
		Rgba(const byte& red, const byte& green, const byte& blue, const byte& alpha) { set(red, green, blue, alpha); }
		Rgba(const COLORREF& rgba) { set(rgba); }
		void set(const Rgb& rgb) { r = rgb.r, g = rgb.g, b = rgb.b, a = 0; }
		void set(const byte& red, const byte& green, const byte& blue, const byte& alpha) { r = red, g = green, b = blue, a = alpha; }
		void set(const COLORREF& rgba) { r = GetRValue(rgba), g = GetGValue(rgba), b = GetBValue(rgba), a = GetAValue(rgba); }
		bool equal(const Rgba& rgba, const byte& extend = 10) const { return (InRange(r, rgba.r, extend) && InRange(g, rgba.g, extend) && InRange(b, rgba.b, extend) && InRange(a, rgba.a, extend)); }
		bool equal(const COLORREF& rgba, const byte& extend = 10) const { return (InRange(r, GetRValue(rgba), extend) && InRange(g, GetRValue(rgba), extend) && InRange(b, GetBValue(rgba), extend) && InRange(a, GetAValue(rgba), extend)); }
		Rgb toRgb() const { return Rgb(r, g, b); }
		COLORREF toCOLORREF() const { return RGBA(r, g, b, a); }
	};

	template<class T>
	class XMap
	{
		T* _map = 0;
		uint32 _positon = 0;
		uint32 _width = 0;
		uint32 _height = 0;
		uint32 _count = 0;
	public:
		XMap() { }
		XMap(const XMap& xMap) { copy(xMap); }
		XMap(uint32 width, uint32 height) { create(width, height); }
		~XMap() { release(); }

		void operator=(const XMap& xMap) { copy(xMap); }
		T* operator[](uint32 row) { return _map + (_width * row); }
		const T* operator[](uint32 row) const { return _map + (_width * row); }

		void* data() { return _map; }
		const void* data() const { return _map; }

		T& map(uint32 positon = 0) { return _map[positon]; }
		const T& map(uint32 positon = 0) const { return _map[positon]; }

		T& point(uint32 row, uint32 col) { return _map[((_width * row) + col)]; }
		const T& point(uint32 row, uint32 col) const { return _map[((_width * row) + col)]; }

		T& Iterate() { if (_count) { if (_positon < _count) { _positon++; return *(_map + _positon - 1); } IterateReset(), Iterate(); } return _map[0]; }
		void IterateReset() { _positon = 0; }
		uint32 width() const { return _width; }
		uint32 height() const { return _height; }
		uint32 count() const { return _count; }
		uint32 bytes() const { return (_count * sizeof(T)); }
		void create(uint32 width = 1, uint32 height = 1) { if (width != _width || _height != height) { release(); if (width && height) { _width = width; _height = height; _count = width * height; _map = new T[_count]; } } }
		void copy(const XMap& xMap) { release(); create(xMap._width, xMap._height); memcpy_s(_map, bytes(), xMap._map, xMap.bytes()); }
		void copy_s(const XMap& xMap) { release(); create(xMap._width, xMap._height); for (uint32 u = 0; u < _count; u++) _map[u] = xMap._map[u]; }
		void fill(const T& point) { for (uint32 u = 0; u < (_count); u++) _map[u] = point; }
		void clear() { memset(_map, 0, _count * sizeof(T)); }
		void release() { if (_map) { delete[] _map; _map = nullptr; } _width = 0; _height = 0; _count = 0; }
	};

	typedef XMap<Rgb> RgbMap;
	typedef XMap<Rgba> RgbaMap;

	class Color
	{
	public:
		struct FindResult { bool find = 0; POINT pt = { 0 }; };

		static bool Equal(const Rgb& rgb1, const Rgb& rgb2, byte extend) { return (InRange(rgb1.r, rgb2.r, extend) && InRange(rgb1.g, rgb2.g, extend) && InRange(rgb1.b, rgb2.b, extend)); }
		static bool Equal(const Rgb& rgba1, const Rgb& min, const Rgb& max) { return (InRange(rgba1.r, min.r, max.r, 0) && InRange(rgba1.g, min.g, max.g, 0) && InRange(rgba1.b, min.b, max.b, 0)); 		}
		static bool Equal(const Rgba& rgb1, const Rgba& rgba2, byte extend) { return (InRange(rgb1.r, rgba2.r, extend) && InRange(rgb1.g, rgba2.g, extend) && InRange(rgb1.b, rgba2.b, extend)); }
		static bool Equal(const Rgba& rgba1, const Rgba& min, const Rgba& max) { return (InRange(rgba1.r, min.r, max.r, 0) && InRange(rgba1.g, min.g, max.g, 0) && InRange(rgba1.b, min.b, max.b, 0)); }
		static bool Equal(COLORREF rgb, COLORREF refer, byte extend) { return (InRange(GetRValue(rgb), GetRValue(refer), extend) && InRange(GetGValue(rgb), GetGValue(refer), extend) && InRange(GetBValue(rgb), GetBValue(refer), extend)); }
		static bool Equal(COLORREF rgb, COLORREF _min, COLORREF _max) { return (InRange(GetRValue(rgb), GetRValue(_min), GetRValue(_max), 0) && InRange(GetGValue(rgb), GetGValue(_min), GetGValue(_max), 0) && InRange(GetBValue(rgb), GetBValue(_min), GetBValue(_max), 0)); }

		// POINT: x/y >= 0;
		static bool Find(const RgbMap& map, POINT pt, const Rgb& rgb, byte extend = 10) { if (pt.x < 0 || pt.y < 0) return 0; if (pt.x >= map.width()) pt.x = map.width() - 1; if (pt.y >= map.width()) pt.y = map.width() - 1; return Equal(map[pt.y][pt.x], rgb, extend); }
		// POINT: x/y >= 0;
		static bool Find(HDC hdc, POINT pt, COLORREF color, byte extend = 10) { if (pt.x < 0 || pt.y < 0) return 0; return Equal(color, GetPixel(hdc, pt.x, pt.y), extend); }

		static FindResult FindOr(const RgbMap& rgbMap, Rgb rgb, byte extend = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX }) {
			if (!rgbMap.count()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > rgbMap.width()) rect.right = rgbMap.width();
			if (rect.bottom > rgbMap.height()) rect.bottom = rgbMap.height();
			for (uint32 x = 0, y = 0, xmax = rect.right - rect.left, ymax = rect.bottom - rect.top; y < ymax; y++) for (x = 0; x < xmax; x++) if (Equal(rgbMap[rect.top + y][rect.left + x], rgb, extend)) return { true, rect.left + (LONG)x, rect.top + (LONG)y };
			return {};
		}
		static FindResult FindOr(const RgbMap& rgbMap, Rgb min, Rgb max, RECT rect = { 0, 0, LONG_MAX, LONG_MAX }) {
			if (!rgbMap.count()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > rgbMap.width()) rect.right = rgbMap.width();
			if (rect.bottom > rgbMap.height()) rect.bottom = rgbMap.height();
			for (uint32 x = 0, y = 0, xmax = rect.right - rect.left, ymax = rect.bottom - rect.top; y < ymax; y++) for (x = 0; x < xmax; x++) if (Equal(rgbMap[rect.top + y][rect.left + x], min, max)) return { true, rect.left + (LONG)x, rect.top + (LONG)y };
			return {};
		}
		static FindResult FindOr(const RgbaMap& rgbaMap, Rgba rgb, byte extend = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX }) {
			if (!rgbaMap.count()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > rgbaMap.width()) rect.right = rgbaMap.width();
			if (rect.bottom > rgbaMap.height()) rect.bottom = rgbaMap.height();
			for (uint32 x = 0, y = 0, xmax = rect.right - rect.left, ymax = rect.bottom - rect.top; y < ymax; y++) for (x = 0; x < xmax; x++) if (Equal(rgbaMap[rect.top + y][rect.left + x], rgb, extend)) return { true, rect.left + (LONG)x, rect.top + (LONG)y };
			return {};
		}
		static FindResult FindOr(const RgbaMap& rgbaMap, Rgba min, Rgba max, RECT rect = { 0, 0, LONG_MAX, LONG_MAX }) {
			if (!rgbaMap.count()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > rgbaMap.width()) rect.right = rgbaMap.width();
			if (rect.bottom > rgbaMap.height()) rect.bottom = rgbaMap.height();
			for (uint32 x = 0, y = 0, xmax = rect.right - rect.left, ymax = rect.bottom - rect.top; y < ymax; y++) for (x = 0; x < xmax; x++) if (Equal(rgbaMap[rect.top + y][rect.left + x], min, max)) return { true, rect.left + (LONG)x, rect.top + (LONG)y };
			return {};
		}

		static bool FindAnd(const RgbMap& rgbMap, Rgb rgb, byte extend = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (!rgbMap.count()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > rgbMap.width()) rect.right = rgbMap.width();
			if (rect.bottom > rgbMap.height()) rect.bottom = rgbMap.height();
			for (uint32 x = 0, y = 0, xmax = rect.right - rect.left, ymax = rect.bottom - rect.top; y < ymax; y++) for (x = 0; x < xmax; x++) if (!Equal(rgbMap[rect.top + y][rect.left + x], rgb, extend)) return { 0 };
			return 1;
		}
		static bool FindAnd(const RgbMap& rgbMap, Rgb min, Rgb max, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (!rgbMap.count()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > rgbMap.width()) rect.right = rgbMap.width();
			if (rect.bottom > rgbMap.height()) rect.bottom = rgbMap.height();
			for (uint32 x = 0, y = 0, xmax = rect.right - rect.left, ymax = rect.bottom - rect.top; y < ymax; y++) for (x = 0; x < xmax; x++) if (!Equal(rgbMap[rect.top + y][rect.left + x], min, max)) return { 0 };
			return 1;
		}
		static bool FindAnd(const RgbaMap& rgbaMap, Rgba rgb, byte extend = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (!rgbaMap.count()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > rgbaMap.width()) rect.right = rgbaMap.width();
			if (rect.bottom > rgbaMap.height()) rect.bottom = rgbaMap.height();
			for (uint32 x = 0, y = 0, xmax = rect.right - rect.left, ymax = rect.bottom - rect.top; y < ymax; y++) for (x = 0; x < xmax; x++) if (!Equal(rgbaMap[rect.top + y][rect.left + x], rgb, extend)) return { 0 };
			return 1;
		}
		static bool FindAnd(const RgbaMap& rgbaMap, Rgba min, Rgba max, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (!rgbaMap.count()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > rgbaMap.width()) rect.right = rgbaMap.width();
			if (rect.bottom > rgbaMap.height()) rect.bottom = rgbaMap.height();
			for (uint32 x = 0, y = 0, xmax = rect.right - rect.left, ymax = rect.bottom - rect.top; y < ymax; y++) for (x = 0; x < xmax; x++) if (!Equal(rgbaMap[rect.top + y][rect.left + x], min, max)) return { 0 };
			return 1;
		}
	};
}