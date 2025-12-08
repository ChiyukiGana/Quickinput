#pragma once
#include "inc_header.h"

namespace QiCvt
{
    constexpr float ABS_SIZEF = 10000.0f;
    constexpr LONG ABS_SIZE = ABS_SIZEF;
    constexpr POINT FULL_RANGEP = { 0L,ABS_SIZE };
    constexpr RECT FULL_RANGER = { 0L,0L,ABS_SIZE,ABS_SIZE };
    constexpr SIZE FULL_SIZE = { ABS_SIZE,ABS_SIZE };
    constexpr SIZEF FULL_SIZEF = { 1.0F,1.0F };

    static int FtI(float abs) { return static_cast<int>(abs * ABS_SIZEF); };
    static float ItF(int abs) { return static_cast<float>(abs) / ABS_SIZEF; };

    static POINT P_FtI(const POINTF& abs) { return { FtI(abs.x), FtI(abs.y) }; }
    static POINTF P_ItF(const POINT& abs) { return { ItF(abs.x), ItF(abs.y) }; }
    static RECT R_FtI(const RECTF& abs) { return { FtI(abs.left), FtI(abs.top), FtI(abs.right), FtI(abs.bottom) }; }
    static RECTF R_ItF(const RECT& abs) { return { ItF(abs.left), ItF(abs.top), ItF(abs.right), ItF(abs.bottom) }; }

    static int Clip(int abs, const POINT& range) { return range.x + static_cast<float>(abs * static_cast<float>(range.y - range.x) / ABS_SIZEF); }
    static POINT Clip(const POINT& abs, const RECT& range) { return { Clip(abs.x, {range.left,range.right}), Clip(abs.y, {range.top,range.bottom}) }; }

	static POINT Clamp(const POINT& point, const SIZE& size) { return { std::clamp(point.x, 0L, size.cx), std::clamp(point.y, 0L, size.cy) }; }
	static POINTF Clamp(const POINTF& point, const SIZEF& size) { return { std::clamp(point.x, 0.0f, size.cx), std::clamp(point.y, 0.0f, size.cy) }; }
	static RECT Clamp(const RECT& rect, const SIZE& size) { return { std::clamp(rect.left, 0L, size.cx), std::clamp(rect.top, 0L, size.cy), std::clamp(rect.right, 0L, size.cx), std::clamp(rect.bottom, 0L, size.cy) }; }
	static RECTF Clamp(const RECTF& rect, const SIZEF& size) { return { std::clamp(rect.left, 0.0f, size.cx), std::clamp(rect.top, 0.0f, size.cy), std::clamp(rect.right, 0.0f, size.cx), std::clamp(rect.bottom, 0.0f, size.cy) }; }

    static int RtA(int rel, int size, const POINT& range = FULL_RANGEP)
    {
        if (rel <= 0 || size <= 0 || range.y - range.x <= 0) return 0;
        float left = static_cast<float>(range.x) / ABS_SIZEF * static_cast<float>(size - 1);
        float right = static_cast<float>(range.y) / ABS_SIZEF * static_cast<float>(size - 1);
        return (static_cast<float>(std::clamp<float>(rel, left, right)) - left) * (ABS_SIZEF / (right - left));
    }
    static float fRtA(int rel, int size, const POINT& range = FULL_RANGEP)
    {
        if (rel <= 0 || size <= 0 || range.y - range.x <= 0) return 0.0f;
        float left = static_cast<float>(range.x) / ABS_SIZEF * static_cast<float>(size - 1);
        float right = static_cast<float>(range.y) / ABS_SIZEF * static_cast<float>(size - 1);
        return (static_cast<float>(std::clamp<float>(rel, left, right)) - left) * (ABS_SIZEF / (right - left)) / ABS_SIZEF;
    }
    static int AtR(int abs, int size, const POINT& range = FULL_RANGEP)
    {
        if (range.y - range.x < 0) return 0;
        return static_cast<int>(static_cast<float>(size) / ABS_SIZEF * static_cast<float>(range.x + static_cast<int>(static_cast<float>(abs) * (static_cast<float>(range.y - range.x) / ABS_SIZEF))));
    }
    static int fAtR(float abs, int size, const POINT& range = FULL_RANGEP)
    {
        float absScaled = abs * ABS_SIZEF;
        if (range.y - range.x < 0) return 0;
        return static_cast<int>(static_cast<float>(size) / ABS_SIZEF * static_cast<float>(range.x + static_cast<int>(static_cast<float>(absScaled) * (static_cast<float>(range.y - range.x) / ABS_SIZEF))));
    }
    static int RtAnClip(int rel, int size) { return ABS_SIZEF / static_cast<float>(size) * static_cast<float>(rel); }
    static int AtRnClip(int abs, int size) { return static_cast<float>(size) / ABS_SIZEF * static_cast<float>(abs); }

