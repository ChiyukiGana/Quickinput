#pragma once
#include "window.h"
#include "color.h"
#include "image.h"
namespace CG
{
	class MapView
	{
		static inline const SIZE minSize = { 200, 150 };
		static LRESULT _stdcall RgbMapViewProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
		{
			switch (msg)
			{
			case WM_PAINT:
			{
				if (wp)
				{
					PAINTSTRUCT ps;
					HDC hdc = BeginPaint(wnd, &ps);
					HBITMAP hbmp = nullptr;
					SIZE size = {};
					if (wp == 1)
					{
						const RgbMap* map = (const RgbMap*)lp;
						size.cx = map->width(), size.cy = map->height();
						if (map->count()) hbmp = Image::toBmp32(*map, hdc);

					}
					else if (wp == 2)
					{
						const RgbaMap* map = (const RgbaMap*)lp;
						size.cx = map->width(), size.cy = map->height();
						if (map->count()) hbmp = Image::toBmp32(*map, hdc);
					}

					Window::setSize(wnd, { size.cx + 16, size.cy + 40 });

					if (hbmp)
					{
						HDC mem = CreateCompatibleDC(hdc);
						SelectObject(mem, hbmp);
						BitBlt(hdc, 0, 0, size.cx, size.cy, mem, 0, 0, SRCCOPY);
						DeleteObject(hbmp);
						DeleteDC(mem);
					}
					else TextOutW(hdc, 5, 5, L"No image", 8);
					EndPaint(wnd, &ps);
				}
				break;
			}
			case WM_CLOSE:
				PostQuitMessage(0);
				break;
			}
			return DefWindowProcW(wnd, msg, wp, lp);
		}

	public:
		static void Rgb(const RgbMap& map)
		{
			Window::Register(L"RgbMapViewClass", RgbMapViewProc, true);
			HWND wnd = Window::Create(L"RgbMapView", L"RgbMapViewClass", WS_SYSMENU | WS_VISIBLE, 0, 0, 0);
			SendMessageW(wnd, WM_PAINT, 1, (LPARAM)&map);
			MSG msg; while (GetMessageW(&msg, wnd, 0, 0) == true) DispatchMessageW(&msg);
		}
		static void Rgba(const RgbaMap& map)
		{
			Window::Register(L"RgbMapViewClass", RgbMapViewProc, true);
			HWND wnd = Window::Create(L"RgbaMapView", L"RgbMapViewClass", WS_SYSMENU | WS_VISIBLE, 0, 0, 0);
			SendMessageW(wnd, WM_PAINT, 2, (LPARAM)&map);
			MSG msg; while (GetMessageW(&msg, wnd, 0, 0) == true) DispatchMessageW(&msg);
		}
	};
}