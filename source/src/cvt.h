#pragma once
#include "inc_header.h"

namespace QiCvt
{
    static int FtI(float abs) { return static_cast<int>(abs * 10000.0f); };
    static float ItF(int abs) { return static_cast<float>(abs) / 10000.0f; };

    static POINT P_FtI(const POINTF& abs) { return { FtI(abs.x), FtI(abs.y)}; }
    static POINTF P_ItF(const POINT& abs) { return { ItF(abs.x), ItF(abs.y) }; }
    static RECT R_FtI(const RECTF& abs) { return { FtI(abs.left), FtI(abs.top), FtI(abs.right), FtI(abs.bottom) }; }
    static RECTF R_ItF(const RECT& abs) { return { ItF(abs.left), ItF(abs.top), ItF(abs.right), ItF(abs.bottom) }; }

    static int Clamp(int abs, const POINT& range = { 0,10000 })
    {
        return range.x + static_cast<float>(abs * static_cast<float>(range.y - range.x) / 10000.0f);
    }
    static POINT Clamp(const POINT& abs, const RECT& range = { 0,0,10000,10000 })
    {
        return { Clamp(abs.x, {range.left,range.right}), Clamp(abs.y, {range.top,range.bottom}) };
    }
	static POINT Clamp(const POINT& point, const SIZE& size)
	{
		POINT p(point);
		if (p.x < 0 || p.y < 0 || (size.cx < 0) || (size.cy < 0)) return { 0,0 };
		if (p.x > size.cx) p.x = size.cx;
		if (p.y > size.cy) p.y = size.cy;
		return p;
	}
	static RECT Clamp(const RECT& rect, const SIZE& size)
	{
		RECT r({ (std::min)(rect.left, rect.right), (std::min)(rect.top, rect.bottom), (std::max)(rect.left, rect.right), (std::max)(rect.top, rect.bottom) });
		if ((rect.left < 0) || (rect.top < 0) || (size.cx <= 0) || (size.cy <= 0) || (rect.right - rect.left <= 0) || (rect.bottom - rect.top <= 0)) return { 0,0,0,0 };
		if (r.left >= size.cx) r.left = size.cx - 1, r.right = size.cx;
		else if (r.right > size.cx) r.right = size.cx;
		if (r.top >= size.cy) r.top = size.cy - 1, r.bottom = size.cy;
		else if (r.bottom > size.cy) r.bottom = size.cy;
		return r;
	}

    static int RtA(int rel, int size, const POINT& range = { 0,10000 })
    {
        if (range.y - range.x < 0) return 0;
        if (size <= 1) return range.x;
        float left = static_cast<float>(range.x) / 10000.0f * static_cast<float>(size - 1);
        float right = static_cast<float>(range.y) / 10000.0f * static_cast<float>(size - 1);
        if (rel < left) return 0;
        if (rel > right) return 10000;
        return (static_cast<float>(rel) - left) * (10000.0f / (right - left));
    }
    static float fRtA(int rel, int size, const POINT& range = { 0,10000 })
    {
        if (range.y - range.x < 0) return 0;
        if (size <= 1) return range.x;
        float left = static_cast<float>(range.x) / 10000.0f * static_cast<float>(size - 1);
        float right = static_cast<float>(range.y) / 10000.0f * static_cast<float>(size - 1);
        if (rel < left) return 0;
        if (rel > right) return 10000;
        return (static_cast<float>(rel) - left) * (10000.0f / (right - left)) / 10000.0f;
    }
    static int AtR(int abs, int size, const POINT& range = { 0,10000 })
    {
        if (range.y - range.x < 0) return 0;
        return static_cast<int>(static_cast<float>(size) / 10000.0f * static_cast<float>(range.x + static_cast<int>(static_cast<float>(abs) * (static_cast<float>(range.y - range.x) / 10000.0f))));
    }
    static int fAtR(float abs, int size, const POINT& range = { 0,10000 })
    {
        float absScaled = abs * 10000.0f;
        if (range.y - range.x < 0) return 0;
        return static_cast<int>(static_cast<float>(size) / 10000.0f * static_cast<float>(range.x + static_cast<int>(static_cast<float>(absScaled) * (static_cast<float>(range.y - range.x) / 10000.0f))));
    }

    static POINT P_RtA(const POINT& rel, const SIZE& size, const RECT& range = { 0,0,10000,10000 })
    {
        POINT point = Clamp(rel, size);
        return { RtA(point.x, size.cx, {range.left,range.right}), RtA(point.y, size.cy, {range.top,range.bottom}) };
    }
    static POINTF PF_RtA(const POINT& rel, const SIZE& size, const RECT& range = { 0,0,10000,10000 })
    {
        POINT point = Clamp(rel, size);
        return { fRtA(point.x, size.cx, {range.left,range.right}), fRtA(point.y, size.cy, {range.top,range.bottom}) };
    }
    static RECT R_RtA(const RECT& rel, const SIZE& size, const RECT& range = { 0,0,10000,10000 })
    {
        RECT rect = Clamp(rel, size);
        return {
            RtA(rect.left, size.cx, {range.left, range.right}),
            RtA(rect.top, size.cy, {range.top, range.bottom}),
            RtA(rect.right, size.cx, {range.left, range.right}),
            RtA(rect.bottom, size.cy, {range.top, range.bottom})
        };
    }
    static RECTF RF_RtA(const RECT& rel, const SIZE& size, const RECT& range = { 0,0,10000,10000 })
    {
        RECT rect = Clamp(rel, size);
        return {
            fRtA(rect.left, size.cx, {range.left, range.right}),
            fRtA(rect.top, size.cy, {range.top, range.bottom}),
            fRtA(rect.right, size.cx, {range.left, range.right}),
            fRtA(rect.bottom, size.cy, {range.top, range.bottom})
        };
    }

