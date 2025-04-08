﻿#pragma once
#include <string>
#include <windows.h>
#include <winternl.h>
#include <qdesktopservices.h>
#include <src/picosha2/picosha2.h>
#include <src/tools/string.h>
#include <src/tools/file.h>
#include <src/tools/process.h>
#pragma comment(lib,"ntdll.lib")

typedef struct _PROCESS_BASIC_INFORMATION_EX {
	PVOID Reserved1;
	PPEB PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	DWORD ParentProcessId;
} PROCESS_BASIC_INFORMATION_EX;

#pragma optimize("",off)
static const char* integrity_verify_GetTextSha256()
{
	static const char textSha256[] = "3a52168b64dd05ac32489a5b96ff7cf885352ce6239d19d4ff0736b8ef23690d";
	return textSha256;
}
#pragma optimize("",on)

static std::string integrity_verify_Sha256TextSection(std::wstring filePath = std::wstring()) {
	if (filePath.empty())
	{
		wchar_t path[0x7FFF];
		GetModuleFileNameW(nullptr, path, 0x7FFF);
		filePath = path;
	}
	std::vector<BYTE> result;
	HANDLE hFile = nullptr;
	HANDLE hFileMapping = nullptr;
	LPVOID pBaseAddress = nullptr;

	try {
		hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (hFile == INVALID_HANDLE_VALUE) throw std::runtime_error("无法打开文件");

		hFileMapping = CreateFileMappingW(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);

		if (hFileMapping == nullptr) throw std::runtime_error("无法创建文件映射");

		pBaseAddress = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);

		if (pBaseAddress == nullptr) throw std::runtime_error("无法映射文件视图");

		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pBaseAddress;
		if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) throw std::runtime_error("无效的DOS头");

		PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)pBaseAddress + pDosHeader->e_lfanew);
		if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) throw std::runtime_error("无效的NT头");

		PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
		WORD numSections = pNtHeaders->FileHeader.NumberOfSections;

		for (WORD i = 0; i < numSections; ++i) {
			if (strcmp((const char*)pSectionHeader[i].Name, ".text") == 0) {
				DWORD sizeToRead = pSectionHeader[i].SizeOfRawData;
				if (sizeToRead > 1024) sizeToRead = 1024;
				BYTE* textSectionStart = (BYTE*)pBaseAddress + pSectionHeader[i].PointerToRawData;
				result.assign(textSectionStart, textSectionStart + sizeToRead);
				break;
			}
		}

		if (result.empty()) throw std::runtime_error("未找到.text段");
	}
	catch (const std::exception& e)
	{
		MessageBoxW(nullptr, L"程序已损坏", L"Error", MB_ICONERROR);
		exit(0);
	}

	if (pBaseAddress) UnmapViewOfFile(pBaseAddress);
	if (hFileMapping) CloseHandle(hFileMapping);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	if (result.empty()) return std::string();
	return picosha2::hash256_hex_string(result.begin(), result.end());
}

static void integrity_verify()
{
	bool failed = false;
	bool env = false;
	do
	{
		std::wstring exeName = QiTools::Process::exeName();
		std::wstring exePath = QiTools::Process::exePath();
		std::wstring runPath = QiTools::Process::runPath();
		// 从内存创建
		if (true)
		{
			if (!QiTools::File::PathState(exePath) || !QiTools::File::PathState(runPath) || !QiTools::Process::find(exeName))
			{
				failed = true;
				break;
			}
		}
		// 释放到隐藏目录
		if (true)
		{
			DWORD file = GetFileAttributesW(exePath.c_str());
			DWORD folder = GetFileAttributesW(runPath.c_str());
			if ((file == FILE_ATTRIBUTE_HIDDEN) || (file == FILE_ATTRIBUTE_SYSTEM) || (folder == FILE_ATTRIBUTE_HIDDEN))
			{
				failed = true;
				break;
			}
		}
		// 父进程
		if (true)
		{
			PROCESS_BASIC_INFORMATION_EX pbi;
			NTSTATUS status = NtQueryInformationProcess(GetCurrentProcess(), ProcessBasicInformation, &pbi, sizeof(pbi), NULL);
			if (status != 0)
			{
				failed = env = true;
				break;
			}

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pbi.ParentProcessId);
			if (!hProcess)
			{
				failed = env = true;
				break;
			}
			WCHAR path[MAX_PATH];
			DWORD size = MAX_PATH;
			if (!QueryFullProcessImageNameW(hProcess, 0, path, &size))
			{
				CloseHandle(hProcess);
				failed = env = true;
				break;
			}
			CloseHandle(hProcess);

			bool exist = false;
			do
			{
				WCHAR envPath[MAX_PATH];
				ExpandEnvironmentStringsW(L"%windir%\\explorer.exe", envPath, MAX_PATH);
				if (_wcsicmp(path, envPath) == 0)
				{
					exist = true;
					break;
				}
				ExpandEnvironmentStringsW(L"%windir%\\System32\\svchost.exe", envPath, MAX_PATH);
				if (_wcsicmp(path, envPath) == 0)
				{
					exist = true;
					break;
				}
				if (std::wstring(path).find(L"devenv.exe") != std::wstring::npos ||
					std::wstring(path).find(L"7zFM.exe") != std::wstring::npos ||
					std::wstring(path).find(L"zip") != std::wstring::npos ||
					std::wstring(path).find(L"Zip") != std::wstring::npos ||
					std::wstring(path).find(L"ZIP") != std::wstring::npos ||
					std::wstring(path).find(L"rar") != std::wstring::npos ||
					std::wstring(path).find(L"Rar") != std::wstring::npos ||
					std::wstring(path).find(L"RAR") != std::wstring::npos ||
					std::wstring(path).find(L"压缩") != std::wstring::npos ||
					std::wstring(path).find(L"压.exe") != std::wstring::npos
					)
				{
					exist = true;
					break;
				}
			} while (false);
			if (!exist)
			{
				failed = env = true;
				break;
			}
		}
		// 植入了代码段
		if (true)
		{
			std::string sha256 = integrity_verify_Sha256TextSection();
			if (sha256.empty())
			{
				MessageBoxW(nullptr, L"程序完整性验证错误", L"Error", MB_ICONERROR);
				exit(0);
			}
			if (sha256 != integrity_verify_GetTextSha256())
			{
				failed = true;
				break;
			}
		}
	} while (false);
	if (failed)
	{
		if (env)
		{
			MessageBoxW(nullptr, L"程序启动环境异常\n访问http://qinput.cyk.moe\n获取技术支持/商业授权", L"请勿私自销售软件", MB_ICONERROR);
			ShellExecuteW(nullptr, L"open", L"http://qinput.cyk.moe", nullptr, nullptr, SW_HIDE);
			exit(0);
		}
		else
		{
			MessageBoxW(nullptr, L"程序已损坏\n访问http://qinput.cyk.moe\n获取技术支持/商业授权", L"请勿私自销售软件", MB_ICONERROR);
			ShellExecuteW(nullptr, L"open", L"http://qinput.cyk.moe", nullptr, nullptr, SW_HIDE);
			exit(0);
		}
	}
}