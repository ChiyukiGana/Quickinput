#pragma once
#include <windows.h>
#include <Shlobj.h>
#include <string>
namespace QiTools
{
	class System
	{
	public:
		static void enumScreen(std::function<bool(HMONITOR h, const MONITORINFOEXW& m)> f)
		{
			using pf = std::function<bool(HMONITOR, const MONITORINFOEXW&)>*;
			EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR hm, HDC, LPRECT, LPARAM p) {
				MONITORINFOEXW mti{};
				mti.cbSize = sizeof(MONITORINFOEXW);
				GetMonitorInfoW(hm, &mti);
				if (mti.szDevice[0] != 0 && (*reinterpret_cast<pf>(p))(hm, mti)) return FALSE;
				return TRUE;
				}, reinterpret_cast<LPARAM>(&f));
		}
		static SIZE physPrimaryScreenSize()
		{
			SIZE size{};
			enumScreen([&size](HMONITOR h, const MONITORINFOEXW& m) {
				if (m.dwFlags & MONITORINFOF_PRIMARY)
				{
					DEVMODEW dm{};
					dm.dmSize = sizeof(DEVMODEW);
					EnumDisplaySettingsW(m.szDevice, ENUM_CURRENT_SETTINGS, &dm);
					size = { (LONG)dm.dmPelsWidth, (LONG)dm.dmPelsHeight };
					return true;
				}
				return false;
				});
			return size;
		}
		static RECT physPrimaryScreenRect()
		{
			SIZE size{};
			enumScreen([&size](HMONITOR h, const MONITORINFOEXW& m) {
				if (m.dwFlags & MONITORINFOF_PRIMARY)
				{
					DEVMODEW dm{};
					dm.dmSize = sizeof(DEVMODEW);
					EnumDisplaySettingsW(m.szDevice, ENUM_CURRENT_SETTINGS, &dm);
					size = { (LONG)dm.dmPelsWidth, (LONG)dm.dmPelsHeight };
					return true;
				}
				return false;
				});
			return { 0, 0, size.cx, size.cy };
		}
		static SIZE logicPrimaryScreenSize()
		{
			SIZE size{};
			enumScreen([&size](HMONITOR h, const MONITORINFOEXW& m) {
				if (m.dwFlags & MONITORINFOF_PRIMARY)
				{
					size = { m.rcMonitor.right - m.rcMonitor.left, m.rcMonitor.bottom - m.rcMonitor.top };
					return true;
				}
				return false;
				});
			return size;
		}
		static RECT logicPrimaryScreenRect()
		{
			RECT rect{};
			enumScreen([&rect](HMONITOR h, const MONITORINFOEXW& m) {
				if (m.dwFlags & MONITORINFOF_PRIMARY)
				{
					rect = m.rcMonitor;
					return true;
				}
				return false;
				});
			return rect;
		}
		static RECT virtualScreenRect()
		{
			const int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
			const int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
			return { x, y, x + GetSystemMetrics(SM_CXVIRTUALSCREEN) - 1, y + GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1 };
		}
		static float primaryScreenScale()
		{
			float scale{};
			enumScreen([&scale](HMONITOR h, const MONITORINFOEXW& m) {
				if (m.dwFlags & MONITORINFOF_PRIMARY)
				{
					DEVMODEW dm{};
					dm.dmSize = sizeof(DEVMODEW);
					EnumDisplaySettingsW(m.szDevice, ENUM_CURRENT_SETTINGS, &dm);
					scale = static_cast<float>(dm.dmPelsWidth) / (m.rcMonitor.right - m.rcMonitor.left);
					return true;
				}
				return false;
				});
			return scale;
		}
		static float screenScaleRote() {
			HMONITOR hmt = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
			MONITORINFOEXW mti = {}; mti.cbSize = sizeof(MONITORINFOEXW); GetMonitorInfoW(hmt, &mti);
			DEVMODEW dm = { sizeof(DEVMODEW) }; EnumDisplaySettingsW(mti.szDevice, ENUM_CURRENT_SETTINGS, &dm);
			return (float)(dm.dmPelsHeight) / (float)(mti.rcMonitor.bottom - mti.rcMonitor.top);
		}
		static std::wstring environmentVariable(LPCWSTR name) { PATH_BUFFERW(str); GetEnvironmentVariableW(name, str, PATH_BUFFER_SIZE); return str; }
		static std::wstring folderPath(int csidl = CSIDL_DESKTOP) { PATH_BUFFERW(str); SHGetFolderPathW(0, csidl, 0, SHGFP_TYPE_CURRENT, str); return str; }
		static bool ClipBoardText(LPCWSTR str) {
			size_t size = (wcslen(str) + 1) * sizeof(wchar_t);
			HANDLE hGlobalMemory = GlobalAlloc(GMEM_MOVEABLE, size);
			if (!hGlobalMemory) return false;
			wchar_t* pTextMemory = reinterpret_cast<wchar_t*>(GlobalLock(hGlobalMemory));
			if (!pTextMemory)
			{
				GlobalFree(hGlobalMemory);
				return false;
			}
			wcscpy_s((wchar_t*)pTextMemory, size, str);
			GlobalUnlock(hGlobalMemory);
			if (!OpenClipboard(nullptr)) return false;
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hGlobalMemory);
			CloseClipboard();
			return true;
		}
		static std::wstring ClipBoardText() {
			if (!OpenClipboard(nullptr)) return std::wstring();
			HANDLE hGlobalMemory = GetClipboardData(CF_UNICODETEXT);
			if (!hGlobalMemory) return std::wstring();
			wchar_t* pTextMemory = reinterpret_cast<wchar_t*>(GlobalLock(hGlobalMemory));
			if (!pTextMemory) return std::wstring();
			size_t size = wcslen(pTextMemory);
			if (!size) return std::wstring();
			std::wstring text(pTextMemory, pTextMemory + size);
			GlobalUnlock(hGlobalMemory);
			CloseClipboard();
			return text;
		}
		static bool ExecuteCmd(std::wstring cmd, std::string& output) {
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.bInheritHandle = TRUE;
			sa.lpSecurityDescriptor = NULL;
			HANDLE hReadPipe, hWritePipe;
			if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) return false;
			STARTUPINFOW si = {};
			PROCESS_INFORMATION pi = {};
			si.cb = sizeof(STARTUPINFOW);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.hStdOutput = hWritePipe;
			si.hStdError = hWritePipe;
			si.wShowWindow = SW_HIDE;
			cmd = std::wstring(L" /c ") + cmd;
			std::unique_ptr<wchar_t[]> cmdLine = std::make_unique<wchar_t[]>(cmd.size() + 1);
			wcscpy_s(cmdLine.get(), cmd.size() + 1, cmd.c_str());
			wchar_t cmdPath[64]; ExpandEnvironmentStringsW(L"%ComSpec%", cmdPath, 64);
			if (CreateProcessW(cmdPath, cmdLine.get(), NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &si, &pi))
			{
				CloseHandle(hWritePipe);
				std::string result;
				char buffer[4096];
				DWORD bytesRead;
				while (true)
				{
					if (!ReadFile(hReadPipe, buffer, 4096, &bytesRead, NULL) || bytesRead == 0) break;
					result.append(buffer, bytesRead);
				}
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				output = result;
				return true;
			}
			CloseHandle(hWritePipe);
			CloseHandle(hReadPipe);
			return false;
		}
	};
}