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
		static bool open(const std::wstring& url)
		{
			if (url.empty()) return false;
			std::wstring file;
			std::wstring args;
			size_t exePos = url.find(L".exe");
			if (exePos == std::wstring::npos) exePos = url.find(L".EXE");
			if (exePos != std::wstring::npos)
			{
				size_t argPos = url.find(L" args:", exePos);
				if (argPos == std::wstring::npos) argPos = url.find(L" ARGS:");
				if (argPos != std::wstring::npos)
				{
					file = url.substr(0, exePos + 4);
					args = url.substr(argPos + 6);
				}
			}
			if (file.empty()) file = url;
			return reinterpret_cast<size_t>(ShellExecuteW(nullptr, L"open", file.c_str(), args.empty() ? nullptr : args.c_str(), nullptr, SW_SHOW)) > 32;
		}
		static std::wstring path(HANDLE process)
		{
			std::wstring fullPath;
			if (process && process != INVALID_HANDLE_VALUE)
			{
				DWORD len = MAX_PATH;
				wchar_t buf[MAX_PATH];
				if (QueryFullProcessImageNameW(process, 0, buf, &len)) fullPath = buf;
			}
			return fullPath;
		}
		static std::wstring path(DWORD processId)
		{
			std::wstring fullPath;
			HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, 0, processId);
			if (process && process != INVALID_HANDLE_VALUE)
			{
				fullPath = path(process);
				CloseHandle(process);
			}
			return fullPath;
		}
		static size_t find(const std::wstring& exe) {
			std::wstring exePath = String::toUpper(exe);
			size_t pos = 0; while ((pos = exePath.find('/', pos)) != std::string::npos) { exePath.replace(pos, 1, L"\\"); pos += 1; }

			PROCESSENTRY32W pe = { 0 };
			pe.dwSize = sizeof(PROCESSENTRY32W);
			HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			size_t count = 0;

			Process32FirstW(hProcess, &pe);
			do {
				std::wstring name = String::toUpper(pe.szExeFile);
				if (exePath.find(name) != std::wstring::npos)
				{
					if (exePath.find('\\') == std::wstring::npos) count += exePath == name;
					else count += exePath == String::toUpper(path(pe.th32ProcessID));
				}
			} while (Process32NextW(hProcess, &pe));
			return count;
		}
		static size_t close(const std::wstring& exe) {
			std::wstring exePath = String::toUpper(exe);
			size_t pos = 0; while ((pos = exePath.find('/', pos)) != std::string::npos) { exePath.replace(pos, 1, L"\\"); pos += 1; }

			PROCESSENTRY32W pe = { 0 }; pe.dwSize = sizeof(PROCESSENTRY32W);
			HANDLE hProcShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			size_t count = 0;

			Process32FirstW(hProcShot, &pe);
			do {
				std::wstring name = String::toUpper(pe.szExeFile);
				if (exePath.find(name) != std::wstring::npos)
				{
					bool find = false;
					if (exePath.find('\\') == std::wstring::npos) find = exePath == name;
					else find = exePath == String::toUpper(path(pe.th32ProcessID));
					if (find)
					{
						HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, 0, pe.th32ProcessID);
						if (process && process != INVALID_HANDLE_VALUE)
						{
							count += TerminateProcess(process, 0) == TRUE;
							CloseHandle(process);
						}
					}
				}
			} while (Process32NextW(hProcShot, &pe));
			return count;
		}
	};
}