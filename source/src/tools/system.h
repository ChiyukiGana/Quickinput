#pragma once
#pragma warning(disable:4996)
#include <windows.h>
#include <VersionHelpers.h>
#include <Shlobj.h>
#include <time.h>
#include <string>
namespace QiTools
{
	class System
	{
	public:
		// need manifest to support
		static OSVERSIONINFOW Version()
		{
			OSVERSIONINFOW info = { sizeof(OSVERSIONINFOW) };
			GetVersionExW(&info);
			return info;
		}
		static SIZE screenSize()
		{
			HMONITOR hmt = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
			MONITORINFOEXW mti = {};
			mti.cbSize = sizeof(MONITORINFOEXW);
			GetMonitorInfoW(hmt, &mti);
			DEVMODEW dm = { sizeof(DEVMODEW) };
			EnumDisplaySettingsW(mti.szDevice, ENUM_CURRENT_SETTINGS, &dm);
			return { (LONG)dm.dmPelsWidth, (LONG)dm.dmPelsHeight };
		}
		static SIZE screenSize(DWORD id)
		{
			DISPLAY_DEVICEW device = { sizeof(DISPLAY_DEVICEW) };
			EnumDisplayDevicesW(nullptr, id, &device, 0);
			DEVMODEW dm = { sizeof(DEVMODEW) };
			EnumDisplaySettingsW(device.DeviceName, ENUM_CURRENT_SETTINGS, &dm);
			return { (LONG)dm.dmPelsWidth, (LONG)dm.dmPelsHeight };
		}
		static SIZE screenVSize() { return { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) }; }
		static SIZE screenVSize(HDC dc) { return { GetDeviceCaps(dc, HORZRES), GetDeviceCaps(dc, VERTRES) }; }
		static float screenScaleRote() {
			HMONITOR hmt = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
			MONITORINFOEXW mti = {}; mti.cbSize = sizeof(MONITORINFOEXW); GetMonitorInfoW(hmt, &mti);
			DEVMODEW dm = { sizeof(DEVMODEW) }; EnumDisplaySettingsW(mti.szDevice, ENUM_CURRENT_SETTINGS, &dm);
			return (float)(dm.dmPelsHeight) / (float)(mti.rcMonitor.bottom - mti.rcMonitor.top);
		}
		static std::wstring environmentVariable(LPCWSTR name) { wchar_t str[MAX_PATH]; GetEnvironmentVariableW(name, str, MAX_PATH); return str; }
		static std::wstring folderPath(int csidl = CSIDL_DESKTOP) { wchar_t s[MAX_PATH]; SHGetFolderPathW(0, csidl, 0, SHGFP_TYPE_CURRENT, s); return s; }
		static bool ClipBoardText(LPCWSTR str) {
			size_t size = (wcslen(str) + 1) * sizeof(wchar_t);
			HANDLE hGlobalMemory = GlobalAlloc(GMEM_MOVEABLE, size);
			if (!hGlobalMemory) return false;
			HANDLE pGlobalMemory = GlobalLock(hGlobalMemory);
			if (!pGlobalMemory) return false;
			wcscpy_s((wchar_t*)pGlobalMemory, size, str);
			GlobalUnlock(pGlobalMemory);
			if (!OpenClipboard(nullptr)) return 0;
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hGlobalMemory);
			CloseClipboard();
			return true;
		}
	};
}