#pragma once
#include <string>
#include <windows.h>
#include <winternl.h>
#include <qdesktopservices.h>
#include <src/picosha2/picosha2.h>
#include <src/tools/string.h>
#include <src/tools/file.h>
#include <src/tools/process.h>
#pragma comment(lib,"ntdll.lib")
#include "integrity_verify.h"

#pragma optimize("",off)
static const char integrity_verify_textSha256[] = "c0c6dcb1230bb97e383ce6bf7db9c7d7ad9ae4570ea3df9edaf7dada59b2d617";
#pragma optimize("",on)

struct PROCESS_BASIC_INFORMATION_EX {
	PVOID Reserved1;
	PPEB PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	DWORD ParentProcessId;
};

std::string integrity_verify_Sha256TextSection(std::wstring filePath = std::wstring()) {
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

bool integrity_verify_find(const std::wstring str, const std::wstring str2)
{
	std::wstring lstr = str;
	for (wchar_t& i : lstr) i = (wchar_t)std::tolower(i);
	std::wstring lstr2 = str2;
	for (wchar_t& i : lstr) i = (wchar_t)std::tolower(i);
	return lstr.find(lstr2) != std::string::npos;
}

void integrity_verify()
{
	bool failed = false;
	bool env = false;
	std::wstring parent;
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

			if (integrity_verify_find(path, L"explorer.exe") ||
				integrity_verify_find(path, L"svchost.exe") ||
				integrity_verify_find(path, L"devenv.exe") ||
				integrity_verify_find(path, L"7zFM.exe") ||
				integrity_verify_find(path, L"zip") ||
				integrity_verify_find(path, L"rar") ||
				integrity_verify_find(path, L"压缩") ||
				integrity_verify_find(path, L"压.exe") ||
				integrity_verify_find(path, L"dopus") ||
				integrity_verify_find(path, L"360") ||
				integrity_verify_find(path, L"explorer64.exe"));
			else
			{
				failed = env = true;
				parent = path;
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
			if (sha256 != integrity_verify_textSha256)
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
			const std::wstring text = L"程序启动环境异常，请通过资源管理器运行\n访问http://qinput.cyk.moe\n获取技术支持/商业授权";
			if (parent.empty()) MessageBoxW(nullptr, text.c_str(), L"启动失败", MB_ICONERROR);
			else MessageBoxW(nullptr, (text + std::wstring(L"\n\n 启动进程：") + parent).c_str(), L"启动失败", MB_ICONERROR);
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