#pragma once
#include "inc_header.h"

namespace QiCvt
{
    constexpr float ABS_SIZEF = 1.0F;
    constexpr LONG ABS_SIZE = 10000L;
    constexpr POINT FULL_RANGEP = { 0L, ABS_SIZE };
    constexpr RECT FULL_RANGER = { 0L, 0L, ABS_SIZE, ABS_SIZE };
    constexpr RECT FULL_ABS_RANGE = { 0L, 0L, ABS_SIZE, ABS_SIZE };
    constexpr RECTF FULL_ABSF_RANGE = { 1.0F,1.0F };

    constexpr POINT Clamp(const POINT& point, const RECT& range) { return { std::clamp(point.x, range.left, range.right), std::clamp(point.y, range.top, range.bottom) }; }
    constexpr POINTF Clamp(const POINTF& point, const RECTF& range) { return { std::clamp(point.x, range.left, range.right), std::clamp(point.y, range.top, range.bottom) }; }
    constexpr RECT Clamp(const RECT& rect, const RECT& range) { return { std::clamp(rect.left, range.left, range.right), std::clamp(rect.top, range.top, range.bottom), std::clamp(rect.right, range.left, range.right), std::clamp(rect.bottom, range.top, range.bottom) }; }
    constexpr RECTF Clamp(const RECTF& rect, const RECTF& range) { return { std::clamp(rect.left, range.left, range.right), std::clamp(rect.top, range.top, range.bottom), std::clamp(rect.right, range.left, range.right), std::clamp(rect.bottom, range.top, range.bottom) }; }

    constexpr int Scale(int abs, int abs_range1 = 0, int abs_range2 = ABS_SIZE) { return ConvertRange<int, int>(abs, 0, ABS_SIZE, abs_range1, abs_range2); }
    constexpr POINT Scale(const POINT& abs, const RECT& range) { return { Scale(abs.x, range.left, range.right), Scale(abs.y, range.top, range.bottom) }; }

    constexpr int Af_A(float abs_f) { return ConvertRange<float, int>(abs_f, 0, 1.0f, 0, ABS_SIZE); }
    constexpr float A_Af(int abs) { return ConvertRange<float, int>(abs, 0, ABS_SIZE, 0, 1.0f); }

    constexpr POINT P_Af_A(const POINTF& abs_f) { return { Af_A(abs_f.x), Af_A(abs_f.y) }; }
    constexpr POINTF P_A_Af(const POINT& abs) { return { A_Af(abs.x), A_Af(abs.y) }; }
    constexpr RECT R_Af_A(const RECTF& abs_f) { return { Af_A(abs_f.left), Af_A(abs_f.top), Af_A(abs_f.right), Af_A(abs_f.bottom) }; }
    constexpr RECTF R_A_Af(const RECT& abs) { return { A_Af(abs.left), A_Af(abs.top), A_Af(abs.right), A_Af(abs.bottom) }; }

    constexpr int R_A(int rel, int rel_range1, int rel_range2, int abs_range1 = 0, int abs_range2 = ABS_SIZE)
    {
        const int sub_range1 = ConvertRange<int, int>(abs_range1, 0, ABS_SIZE, rel_range1, rel_range2);
        const int sub_range2 = ConvertRange<int, int>(abs_range2, 0, ABS_SIZE, rel_range1, rel_range2);
        const int abs = ConvertRange<int, int>(rel, sub_range1, sub_range2, 0, ABS_SIZE);
        return abs;
    }
    constexpr float R_Af(int rel, int rel_range1, int rel_range2, int abs_range1 = 0, int abs_range2 = ABS_SIZE)
    {
        const int sub_range1 = ConvertRange<int, int>(abs_range1, 0, ABS_SIZE, rel_range1, rel_range2);
        const int sub_range2 = ConvertRange<int, int>(abs_range2, 0, ABS_SIZE, rel_range1, rel_range2);
        const int abs = ConvertRange<int, float>(rel, sub_range1, sub_range2, 0, 1.0f);
        return abs;
    }
    constexpr int A_R(int abs, int rel_range1, int rel_range2, int abs_range1 = 0, int abs_range2 = ABS_SIZE)
    {
        const int sub_range1 = ConvertRange<int, int>(abs_range1, 0, ABS_SIZE, rel_range1, rel_range2);
        const int sub_range2 = ConvertRange<int, int>(abs_range2, 0, ABS_SIZE, rel_range1, rel_range2);
        const int rel = ConvertRange<int, int>(abs, 0, ABS_SIZE, sub_range1, sub_range2);
        return rel;
    }
    constexpr int Af_R(int abs, int rel_range1, int rel_range2, int abs_range1 = 0, int abs_range2 = ABS_SIZE)
    {
        const int sub_range1 = ConvertRange<int, int>(abs_range1, 0, ABS_SIZE, rel_range1, rel_range2);
        const int sub_range2 = ConvertRange<int, int>(abs_range2, 0, ABS_SIZE, rel_range1, rel_range2);
        const int rel = ConvertRange<int, int>(abs, 0, 1.0f, sub_range1, sub_range2);
        return rel;
    }

