#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include <atlimage.h>

#ifndef QIOCR_SHARED
#pragma comment(lib,"onnxruntime.lib")
#pragma comment(lib,"opencv_core4110.lib")
#pragma comment(lib,"opencv_imgproc4110.lib")
#pragma comment(lib,"zlib.lib")
#pragma comment(lib,"qiocr.lib")
#endif

struct QiOcrInterface
{
	virtual std::vector<std::string> scan_list(const CImage& image, bool skipDet = false) = 0;
	virtual std::vector<std::string> scan_list(const RECT& rect_screen, bool skipDet = false) = 0;
	virtual std::string scan(const CImage& image, bool skipDet = false) = 0;
	virtual std::string scan(const RECT& rect_screen, bool skipDet = false) = 0;
};

using PFQiOcrInterfaceInit = QiOcrInterface*(*)();
using PFQiOcrInterfaceInitFromMemory = QiOcrInterface * (*)(void*, size_t, void*, size_t, void*, size_t);

#ifdef QIOCR_SHARED
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
inline QiOcrInterface* QiOcrInterfaceInit(void* recData, size_t recSize, void* keysData, size_t keysSize, void* detData, size_t detSize)
{
	HMODULE hModule = LoadLibraryW(L"qiocr.dll");
	if (!hModule)
	{
		hModule = LoadLibraryW(L"OCR\\qiocr.dll");
		if (!hModule) return nullptr;
	}
	PFQiOcrInterfaceInitFromMemory pFunction = (PFQiOcrInterfaceInitFromMemory)GetProcAddress(hModule, "QiOcrInterfaceInitInterfaceFromMemory");
	if (!pFunction)
	{
		FreeLibrary(hModule);
		return nullptr;
	}
	QiOcrInterface* pInterface = pFunction(recData, recSize, keysData, keysSize, detData, detSize);
	if (!pInterface)
	{
		FreeLibrary(hModule);
		return nullptr;
	}
	return pInterface;
}
#else
QiOcrInterface* QiOcrInterfaceInit();
QiOcrInterface* QiOcrInterfaceInit(void* recData, size_t recSize, void* keysData, size_t keysSize, void* detData, size_t detSize);
#endif