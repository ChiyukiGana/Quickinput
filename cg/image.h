#pragma once

#include <windows.h>
#include <atlimage.h>
#include <dwmapi.h> //DwmGetWindowAttribute
#pragma comment(lib, "dwmapi.lib") //DwmGetWindowAttribute

#include "file.h"
#include "color.h"
#include "system.h"
#include <iostream>
namespace CG {
	class Image
	{
	public:

		static void ScreenShot(LPCWSTR path) {
			HDC hScr = GetDC(0);
			CImage image;
			SIZE scr = System::screenSize();
			image.Create(scr.cx, scr.cy, GetDeviceCaps(hScr, BITSPIXEL));
			BitBlt(image.GetDC(), 0, 0, scr.cx, scr.cy, hScr, 0, 0, SRCCOPY);
			ReleaseDC(0, hScr);
			image.ReleaseDC();
			image.Save(path);
		}

		static bool WindowShot(LPCWSTR path) {
			CImage image;
			HDC hScr = GetDC(0);
			SIZE scr = System::screenVSize();
			HWND wnd = GetForegroundWindow();
			RECT rect = { 0 };
			DwmGetWindowAttribute(wnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));

			if (rect.left < 0) rect.left = 0;
			if (rect.top < 0) rect.top = 0;
			if (rect.right > scr.cx) rect.right = scr.cx;
			if (rect.bottom > scr.cy) rect.bottom = scr.cy;
			if (rect.right < 0) return 0;
			if (rect.bottom < 0) return 0;
			if (rect.left > scr.cx) return 0;
			if (rect.top > scr.cy) return 0;

			if (image.Create(rect.right - rect.left, rect.bottom - rect.top, GetDeviceCaps(hScr, BITSPIXEL))) {
				if (BitBlt(image.GetDC(), 0, 0, rect.right - rect.left, rect.bottom - rect.top, hScr, rect.left, rect.top, SRCCOPY)) {
					ReleaseDC(0, hScr);
					image.ReleaseDC();
					image.Save(path);
					return 1;
				}
			}
			ReleaseDC(0, hScr);
			image.ReleaseDC();
			return 0;
		}

		static bool HdcRgbmap(HDC hdc, RgbMap& rgbMap, SIZE size, POINT lt = { 0, 0 })
		{
			rgbMap.create(size.cx, size.cy);
			BITMAPINFO bitmapInfo = { 0 };
			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biWidth = rgbMap.width();
			bitmapInfo.bmiHeader.biHeight = rgbMap.height();
			bitmapInfo.bmiHeader.biPlanes = 1;
			bitmapInfo.bmiHeader.biBitCount = sizeof(Rgb) * 8;
			HBITMAP hBitmap = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, 0, 0, 0);
			if (hBitmap)
			{
				LONG result = 0;
				HDC hdcMem = CreateCompatibleDC(hdc);
				HGDIOBJ hGdiobj = SelectObject(hdcMem, hBitmap);
				if (hGdiobj && hGdiobj != HGDI_ERROR && BitBlt(hdcMem, 0, 0, rgbMap.width(), rgbMap.height(), hdc, lt.x, lt.y, SRCCOPY)) result = GetBitmapBits(hBitmap, rgbMap.bytes(), rgbMap.accessMap());
				DeleteObject(hBitmap);
				DeleteDC(hdcMem);
				return result;
			}
			return false;
		}

		static bool HdcRgbmap(HDC hdc, RgbMap& rgbMap, RECT rect) { return HdcRgbmap(hdc, rgbMap, { rect.right - rect.left, rect.bottom - rect.top }, { rect.left, rect.top }); }

		static void ScreenRgbmap(RgbMap& rgbMap)
		{
			HDC hdc = GetDC(0);
			HdcRgbmap(hdc, rgbMap, System::screenSize());
			ReleaseDC(0, hdc);
		}
	};
}