    static POINT P_RtA(const POINT& rel, const SIZE& size, const RECT& range = FULL_RANGER) { return Clamp(POINT{ RtA(rel.x, size.cx, {range.left,range.right}), RtA(rel.y, size.cy, {range.top,range.bottom}) }, FULL_SIZE); }
    static POINTF PF_RtA(const POINT& rel, const SIZE& size, const RECT& range = FULL_RANGER) { return Clamp(POINTF{ fRtA(rel.x, size.cx, {range.left,range.right}), fRtA(rel.y, size.cy, {range.top,range.bottom}) }, FULL_SIZEF); }
    static RECT R_RtA(const RECT& rel, const SIZE& size, const RECT& range = FULL_RANGER)
    {
        return Clamp(RECT{
            RtA(rel.left, size.cx, {range.left, range.right}),
            RtA(rel.top, size.cy, {range.top, range.bottom}),
            RtA(rel.right, size.cx, {range.left, range.right}),
            RtA(rel.bottom, size.cy, {range.top, range.bottom})
            }, FULL_SIZE);
    }
    static RECTF RF_RtA(const RECT& rel, const SIZE& size, const RECT& range = FULL_RANGER)
    {
        return Clamp(RECTF{
            fRtA(rel.left, size.cx, {range.left, range.right}),
            fRtA(rel.top, size.cy, {range.top, range.bottom}),
            fRtA(rel.right, size.cx, {range.left, range.right}),
            fRtA(rel.bottom, size.cy, {range.top, range.bottom})
            }, SIZEF{ 1.0f,1.0f });
    }
    static RECT R_RtAnClip(const RECT& rel, const SIZE& size) { return { RtAnClip(rel.left, size.cx), RtAnClip(rel.top, size.cy), RtAnClip(rel.right, size.cx), RtAnClip(rel.bottom, size.cy) }; }

    static POINT P_AtR(const POINT& abs, const SIZE& size, const RECT& range = FULL_RANGER)
    {
        return Clamp(
            POINT({
                AtR(abs.x, size.cx, {range.left, range.right}),
                AtR(abs.y, size.cy, {range.top, range.bottom})
                }),
            size
        );
    }
    static POINT PF_AtR(const POINTF& abs, const SIZE& size, const RECT& range = FULL_RANGER)
    {
        return Clamp(
            POINT({
                fAtR(abs.x, size.cx, {range.left, range.right}),
                fAtR(abs.y, size.cy, {range.top, range.bottom})
                }),
            size
        );
    }
    static RECT R_AtR(const RECT& abs, const SIZE& size, const RECT& range = FULL_RANGER)
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
    static RECT RF_AtR(const RECTF& abs, const SIZE& size, const RECT& range = FULL_RANGER)
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
    static POINT SP_RtA(const POINT& rel, const RECT& range = FULL_RANGER) { return P_RtA(rel, Qi::screen, range); }
    static POINTF SPF_RtA(const POINT& abs, const RECT& range = FULL_RANGER) { return PF_RtA(abs, Qi::screen, range); }
    static RECT SR_RtA(const RECT& rel, const RECT& range = FULL_RANGER) { return R_RtA(rel, Qi::screen, range); }
    static RECTF SRF_RtA(const RECT& rel, const RECT& range = FULL_RANGER) { return RF_RtA(rel, Qi::screen, range); }
    static RECT SR_RtAnClip(const RECT& rel) { return R_RtAnClip(rel, Qi::screen); }

    static POINT SP_AtR(const POINT& abs, const RECT& range = FULL_RANGER) { return P_AtR(abs, Qi::screen, range); }
    static POINT SPF_AtR(const POINTF& abs, const RECT& range = FULL_RANGER) { return PF_AtR(abs, Qi::screen, range); }
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