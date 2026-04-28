#pragma once
#include "inc_project.h"

class QiGraphicsCapture : public GraphicsCapture
{
public:
	static constexpr LONG npos = LONG_MAX;
	static constexpr RECT full = { 0, 0, npos, npos };
	QiGraphicsCapture() {}
	QiGraphicsCapture(const QiGraphicsCapture&) {}
	QiGraphicsCapture& operator=(const QiGraphicsCapture&) {}
	QiGraphicsCapture(QiGraphicsCapture&&) {}
	QiGraphicsCapture& operator=(QiGraphicsCapture&&) {}
	bool capture(std::function<bool(size_t w, size_t h, size_t wb, void* d)> proc, RECT rect = full, clock_t timeout = 1000)
	{
		GraphicsCapture::Direct3D11CaptureFrame frame = GraphicsCapture::capture(timeout);
		if (!frame) return false;

		GraphicsCapture::IDirect3DSurface surface = frame.Surface();
		if (!surface) return false;

		GraphicsCapture::IDirect3DDxgiInterfaceAccess access;
		if (!surface.try_as(access)) return false;

		winrt::com_ptr<IDXGISurface> giSurface;
		if (FAILED(access->GetInterface(IID_PPV_ARGS(giSurface.put())))) return false;

		DXGI_SURFACE_DESC gidesc;
		giSurface->GetDesc(&gidesc);

		rect.left = std::clamp<LONG>(rect.left, 0, gidesc.Width);
		rect.right = std::clamp<LONG>(rect.right, 0, gidesc.Width);
		rect.top = std::clamp<LONG>(rect.top, 0, gidesc.Height);
		rect.bottom = std::clamp<LONG>(rect.bottom, 0, gidesc.Height);

		D3D11_TEXTURE2D_DESC desc{};
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.Format = gidesc.Format;
		desc.Width = rect.right - rect.left;
		desc.Height = rect.bottom - rect.top;

		winrt::com_ptr<ID3D11Texture2D> texture;
		if (FAILED(m_device->CreateTexture2D(&desc, nullptr, texture.put()))) return false;

		winrt::com_ptr<ID3D11Resource> resource;
		access->GetInterface(IID_PPV_ARGS(&resource));

		D3D11_BOX box = { rect.left, rect.top, 0, rect.right, rect.bottom, 1 };
		m_context->CopySubresourceRegion(texture.get(), 0, 0, 0, 0, resource.get(), 0, &box);

		D3D11_MAPPED_SUBRESOURCE dxmap;
		if (FAILED(m_context->Map(texture.get(), 0, D3D11_MAP_READ, 0, &dxmap))) return false;

		bool r = proc(desc.Width, desc.Height, dxmap.RowPitch, dxmap.pData);

		m_context->Unmap(texture.get(), 0);
		return r;
	}
	bool captureRgbMap(RgbMap& map, RECT rect = full, clock_t timeout = 1000)
	{
		return capture([&](size_t width, size_t height, size_t widthBytes, void* data) {
			map.assign(width, height);
			auto iter = map.begin();
			for (size_t y = 0; y < height; y++)
			{
				const auto* py = reinterpret_cast<const _BGRA_*>(reinterpret_cast<const BYTE*>(data) + widthBytes * y);
				for (size_t x = 0; x < width; x++)
				{
					const auto* ptr = py + x;
					iter->r = ptr->r, iter->g = ptr->g, iter->b = ptr->b;
					iter++;
				}
			}
			return true;
			}, rect, timeout);
	}
	bool captureRgbaMap(RgbaMap& map, RECT rect = full, clock_t timeout = 1000)
	{
		return capture([&](size_t width, size_t height, size_t widthBytes, void* data) {
			map.assign(width, height);
			auto iter = map.begin();
			for (size_t y = 0; y < height; y++)
			{
				const auto* py = reinterpret_cast<const _ABGR_*>(reinterpret_cast<const BYTE*>(data) + widthBytes * y);
				for (size_t x = 0; x < width; x++)
				{
					const auto* ptr = py + x;
					iter->r = ptr->r, iter->g = ptr->g, iter->b = ptr->b, iter->a = ptr->a;
					iter++;
				}
			}
			return true;
			}, rect, timeout);
	}
	bool captureCImage(CImage& image, RECT rect = full, clock_t timeout = 1000)
	{
		return capture([&](size_t width, size_t height, size_t widthBytes, void* data) {
			if (!image.IsNull()) image.Destroy();
			if (!image.Create(width, height, sizeof(COLORREF) * 8)) return false;
			const int twidthBytes = image.GetPitch();
			void* tdata = image.GetBits();
			for (size_t y = 0; y < height; y++)
			{
				auto* ty = reinterpret_cast<_ABGR_*>(reinterpret_cast<BYTE*>(tdata) + twidthBytes * y);
				const auto* py = reinterpret_cast<const _ABGR_*>(reinterpret_cast<const BYTE*>(data) + widthBytes * y);
				for (size_t x = 0; x < width; x++)
				{
					auto* tptr = ty + x;
					const auto* ptr = py + x;
					tptr->r = ptr->r, tptr->g = ptr->g, tptr->b = ptr->b, tptr->a = ptr->a;
				}
			}
			return true;
			}, rect, timeout);
	}
};