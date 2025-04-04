#pragma once
#include <windows.h>
#include <TlHelp32.h>
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
		static DWORD find(const std::wstring& exe) {
			PROCESSENTRY32W pe = { 0 };
			pe.dwSize = sizeof(PROCESSENTRY32W);
			HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

			Process32FirstW(hProcess, &pe);
			do
			{
				if (_wcsicmp(pe.szExeFile, exe.c_str()) == 0) return pe.th32ProcessID;
			} while (Process32NextW(hProcess, &pe));
			return 0;
		}
	};
}