#pragma once
#include "image.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <dxgi1_2.h>
#include "msgbox.h"

namespace CG
{
	class DxImage
	{
		static inline ID3D11Device* dx3dDevice = nullptr;
		static inline ID3D11DeviceContext* dx3dContext = nullptr;
		static inline IDXGIDevice* dxgiDevice = nullptr;
		static inline IDXGIAdapter* dxgiAdp = nullptr;
		static inline IDXGIOutputDuplication* dxgiDupl = nullptr;

	public:
		static constexpr LONG npos = (-1);

		static int Initialize(UINT display = 0)
		{
			HRESULT hr;
			if (!dx3dDevice || !dx3dContext)
			{
				if (dx3dDevice)
				{
					dx3dDevice->Release();
					dx3dDevice = nullptr;
				}
				if (dx3dContext)
				{
					dx3dContext->Release();
					dx3dContext = nullptr;
				}
				D3D_FEATURE_LEVEL level;
				hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &dx3dDevice, nullptr, &dx3dContext);
				SetLastError(hr); if (FAILED(hr)) return -1;
			}

			if (!dxgiDevice)
			{
				hr = dx3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
				SetLastError(hr); if (FAILED(hr)) return -2;
			}

			if (!dxgiAdp)
			{
				hr = dxgiDevice->GetAdapter(&dxgiAdp);
				SetLastError(hr); if (FAILED(hr)) return -3;
			}

			if (!dxgiDupl)
			{
				IDXGIOutput* dxgiOutput = nullptr;
				HRESULT hr = dxgiAdp->EnumOutputs(display, &dxgiOutput);
				SetLastError(hr); if (FAILED(hr)) return -4;

				IDXGIOutput1* dxgiOutput1 = nullptr;
				hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&dxgiOutput1);
				dxgiOutput->Release();
				SetLastError(hr); if (FAILED(hr)) return -5;