    static POINT P_AtR(const POINT& abs, const SIZE& size, const RECT& range = { 0,0,10000,10000 })
    {
        return Clamp(
            POINT({
                AtR(abs.x, size.cx, {range.left, range.right}),
                AtR(abs.y, size.cy, {range.top, range.bottom})
                }),
            size
        );
    }
    static POINT PF_AtR(const POINTF& abs, const SIZE& size, const RECT& range = { 0,0,10000,10000 })
    {
        return Clamp(
            POINT({
                fAtR(abs.x, size.cx, {range.left, range.right}),
                fAtR(abs.y, size.cy, {range.top, range.bottom})
                }),
            size
        );
    }
    static RECT R_AtR(const RECT& abs, const SIZE& size, const RECT& range = { 0,0,10000,10000 })
    {
        return Clamp(
            RECT({
                AtR(abs.left, size.cx, {range.left, range.right}),
                AtR(abs.top, size.cy, {range.top, range.bottom}),
                AtR(abs.right, size.cx, {range.left, range.right}),
                AtR(abs.bottom, size.cy, {range.top, range.bottom})
                }),
            size
        );
    }
    static RECT RF_AtR(const RECTF& abs, const SIZE& size, const RECT& range = { 0,0,10000,10000 })
    {
        return Clamp(
            RECT({
                fAtR(abs.left, size.cx, {range.left, range.right}),
                fAtR(abs.top, size.cy, {range.top, range.bottom}),
                fAtR(abs.right, size.cx, {range.left, range.right}),
                fAtR(abs.bottom, size.cy, {range.top, range.bottom})
                }),
            size
        );
    }

    // screen
    static POINT SP_RtA(const POINT& rel, const RECT& range = { 0,0,10000,10000 }) { return P_RtA(rel, Qi::screen, range); }
    static POINTF SPF_RtA(const POINT& abs, const RECT& range = { 0,0,10000,10000 }) { return PF_RtA(abs, Qi::screen, range); }
    static RECT SR_RtA(const RECT& rel, const RECT& range = { 0,0,10000,10000 }) { return R_RtA(rel, Qi::screen, range); }
    static RECTF SRF_RtA(const RECT& rel, const RECT& range = { 0,0,10000,10000 }) { return RF_RtA(rel, Qi::screen, range); }
    
    static POINT SP_AtR(const POINT& abs, const RECT& range = { 0,0,10000,10000 }) { return P_AtR(abs, Qi::screen, range); }
    static POINT SPF_AtR(const POINTF& abs, const RECT& range = { 0,0,10000,10000 }) { return PF_AtR(abs, Qi::screen, range); }
    static RECT SR_AtR(const RECT& abs, const RECT& range = { 0,0,10000,10000 }) { return R_AtR(abs, Qi::screen, range); }
    static RECT SRF_AtR(const RECTF& abs, const RECT& range = { 0,0,10000,10000 }) { return RF_AtR(abs, Qi::screen, range); }

    // window
    static POINT WP_RtA(const POINT& rel, const HWND& wnd, const RECT& range = { 0,0,10000,10000 }) { return P_RtA(rel, Window::size(wnd), range); }
    static POINTF WPF_RtA(const POINT& rel, const HWND& wnd, const RECT& range = { 0,0,10000,10000 }) { return PF_RtA(rel, Window::size(wnd), range); }
    static RECT WR_RtA(const RECT& rel, const HWND& wnd, const RECT& range = { 0,0,10000,10000 }) { return R_RtA(rel, Window::size(wnd), range); }
    static RECTF WRF_RtA(const RECT& rel, const HWND& wnd, const RECT& range = { 0,0,10000,10000 }) { return RF_RtA(rel, Window::size(wnd), range); }
    
    static POINT WP_AtR(const POINT& abs, const HWND& wnd, const RECT& range = { 0,0,10000,10000 }) { return P_AtR(abs, Window::size(wnd), range); }
    static POINT WPF_AtR(const POINTF& abs, const HWND& wnd, const RECT& range = { 0,0,10000,10000 }) { return PF_AtR(abs, Window::size(wnd), range); }
    static RECT WR_AtR(const RECT& abs, const HWND& wnd, const RECT& range = { 0,0,10000,10000 }) { return R_AtR(abs, Window::size(wnd), range); }
    static RECT WRF_AtR(const RECTF& abs, const HWND& wnd, const RECT& range = { 0,0,10000,10000 }) { return RF_AtR(abs, Window::size(wnd), range); }
}