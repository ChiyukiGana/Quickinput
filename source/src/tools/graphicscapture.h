#pragma once
#ifndef GRAPHICS_CAPTURE
#define GRAPHICS_CAPTURE

#include <functional>
#include <d3d11.h>
#include <dxgi.h>
#include <shobjidl_core.h>
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>
#include <winrt/base.h>
#include <winrt/windows.foundation.h>
#include <winrt/windows.foundation.metadata.h>
#include <winrt/windows.graphics.directx.direct3d11.h>
#include <winrt/windows.graphics.capture.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "windowsapp")

class GraphicsCapture
{
public:
	using Size = winrt::Windows::Graphics::SizeInt32;
	using DirectXPixelFormat = winrt::Windows::Graphics::DirectX::DirectXPixelFormat;
	using RtIInspectable = winrt::Windows::Foundation::IInspectable;
	using IDirect3DDevice = winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice;
	using IDirect3DSurface = winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface;
	using Direct3DSurfaceDescription = winrt::Windows::Graphics::DirectX::Direct3D11::Direct3DSurfaceDescription;
	using IDirect3DDxgiInterfaceAccess = winrt::com_ptr<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>;
	using GraphicsCaptureItem = winrt::Windows::Graphics::Capture::GraphicsCaptureItem;
	using GraphicsCaptureSession = winrt::Windows::Graphics::Capture::GraphicsCaptureSession;
	using Direct3D11CaptureFrame = winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame;
	using Direct3D11CaptureFramePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool;
	using FrameEventHandler = winrt::Windows::Foundation::TypedEventHandler<Direct3D11CaptureFramePool, RtIInspectable>;

protected:
	const bool m_support = false;
	const bool m_curosr_capture_support = false;
	const bool m_min_update_interval_support = false;
	HWND m_window = nullptr;
	HMONITOR m_monitor = nullptr;
	bool m_cursor = false;
	bool m_init = false;
	bool m_ready = false;
	bool m_incapture = false;

	winrt::com_ptr<ID3D11Device> m_device;
	winrt::com_ptr<ID3D11DeviceContext> m_context;
	winrt::com_ptr<IDXGIDevice> m_gi;
	GraphicsCaptureItem m_target = nullptr;
	IDirect3DDevice m_rtdevice = nullptr;
	Direct3D11CaptureFramePool m_framePool = nullptr;
	GraphicsCaptureSession m_session = nullptr;
	FrameEventHandler m_proc;

public:

	static bool IsSupported()
	{
		return winrt::Windows::Graphics::Capture::GraphicsCaptureSession::IsSupported();
	}
	static bool IsCursorCaptureSupported()
	{
		try { return winrt::Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(L"Windows.Graphics.Capture.GraphicsCaptureSession", L"IsCursorCaptureEnabled"); }
		catch (const winrt::hresult_error& e) {}
		return false;
	}
	static bool IsMinUpdateIntervalSupported()
	{
		try { return winrt::Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(L"Windows.Graphics.Capture.GraphicsCaptureSession", L"MinUpdateInterval"); }
		catch (const winrt::hresult_error& e) {}
		return false;
	}
	static auto PickTarget(HWND window = nullptr)
	{
		if (!IsSupported()) return winrt::Windows::Graphics::Capture::GraphicsCaptureItem(nullptr);
		winrt::Windows::Graphics::Capture::GraphicsCapturePicker picker;
		winrt::com_ptr<IInitializeWithWindow> initializeWithWindow(picker.as<::IInitializeWithWindow>());
		initializeWithWindow->Initialize(window ? window : GetForegroundWindow());
		return picker.PickSingleItemAsync().get();
	}

private:
	static bool CreateDevice(winrt::com_ptr<ID3D11Device>& r_device, winrt::com_ptr<ID3D11DeviceContext>& r_context, winrt::com_ptr<IDXGIDevice>& r_gi, winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice& r_rtdevice)
	{
		constexpr D3D_DRIVER_TYPE driverTypes[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE };
		constexpr UINT driverTypesCount = ARRAYSIZE(driverTypes);

		bool ok = false;
		winrt::com_ptr<ID3D11Device> device;
		winrt::com_ptr<ID3D11DeviceContext> context;
		for (UINT driverIndex = 0; driverIndex < driverTypesCount; driverIndex++) if (SUCCEEDED(D3D11CreateDevice(nullptr, driverTypes[driverIndex], nullptr, NULL, nullptr, NULL, D3D11_SDK_VERSION, device.put(), nullptr, context.put()))) { ok = true; break; }
		if (!ok) return false;

		winrt::com_ptr<IDXGIDevice> gi = device.as<IDXGIDevice>();
		if (!gi) return false;

		winrt::com_ptr<IInspectable> irtdevice;
		if (FAILED(CreateDirect3D11DeviceFromDXGIDevice(gi.get(), irtdevice.put()))) return false;

		IDirect3DDevice rtdevice = irtdevice.as<IDirect3DDevice>();
		if (!rtdevice) return false;

		r_device = std::move(device);
		r_context = std::move(context);
		r_gi = std::move(gi);
		r_rtdevice = irtdevice.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();
		return true;
	}

public:
	GraphicsCapture(GraphicsCapture&&) = delete;
	GraphicsCapture& operator=(GraphicsCapture&&) = delete;
	GraphicsCapture() : m_support(IsSupported()), m_curosr_capture_support(IsCursorCaptureSupported()), m_min_update_interval_support(IsMinUpdateIntervalSupported()) { m_init = CreateDevice(m_device, m_context, m_gi, m_rtdevice); }
	~GraphicsCapture() { close(); }

