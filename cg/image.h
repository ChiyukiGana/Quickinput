#pragma once

#include <windows.h>
#include <atlimage.h>
#include <dwmapi.h> //DwmGetWindowAttribute
#pragma comment(lib, "dwmapi.lib") //DwmGetWindowAttribute
#include "file.h"
#include "color.h"
#include "system.h"

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
			if (rect.right < 0) return false;
			if (rect.bottom < 0) return false;
			if (rect.left > scr.cx) return false;
			if (rect.top > scr.cy) return false;

			if (image.Create(rect.right - rect.left, rect.bottom - rect.top, GetDeviceCaps(hScr, BITSPIXEL))) {
				if (BitBlt(image.GetDC(), 0, 0, rect.right - rect.left, rect.bottom - rect.top, hScr, rect.left, rect.top, SRCCOPY)) {
					ReleaseDC(0, hScr);
					image.ReleaseDC();
					image.Save(path);
					return true;
				}
			}
			ReleaseDC(0, hScr);
			image.ReleaseDC();
			return false;
		}

		static HBITMAP HdcHbitmap(HDC hdc, const SIZE& size, const POINT& lt = {})
		{
			BITMAPINFO bitmapInfo = {};
			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biBitCount = 24;
			bitmapInfo.bmiHeader.biWidth = size.cx;
			bitmapInfo.bmiHeader.biHeight = size.cy;
			bitmapInfo.bmiHeader.biSizeImage = size.cx * size.cy * 3;
			bitmapInfo.bmiHeader.biPlanes = 1;
			HBITMAP hBitmap = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, 0, 0, 0);
			if (hBitmap)
			{
				HDC hdcMem = CreateCompatibleDC(hdc); SelectObject(hdcMem, hBitmap);
				if (BitBlt(hdcMem, 0, 0, size.cx, size.cy, hdc, lt.x, lt.y, SRCCOPY))
				{
					DeleteDC(hdcMem);
					return hBitmap;
				}
				DeleteDC(hdcMem);
			}
			return 0;
		}
		static bool HbitmapRgbmap(HBITMAP hBitmap, RgbMap& rgbMap)
		{
			BITMAP bitmap; GetObjectW(hBitmap, sizeof(BITMAP), &bitmap);
			if (!(bitmap.bmWidth && bitmap.bmHeight)) return false;
			rgbMap.create(bitmap.bmWidth, bitmap.bmHeight);
			rgbMap.IterateReset();
			for (size_t y = bitmap.bmHeight; y > 0; y--)
			{
				for (size_t x = 0; x < bitmap.bmWidth; x++)
				{
					byte* ptr = ((byte*)bitmap.bmBits) + (bitmap.bmWidthBytes * (y - 1)) + (x * 3);
					Rgb& rgb = *rgbMap.Iterate();
					rgb.r = *(ptr + 2), rgb.g = *(ptr + 1), rgb.b = *(ptr);
				}
			}
			return true;
		}

		static bool HdcRgbmap(HDC hdc, RgbMap& rgbMap, const SIZE& size, const POINT& lt = {})
		{
			BITMAPINFO bitmapInfo = {};
			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biBitCount = 24;
			bitmapInfo.bmiHeader.biWidth = size.cx;
			bitmapInfo.bmiHeader.biHeight = size.cy;
			bitmapInfo.bmiHeader.biSizeImage = size.cx * size.cy * 3;
			bitmapInfo.bmiHeader.biPlanes = 1;
			HBITMAP hBitmap = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, 0, 0, 0);
			if (hBitmap)
			{
				HDC hdcMem = CreateCompatibleDC(hdc); SelectObject(hdcMem, hBitmap);
				if (BitBlt(hdcMem, 0, 0, size.cx, size.cy, hdc, lt.x, lt.y, SRCCOPY))
				{
					BITMAP bitmap; GetObjectW(hBitmap, sizeof(BITMAP), &bitmap);
					if (!(bitmap.bmWidth && bitmap.bmHeight)) return false;
					rgbMap.create(size.cx, size.cy);
					rgbMap.IterateReset();
					for (size_t y = bitmap.bmHeight; y > 0; y--)
					{
						for (size_t x = 0; x < bitmap.bmWidth; x++)
						{
							byte* ptr = ((byte*)bitmap.bmBits) + (bitmap.bmWidthBytes * (y - 1)) + (x * 3);
							Rgb& rgb = *rgbMap.Iterate();
							rgb.r = *(ptr + 2), rgb.g = *(ptr + 1), rgb.b = *(ptr);
						}
					}
					DeleteDC(hdcMem);
					DeleteObject(hBitmap);
					return true;
				}
				DeleteDC(hdcMem);
			}
			return false;
		}
		static bool HdcRgbmap(HDC hdc, RgbMap& rgbMap, const RECT& rect) { return HdcRgbmap(hdc, rgbMap, { rect.right - rect.left + 1, rect.bottom - rect.top + 1 }, { rect.left, rect.top }); }

		static void ScreenRgbmap(RgbMap& rgbMap, const SIZE& size = System::screenSize(), const POINT& lt = {})
		{
			HDC hdc = GetDC(0);
			HdcRgbmap(hdc, rgbMap, size, lt);
			ReleaseDC(0, hdc);
		}
		static void ScreenRgbmap(RgbMap& rgbMap, const RECT& rect) { return ScreenRgbmap(rgbMap, { rect.right - rect.left + 1, rect.bottom - rect.top + 1 }, { rect.left, rect.top }); }

		static HBITMAP ScreenBitmap(const SIZE& size = System::screenSize(), const POINT lt = {})
		{
			HDC hdc = GetDC(0);
			HdcHbitmap(hdc, size, lt);
			ReleaseDC(0, hdc);
		}
		static HBITMAP ScreenBitmap(const RECT rect) { ScreenBitmap({ rect.right - rect.left + 1, rect.bottom - rect.top + 1 }, { rect.left, rect.top }); }

		struct FindResult { bool find = false; POINT pt = { 0 }; };
		static FindResult Find(const RgbMap& rgbMap, const RgbMap& srcMap, byte similar = 80, byte sampleRote = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (!rgbMap.count() || !srcMap.count()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.left >= rgbMap.width()) rect.left = rgbMap.width();
			if (rect.top >= rgbMap.height()) rect.top = rgbMap.height();
			if (rect.right >= rgbMap.width()) rect.right = rgbMap.width();
			if (rect.bottom >= rgbMap.height()) rect.bottom = rgbMap.height();
			if (rect.right <= rect.left || rect.bottom <= rect.top) return {};
			if (sampleRote > 100) sampleRote = 100;
			int32 spaceX, spaceY, countX, countY;
			spaceX = srcMap.width() * (float)sampleRote / 100.0f;
			spaceY = srcMap.height() * (float)sampleRote / 100.0f;
			if (spaceX == 0) spaceX = 1;
			if (spaceY == 0) spaceY = 1;
			countX = srcMap.width() / spaceX;
			countY = srcMap.height() / spaceY;
			struct SampleMap { Rgb rgb; POINT pt; };
			PointMap<SampleMap> sampleMap(countX, countY);
			for (size_t y = 0; y < sampleMap.height(); y++)
			{
				for (size_t x = 0; x < sampleMap.width(); x++)
				{
					SampleMap& sample = sampleMap.point(y, x);
					sample.pt.x = x * spaceX;
					sample.pt.y = y * spaceY;
					sample.rgb = srcMap.point(sample.pt.y, sample.pt.x);
				}
			}
			byte extend = 100 - similar;
			forltx(mapX, mapY, rect.left, rect.top, rect.right, rect.bottom)
			{
				if (rgbMap.point(mapY, mapX).equal(sampleMap.map(0).rgb, extend))
				{
					int32 t = 0, f = 0;
					forlt(sampleMap.width(), sampleMap.height())
					{
						const SampleMap& sample = sampleMap.point(y, x);
						if (rgbMap.point(mapY + sample.pt.y, mapX + sample.pt.x).equal(sample.rgb, extend)) t++;
						else f++;
					}
					if ((byte)((100.0f / (float)(t + f)) * (float)t) > similar)
					{
						return { true, {(LONG)mapX, (LONG)mapY} };
					}
				}
			}
			return {};
		}
	};

	class BITMAPTool
	{
		HBITMAP _hbmp = 0;
		BITMAP _bmp = {};
		bool _clean = false;
	public:
		BITMAPTool() {}
		BITMAPTool(HBITMAP hBitmap, bool clean = false) { SetBitmap(hBitmap, clean); }
		~BITMAPTool() { if (_clean) DeleteObject(_hbmp); }

		void SetBitmap(HBITMAP hBitmap, bool clean = false) { if (hBitmap) { GetObjectW(_hbmp, sizeof(BITMAP), &_bmp); } }

		bool LoadFromHDC(HDC hdc, const SIZE& size, const POINT& lt = {})
		{
			BITMAPINFO bitmapInfo = {};
			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biBitCount = 24;
			bitmapInfo.bmiHeader.biWidth = size.cx;
			bitmapInfo.bmiHeader.biHeight = size.cy;
			bitmapInfo.bmiHeader.biSizeImage = size.cx * size.cy * 3;
			bitmapInfo.bmiHeader.biPlanes = 1;
			_hbmp = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, 0, 0, 0);
			if (_hbmp)
			{
				HDC hdcMem = CreateCompatibleDC(hdc); SelectObject(hdcMem, _hbmp);
				if (BitBlt(hdcMem, 0, 0, size.cx, size.cy, hdc, lt.x, lt.y, SRCCOPY))
				{
					GetObjectW(_hbmp, sizeof(BITMAP), &_bmp);
					DeleteDC(hdcMem);
					return true;
				}
				DeleteDC(hdcMem);
			}
			return false;
		}
		bool LoadFromHDC(HDC hdc, const RECT& rect) { return LoadFromHDC(hdc, { rect.right - rect.left, rect.bottom - rect.top }, { rect.left, rect.top }); }
		bool LoadFromScreen(const SIZE& size, const POINT& lt = {}) { HDC hdc = GetDC(0); bool r = LoadFromHDC(hdc, size, lt); ReleaseDC(0, hdc); return r; }
		bool LoadFromScreen(const RECT& rect) { return LoadFromScreen({ rect.right - rect.left, rect.bottom - rect.top }, { rect.left, rect.top }); }
		bool LoadFromRgbMap(RgbMap& rgbMap)
		{
			if (rgbMap.width() && rgbMap.height())
			{
				BITMAPINFO bitmapInfo = {};
				bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bitmapInfo.bmiHeader.biBitCount = 24;
				bitmapInfo.bmiHeader.biWidth = rgbMap.width();
				bitmapInfo.bmiHeader.biHeight = rgbMap.height();
				bitmapInfo.bmiHeader.biSizeImage = rgbMap.bytes();
				bitmapInfo.bmiHeader.biPlanes = 1;
				HDC hdc = GetDC(0);
				_hbmp = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, 0, 0, 0);
				ReleaseDC(0, hdc);
				if (_hbmp)
				{
					GetObjectW(_hbmp, sizeof(BITMAP), &_bmp);
					rgbMap.IterateReset();
					for (size_t y = _bmp.bmHeight, x = 0; y > 0; y--)
					{
						for (x = 0; x < _bmp.bmWidth; x++)
						{
							byte* ptr = ((byte*)_bmp.bmBits) + (_bmp.bmWidthBytes * (y - 1)) + (x * 3);
							Rgb& rgb = *rgbMap.Iterate();
							*(ptr + 2) = rgb.r;
							*(ptr + 1) = rgb.g;
							*ptr = rgb.b;
						}
						x *= 3;
						while (x < _bmp.bmWidthBytes)
						{
							*(((byte*)_bmp.bmBits) + (_bmp.bmWidthBytes * (y - 1)) + x) = 0, x++;
						}
					}
				}
			}
			return false;
		}

		uint32 width() const { return _bmp.bmWidth; }
		uint32 height() const { return _bmp.bmHeight; }
		HBITMAP bitmap() const { return _hbmp; }
		void* data() { return _bmp.bmBits; }
		Rgb pixel(uint32 x, uint32 y)
		{
			if (_hbmp)
			{
				byte* ptr = (byte*)_bmp.bmBits + (((_bmp.bmHeight - y - 1) * _bmp.bmWidthBytes) + (x * 3));
				return { *(ptr + 2), *(ptr + 1), *ptr };
			}
		}

		static HBITMAP FromRgbMap(RgbMap& rgbMap)
		{
			if (rgbMap.width() && rgbMap.height())
			{
				BITMAPINFO bitmapInfo = {};
				bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bitmapInfo.bmiHeader.biBitCount = 24;
				bitmapInfo.bmiHeader.biWidth = rgbMap.width();
				bitmapInfo.bmiHeader.biHeight = rgbMap.height();
				bitmapInfo.bmiHeader.biSizeImage = rgbMap.bytes();
				bitmapInfo.bmiHeader.biPlanes = 1;
				HDC hdc = GetDC(0);
				HBITMAP hbmp = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, 0, 0, 0);
				BITMAP bmp;
				ReleaseDC(0, hdc);
				if (hbmp)
				{
					GetObjectW(hbmp, sizeof(BITMAP), &bmp);
					rgbMap.IterateReset();
					for (size_t y = bmp.bmHeight, x = 0; y > 0; y--)
					{
						for (x = 0; x < bmp.bmWidth; x++)
						{
							byte* ptr = ((byte*)bmp.bmBits) + (bmp.bmWidthBytes * (y - 1)) + (x * 3);
							Rgb& rgb = *rgbMap.Iterate();
							*(ptr + 2) = rgb.r;
							*(ptr + 1) = rgb.g;
							*ptr = rgb.b;
						}
						x *= 3;
						while (x < bmp.bmWidthBytes)
						{
							*(((byte*)bmp.bmBits) + (bmp.bmWidthBytes * (y - 1)) + x) = 0, x++;
						}
					}
					return hbmp;
				}
			}
			return 0;
		}
	};
}