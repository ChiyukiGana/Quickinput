#pragma once
#include <windows.h>
#include <timeapi.h>
namespace QiTools {
	class Thread {
	public:
		static HANDLE Start(PTHREAD_START_ROUTINE pfunc, LPVOID lParam = 0) { return CreateThread(0, 0, pfunc, lParam, 0, 0); }
		static BOOL Close(HANDLE hThread, DWORD exitCode = 0) { if (hThread) return TerminateThread(hThread, exitCode); return TRUE; }
		static DWORD Wait(HANDLE hThread, bool resetHandle = true) {
			DWORD exitCode;
			WaitForSingleObject(hThread, INFINITE);
			GetExitCodeThread(hThread, &exitCode);
			if (resetHandle) hThread = nullptr;
			return exitCode;
		}
	};
}