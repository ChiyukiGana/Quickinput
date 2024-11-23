#pragma once
#include <windows.h>
#include "path.h"
namespace QiTools
{
	class Process
	{
	public:
		static std::wstring runPath() { WCHAR path[0x7FFF]; GetCurrentDirectoryW(0x7FFF, path); return path; }
		static bool RunPath(std::wstring path = L"") { if (path.size()) return SetCurrentDirectoryW(path.c_str()); else return SetCurrentDirectoryW(Path::RemoveFile(exePath()).c_str()); }
		static std::wstring exePath() { WCHAR path[0x7FFF]; GetModuleFileNameW(0, path, 0x7FFF); return path; }
		static std::wstring exeName() { return Path::GetFile(exePath()); }
		static bool isRunning(LPCWSTR mutexName) { HANDLE handle = CreateMutexW(0, 0, mutexName); if (GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(handle); return true; } CloseHandle(handle); return false; }
		static void RunOnce(LPCWSTR mutexName) { HANDLE handle = CreateMutexW(0, 0, mutexName); if (GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(handle); exit(0); } }
	};
}