	bool isInit() const { return m_init; }
	bool isReady() const { return m_init && m_ready; }
	bool isSupported() const { return m_support; }
	bool isCursorCaptureSupported() const { return m_curosr_capture_support; }

	HWND window() const { return m_window; }
	HMONITOR monitor() const { return m_monitor; }
	const Direct3D11CaptureFramePool& framePool() const { return m_framePool; }
	const GraphicsCaptureItem& target() const { return m_target; }

	void setTarget(const GraphicsCaptureItem& target) { m_target = target; }
	bool setTarget(HWND window)
	{
		m_window = window;
		m_target = GraphicsCaptureItem::TryCreateFromWindowId(winrt::Windows::UI::WindowId{ reinterpret_cast<uint64_t>(window) });
		return m_target.operator bool();
	}
	bool setTarget(HMONITOR monitor)
	{
		m_monitor = monitor;
		m_target = GraphicsCaptureItem::TryCreateFromDisplayId(winrt::Windows::Graphics::DisplayId{ reinterpret_cast<uint64_t>(monitor) });
		return m_target.operator bool();
	}
	void setProc(const FrameEventHandler& proc) { m_proc = proc; }
	void setCursorCapture(bool cursor) { m_cursor = cursor; }
	bool setup(clock_t interval = 16 /* ~60fps */, bool async = true)
	{
		close();
		if (!m_init || !m_target) return false;
		Direct3D11CaptureFramePool framePool = async ? Direct3D11CaptureFramePool::CreateFreeThreaded(m_rtdevice, winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, m_target.Size()) : Direct3D11CaptureFramePool::Create(m_rtdevice, winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, m_target.Size());
		if (!framePool) return false;
		GraphicsCaptureSession session = framePool.CreateCaptureSession(m_target);
		if (!session) return false;
		if (m_curosr_capture_support) session.IsCursorCaptureEnabled(m_cursor);
		if (m_min_update_interval_support) session.MinUpdateInterval(std::chrono::milliseconds(interval));
		m_framePool = std::move(framePool);
		m_session = std::move(session);
		if (m_proc) m_framePool.FrameArrived(m_proc);
		return m_ready = true;
	}

	bool start()
	{
		if (m_init && m_ready && !m_incapture)
		{
			m_session.StartCapture();
			return m_incapture = true;
		}
		return false;
	}
	void close()
	{
		if (m_init && m_ready)
		{
			m_session.Close(), m_framePool.Close();
			m_ready = m_incapture = false;
		}
	}
	Direct3D11CaptureFrame capture(clock_t timeout = 1000)
	{
		if (!m_incapture) start();
		if (!m_incapture) return nullptr;
		const clock_t end = clock() + timeout;
		do
		{
			Direct3D11CaptureFrame frame = m_framePool.TryGetNextFrame();
			if (frame) return frame;
			Sleep(1);
		} while (clock() < end);
		return nullptr;
	}
};
#endif