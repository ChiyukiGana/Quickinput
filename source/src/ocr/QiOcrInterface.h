#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include <atlimage.h>

struct QiOcrInterface
{
	virtual std::vector<std::string> scan_list(const CImage& image) = 0;
	virtual std::vector<std::string> scan_list(const RECT& rect_screen) = 0;
	virtual std::string scan(const CImage& image) = 0;
	virtual std::string scan(const RECT& rect_screen) = 0;
};

using PFQiOcrInterfaceInit = QiOcrInterface*(*)();

#pragma optimize("",off)
inline QiOcrInterface* QiOcrInterfaceInit()
{
	HMODULE hModule = LoadLibraryW(L"qiocr.dll");
	if (!hModule)
	{
		hModule = LoadLibraryW(L"OCR\\qiocr.dll");
		if (!hModule) return nullptr;
	}
	PFQiOcrInterfaceInit pFunction = (PFQiOcrInterfaceInit)GetProcAddress(hModule, "QiOcrInterfaceInitInterface");
	if (!pFunction)
	{
		FreeLibrary(hModule);
		return nullptr;
	}
	QiOcrInterface* pInterface = pFunction();
	if (!pInterface)
	{
		FreeLibrary(hModule);
		return nullptr;
	}
	return pInterface;
}
#pragma optimize("",on)