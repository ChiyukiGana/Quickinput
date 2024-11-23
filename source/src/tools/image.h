#pragma once
#include <windows.h>
#include "color.h"
#include "system.h"
namespace QiTools {
	class Image
	{
		struct _RGB_ { byte r; byte g; byte b; };
		struct _BGR_ { byte b; byte g; byte r; };
		struct _ARGB_ { byte a; byte r; byte g; byte b; };
		struct _RGBA_ { byte r; byte g; byte b; byte a; };
		struct _ABGR_ { byte a; byte b; byte g; byte r; };
		struct _BGRA_ { byte b; byte g; byte r; byte a; };
	public:
		static constexpr LONG npos = (-1);
		static HBITMAP toBmp24(HDC hdc, const RECT& rect)
		{
			if (((rect.right - rect.left) < 1) || ((rect.bottom - rect.top) < 1)) return nullptr;
			BITMAPINFO bitmapInfo = {};
			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biBitCount = 24;
			bitmapInfo.bmiHeader.biWidth = rect.right - rect.left;
			bitmapInfo.bmiHeader.biHeight = rect.bottom - rect.top;
			bitmapInfo.bmiHeader.biSizeImage = bitmapInfo.bmiHeader.biWidth * bitmapInfo.bmiHeader.biHeight * 3;
			bitmapInfo.bmiHeader.biPlanes = 1;
			void* unuse;
			HBITMAP hBitmap = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, &unuse, 0, 0);
			if (hBitmap)
			{
				HDC hdcMem = CreateCompatibleDC(hdc); SelectObject(hdcMem, hBitmap);
				if (BitBlt(hdcMem, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, rect.left, rect.top, SRCCOPY))
				{
					DeleteDC(hdcMem);
					return hBitmap;
				}
				DeleteObject(hBitmap);
				DeleteDC(hdcMem);
			}
			return 0;
		}
		static HBITMAP toBmp32(HDC hdc, const RECT& rect)
		{
			if (((rect.right - rect.left) < 1) || ((rect.bottom - rect.top) < 1)) return nullptr;
			BITMAPINFO bitmapInfo = {};
			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biBitCount = 32;
			bitmapInfo.bmiHeader.biWidth = rect.right - rect.left;
			bitmapInfo.bmiHeader.biHeight = rect.bottom - rect.top;
			bitmapInfo.bmiHeader.biSizeImage = bitmapInfo.bmiHeader.biWidth * bitmapInfo.bmiHeader.biHeight * 4;
			bitmapInfo.bmiHeader.biPlanes = 1;
			void* unuse;
			HBITMAP hBitmap = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, &unuse, 0, 0);
			if (hBitmap)
			{
				HDC hdcMem = CreateCompatibleDC(hdc); SelectObject(hdcMem, hBitmap);
				if (BitBlt(hdcMem, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, rect.left, rect.top, SRCCOPY))
				{
					DeleteDC(hdcMem);
					return hBitmap;
				}
				DeleteObject(hBitmap);
				DeleteDC(hdcMem);
			}
			return 0;
		}
		static HBITMAP toBmp32(const RgbMap& rgbMap, const HDC& hdc = nullptr)
		{
			HDC dc = hdc; if (!hdc) dc = GetDC(nullptr);
			BITMAPINFO bitmapInfo = {};
			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biBitCount = 24;
			bitmapInfo.bmiHeader.biWidth = rgbMap.width();
			bitmapInfo.bmiHeader.biHeight = 0 - rgbMap.height();
			bitmapInfo.bmiHeader.biSizeImage = rgbMap.bytes();
			bitmapInfo.bmiHeader.biPlanes = 1;
			size_t widthBytes = AlignmentSize(rgbMap.width() * 3, 4);
			byte* buffer = new byte[widthBytes * rgbMap.height()];
			for (size_t y = 0, i = 0; y < rgbMap.height(); y++)
			{
				_BGR_* py = (_BGR_*)(buffer + (widthBytes * y));
				for (size_t x = 0; x < rgbMap.width(); x++, i++)
				{
					_BGR_* ptr = py + x;
					const Rgb& rgb = rgbMap.map(i);
					ptr->r = rgb.r, ptr->g = rgb.g, ptr->b = rgb.b;
				}
			}
			HBITMAP hbmp = CreateDIBitmap(dc, &bitmapInfo.bmiHeader, CBM_INIT, buffer, &bitmapInfo, DIB_RGB_COLORS);
			if (!hdc) ReleaseDC(nullptr, dc);
			delete[] buffer;
			return hbmp;
		}
		static HBITMAP toBmp32(const RgbaMap& rgbaMap, const HDC& hdc)
		{
			HDC dc = hdc; if (!hdc) dc = GetDC(nullptr);
			BITMAPINFO bitmapInfo = {};
			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biBitCount = 32;
			bitmapInfo.bmiHeader.biWidth = rgbaMap.width();
			bitmapInfo.bmiHeader.biHeight = 0 - rgbaMap.height();
			bitmapInfo.bmiHeader.biSizeImage = rgbaMap.bytes();
			bitmapInfo.bmiHeader.biPlanes = 1;

			_RGBA_* buffer = new _RGBA_[rgbaMap.width() * rgbaMap.height()];
			for (size_t i = 0; i < rgbaMap.count(); i++)
			{
				_RGBA_* ptr = buffer + i;
				const Rgba& rgba = rgbaMap.map(i);
				ptr->r = rgba.r, ptr->g = rgba.g, ptr->b = rgba.b, ptr->a = rgba.a;
			}
			HBITMAP hbmp = CreateDIBitmap(dc, &bitmapInfo.bmiHeader, CBM_INIT, buffer, &bitmapInfo, DIB_RGB_COLORS);
			if (!hdc) ReleaseDC(nullptr, dc);
			delete[] buffer;
			return hbmp;
		}
		
		static bool toRgbMap(const HBITMAP& hBitmap, RgbMap& rgbMap)
		{
			BITMAP bmp; GetObjectW(hBitmap, sizeof(BITMAP), &bmp);
			if (!(bmp.bmWidth && bmp.bmHeight)) return false;

			if (bmp.bmBitsPixel == 24)
			{
				rgbMap.create(bmp.bmWidth, bmp.bmHeight);
				rgbMap.IterateReset();
				for (size_t y = 0; y < bmp.bmHeight; y++)
				{
					const _BGR_* py = (const _BGR_*)((const byte*)bmp.bmBits + (bmp.bmWidthBytes) * (bmp.bmHeight - y - 1));
					for (size_t x = 0; x < bmp.bmWidth; x++)
					{
						const _BGR_* ptr = py + x;
						Rgb& rgb = rgbMap.Iterate();
						rgb.r = ptr->r, rgb.g = ptr->g, rgb.b = ptr->b;
					}
				}
				return true;
			}
			else if (bmp.bmBitsPixel == 32)
			{
				rgbMap.create(bmp.bmWidth, bmp.bmHeight);
				rgbMap.IterateReset();
				for (size_t y = 0; y < bmp.bmHeight; y++)
				{
					const _ABGR_* py = (const _ABGR_*)bmp.bmBits + bmp.bmWidth * (bmp.bmHeight - y - 1);
					for (size_t x = 0; x < bmp.bmWidth; x++)
					{
						const _ABGR_* ptr = py + x;
						Rgb& rgba = rgbMap.Iterate();
						rgba.r = ptr->r, rgba.g = ptr->g, rgba.b = ptr->b;
					}
				}
				return true;
			}
			return false;
		}
		static bool toRgbaMap(const HBITMAP& hBitmap, RgbaMap& rgbaMap)
		{
			BITMAP bmp; GetObjectW(hBitmap, sizeof(BITMAP), &bmp);
			if (!(bmp.bmWidth && bmp.bmHeight)) return false;

			if (bmp.bmBitsPixel == 24)
			{
				rgbaMap.create(bmp.bmWidth, bmp.bmHeight);
				rgbaMap.IterateReset();
				for (size_t y = 0; y < bmp.bmHeight; y++)
				{
					const _BGR_* py = (const _BGR_*)((const byte*)bmp.bmBits + (bmp.bmWidthBytes) * (bmp.bmHeight - y - 1));
					for (size_t x = 0; x < bmp.bmWidth; x++)
					{
						const _BGR_* ptr = py + x;
						Rgba& rgb = rgbaMap.Iterate();
						rgb.r = ptr->r, rgb.g = ptr->g, rgb.b = ptr->b;
					}
				}
				return true;
			}
			else if (bmp.bmBitsPixel == 32)
			{
				rgbaMap.create(bmp.bmWidth, bmp.bmHeight);
				rgbaMap.IterateReset();
				for (size_t y = 0; y < bmp.bmHeight; y++)
				{
					const _ABGR_* py = (const _ABGR_*)bmp.bmBits + bmp.bmWidth * (bmp.bmHeight - y - 1);
					for (size_t x = 0; x < bmp.bmWidth; x++)
					{
						const _ABGR_* ptr = py + x;
						Rgba& rgba = rgbaMap.Iterate();
						rgba.r = ptr->r, rgba.g = ptr->g, rgba.b = ptr->b;
					}
				}
				return true;
			}
			return false;
		}
		static bool toRgbMap(const HDC& hdc, RgbMap& rgbMap, const RECT& rect)
		{
			HBITMAP hbmp = toBmp24(hdc, rect);
			if (hbmp)
			{
				if (toRgbMap(hbmp, rgbMap))
				{
					DeleteObject(hbmp);
					return true;
				}
				DeleteObject(hbmp);
			}
			return false;
		}
		static bool toRgbaMap(const HDC& hdc, RgbaMap& rgbaMap, const RECT& rect)
		{
			HBITMAP hbmp = toBmp32(hdc, rect);
			if (hbmp)
			{
				if (toRgbaMap(hbmp, rgbaMap))
				{
					DeleteObject(hbmp);
					return true;
				}
				DeleteObject(hbmp);
			}
			return false;
		}

		static HBITMAP ScreenBmp24(RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			HDC hdc = GetDC(0);
			SIZE size = System::screenSize();
			if (rect.right > size.cx) rect.right = size.cx;
			if (rect.bottom > size.cy) rect.bottom = size.cy;
			HBITMAP hbmp = toBmp24(hdc, rect);
			ReleaseDC(0, hdc);
			return hbmp;
		}
		static HBITMAP ScreenBmp32(RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			HDC hdc = GetDC(0);
			SIZE size = System::screenSize();
			if (rect.right > size.cx) rect.right = size.cx;
			if (rect.bottom > size.cy) rect.bottom = size.cy;
			HBITMAP hbmp = toBmp32(hdc, rect);
			ReleaseDC(0, hdc);
			return hbmp;
		}
		static bool ScreenRgbMap(RgbMap& rgbMap, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			HBITMAP hbmp = ScreenBmp24(rect);
			if (hbmp)
			{
				if (toRgbMap(hbmp, rgbMap))
				{
					DeleteObject(hbmp);
					return true;
				}
				DeleteObject(hbmp);
			}
			return false;
		}
		static bool ScreenRgbaMap(RgbaMap& rgbaMap, const RECT& rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			HBITMAP hbmp = ScreenBmp32(rect);
			if (hbmp)
			{
				if (toRgbaMap(hbmp, rgbaMap))
				{
					DeleteObject(hbmp);
					return true;
				}
				DeleteObject(hbmp);
			}
			return false;
		}

		static POINT Find(const RgbMap& rgbMap, const RgbMap& srcMap, byte similar = 80, byte sampleRote = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (!rgbMap.count() || !srcMap.count()) return { npos, npos };
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > rgbMap.width()) rect.right = rgbMap.width();
			if (rect.bottom > rgbMap.height()) rect.bottom = rgbMap.height();
			if (((rect.right - rect.left) < 1) || ((rect.bottom - rect.top) < 1)) return { npos, npos };
			if (sampleRote > 100) sampleRote = 100;
			int spaceX, spaceY, countX, countY;
			spaceX = srcMap.width() * (float)sampleRote / 100.0f;
			spaceY = srcMap.height() * (float)sampleRote / 100.0f;
			if (spaceX == 0) spaceX = 1;
			if (spaceY == 0) spaceY = 1;
			countX = srcMap.width() / spaceX;
			countY = srcMap.height() / spaceY;
			struct SampleMap { Rgb rgb; int x = 0, y = 0; };
			XMap<SampleMap> sampleMap(countX, countY);
			for (size_t y = 0; y < sampleMap.height(); y++)
			{
				for (size_t x = 0; x < sampleMap.width(); x++)
				{
					SampleMap& sample = sampleMap.point(y, x);
					sample.x = x * spaceX;
					sample.y = y * spaceY;
					sample.rgb = srcMap.point(sample.y, sample.x);
				}
			}
			byte extend = 100 - similar;
			forltx(mapX, mapY, rect.left, rect.top, rect.right, rect.bottom)
			{
				if (rgbMap.point(mapY, mapX).equal(sampleMap.map(0).rgb, extend))
				{
					int t = 0, f = 0;
					forlt(sampleMap.width(), sampleMap.height())
					{
						int px = mapX + sampleMap.point(y, x).x;
						int py = mapY + sampleMap.point(y, x).y;
						if (px < rect.right && py < rect.bottom && (rgbMap.point(py, px).equal(sampleMap.point(y, x).rgb, extend))) t++;
						else f++;
					}
					if ((byte)((100.0f / (float)(t + f)) * (float)t) > similar)
					{
						return { (LONG)mapX, (LONG)mapY };
					}
				}
			}
			return { npos, npos };
		}
		static POINT Find(const RgbaMap& rgbMap, const RgbaMap& srcMap, byte similar = 80, byte sampleRote = 10, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (!rgbMap.count() || !srcMap.count()) return { npos, npos };
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right >= rgbMap.width()) rect.right = rgbMap.width();
			if (rect.bottom >= rgbMap.height()) rect.bottom = rgbMap.height();
			if (((rect.right - rect.left) <= 0) || ((rect.bottom - rect.top) <= 0)) return { npos, npos };
			if (sampleRote > 100) sampleRote = 100;
			int spaceX, spaceY, countX, countY;
			spaceX = srcMap.width() * (float)sampleRote / 100.0f;
			spaceY = srcMap.height() * (float)sampleRote / 100.0f;
			if (spaceX == 0) spaceX = 1;
			if (spaceY == 0) spaceY = 1;
			countX = srcMap.width() / spaceX;
			countY = srcMap.height() / spaceY;
			struct SampleMap { Rgba rgba; int x = 0, y = 0; };
			XMap<SampleMap> sampleMap(countX, countY);
			for (size_t y = 0; y < sampleMap.height(); y++)
			{
				for (size_t x = 0; x < sampleMap.width(); x++)
				{
					SampleMap& sample = sampleMap.point(y, x);
					sample.x = x * spaceX;
					sample.y = y * spaceY;
					sample.rgba = srcMap.point(sample.y, sample.x);
				}
			}
			byte extend = 100 - similar;
			forltx(mapX, mapY, rect.left, rect.top, rect.right, rect.bottom)
			{
				if (rgbMap.point(mapY, mapX).equal(sampleMap.map(0).rgba, extend))
				{
					int t = 0, f = 0;
					forlt(sampleMap.width(), sampleMap.height())
					{
						int px = mapX + sampleMap.point(y, x).x;
						int py = mapY + sampleMap.point(y, x).y;
						if (px < rect.right && py < rect.bottom && (rgbMap.point(py, px).equal(sampleMap.point(y, x).rgba, extend))) t++;
						else f++;
					}
					if ((byte)((100.0f / (float)(t + f)) * (float)t) > similar)
					{
						return { (LONG)mapX, (LONG)mapY };
					}
				}
			}
			return { npos, npos };
		}
	};
}