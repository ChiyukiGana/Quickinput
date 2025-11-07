#pragma once
#include <windows.h>
#include <atlimage.h>
#include "base.h"
#include "map2d.h"
namespace QiTools {
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
	using RgbMap = map2d<Rgb>;
	using RgbaMap = map2d<Rgba>;
	class Color
	{
	public:
		using Result = std::optional<POINT>;
		static bool Equal(const Rgb& rgb1, const Rgb& rgb2, byte extend) { return (InRange(rgb1.r, rgb2.r, extend) && InRange(rgb1.g, rgb2.g, extend) && InRange(rgb1.b, rgb2.b, extend)); }
		static bool Equal(const Rgb& rgba1, const Rgb& min, const Rgb& max) { return (InRange(rgba1.r, min.r, max.r, (byte)0) && InRange(rgba1.g, min.g, max.g, (byte)0) && InRange(rgba1.b, min.b, max.b, (byte)0)); }
		static bool Equal(const Rgba& rgb1, const Rgba& rgba2, byte extend) { return (InRange(rgb1.r, rgba2.r, extend) && InRange(rgb1.g, rgba2.g, extend) && InRange(rgb1.b, rgba2.b, extend)); }
		static bool Equal(const Rgba& rgba1, const Rgba& min, const Rgba& max) { return (InRange(rgba1.r, min.r, max.r, (byte)0) && InRange(rgba1.g, min.g, max.g, (byte)0) && InRange(rgba1.b, min.b, max.b, (byte)0)); }
		static bool Equal(COLORREF rgb, COLORREF refer, byte extend) { return (InRange(GetRValue(rgb), GetRValue(refer), extend) && InRange(GetGValue(rgb), GetGValue(refer), extend) && InRange(GetBValue(rgb), GetBValue(refer), extend)); }
		static bool Equal(COLORREF rgb, COLORREF _min, COLORREF _max) { return (InRange(GetRValue(rgb), GetRValue(_min), GetRValue(_max), (BYTE)0) && InRange(GetGValue(rgb), GetGValue(_min), GetGValue(_max), (BYTE)0) && InRange(GetBValue(rgb), GetBValue(_min), GetBValue(_max), (BYTE)0)); }
		// POINT: x/y >= 0;
		static bool Find(const RgbMap& map, POINT pt, const Rgb& rgb, byte extend = 10) { if (pt.x < 0 || pt.y < 0) return 0; if (pt.x >= map.width()) pt.x = map.width() - 1; if (pt.y >= map.width()) pt.y = map.width() - 1; return Equal(map.at(pt.y, pt.x), rgb, extend); }
		// POINT: x/y >= 0;
		static bool Find(HDC hdc, POINT pt, COLORREF color, byte extend = 10) { if (pt.x < 0 || pt.y < 0) return 0; return Equal(color, GetPixel(hdc, pt.x, pt.y), extend); }

		static Result FindOr(const RgbMap& map, Rgb rgb, byte extend = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX }) {
			if (map.empty()) return {};
			rect.left = std::clamp((int)rect.left, 0, (int)map.width());
			rect.right = std::clamp((int)rect.right, 0, (int)map.width());
			rect.top = std::clamp((int)rect.top, 0, (int)map.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)map.height());
			for (size_t y = rect.left; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (Equal(map.at(x, y), rgb, extend)) return POINT{ (LONG)x, (LONG)y };
			return {};
		}
		static Result FindOr(const RgbMap& map, Rgb min, Rgb max, RECT rect = { 0, 0, LONG_MAX, LONG_MAX }) {
			if (map.empty()) return {};
			rect.left = std::clamp((int)rect.left, 0, (int)map.width());
			rect.right = std::clamp((int)rect.right, 0, (int)map.width());
			rect.top = std::clamp((int)rect.top, 0, (int)map.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)map.height());
			for (size_t y = rect.left; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (Equal(map.at(x, y), min, max)) return POINT{ (LONG)x, (LONG)y };
			return {};
		}
		static Result FindOr(const RgbaMap& rgbaMap, Rgba rgb, byte extend = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX }) {
			if (rgbaMap.empty()) return {};
			rect.left = std::clamp((int)rect.left, 0, (int)rgbaMap.width() - 1);
			rect.right = std::clamp((int)rect.right, 0, (int)rgbaMap.width() - 1);
			rect.top = std::clamp((int)rect.top, 0, (int)rgbaMap.height() - 1);
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)rgbaMap.height() - 1);
			for (size_t y = rect.left; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (Equal(rgbaMap.at(x, y), rgb, extend)) return POINT{ (LONG)x, (LONG)y };
			return {};
		}
		static Result FindOr(const RgbaMap& rgbaMap, Rgba min, Rgba max, RECT rect = { 0, 0, LONG_MAX, LONG_MAX }) {
			if (rgbaMap.empty()) return {};
			rect.left = std::clamp((int)rect.left, 0, (int)rgbaMap.width());
			rect.right = std::clamp((int)rect.right, 0, (int)rgbaMap.width());
			rect.top = std::clamp((int)rect.top, 0, (int)rgbaMap.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)rgbaMap.height());
			for (size_t y = rect.left; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (Equal(rgbaMap.at(x, y), min, max)) return POINT{ (LONG)x, (LONG)y };
			return {};
		}
		static bool FindAnd(const RgbMap& map, Rgb rgb, byte extend = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (map.empty()) return false;
			rect.left = std::clamp((int)rect.left, 0, (int)map.width());
			rect.right = std::clamp((int)rect.right, 0, (int)map.width());
			rect.top = std::clamp((int)rect.top, 0, (int)map.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)map.height());
			for (size_t y = rect.left; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (!Equal(map.at(x, y), rgb, extend)) return { 0 };
			return true;
		}
		static bool FindAnd(const RgbMap& map, Rgb min, Rgb max, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (map.empty()) return false;
			rect.left = std::clamp((int)rect.left, 0, (int)map.width());
			rect.right = std::clamp((int)rect.right, 0, (int)map.width());
			rect.top = std::clamp((int)rect.top, 0, (int)map.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)map.height());
			for (size_t y = rect.left; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (!Equal(map.at(x, y), min, max)) return { 0 };
			return true;
		}
		static bool FindAnd(const RgbaMap& rgbaMap, Rgba rgb, byte extend = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (rgbaMap.empty()) return false;
			rect.left = std::clamp((int)rect.left, 0, (int)rgbaMap.width());
			rect.right = std::clamp((int)rect.right, 0, (int)rgbaMap.width());
			rect.top = std::clamp((int)rect.top, 0, (int)rgbaMap.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)rgbaMap.height());
			for (size_t y = rect.left; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (!Equal(rgbaMap.at(x, y), rgb, extend)) return { 0 };
			return true;
		}
		static bool FindAnd(const RgbaMap& rgbaMap, Rgba min, Rgba max, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (rgbaMap.empty()) return false;
			rect.left = std::clamp((int)rect.left, 0, (int)rgbaMap.width());
			rect.right = std::clamp((int)rect.right, 0, (int)rgbaMap.width());
			rect.top = std::clamp((int)rect.top, 0, (int)rgbaMap.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)rgbaMap.height());
			for (size_t y = rect.left; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (!Equal(rgbaMap.at(x, y), min, max)) return { 0 };
			return true;
		}
	};
}