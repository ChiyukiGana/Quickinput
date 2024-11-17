#pragma once
#include "image.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <dxgi1_2.h>

namespace CG
{
	class DxImage
	{
		bool init = false;
		bool dx3dInit = false;
		ID3D11Device* dx3dDevice = nullptr;
		ID3D11DeviceContext* dx3dContext = nullptr;
		bool dxgiInit = false;
		IDXGIDevice* dxgiDevice = nullptr;
		IDXGIAdapter* dxgiAdp = nullptr;
		bool outputInit = false;
		IDXGIOutputDuplication* dxgiDupl = nullptr;

		bool InitDX3D()
		{
			D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
			D3D_FEATURE_LEVEL level;
			HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, levels, 1, D3D11_SDK_VERSION, &dx3dDevice, &level, &dx3dContext);
			if (FAILED(hr)) return false;
			return true;
		}
		bool InitDXGI()
		{
			HRESULT hr = dx3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
			if (FAILED(hr)) return false;

			hr = dxgiDevice->GetAdapter(&dxgiAdp);
			if (FAILED(hr))
			{
				dx3dDevice->Release();
				return false;
			}
			return true;
		}
		bool InitOutput(UINT id)
		{
			IDXGIOutput* dxgiOutput = nullptr;
			HRESULT hr = dxgiAdp->EnumOutputs(id, &dxgiOutput);
			if (FAILED(hr)) return false;

			IDXGIOutput1* dxgiOutput1 = nullptr;
			hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&dxgiOutput1);
			dxgiOutput->Release();
			if (FAILED(hr)) return false;

			hr = dxgiOutput1->DuplicateOutput(dx3dDevice, &dxgiDupl);
			dxgiOutput1->Release();
			if (FAILED(hr)) return false;

			return true;
		}
		void ReleaseDX3D()
		{
			init = false;
			if (dx3dInit)
			{
				dx3dDevice->Release();
				dx3dContext->Release();
				dx3dInit = false;
			}
		}
		void ReleaseDXGI()
		{
			init = false;
			if (dxgiInit)
			{
				dxgiDevice->Release();
				dxgiAdp->Release();
				dxgiInit = false;
			}
		}
		void ReleaseOutput()
		{
			init = false;
			if (outputInit)
			{
				dxgiDupl->Release();
				outputInit = false;
			}
		}

	public:

		DxImage() { Initialize(0); };
		DxImage(UINT screenId) { Initialize(screenId); };
		~DxImage() { Release(); }

		bool Initialize(UINT screenId = 0)
		{
			if (!dx3dInit) dx3dInit = InitDX3D();
			if (!dx3dInit) return false;
			if (!dxgiInit) dxgiInit = InitDXGI();
			if (!dxgiInit) return false;
			if (!outputInit) outputInit = InitOutput(screenId);
			if (!outputInit) return false;
			init = true;
			Sleep(100);
			return true;
		}
		bool IsInitialized() { return init; }
		void Release()
		{
			ReleaseDX3D();
			ReleaseDXGI();
			ReleaseOutput();
		}

		SIZE SurfaceSize()
		{
			DXGI_OUTDUPL_DESC desc;
			dxgiDupl->GetDesc(&desc);
			return { (LONG)desc.ModeDesc.Width, (LONG)desc.ModeDesc.Height };
		}

		bool Texture(ID3D11Texture2D** pTexture, SIZE& rel, RECT rect = { 0, 0, LONG_MAX, LONG_MAX })
		{
			if (!init) return false;
			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
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

		bool RgbMap(RgbMap& rgbMap, const RECT& rect = { 0, 0, LONG_MAX, LONG_MAX })
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
		bool RgbaMap(RgbaMap& rgbaMap, const RECT& rect = { 0, 0, LONG_MAX, LONG_MAX })
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
	};
}