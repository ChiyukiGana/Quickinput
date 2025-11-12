#pragma once
#include <windows.h>
#include "color.h"
#include "system.h"
namespace QiTools {
	class Image
	{
	public:
		using Result = std::optional<POINT>;
		using Results = std::vector<POINT>;
		static constexpr LONG npos = LONG_MAX;
		static constexpr RECT full = { 0, 0, npos, npos };

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
				if (BitBlt(hdcMem, 0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight, hdc, rect.left, rect.top, SRCCOPY))
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
				if (BitBlt(hdcMem, 0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight, hdc, rect.left, rect.top, SRCCOPY))
				{
					DeleteDC(hdcMem);
					return hBitmap;
				}
				DeleteObject(hBitmap);
				DeleteDC(hdcMem);
			}
			return 0;
		}
		static HBITMAP toBmp32(const RgbMap& map, const HDC& hdc = nullptr)
		{
			HDC dc = hdc; if (!hdc) dc = GetDC(nullptr);
			BITMAPINFO bitmapInfo = {};
			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biBitCount = 24;
			bitmapInfo.bmiHeader.biWidth = map.width();
			bitmapInfo.bmiHeader.biHeight = 0 - map.height();
			bitmapInfo.bmiHeader.biSizeImage = map.bytes();
			bitmapInfo.bmiHeader.biPlanes = 1;
			size_t widthBytes = AlignmentSize(map.width() * 3, 4);
			byte* buffer = new byte[widthBytes * map.height()];
			for (size_t y = 0, i = 0; y < map.height(); y++)
			{
				_BGR_* py = (_BGR_*)(buffer + (widthBytes * y));
				for (size_t x = 0; x < map.width(); x++, i++)
				{
					_BGR_* ptr = py + x;
					const Rgb& rgb = map.at(i);
					ptr->r = rgb.r, ptr->g = rgb.g, ptr->b = rgb.b;
				}
			}
			HBITMAP hbmp = CreateDIBitmap(dc, &bitmapInfo.bmiHeader, CBM_INIT, buffer, &bitmapInfo, DIB_RGB_COLORS);
			if (!hdc) ReleaseDC(nullptr, dc);
			delete[] buffer;
			return hbmp;
		}
		static HBITMAP toBmp32(const RgbaMap& map, const HDC& hdc)
		{
			HDC dc = hdc; if (!hdc) dc = GetDC(nullptr);
			BITMAPINFO bitmapInfo = {};
			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biBitCount = 32;
			bitmapInfo.bmiHeader.biWidth = map.width();
			bitmapInfo.bmiHeader.biHeight = 0 - map.height();
			bitmapInfo.bmiHeader.biSizeImage = map.bytes();
			bitmapInfo.bmiHeader.biPlanes = 1;

			_RGBA_* buffer = new _RGBA_[map.width() * map.height()];
			for (size_t i = 0; i < map.size(); i++)
			{
				_RGBA_* ptr = buffer + i;
				const Rgba& rgba = map.at(i);
				ptr->r = rgba.r, ptr->g = rgba.g, ptr->b = rgba.b, ptr->a = rgba.a;
			}
			HBITMAP hbmp = CreateDIBitmap(dc, &bitmapInfo.bmiHeader, CBM_INIT, buffer, &bitmapInfo, DIB_RGB_COLORS);
			if (!hdc) ReleaseDC(nullptr, dc);
			delete[] buffer;
			return hbmp;
		}

		static CImage toCImage24(HDC hdc, const RECT& rect)
		{
			int w = rect.right - rect.left;
			int h = rect.bottom - rect.top;
			if (w < 1 || h < 1) return CImage();

			CImage image;
			image.Create(w, h, 24);
			if (BitBlt(image.GetDC(), 0, 0, w, h, hdc, rect.left, rect.top, SRCCOPY)) return image;
			return CImage();
		}
		static CImage toCImage32(HDC hdc, const RECT& rect)
		{
			int w = rect.right - rect.left;
			int h = rect.bottom - rect.top;
			if (w < 1 || h < 1) return CImage();

			CImage image;
			image.Create(w, h, 32);
			if (BitBlt(image.GetDC(), 0, 0, w, h, hdc, rect.left, rect.top, SRCCOPY)) return image;
			return CImage();
		}

		static bool toRgbMap(const HBITMAP& hBitmap, RgbMap& map)
		{
			BITMAP bmp; GetObjectW(hBitmap, sizeof(BITMAP), &bmp);
			if (!(bmp.bmWidth && bmp.bmHeight)) return false;

			if (bmp.bmBitsPixel == 24)
			{
				map.assign(bmp.bmWidth, bmp.bmHeight);
				auto iter = map.begin();
				for (size_t y = 0; y < bmp.bmHeight; y++)
				{
					const _BGR_* py = (const _BGR_*)((const byte*)bmp.bmBits + (bmp.bmWidthBytes) * (bmp.bmHeight - y - 1));
					for (size_t x = 0; x < bmp.bmWidth; x++)
					{
						const _BGR_* ptr = py + x;
						iter->r = ptr->r, iter->g = ptr->g, iter->b = ptr->b;
						iter++;
					}
				}
				return true;
			}
			else if (bmp.bmBitsPixel == 32)
			{
				map.assign(bmp.bmWidth, bmp.bmHeight);
				auto iter = map.begin();
				for (size_t y = 0; y < bmp.bmHeight; y++)
				{
					const _ABGR_* py = (const _ABGR_*)bmp.bmBits + bmp.bmWidth * (bmp.bmHeight - y - 1);
					for (size_t x = 0; x < bmp.bmWidth; x++)
					{
						const _ABGR_* ptr = py + x;
						iter->r = ptr->r, iter->g = ptr->g, iter->b = ptr->b;
						iter++;
					}
				}
				return true;
			}
			return false;
		}
		static bool toRgbaMap(const HBITMAP& hBitmap, RgbaMap& map)
		{
			BITMAP bmp; GetObjectW(hBitmap, sizeof(BITMAP), &bmp);
			if (!(bmp.bmWidth && bmp.bmHeight)) return false;

			if (bmp.bmBitsPixel == 24)
			{
				map.assign(bmp.bmWidth, bmp.bmHeight);
				auto iter = map.begin();
				for (size_t y = 0; y < bmp.bmHeight; y++)
				{
					const _BGR_* py = (const _BGR_*)((const byte*)bmp.bmBits + (bmp.bmWidthBytes) * (bmp.bmHeight - y - 1));
					for (size_t x = 0; x < bmp.bmWidth; x++)
					{
						const _BGR_* ptr = py + x;
						iter->r = ptr->r, iter->g = ptr->g, iter->b = ptr->b, iter->a = 255;
						iter++;
					}
				}
				return true;
			}
			else if (bmp.bmBitsPixel == 32)
			{
				map.assign(bmp.bmWidth, bmp.bmHeight);
				auto iter = map.begin();
				for (size_t y = 0; y < bmp.bmHeight; y++)
				{
					const _ABGR_* py = (const _ABGR_*)bmp.bmBits + bmp.bmWidth * (bmp.bmHeight - y - 1);
					for (size_t x = 0; x < bmp.bmWidth; x++)
					{
						const _ABGR_* ptr = py + x;
						iter->r = ptr->r, iter->g = ptr->g, iter->b = ptr->b, iter->a = ptr->a;;
						iter++;
					}
				}
				return true;
			}
			return false;
		}
		static bool toRgbMap(const HDC& hdc, const RECT& rect, RgbMap& map)
		{
			HBITMAP hbmp = toBmp24(hdc, rect);
			if (hbmp)
			{
				if (toRgbMap(hbmp, map))
				{
					DeleteObject(hbmp);
					return true;
				}
				DeleteObject(hbmp);
			}
			return false;
		}
		static bool toRgbaMap(const HDC& hdc, const RECT& rect, RgbaMap& map)
		{
			HBITMAP hbmp = toBmp32(hdc, rect);
			if (hbmp)
			{
				if (toRgbaMap(hbmp, map))
				{
					DeleteObject(hbmp);
					return map;
				}
				DeleteObject(hbmp);
			}
			return {};
		}

		static HBITMAP ScreenBmp24(RECT rect = full)
		{
			HDC hdc = GetDC(0);
			SIZE size = System::screenSize();
			rect.left = std::clamp((int)rect.left, 0, (int)size.cx);
			rect.right = std::clamp((int)rect.right, 0, (int)size.cx);
			rect.top = std::clamp((int)rect.top, 0, (int)size.cy);
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)size.cy);
			HBITMAP hbmp = toBmp24(hdc, rect);
			ReleaseDC(0, hdc);
			return hbmp;
		}
		static HBITMAP ScreenBmp32(RECT rect = full)
		{
			HDC hdc = GetDC(0);
			SIZE size = System::screenSize();
			rect.left = std::clamp((int)rect.left, 0, (int)size.cx);
			rect.right = std::clamp((int)rect.right, 0, (int)size.cx);
			rect.top = std::clamp((int)rect.top, 0, (int)size.cy);
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)size.cy);
			HBITMAP hbmp = toBmp32(hdc, rect);
			ReleaseDC(0, hdc);
			return hbmp;
		}
		static CImage ScreenCImage24(RECT rect = full)
		{
			HDC hdc = GetDC(0);
			SIZE size = System::screenSize();
			rect.left = std::clamp((int)rect.left, 0, (int)size.cx);
			rect.right = std::clamp((int)rect.right, 0, (int)size.cx);
			rect.top = std::clamp((int)rect.top, 0, (int)size.cy);
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)size.cy);
			CImage image = toCImage24(hdc, rect);
			ReleaseDC(0, hdc);
			return image;
		}
		static CImage ScreenCImage32(RECT rect = full)
		{
			HDC hdc = GetDC(0);
			SIZE size = System::screenSize();
			rect.left = std::clamp((int)rect.left, 0, (int)size.cx);
			rect.right = std::clamp((int)rect.right, 0, (int)size.cx);
			rect.top = std::clamp((int)rect.top, 0, (int)size.cy);
			rect.bottom = std::clamp((int)rect.bottom, 0, (int)size.cy);
			CImage image = toCImage32(hdc, rect);
			ReleaseDC(0, hdc);
			return image;
		}
		static bool ScreenRgbMap(RgbMap& map, const RECT& rect = full)
		{
			HBITMAP hbmp = ScreenBmp24(rect);
			if (hbmp)
			{
				if (toRgbMap(hbmp, map))
				{
					DeleteObject(hbmp);
					return true;
				}
				DeleteObject(hbmp);
			}
			return false;
		}
		static bool ScreenRgbaMap(RgbaMap& map, const RECT& rect = full)
		{
			HBITMAP hbmp = ScreenBmp32(rect);
			if (hbmp)
			{
				if (toRgbaMap(hbmp, map))
				{
					DeleteObject(hbmp);
					return true;
				}
				DeleteObject(hbmp);
			}
			return false;
		}

		static Result Find(const RgbMap& map, const RgbMap& srcMap, byte similar = 80, byte sampleRote = 10, RECT rect = full)
		{
			if (map.empty() || srcMap.empty()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > map.width()) rect.right = map.width();
			if (rect.bottom > map.height()) rect.bottom = map.height();
			if (((rect.right - rect.left) < 1) || ((rect.bottom - rect.top) < 1)) return {};
			if (sampleRote > 100) sampleRote = 100;
			int spaceX, spaceY, countX, countY;
			spaceX = srcMap.width() * (float)sampleRote / 100.0f;
			spaceY = srcMap.height() * (float)sampleRote / 100.0f;
			if (spaceX == 0) spaceX = 1;
			if (spaceY == 0) spaceY = 1;
			countX = srcMap.width() / spaceX;
			countY = srcMap.height() / spaceY;
			struct SampleMap { const Rgb* rgb = nullptr; size_t x = 0, y = 0; };
			map2d<SampleMap> sampleMap(countX, countY);
			for (size_t y = 0; y < sampleMap.height(); y++)
			{
				for (size_t x = 0; x < sampleMap.width(); x++)
				{
					SampleMap& sample = sampleMap.at(x, y);
					sample.x = x * spaceX;
					sample.y = y * spaceY;
					sample.rgb = &srcMap.at(sample.x, sample.y);
				}
			}
			byte extend = 100 - similar;
			for (size_t my = rect.top; my < rect.bottom; my++)
			{
				for (size_t mx = rect.left; mx < rect.right; mx++)
				{
					if (map.at(mx, my).equal(*sampleMap.at(0).rgb, extend))
					{
						int t = 0, f = 0;
						for (size_t sy = 0; sy < sampleMap.height(); sy++)
						{
							for (size_t sx = 0; sx < sampleMap.width(); sx++)
							{
								int px = mx + sampleMap.at(sx, sy).x;
								int py = my + sampleMap.at(sx, sy).y;
								if (px < rect.right && py < rect.bottom && (map.at(px, py).equal(*sampleMap.at(sx, sy).rgb, extend))) t++;
								else f++;
							}
						}
						if ((byte)((100.0f / (float)(t + f)) * (float)t) > similar) return POINT{ (LONG)mx, (LONG)my };
					}
				}
			}
			return {};
		}
		static Result Find(const RgbaMap& map, const RgbaMap& srcMap, byte similar = 80, byte sampleRote = 10, RECT rect = full)
		{
			if (map.empty() || srcMap.empty()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right >= map.width()) rect.right = map.width();
			if (rect.bottom >= map.height()) rect.bottom = map.height();
			if (((rect.right - rect.left) <= 0) || ((rect.bottom - rect.top) <= 0)) return {};
			if (sampleRote > 100) sampleRote = 100;
			int spaceX, spaceY, countX, countY;
			spaceX = srcMap.width() * (float)sampleRote / 100.0f;
			spaceY = srcMap.height() * (float)sampleRote / 100.0f;
			if (spaceX == 0) spaceX = 1;
			if (spaceY == 0) spaceY = 1;
			countX = srcMap.width() / spaceX;
			countY = srcMap.height() / spaceY;
			struct SampleMap { const Rgba* rgba = nullptr; size_t x = 0, y = 0; };
			map2d<SampleMap> sampleMap(countX, countY);
			for (size_t y = 0; y < sampleMap.height(); y++)
			{
				for (size_t x = 0; x < sampleMap.width(); x++)
				{
					SampleMap& sample = sampleMap.at(x, y);
					sample.x = x * spaceX;
					sample.y = y * spaceY;
					sample.rgba = &srcMap.at(sample.x, sample.y);
				}
			}
			byte extend = 100 - similar;
			for (size_t my = rect.top; my < rect.bottom; my++)
			{
				for (size_t mx = rect.left; mx < rect.right; mx++)
				{
					if (map.at(mx, my).equal(*sampleMap.at(0).rgba, extend))
					{
						int t = 0, f = 0;
						for (size_t sy = 0; sy < sampleMap.height(); sy++)
						{
							for (size_t sx = 0; sx < sampleMap.width(); sx++)
							{
								int px = mx + sampleMap.at(sx, sy).x;
								int py = my + sampleMap.at(sx, sy).y;
								if (px < rect.right && py < rect.bottom && (map.at(px, py).equal(*sampleMap.at(sx, sy).rgba, extend))) t++;
								else f++;
							}
						}
						if ((byte)((100.0f / (float)(t + f)) * (float)t) > similar) return POINT{ (LONG)mx, (LONG)my };
					}
				}
			}
			return {};
		}
		static Results FindAll(const RgbMap& map, const RgbMap& srcMap, byte similar = 80, byte sampleRote = 10, RECT rect = full)
		{
			if (map.empty() || srcMap.empty()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > map.width()) rect.right = map.width();
			if (rect.bottom > map.height()) rect.bottom = map.height();
			if (((rect.right - rect.left) < 1) || ((rect.bottom - rect.top) < 1)) return {};
			if (sampleRote > 100) sampleRote = 100;
			int spaceX, spaceY, countX, countY;
			spaceX = srcMap.width() * (float)sampleRote / 100.0f;
			spaceY = srcMap.height() * (float)sampleRote / 100.0f;
			if (spaceX == 0) spaceX = 1;
			if (spaceY == 0) spaceY = 1;
			countX = srcMap.width() / spaceX;
			countY = srcMap.height() / spaceY;
			struct SampleMap { const Rgb* rgb = nullptr; size_t x = 0, y = 0; };
			map2d<SampleMap> sampleMap(countX, countY);
			for (size_t y = 0; y < sampleMap.height(); y++)
			{
				for (size_t x = 0; x < sampleMap.width(); x++)
				{
					SampleMap& sample = sampleMap.at(x, y);
					sample.x = x * spaceX;
					sample.y = y * spaceY;
					sample.rgb = &srcMap.at(sample.x, sample.y);
				}
			}
			Results results;
			byte extend = 100 - similar;
			map2d<char> visited(map.width(), map.height(), 0);
			for (size_t my = rect.top; my < rect.bottom; my++)
			{
				for (size_t mx = rect.left; mx < rect.right; mx++)
				{
					if (visited.at(mx, my)) continue;
					if (map.at(mx, my).equal(*sampleMap.front().rgb, extend))
					{
						bool visit = false;
						size_t t = 0, f = 0;
						for (size_t sy = 0; sy < sampleMap.height(); sy++)
						{
							for (size_t sx = 0; sx < sampleMap.width(); sx++)
							{
								size_t px = mx + sampleMap.at(sx, sy).x;
								size_t py = my + sampleMap.at(sx, sy).y;
								if (px < rect.right && py < rect.bottom)
								{
									if (visited.at(px, py))
									{
										visit = true;
										break;
									}
									(map.at(px, py).equal(*sampleMap.at(sx, sy).rgb, extend)) ? t++ : f++;
								}
								else f++;
							}
							if (visit) break;
						}
						if (visit) continue;
						if ((byte)((100.0f / (float)(t + f)) * (float)t) > similar)
						{
							results.push_back({ (LONG)mx, (LONG)my });
							for (size_t vy = 0; vy < srcMap.height(); vy++)
							{
								for (size_t vx = 0; vx < srcMap.width(); vx++)
								{
									size_t px = mx + vx;
									size_t py = my + vy;
									if (px < map.width() && py < map.height()) visited.at(px, py) = 1;
								}
							}
						}
					}
				}
			}
			return results;
		}
		static Results FindAll(const RgbaMap& map, const RgbaMap& srcMap, byte similar = 80, byte sampleRote = 10, RECT rect = full)
		{
			if (map.empty() || srcMap.empty()) return {};
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > map.width()) rect.right = map.width();
			if (rect.bottom > map.height()) rect.bottom = map.height();
			if (((rect.right - rect.left) < 1) || ((rect.bottom - rect.top) < 1)) return {};
			if (sampleRote > 100) sampleRote = 100;
			int spaceX, spaceY, countX, countY;
			spaceX = srcMap.width() * (float)sampleRote / 100.0f;
			spaceY = srcMap.height() * (float)sampleRote / 100.0f;
			if (spaceX == 0) spaceX = 1;
			if (spaceY == 0) spaceY = 1;
			countX = srcMap.width() / spaceX;
			countY = srcMap.height() / spaceY;
			struct SampleMap { const Rgba* rgba = nullptr; size_t x = 0, y = 0; };
			map2d<SampleMap> sampleMap(countX, countY);
			for (size_t y = 0; y < sampleMap.height(); y++)
			{
				for (size_t x = 0; x < sampleMap.width(); x++)
				{
					SampleMap& sample = sampleMap.at(x, y);
					sample.x = x * spaceX;
					sample.y = y * spaceY;
					sample.rgba = &srcMap.at(sample.x, sample.y);
				}
			}
			Results results;
			byte extend = 100 - similar;
			map2d<char> visited(map.width(), map.height(), 0);
			for (size_t my = rect.top; my < rect.bottom; my++)
			{
				for (size_t mx = rect.left; mx < rect.right; mx++)
				{
					if (visited.at(mx, my)) continue;
					if (map.at(mx, my).equal(*sampleMap.front().rgba, extend))
					{
						bool visit = false;
						size_t t = 0, f = 0;
						for (size_t sy = 0; sy < sampleMap.height(); sy++)
						{
							for (size_t sx = 0; sx < sampleMap.width(); sx++)
							{
								size_t px = mx + sampleMap.at(sx, sy).x;
								size_t py = my + sampleMap.at(sx, sy).y;
								if (px < rect.right && py < rect.bottom)
								{
									if (visited.at(px, py))
									{
										visit = true;
										break;
									}
									(map.at(px, py).equal(*sampleMap.at(sx, sy).rgba, extend)) ? t++ : f++;
								}
								else f++;
							}
							if (visit) break;
						}
						if (visit) continue;
						if ((byte)((100.0f / (float)(t + f)) * (float)t) > similar)
						{
							results.push_back({ (LONG)mx, (LONG)my });
							for (size_t vy = 0; vy < srcMap.height(); vy++)
							{
								for (size_t vx = 0; vx < srcMap.width(); vx++)
								{
									size_t px = mx + vx;
									size_t py = my + vy;
									if (px < map.width() && py < map.height()) visited.at(px, py) = 1;
								}
							}
						}
					}
				}
			}
			return results;
		}
	};
}