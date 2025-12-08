#pragma once
#include <string>
#include <windows.h>
#include <winternl.h>
#include <qdesktopservices.h>
#include <src/picosha2/picosha2.h>
#include <src/tools/string.h>
#include <src/tools/file.h>
#include <src/tools/process.h>
#include "integrity_verify.h"

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
	catch (...)
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
	std::function error = [] { MessageBoxW(nullptr, L"程序异常", L"错误", MB_ICONERROR), exit(0); };
	std::wstring exeName = QiTools::Process::exeName();
	std::wstring exePath = QiTools::Process::exePath();
	std::wstring runPath = QiTools::Process::runPath();

	if (exePath.size() < 4 || exePath.substr(exePath.size() - 4) != L".exe") error();

	if (!QiTools::File::FileState(exePath) || !QiTools::File::PathState(runPath) || !QiTools::Process::find(exeName)) error();
	if (QiTools::File::FolderIsSystem(runPath) || QiTools::File::FileIsHide(exePath) || QiTools::File::FileIsSystem(exePath)) error();

	std::string sha256 = integrity_verify_Sha256TextSection(exePath);
	if (sha256.empty() || sha256 != integrity_verify_textSha256) error();
}