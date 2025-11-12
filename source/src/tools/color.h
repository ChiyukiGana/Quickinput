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
		bool equal(const Rgb& rgb, const byte& extend = 10) const { return (InRange<short>(r, rgb.r, extend) && InRange<short>(g, rgb.g, extend) && InRange<short>(b, rgb.b, extend)); }
		bool equal(const Rgb& rgb_min, const Rgb& rgb_max, const byte& extend = 0) const { return (InRange<short>(r, rgb_min.r, rgb_max.r, extend) && InRange<short>(g, rgb_min.g, rgb_max.g, extend) && InRange<short>(b, rgb_min.b, rgb_max.b, extend)); }
		bool equal(const COLORREF& rgb, const byte& extend = 10) const { return (InRange<short>(r, GetRValue(rgb), extend) && InRange<short>(g, GetGValue(rgb), extend) && InRange<short>(b, GetBValue(rgb), extend)); }
		bool equal(const COLORREF& rgb_min, const COLORREF& rgb_max, const byte& extend = 10) const { return (InRange<short>(r, GetRValue(rgb_min), GetRValue(rgb_max), extend) && InRange<short>(g, GetGValue(rgb_min), GetGValue(rgb_max), extend) && InRange<short>(b, GetBValue(rgb_min), GetBValue(rgb_max), extend)); }
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
		bool equal(const Rgba& rgba, const byte& extend = 10) const { return (InRange<short>(r, rgba.r, extend) && InRange<short>(g, rgba.g, extend) && InRange<short>(b, rgba.b, extend) && InRange<short>(a, rgba.a, extend)); }
		bool equal(const Rgba& rgba_min, const Rgba& rgba_max, const byte& extend = 0) const { return (InRange<short>(r, rgba_min.r, rgba_max.r, extend) && InRange<short>(g, rgba_min.g, rgba_max.g, extend) && InRange<short>(b, rgba_min.b, rgba_max.b, extend) && InRange<short>(a, rgba_min.a, rgba_max.a, extend)); }
		bool equal(const COLORREF& rgba, const byte& extend = 10) const { return (InRange<short>(r, GetRValue(rgba), extend) && InRange<short>(g, GetRValue(rgba), extend) && InRange<short>(b, GetBValue(rgba), extend) && InRange<short>(a, GetAValue(rgba), extend)); }
		bool equal(const COLORREF& rgba_min, const COLORREF& rgba_max, const byte& extend = 10) const { return (InRange<short>(r, GetRValue(rgba_min), GetRValue(rgba_max), extend) && InRange<short>(g, GetGValue(rgba_min), GetGValue(rgba_max), extend) && InRange<short>(a, GetBValue(rgba_min), GetBValue(rgba_max), extend) && InRange<short>(b, GetAValue(rgba_min), GetAValue(rgba_max), extend)); }
		Rgb toRgb() const { return Rgb(r, g, b); }
		COLORREF toCOLORREF() const { return RGBA(r, g, b, a); }
	};
	using RgbMap = map2d<Rgb>;
	using RgbaMap = map2d<Rgba>;
	class Color
	{
	public:
		using Result = std::optional<POINT>;
		static constexpr LONG npos = LONG_MAX;
		static constexpr RECT full = { 0, 0, npos, npos };

		static bool Find(const RgbMap& map, POINT pt, const Rgb& rgb, byte extend = 10) { if (pt.x < 0 || pt.y < 0) return 0; if (pt.x >= map.width()) pt.x = map.width() - 1; if (pt.y >= map.width()) pt.y = map.width() - 1; return map.at(pt.y, pt.x).equal(rgb, extend); }
		static bool Find(HDC hdc, POINT pt, COLORREF color, byte extend = 10) { if (pt.x < 0 || pt.y < 0) return 0; return Rgb(GetPixel(hdc, pt.x, pt.y)).equal(color, extend); }

		static Result FindOr(const RgbMap& map, Rgb rgb, byte extend = 10, RECT rect = full) {
			if (map.empty()) return {};
			rect.left = std::clamp((int)rect.left, 0, (int)map.width());
			rect.right = std::clamp((int)rect.right, 0, (int)map.width());
			rect.top = std::clamp((int)rect.top, 0, (int)map.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)map.height());
			for (size_t y = rect.top; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (map.at(x, y).equal(rgb, extend)) return POINT{ (LONG)x, (LONG)y };
			return {};
		}
		static Result FindOr(const RgbMap& map, Rgb min, Rgb max, byte extend = 0, RECT rect = full) {
			if (map.empty()) return {};
			rect.left = std::clamp((int)rect.left, 0, (int)map.width());
			rect.right = std::clamp((int)rect.right, 0, (int)map.width());
			rect.top = std::clamp((int)rect.top, 0, (int)map.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)map.height());
			for (size_t y = rect.top; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (map.at(x, y).equal(min, max, extend)) return POINT{ (LONG)x, (LONG)y };
			return {};
		}
		static Result FindOr(const RgbaMap& map, Rgba rgb, byte extend = 10, RECT rect = full) {
			if (map.empty()) return {};
			rect.left = std::clamp((int)rect.left, 0, (int)map.width());
			rect.right = std::clamp((int)rect.right, 0, (int)map.width());
			rect.top = std::clamp((int)rect.top, 0, (int)map.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)map.height());
			for (size_t y = rect.top; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (map.at(x, y).equal(rgb, extend)) return POINT{ (LONG)x, (LONG)y };
			return {};
		}
		static Result FindOr(const RgbaMap& map, Rgba min, Rgba max, byte extend = 0, RECT rect = full) {
			if (map.empty()) return {};
			rect.left = std::clamp((int)rect.left, 0, (int)map.width());
			rect.right = std::clamp((int)rect.right, 0, (int)map.width());
			rect.top = std::clamp((int)rect.top, 0, (int)map.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)map.height());
			for (size_t y = rect.top; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (map.at(x, y).equal(min, max, extend)) return POINT{ (LONG)x, (LONG)y };
			return {};
		}
		static bool FindAnd(const RgbMap& map, Rgb rgb, byte extend = 10, RECT rect = full)
		{
			if (map.empty()) return false;
			rect.left = std::clamp((int)rect.left, 0, (int)map.width());
			rect.right = std::clamp((int)rect.right, 0, (int)map.width());
			rect.top = std::clamp((int)rect.top, 0, (int)map.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)map.height());
			for (size_t y = rect.top; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (!map.at(x, y).equal(rgb, extend)) return false;
			return true;
		}
		static bool FindAnd(const RgbMap& map, Rgb min, Rgb max, byte extend = 0, RECT rect = full)
		{
			if (map.empty()) return false;
			rect.left = std::clamp((int)rect.left, 0, (int)map.width());
			rect.right = std::clamp((int)rect.right, 0, (int)map.width());
			rect.top = std::clamp((int)rect.top, 0, (int)map.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)map.height());
			for (size_t y = rect.top; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (!map.at(x, y).equal(min, max, extend)) return false;
			return true;
		}
		static bool FindAnd(const RgbaMap& map, Rgba rgb, byte extend = 10, RECT rect = full)
		{
			if (map.empty()) return false;
			rect.left = std::clamp((int)rect.left, 0, (int)map.width());
			rect.right = std::clamp((int)rect.right, 0, (int)map.width());
			rect.top = std::clamp((int)rect.top, 0, (int)map.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)map.height());
			for (size_t y = rect.top; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (!map.at(x, y).equal(rgb, extend)) return false;
			return true;
		}
		static bool FindAnd(const RgbaMap& map, Rgba min, Rgba max, byte extend = 0, RECT rect = full)
		{
			if (map.empty()) return false;
			rect.left = std::clamp((int)rect.left, 0, (int)map.width());
			rect.right = std::clamp((int)rect.right, 0, (int)map.width());
			rect.top = std::clamp((int)rect.top, 0, (int)map.height());
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)map.height());
			for (size_t y = rect.top; y < rect.bottom; y++) for (size_t x = rect.left; x < rect.right; x++) if (!map.at(x, y).equal(min, max, extend)) return false;
			return true;
		}
	};
}