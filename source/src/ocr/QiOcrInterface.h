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
	virtual void release() = 0;
};

using PFQiOcrInterfaceVersion = size_t(*)();
using PFQiOcrInterfaceInit = QiOcrInterface * (*)(size_t);
using PFQiOcrInterfaceInitFromMemory = QiOcrInterface * (*)(void*, size_t, void*, size_t, void*, size_t, size_t);

class QiOcrModule
{
	HMODULE dll = nullptr;
	QiOcrInterface* ocr = nullptr;
public:
	QiOcrModule() : dll(nullptr), ocr(nullptr) {}
	QiOcrModule(const QiOcrModule&) = delete;
	QiOcrModule(QiOcrModule&& other) { operator=(std::move(other)); }
	QiOcrModule(HMODULE dll, QiOcrInterface* ocr) : dll(dll), ocr(ocr) {}
	~QiOcrModule() { release(); }
	QiOcrModule& operator=(const QiOcrModule&) = delete;
	QiOcrModule& operator=(QiOcrModule&& other)
	{
		release();
		dll = other.dll;
		ocr = other.ocr;
		other.dll = nullptr;
		other.ocr = nullptr;
		return *this;
	}
	void release()
	{
		if (ocr)
		{
			ocr->release();
			delete ocr;
			ocr = nullptr;
		}
		if (dll)
		{
			FreeLibrary(dll);
			dll = nullptr;
		}
	}
	bool valid() const
	{
		return dll && ocr;
	}
	std::vector<std::string> scan_list(const CImage& image, bool skipDet = false)
	{
		if (!valid()) return {};
		return ocr->scan_list(image, skipDet);
	}
	std::vector<std::string> scan_list(const RECT& rect_screen, bool skipDet = false)
	{
		if (!valid()) return {};
		return ocr->scan_list(rect_screen, skipDet);
	}
	std::string scan(const CImage& image, bool skipDet = false)
	{
		if (!valid()) return {};
		return ocr->scan(image, skipDet);
	}
	std::string scan(const RECT& rect_screen, bool skipDet = false)
	{
		if (!valid()) return {};
		return ocr->scan(rect_screen, skipDet);
	}
};

#ifdef QIOCR_SHARED
inline size_t QiOcrInterfaceVersion()
{
	HMODULE hModule = LoadLibraryW(L"qiocr.dll");
	if (!hModule)
	{
		hModule = LoadLibraryW(L"OCR\\qiocr.dll");
		if (!hModule) return 0;
	}
	size_t version = 0;
	PFQiOcrInterfaceVersion pFunction = (PFQiOcrInterfaceVersion)GetProcAddress(hModule, "QiOcrInterfaceVersionInterface");
	if (pFunction) version = pFunction();
	FreeLibrary(hModule);
	return version;
}
inline QiOcrModule QiOcrInterfaceInit(size_t threads = 0)
{
	HMODULE hModule = LoadLibraryW(L"qiocr.dll");
	if (!hModule)
	{
		hModule = LoadLibraryW(L"OCR\\qiocr.dll");
		if (!hModule) return QiOcrModule();
	}
	PFQiOcrInterfaceInit pFunction = (PFQiOcrInterfaceInit)GetProcAddress(hModule, "QiOcrInterfaceInitInterface");
	if (!pFunction)
	{
		FreeLibrary(hModule);
		return QiOcrModule();
	}
	QiOcrInterface* pInterface = pFunction(threads);
	if (!pInterface)
	{
		FreeLibrary(hModule);
		return QiOcrModule();
	}
	return QiOcrModule(hModule, pInterface);
}
inline QiOcrModule QiOcrInterfaceInit(void* recData, size_t recSize, void* keysData, size_t keysSize, void* detData, size_t detSize, size_t threads = 0)
{
	HMODULE hModule = LoadLibraryW(L"qiocr.dll");
	if (!hModule)
	{
		hModule = LoadLibraryW(L"OCR\\qiocr.dll");
		if (!hModule) return QiOcrModule();
	}
	PFQiOcrInterfaceInitFromMemory pFunction = (PFQiOcrInterfaceInitFromMemory)GetProcAddress(hModule, "QiOcrInterfaceInitInterfaceFromMemory");
	if (!pFunction)
	{
		FreeLibrary(hModule);
		return QiOcrModule();
	}
	QiOcrInterface* pInterface = pFunction(recData, recSize, keysData, keysSize, detData, detSize, threads);
	if (!pInterface)
	{
		FreeLibrary(hModule);
		return QiOcrModule();
	}
	return QiOcrModule(hModule, pInterface);
}
#else
size_t QiOcrInterfaceVersion();
QiOcrInterface* QiOcrInterfaceInit(size_t threads = 0);
QiOcrInterface* QiOcrInterfaceInit(void* recData, size_t recSize, void* keysData, size_t keysSize, void* detData, size_t detSize, size_t threads = 0);
#endif