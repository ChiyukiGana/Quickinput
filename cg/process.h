#pragma once

#include <windows.h>
#include <TlHelp32.h>

#include "base.h"
#include "path.h"
#include "file.h"

namespace CG {
	class Process {
	public:

		typedef List<DWORD> PIDS;

		static std::wstring runPath() { WCHAR path[MAX_PATH]; GetCurrentDirectoryW(MAX_PATH, path); return path; }

		static bool RunPath(std::wstring path = L"") {
			if (path.size()) return SetCurrentDirectoryW(path.c_str());
			else return SetCurrentDirectoryW(Path::RemoveFile(exePath()).c_str());
		}

		static std::wstring exePath() { WCHAR path[MAX_PATH]; GetModuleFileNameW(0, path, MAX_PATH); return path; }

		static std::wstring exeName() { return Path::Last(exePath()); }

		static bool isRunning(LPCWSTR mutexName) { HANDLE handle = CreateMutexW(0, 0, mutexName); if (GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(handle); return 1; } CloseHandle(handle); return 0; }
		static void RunOnce(LPCWSTR mutexName) { HANDLE handle = CreateMutexW(0, 0, mutexName); if (GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(handle); exit(0); } }

		// include .exe, return pid
		static DWORD State(const std::wstring exe) {
			PROCESSENTRY32W pe = { 0 };
			pe.dwSize = sizeof(PROCESSENTRY32W);
			HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

			Process32FirstW(hProcess, &pe);
			do
			{
				if (String::toLower(pe.szExeFile) == String::toLower(exe)) return pe.th32ProcessID;
			} while (Process32NextW(hProcess, &pe));
			return 0;
		}

		// include .exe, return pids
		static PIDS Find(LPCWSTR exe) {
			PIDS pids;
			PROCESSENTRY32W pe = { 0 };
			pe.dwSize = sizeof(PROCESSENTRY32W);
			HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

			Process32FirstW(hProcess, &pe);
			do {
				if (String::toLower(pe.szExeFile) == String::toLower(exe)) pids.Add(pe.th32ProcessID);
			} while (Process32NextW(hProcess, &pe));
			return pids;
		}
		/* file: C:\A\1.exe, args : -p */
		static bool Open(std::wstring file, LPCWSTR args, UINT show = SW_SHOW, LPCWSTR workPath = 0) {
			if (workPath) return ShellExecuteW(0, L"open", file.c_str(), args, workPath, show);
			else return ShellExecuteW(0, L"open", file.c_str(), args, Path::RemoveFile(file).c_str(), show);
		}
		/* cmdLine: "C:\A\1.exe" -p */
		static bool OpenC(std::wstring cmdLine, UINT show = SW_SHOW, LPCWSTR workPath = 0) {
			std::wstring file = Path::RemoveArgs(cmdLine);
			std::wstring args = Path::args(cmdLine);
			if (workPath) return ShellExecuteW(0, L"open", file.c_str(), args.c_str(), workPath, show);
			else return ShellExecuteW(0, L"open", file.c_str(), args.c_str(), Path::RemoveFile(file).c_str(), show);
		}
		/* file: C:\A\1.exe, args: -p */
		static bool Start(std::wstring file, LPCWSTR args, UINT show = SW_SHOW, LPCWSTR workPath = 0, DWORD creationFlags = CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT)
		{
			STARTUPINFO si = {}; si.cb = sizeof(STARTUPINFO); si.wShowWindow = show;
			PROCESS_INFORMATION pi = {};
			BOOL b = 0;
			if (workPath) b = CreateProcessW(file.c_str(), (LPWSTR)args, 0, 0, 0, creationFlags, 0, workPath, &si, &pi);
			else b = (CreateProcessW(file.c_str(), (LPWSTR)args, 0, 0, 0, creationFlags, 0, Path::RemoveFile(file).c_str(), &si, &pi));
			if (b)
			{
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				return true;
			}
			return false;
		}
		/* cmdLine: "C:\A\1.exe" - p */
		static bool StartC(std::wstring cmdLine, UINT show = SW_SHOW, LPCWSTR workPath = 0, DWORD creationFlags = CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT) {
			STARTUPINFO si = {}; si.cb = sizeof(STARTUPINFO); si.wShowWindow = show;
			PROCESS_INFORMATION pi = {};
			BOOL b = 0;
			if (workPath) b = CreateProcessW(0, (LPWSTR)cmdLine.c_str(), 0, 0, 0, creationFlags, 0, workPath, &si, &pi);
			else b = (CreateProcessW(0, (LPWSTR)cmdLine.c_str(), 0, 0, 0, creationFlags, 0, Path::RemoveFile(Path::RemoveArgs(cmdLine)).c_str(), &si, &pi));
			if (b)
			{
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				return true;
			}
			return false;
		}
		
