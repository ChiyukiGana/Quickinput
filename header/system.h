#pragma once
#pragma warning(disable: 4996)
#include <windows.h>
#include <WtsApi32.h> // IsScreenLocked
#include <VersionHelpers.h> // Version
#include <time.h>
#include <string>

namespace CG
{
	class System
	{
	public:

		static OSVERSIONINFOW Version()
		{
			OSVERSIONINFOW info = { 0 };
			info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
			GetVersionExW(&info);
			return info;
		}

		struct TimeStruct
		{
			UINT year;
			UINT mon;
			UINT day;
			UINT hour;
			UINT min;
			UINT sec;
		};

		static TimeStruct Time()
		{
			TimeStruct t;

			time_t ts = time(0);
			tm tf;
			localtime_s(&tf, &ts);

			t.year = tf.tm_year + 1900;
			t.mon = tf.tm_mon + 1;
			t.day = tf.tm_mday;
			t.hour = tf.tm_hour;
			t.min = tf.tm_min;
			t.sec = tf.tm_sec;

			return t;
		}

		static SIZE screenSize()
		{
			HWND wnd = GetDesktopWindow();
			HMONITOR mt = MonitorFromWindow(wnd, MONITOR_DEFAULTTONEAREST);

			MONITORINFOEX mti;
			mti.cbSize = sizeof(MONITORINFOEX);
			GetMonitorInfoW(mt, &mti);

			DEVMODEW dm;
			dm.dmSize = sizeof(DEVMODEW);
			dm.dmDriverExtra = 0;
			EnumDisplaySettingsW(mti.szDevice, ENUM_CURRENT_SETTINGS, &dm);

			return { (LONG)dm.dmPelsWidth, (LONG)dm.dmPelsHeight };
		}

		static SIZE screenVSize() { return { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) }; }
		static SIZE screenVSize(HDC dc) { return { GetDeviceCaps(dc, HORZRES), GetDeviceCaps(dc, VERTRES) }; }

		static double screenZoomRote() {
			HWND wnd = GetDesktopWindow();
			HMONITOR mt = MonitorFromWindow(wnd, MONITOR_DEFAULTTONEAREST);

			MONITORINFOEX mti;
			mti.cbSize = sizeof(MONITORINFOEX);
			GetMonitorInfoW(mt, &mti);
			int ylogi = mti.rcMonitor.bottom - mti.rcMonitor.top;

			DEVMODEW dm;
			dm.dmSize = sizeof(DEVMODEW);
			dm.dmDriverExtra = 0;
			EnumDisplaySettingsW(mti.szDevice, ENUM_CURRENT_SETTINGS, &dm);
			int yphy = dm.dmPelsHeight;

			double v = (double)yphy / (double)ylogi;
			return v;
		}

		static std::wstring environmentVariable(LPCWSTR name) { WCHAR str[MAX_PATH]; GetEnvironmentVariableW(name, str, MAX_PATH); return str; }

		static bool isScreenLocked() {
			typedef BOOL(PASCAL* WTSQuerySessionInformationW)(HANDLE hServer, DWORD SessionId, WTS_INFO_CLASS WTSInfoClass, LPWSTR* ppBuffer, DWORD* pBytesReturned);
			typedef void (PASCAL* WTSFreeMemory)(PVOID pMemory);

			WTSINFOEXW* pInfo = 0;
			WTS_INFO_CLASS wtsic = WTSSessionInfoEx;
			bool bRet = 0;
			LPWSTR ppBuffer = 0;
			DWORD dwBytesReturned = 0;
			LONG dwFlags = 0;
			WTSQuerySessionInformationW pWTSQuerySessionInformation = 0;
			WTSFreeMemory pWTSFreeMemory = 0;

			HMODULE hLib = LoadLibraryW(L"wtsapi32.dll");
			if (!hLib) {
				return 0;
			}
			pWTSQuerySessionInformation = (WTSQuerySessionInformationW)GetProcAddress(hLib, "WTSQuerySessionInformationW");
			if (pWTSQuerySessionInformation)
			{
				pWTSFreeMemory = (WTSFreeMemory)GetProcAddress(hLib, "WTSFreeMemory");
				if (pWTSFreeMemory != 0)
				{
					DWORD dwSessionID = WTSGetActiveConsoleSessionId();
					if (pWTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, dwSessionID, wtsic, &ppBuffer, &dwBytesReturned))
					{
						if (dwBytesReturned > 0)
						{
							pInfo = (WTSINFOEXW*)ppBuffer;
							if (pInfo->Level == 1)
							{
								dwFlags = pInfo->Data.WTSInfoExLevel1.SessionFlags;
							}
							if (dwFlags == WTS_SESSIONSTATE_LOCK)
							{
								bRet = 1;
							}
						}
						pWTSFreeMemory(ppBuffer);
						ppBuffer = 0;
					}
				}
			}
			if (hLib) {
				FreeLibrary(hLib);
			}
			return bRet;
		}

		static bool ClipBoardText(LPCWSTR str) {
			size_t len = (wcslen(str) + 1) * sizeof(wchar_t);
			HANDLE hGlobalMemory = GlobalAlloc(GMEM_MOVEABLE, len);
			if (!hGlobalMemory) return 0;

			HANDLE pGlobalMemory = GlobalLock(hGlobalMemory);
			if (!pGlobalMemory) return 0;

			wcscpy_s((LPWSTR)pGlobalMemory, len, str);
			GlobalUnlock(pGlobalMemory);

			if (!OpenClipboard(0)) return 0;

			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hGlobalMemory);
			CloseClipboard();

			return 1;
		}
	};
}