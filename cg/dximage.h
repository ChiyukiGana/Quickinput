#pragma once
#include "image.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#pragma comment(lib, "d3d11.lib")

namespace CG
{
	class DxImage
	{
		typedef HRESULT(WINAPI* DwmGetDxSharedSurface_f)(_In_ HWND hWnd, _Out_ HANDLE* hSurface, _In_ LUID* pAdapterLuid, _Inout_ ULONG* pFmtWindow, _Out_ ULONG* pPresentFlagss, _Out_ ULONGLONG* pWin32kUpdateId);
		static inline ID3D11Device* dx3dDevice = nullptr;
		static inline ID3D11DeviceContext* dx3dContext = nullptr;
		static inline IDXGIDevice* dxgiDevice = nullptr;
		static inline IDXGIAdapter* dxgiAdp = nullptr;
		static inline IDXGIOutputDuplication* dxgiDupl = nullptr;

		static inline HMODULE hLib = nullptr;
		static inline DwmGetDxSharedSurface_f pDwmGetDxSharedSurface = nullptr;

	public:
		static constexpr LONG npos = LONG_MAX;
		static constexpr POINT none = { npos, npos };
		static constexpr RECT full = { 0, 0, npos, npos };

		static bool Initialize()
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
				hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &dx3dDevice, nullptr, &dx3dContext);
				if (FAILED(hr))
				{
					SetLastError(hr);
					return false;
				}
			}

			return true;
		}

		static bool InitializeDisplay(UINT display = 0)
		{
			if (!Initialize()) return false;

			HRESULT hr;

			if (!dxgiDevice)
			{
				hr = dx3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
				if (FAILED(hr))
				{
					SetLastError(hr);
					return false;
				}
			}

			if (!dxgiAdp)
			{
				hr = dxgiDevice->GetAdapter(&dxgiAdp);
				if (FAILED(hr))
				{
					SetLastError(hr);
					return false;
				}
			}

			if (!dxgiDupl)
			{
				IDXGIOutput* dxgiOutput = nullptr;
				HRESULT hr = dxgiAdp->EnumOutputs(display, &dxgiOutput);
				if (FAILED(hr))
				{
					SetLastError(hr);
					return false;
				}

				IDXGIOutput1* dxgiOutput1 = nullptr;
				hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&dxgiOutput1);
				dxgiOutput->Release();
				if (FAILED(hr))
				{
					SetLastError(hr);
					return false;
				}

				hr = dxgiOutput1->DuplicateOutput(dx3dDevice, &dxgiDupl);
				dxgiOutput1->Release();
				if (FAILED(hr))
				{
					SetLastError(hr);
					return false;
				}
			}
			return true;
		}

		static bool InitializeDwm()
		{
			if (!Initialize()) return false;

			if (!hLib)
			{
				hLib = LoadLibraryW(L"user32.dll");
				if (!hLib) return false;
			}
			if (!pDwmGetDxSharedSurface)
			{
				pDwmGetDxSharedSurface = (DwmGetDxSharedSurface_f)GetProcAddress(hLib, "DwmGetDxSharedSurface");
				if (!pDwmGetDxSharedSurface) return false;
			}
			return true;
		}

		static void ReleaseDisplay()
		{
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

		static int ReInitializeDisplay(UINT display = 0)
		{
			ReleaseDisplay();
			return InitializeDisplay(display);
		}

		static void Release()
		{
			ReleaseDisplay();

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
		}

		static SIZE DisplaySize()
		{
			DXGI_OUTDUPL_DESC desc;
			dxgiDupl->GetDesc(&desc);
			return { (LONG)desc.ModeDesc.Width, (LONG)desc.ModeDesc.Height };
		}

		static bool DisplayTexture(ID3D11Texture2D** ppTexture, SIZE* pSize, RECT rect = full)
		{
			DXGI_OUTDUPL_FRAME_INFO dxgiFrameInfo;

			IDXGIResource* dxgiRes;
			HRESULT hr = dxgiDupl->AcquireNextFrame(500, &dxgiFrameInfo, &dxgiRes);
			if (FAILED(hr)) return false;

			ID3D11Texture2D* currentTexture = nullptr;
			hr = dxgiRes->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&currentTexture);
			dxgiRes->Release();
			if (FAILED(hr))
			{
				dxgiDupl->ReleaseFrame();
				return false;
			}

			D3D11_TEXTURE2D_DESC desc;
			currentTexture->GetDesc(&desc);
			desc.Usage = D3D11_USAGE_STAGING;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.BindFlags = 0;
			desc.MiscFlags = 0;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.SampleDesc.Count = 1;

			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > desc.Width) rect.right = desc.Width;
			if (rect.bottom > desc.Height) rect.bottom = desc.Height;

			pSize->cx = desc.Width = rect.right - rect.left;
			pSize->cy = desc.Height = rect.bottom - rect.top;
			desc.Width = AlignmentSize(desc.Width, 32);

			hr = dx3dDevice->CreateTexture2D(&desc, nullptr, ppTexture);
			if (FAILED(hr))
			{
				currentTexture->Release();
				dxgiDupl->ReleaseFrame();
				return false;
			}

			D3D11_BOX box = { rect.left, rect.top, 0, rect.right, rect.bottom, 1 };
			dx3dContext->CopySubresourceRegion(*ppTexture, 0, 0, 0, 0, currentTexture, 0, &box);
			currentTexture->Release();
			dxgiDupl->ReleaseFrame();
			return true;
		}

		static bool WindowTexture(HWND hWnd, ID3D11Texture2D** ppTexture, SIZE* pSize, RECT rect = full)
		{
			HANDLE hSurface;
			HRESULT hr = pDwmGetDxSharedSurface(hWnd, &hSurface, nullptr, nullptr, nullptr, nullptr);
			if (FAILED(hr)) return false;

			ID3D11Texture2D* sharedTexture = nullptr;
			hr = dx3dDevice->OpenSharedResource(hSurface, __uuidof(ID3D11Texture2D), (void**)&sharedTexture);
			if (FAILED(hr)) return false;

			D3D11_TEXTURE2D_DESC desc;
			sharedTexture->GetDesc(&desc);
			desc.Usage = D3D11_USAGE_STAGING;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.BindFlags = 0;
			desc.MiscFlags = 0;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.SampleDesc.Count = 1;

			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > desc.Width) rect.right = desc.Width;
			if (rect.bottom > desc.Height) rect.bottom = desc.Height;

			pSize->cx = desc.Width = rect.right - rect.left;
			pSize->cy = desc.Height = rect.bottom - rect.top;
			desc.Width = AlignmentSize(desc.Width, 32);

			hr = dx3dDevice->CreateTexture2D(&desc, nullptr, ppTexture);
			if (FAILED(hr))
			{
				sharedTexture->Release();
				return false;
			}

			D3D11_BOX box = { rect.left, rect.top, 0, rect.right, rect.bottom, 1 };
			dx3dContext->CopySubresourceRegion(*ppTexture, 0, 0, 0, 0, sharedTexture, 0, &box);
			sharedTexture->Release();
			return true;
		}

		static bool TextureToRgbMap(ID3D11Texture2D* pTexture, SIZE size, RgbMap& rgbMap)
		{
			D3D11_MAPPED_SUBRESOURCE map;
			HRESULT hr = dx3dContext->Map(pTexture, 0, D3D11_MAP_READ, 0, &map);
			if (FAILED(hr))
			{
				pTexture->Release();
				return false;
			}
			D3D11_TEXTURE2D_DESC desc;
			pTexture->GetDesc(&desc);
			rgbMap.create(size.cx, size.cy);
			rgbMap.IterateReset();
			for (size_t y = 0; y < rgbMap.height(); y++)
			{
				const _BGRA_* py = (const _BGRA_*)map.pData + desc.Width * y;
				for (size_t x = 0; x < rgbMap.width(); x++)
				{
					const _BGRA_* ptr = py + x;
					Rgb& rgb = rgbMap.Iterate();
					rgb.r = ptr->r, rgb.g = ptr->g, rgb.b = ptr->b;
				}
			}
			dx3dContext->Unmap(pTexture, 0);
			return true;
		}

		static bool TextureToRgbaMap(ID3D11Texture2D* pTexture, SIZE size, RgbaMap& rgbaMap)
		{
			D3D11_MAPPED_SUBRESOURCE map;
			HRESULT hr = dx3dContext->Map(pTexture, 0, D3D11_MAP_READ, 0, &map);
			if (FAILED(hr))
			{
				pTexture->Release();
				return false;
			}
			D3D11_TEXTURE2D_DESC desc;
			pTexture->GetDesc(&desc);
			rgbaMap.create(size.cx, size.cy);
			rgbaMap.IterateReset();
			if (desc.Width - rgbaMap.width()) for (size_t y = 0; y < rgbaMap.height(); y++) memcpy((_BGRA_*)&rgbaMap.point(y, 0), (const _BGRA_*)map.pData + desc.Width * y, rgbaMap.width() * sizeof(_BGRA_));
			else memcpy(rgbaMap.data(), map.pData, rgbaMap.bytes());
			dx3dContext->Unmap(pTexture, 0);
			return true;
		}

		static bool DisplayRgbMap(RgbMap& rgbMap, const RECT& rect = full)
		{
			SIZE size;
			ID3D11Texture2D* texture;
			if (!DisplayTexture(&texture, &size, rect)) return false;

			if (!TextureToRgbMap(texture, size, rgbMap)) return false;

			texture->Release();
			return true;
		}

		static bool DisplayRgbaMap(RgbaMap& rgbaMap, const RECT& rect = full)
		{
			SIZE size;
			ID3D11Texture2D* texture;
			if (!DisplayTexture(&texture, &size, rect)) return false;

			if (!TextureToRgbaMap(texture, size, rgbaMap)) return false;

			texture->Release();
			return true;
		}

		static bool WindowRgbMap(HWND hWnd, RgbMap& rgbMap, const RECT& rect = full)
		{
			SIZE size;
			ID3D11Texture2D* texture;
			if (!WindowTexture(hWnd, &texture, &size, rect)) return false;

			if (!TextureToRgbMap(texture, size, rgbMap)) return false;

			texture->Release();
			return true;
		}

		static bool WindowRgbaMap(HWND hWnd, RgbaMap& rgbaMap, const RECT& rect = full)
		{
			SIZE size;
			ID3D11Texture2D* texture;
			if (!WindowTexture(hWnd, &texture, &size, rect)) return false;

			if (!TextureToRgbaMap(texture, size, rgbaMap)) return false;

			texture->Release();
			return true;
		}

		static POINT Find(ID3D11Texture2D* pTexture, SIZE size, const Rgba& min, const Rgba& max, RECT rect = full)
		{
			D3D11_MAPPED_SUBRESOURCE map;
			HRESULT hr = dx3dContext->Map(pTexture, 0, D3D11_MAP_READ, 0, &map);
			if (FAILED(hr))
			{
				pTexture->Release();
				return none;
			}
			D3D11_TEXTURE2D_DESC desc;
			pTexture->GetDesc(&desc);

			if (rect.left < 0) rect.left = 0;
			if (rect.right < 0) rect.right = 0;
			if (rect.right > size.cx) rect.right = size.cx;
			if (rect.bottom > size.cy) rect.bottom = size.cy;

			for (uint32 x = 0, y = 0, xmax = rect.right - rect.left, ymax = rect.bottom - rect.top; y < ymax; y++)
			{
				for (x = 0; x < xmax; x++)
				{
					const _BGRA_* p = ((const _BGRA_*)map.pData) + ((desc.Width * (rect.top + y)) + (rect.left + x));
					if (InRange(p->r, min.r, max.r, 0) && InRange(p->g, min.g, max.g, 0) && InRange(p->b, min.b, max.b, 0))
					{
						dx3dContext->Unmap(pTexture, 0);
						return { (LONG)(rect.left + x), (LONG)(rect.top + y) };
					}
				}
			}
			dx3dContext->Unmap(pTexture, 0);
			return none;
		}

		static POINT DisplayFind(const Rgba& min, const Rgba& max, RECT rect = full)
		{
			SIZE size;
			ID3D11Texture2D* texture;
			if (!DisplayTexture(&texture, &size, rect)) return none;
			POINT pt = Find(texture, size, min, max);
			texture->Release();
			return pt;
		}

		static POINT WindowFind(HWND hWnd, const Rgba& min, const Rgba& max, RECT rect = full)
		{
			SIZE size;
			ID3D11Texture2D* texture;
			if (!WindowTexture(hWnd, &texture, &size, rect)) return none;
			POINT pt = Find(texture, size, min, max);
			texture->Release();
			return pt;
		}
	};
}