		static void Close(std::wstring exe) {
			PROCESSENTRY32W pe = { 0 }; pe.dwSize = sizeof(PROCESSENTRY32W);
			HANDLE hProcShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

			Process32FirstW(hProcShot, &pe);
			do {
				if (String::toLower(pe.szExeFile) == String::toLower(exe)) {
					TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, 0, pe.th32ProcessID), 0);
				}
			} while (Process32NextW(hProcShot, &pe));
		}
		/* file: C:\A\1.exe, args: -p */
		static bool StartWithSystem(LPCWSTR file, LPCWSTR args = 0, LPCWSTR workPath = 0, DWORD creationFlags = CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT)
		{
			DWORD pid = 0;
			PROCESSENTRY32W pe = {};
			pe.dwSize = sizeof(PROCESSENTRY32W);
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (Process32FirstW(hSnapshot, &pe))
			{
				do { if (!wcscmp(pe.szExeFile, L"winlogon.exe")) { pid = pe.th32ProcessID; break; } } while (Process32NextW(hSnapshot, &pe));
				if (pid)
				{
					HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
					if (hProcess)
					{
						HANDLE hToken = 0;
						OpenProcessToken(hProcess, TOKEN_DUPLICATE, &hToken);
						CloseHandle(hProcess);
						if (hToken)
						{
							HANDLE hNewToken = 0;
							DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, 0, SecurityIdentification, TokenPrimary, &hNewToken);
							CloseHandle(hToken);
							STARTUPINFOW si = {}; si.cb = sizeof(STARTUPINFOW); si.wShowWindow = SW_SHOW;
							PROCESS_INFORMATION pi = {};
							BOOL b = 0;
							if (workPath) b = CreateProcessWithTokenW(hNewToken, LOGON_NETCREDENTIALS_ONLY, file, (LPWSTR)args, creationFlags, 0, workPath, &si, &pi);
							else b = CreateProcessWithTokenW(hNewToken, LOGON_NETCREDENTIALS_ONLY, file, (LPWSTR)args, creationFlags, 0, Path::RemoveFile(file).c_str(), &si, &pi);
							CloseHandle(hNewToken);
							if (b)
							{
								CloseHandle(pi.hThread);
								CloseHandle(pi.hProcess);
								return true;
							}
						}
					}
				}
			}
			return false;
		}
		/* cmdLine: "C:\A\1.exe" - p */
		static bool StartWithSystemC(LPCWSTR cmdLine, LPCWSTR workPath = 0, DWORD creationFlags = CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT)
		{
			DWORD pid = 0;
			PROCESSENTRY32W pe = {};
			pe.dwSize = sizeof(PROCESSENTRY32W);
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (Process32FirstW(hSnapshot, &pe))
			{
				do { if (!wcscmp(pe.szExeFile, L"winlogon.exe")) { pid = pe.th32ProcessID; break; } } while (Process32NextW(hSnapshot, &pe));
				if (pid)
				{
					HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
					if (hProcess)
					{
						HANDLE hToken = 0;
						OpenProcessToken(hProcess, TOKEN_DUPLICATE, &hToken);
						CloseHandle(hProcess);
						if (hToken)
						{
							HANDLE hNewToken = 0;
							DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, 0, SecurityIdentification, TokenPrimary, &hNewToken);
							CloseHandle(hToken);
							STARTUPINFOW si = {}; si.cb = sizeof(STARTUPINFOW); si.wShowWindow = SW_SHOW;
							PROCESS_INFORMATION pi = {};
							BOOL b = 0;
							if (workPath) b = CreateProcessWithTokenW(hNewToken, LOGON_NETCREDENTIALS_ONLY, 0, (LPWSTR)cmdLine, creationFlags, 0, workPath, &si, &pi);
							else b = CreateProcessWithTokenW(hNewToken, LOGON_NETCREDENTIALS_ONLY, 0, (LPWSTR)cmdLine, creationFlags, 0, Path::RemoveFile(cmdLine).c_str(), &si, &pi);
							CloseHandle(hToken);
							if (b)
							{
								CloseHandle(pi.hThread);
								CloseHandle(pi.hProcess);
								return true;
							}
						}
					}
				}
			}
			return false;
		}
	};
}