				hr = dxgiOutput1->DuplicateOutput(dx3dDevice, &dxgiDupl);
				dxgiOutput1->Release();
				SetLastError(hr); if (FAILED(hr)) return -6;
			}
			return 0;
		}

		static void Release()
		{
			if (dx3dDevice)
			{
				dx3dDevice->Release();
				dx3dDevice = nullptr;
			}
			if (dx3dContext)
			{
				dx3dContext->Release();
				dx3dContext = nullptr;
			}
			if (dxgiDevice)
			{
				dxgiDevice->Release();
				dxgiDevice = nullptr;
			}
			if (dxgiAdp)
			{
				dxgiAdp->Release();
				dxgiAdp = nullptr;
			}
			if (dxgiDupl)
			{
				dxgiDupl->Release();
				dxgiDupl = nullptr;
			}
		}

		static int ReInitialize(UINT display = 0)
		{
			Release();
			return Initialize(display);
		}

		static SIZE Size()
		{
			DXGI_OUTDUPL_DESC desc;
			dxgiDupl->GetDesc(&desc);
			return { (LONG)desc.ModeDesc.Width, (LONG)desc.ModeDesc.Height };
		}

		static bool Texture(ID3D11Texture2D** pTexture, SIZE& rel, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			DXGI_OUTDUPL_FRAME_INFO dxgiFrameInfo;
			IDXGIResource* dxgiRes = nullptr;
			HRESULT hr = dxgiDupl->AcquireNextFrame(100, &dxgiFrameInfo, &dxgiRes); if (FAILED(hr)) return false;
			ID3D11Texture2D* frameTexture = nullptr;
			hr = dxgiRes->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&frameTexture); dxgiRes->Release();
			if (FAILED(hr))
			{
				dxgiDupl->ReleaseFrame();
				return false;
			}
			D3D11_TEXTURE2D_DESC desc; frameTexture->GetDesc(&desc); desc.Usage = D3D11_USAGE_STAGING; desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; desc.BindFlags = 0; desc.MiscFlags = 0; desc.MipLevels = 1; desc.ArraySize = 1; desc.SampleDesc.Count = 1;
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > desc.Width) rect.right = desc.Width;
			if (rect.bottom > desc.Height) rect.bottom = desc.Height;
			if (!InRect({ 0L, 0L, (LONG)desc.Width, (LONG)desc.Height }, rect))
			{
				frameTexture->Release();
				dxgiDupl->ReleaseFrame();
				return false;
			}
			desc.Width = rel.cx = rect.right - rect.left;
			desc.Height = rel.cy = rect.bottom - rect.top;
			//Width needs to be adjusted to 32x
			desc.Width = AlignmentSize(desc.Width, 32);
			hr = dx3dDevice->CreateTexture2D(&desc, nullptr, pTexture);
			if (FAILED(hr))
			{
				frameTexture->Release();
				dxgiDupl->ReleaseFrame();
				return false;
			}
			D3D11_BOX box = { rect.left, rect.top, 0, rect.right, rect.bottom, 1 };
			dx3dContext->CopySubresourceRegion(*pTexture, 0, 0, 0, 0, frameTexture, 0, &box);
			frameTexture->Release();
			dxgiDupl->ReleaseFrame();
			return true;
		}

		static bool RgbMap(RgbMap& rgbMap, const RECT& rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			SIZE rel = {};
			ID3D11Texture2D* texture = nullptr; if (!Texture(&texture, rel, rect)) return false;
			D3D11_MAPPED_SUBRESOURCE map; HRESULT hr = dx3dContext->Map(texture, 0, D3D11_MAP_READ, 0, &map); if (FAILED(hr)) { texture->Release(); return false; }
			D3D11_TEXTURE2D_DESC desc; texture->GetDesc(&desc);
			rgbMap.create(rel.cx, rel.cy);
			rgbMap.IterateReset();
			for (size_t y = 0; y < rgbMap.height(); y++)
			{
				const _RGBA_* py = (const _RGBA_*)map.pData + desc.Width * y;
				for (size_t x = 0; x < rgbMap.width(); x++)
				{
					const _RGBA_* ptr = py + x;
					Rgb& rgb = rgbMap.Iterate();
					rgb.r = ptr->r, rgb.g = ptr->g, rgb.b = ptr->b;
				}
			}
			dx3dContext->Unmap(texture, 0);
			texture->Release();
			return true;
		}

		static bool RgbaMap(RgbaMap& rgbaMap, const RECT& rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			SIZE rel = {};
			ID3D11Texture2D* texture = nullptr; if (!Texture(&texture, rel, rect)) return false;
			D3D11_MAPPED_SUBRESOURCE map; HRESULT hr = dx3dContext->Map(texture, 0, D3D11_MAP_READ, 0, &map); if (FAILED(hr)) { texture->Release(); return false; }
			D3D11_TEXTURE2D_DESC desc; texture->GetDesc(&desc);
			rgbaMap.create(rel.cx, rel.cy);
			if (desc.Width - rgbaMap.width()) for (size_t y = 0; y < rgbaMap.height(); y++) memcpy((_RGBA_*)&rgbaMap.point(y, 0), (const _RGBA_*)map.pData + desc.Width * y, rgbaMap.width() * sizeof(_RGBA_));
			else memcpy(rgbaMap.data(), map.pData, rgbaMap.bytes());
			dx3dContext->Unmap(texture, 0);
			texture->Release();
			return true;
		}

		static POINT Find(RECT displayRect, const Rgba& min, const Rgba& max, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			SIZE rel = {};
			ID3D11Texture2D* texture = nullptr; if (!Texture(&texture, rel, displayRect)) return {npos, npos};
			D3D11_MAPPED_SUBRESOURCE map; HRESULT hr = dx3dContext->Map(texture, 0, D3D11_MAP_READ, 0, &map); if (FAILED(hr)) { texture->Release(); return { npos, npos }; }
			D3D11_TEXTURE2D_DESC desc; texture->GetDesc(&desc);

			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > rel.cx) rect.right = rel.cx;
			if (rect.bottom > rel.cy) rect.bottom = rel.cy;

			for (uint32 x = 0, y = 0, xmax = rect.right - rect.left, ymax = rect.bottom - rect.top; y < ymax; y++)
			{
				for (x = 0; x < xmax; x++)
				{
					const _BGRA_* p = ((const _BGRA_*)map.pData) + ((desc.Width * (rect.top + y)) + (rect.left + x));
					if (InRange(p->r, min.r, max.r, 0) && InRange(p->g, min.g, max.g, 0) && InRange(p->b, min.b, max.b, 0))
					{
						dx3dContext->Unmap(texture, 0);
						texture->Release();
						return { (LONG)(rect.left + x), (LONG)(rect.top + y) };
					}
				}
			}
			dx3dContext->Unmap(texture, 0);
			texture->Release();
			return { npos, npos };
		}
	};
}