    // base
    constexpr POINT P_R_A(const POINT& rel, const RECT& rel_range, const RECT& abs_range = FULL_RANGER)
    {
        return Clamp(POINT{
            R_A(rel.x, rel_range.left, rel_range.right, abs_range.left, abs_range.right),
            R_A(rel.y, rel_range.top, rel_range.bottom, abs_range.top, abs_range.bottom)
            }, FULL_ABS_RANGE);
    }
    constexpr POINTF Pf_R_A(const POINT& rel, const RECT& rel_range, const RECT& abs_range = FULL_RANGER)
    {
        return Clamp(POINTF{
            R_Af(rel.x, rel_range.left, rel_range.right, abs_range.left, abs_range.right),
            R_Af(rel.y, rel_range.top, rel_range.bottom, abs_range.top, abs_range.bottom)
            }, FULL_ABSF_RANGE);
    }
    constexpr RECT R_R_A(const RECT& rel, const RECT& rel_range, const RECT& abs_range = FULL_RANGER)
    {
        return Clamp(RECT{
            R_A(rel.left, rel_range.left, rel_range.right, abs_range.left, abs_range.right),
            R_A(rel.top, rel_range.top, rel_range.bottom, abs_range.top, abs_range.bottom),
            R_A(rel.right, rel_range.left, rel_range.right, abs_range.left, abs_range.right),
            R_A(rel.bottom, rel_range.top, rel_range.bottom, abs_range.top, abs_range.bottom)
            }, FULL_ABS_RANGE);
    }
    constexpr RECTF Rf_R_A(const RECT& rel, const RECT& rel_range, const RECT& abs_range = FULL_RANGER)
    {
        return Clamp(RECTF{
            R_Af(rel.left, rel_range.left, rel_range.right, abs_range.left, abs_range.right),
            R_Af(rel.top, rel_range.top, rel_range.bottom, abs_range.top, abs_range.bottom),
            R_Af(rel.right, rel_range.left, rel_range.right, abs_range.left, abs_range.right),
            R_Af(rel.bottom, rel_range.top, rel_range.bottom, abs_range.top, abs_range.bottom)
            }, FULL_ABSF_RANGE);
    }

    constexpr POINT P_A_R(const POINT& abs, const RECT& rel_range, const RECT& abs_range = FULL_RANGER)
    {
        return Clamp(POINT({
            A_R(abs.x, rel_range.left, rel_range.right, abs_range.left, abs_range.right),
            A_R(abs.y, rel_range.top, rel_range.bottom, abs_range.top, abs_range.bottom)
            }), rel_range);
    }
    constexpr POINT Pf_A_R(const POINTF& abs, const RECT& rel_range, const RECT& abs_range = FULL_RANGER)
    {
        return Clamp(POINT({
            Af_R(abs.x, rel_range.left, rel_range.right, abs_range.left, abs_range.right),
            Af_R(abs.y, rel_range.top, rel_range.bottom, abs_range.top, abs_range.bottom)
            }), rel_range);
    }
    constexpr RECT R_A_R(const RECT& abs, const RECT& rel_range, const RECT& abs_range = FULL_RANGER)
    {
        return Clamp(RECT({
            A_R(abs.left, rel_range.left, rel_range.right, abs_range.left, abs_range.right),
            A_R(abs.top, rel_range.top, rel_range.bottom, abs_range.top, abs_range.bottom),
            A_R(abs.right, rel_range.left, rel_range.right, abs_range.left, abs_range.right),
            A_R(abs.bottom, rel_range.top, rel_range.bottom, abs_range.top, abs_range.bottom)
            }), rel_range);
    }
    constexpr RECT Rf_A_R(const RECTF& abs, const RECT& rel_range, const RECT& abs_range = FULL_RANGER)
    {
        return Clamp(RECT({
            Af_R(abs.left, rel_range.left, rel_range.right, abs_range.left, abs_range.right),
            Af_R(abs.top, rel_range.top, rel_range.bottom, abs_range.top, abs_range.bottom),
            Af_R(abs.right, rel_range.left, rel_range.right, abs_range.left, abs_range.right),
            Af_R(abs.bottom, rel_range.top, rel_range.bottom, abs_range.top, abs_range.bottom)
            }), rel_range);
    }

    // screen
    constexpr POINT SP_R_A(const POINT& rel, const RECT& abs_range = FULL_RANGER) { return P_R_A(rel, Qi::set.allScreen ? Qi::virtualScreen : Qi::primaryScreen, abs_range); }
    constexpr POINTF SPf_R_A(const POINT& abs, const RECT& abs_range = FULL_RANGER) { return Pf_R_A(abs, Qi::set.allScreen ? Qi::virtualScreen : Qi::primaryScreen, abs_range); }
    constexpr RECT SR_R_A(const RECT& rel, const RECT& abs_range = FULL_RANGER) { return R_R_A(rel, Qi::set.allScreen ? Qi::virtualScreen : Qi::primaryScreen, abs_range); }
    constexpr RECTF SRf_R_A(const RECT& rel, const RECT& abs_range = FULL_RANGER) { return Rf_R_A(rel, Qi::set.allScreen ? Qi::virtualScreen : Qi::primaryScreen, abs_range); }

    constexpr POINT SP_A_R(const POINT& abs, const RECT& abs_range = FULL_RANGER) { return P_A_R(abs, Qi::set.allScreen ? Qi::virtualScreen : Qi::primaryScreen, abs_range); }
    constexpr POINT SPf_A_R(const POINTF& abs, const RECT& abs_range = FULL_RANGER) { return Pf_A_R(abs, Qi::set.allScreen ? Qi::virtualScreen : Qi::primaryScreen, abs_range); }
    constexpr RECT SR_A_R(const RECT& abs, const RECT& abs_range = FULL_RANGER) { return R_A_R(abs, Qi::set.allScreen ? Qi::virtualScreen : Qi::primaryScreen, abs_range); }
    constexpr RECT SRf_A_R(const RECTF& abs, const RECT& abs_range = FULL_RANGER) { return Rf_A_R(abs, Qi::set.allScreen ? Qi::virtualScreen : Qi::primaryScreen, abs_range); }

    // window
    constexpr POINT WP_R_A(const POINT& rel, const HWND& wnd, const RECT& abs_range = FULL_RANGER) { return P_R_A(rel, Window::sizeRect(wnd), abs_range); }
    constexpr POINTF WPf_R_A(const POINT& rel, const HWND& wnd, const RECT& abs_range = FULL_RANGER) { return Pf_R_A(rel, Window::sizeRect(wnd), abs_range); }
    constexpr RECT WR_R_A(const RECT& rel, const HWND& wnd, const RECT& abs_range = FULL_RANGER) { return R_R_A(rel, Window::sizeRect(wnd), abs_range); }
    constexpr RECTF WRf_R_A(const RECT& rel, const HWND& wnd, const RECT& abs_range = FULL_RANGER) { return Rf_R_A(rel, Window::sizeRect(wnd), abs_range); }
    
    constexpr POINT WP_A_R(const POINT& abs, const HWND& wnd, const RECT& abs_range = FULL_RANGER) { return P_A_R(abs, Window::sizeRect(wnd), abs_range); }
    constexpr POINT WPf_A_R(const POINTF& abs, const HWND& wnd, const RECT& abs_range = FULL_RANGER) { return Pf_A_R(abs, Window::sizeRect(wnd), abs_range); }
    constexpr RECT WR_A_R(const RECT& abs, const HWND& wnd, const RECT& abs_range = FULL_RANGER) { return R_A_R(abs, Window::sizeRect(wnd), abs_range); }
    constexpr RECT WRf_A_R(const RECTF& abs, const HWND& wnd, const RECT& abs_range = FULL_RANGER) { return Rf_A_R(abs, Window::sizeRect(wnd), abs_range); }
}