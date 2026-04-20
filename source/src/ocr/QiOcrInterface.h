#pragma once
#ifndef QIOCR_H
#define QIOCR_H
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
	virtual std::vector<std::string> scan_list(const CImage& image, bool skipDet = false, std::vector<POINT>* centers = nullptr) = 0;
	virtual std::vector<std::string> scan_list(const RECT& rect_screen, bool skipDet = false, std::vector<POINT>* centers = nullptr) = 0;
	virtual std::string scan(const CImage& image, bool skipDet = false) = 0;
	virtual std::string scan(const RECT& rect_screen, bool skipDet = false) = 0;
	virtual void release() = 0; // do not use delete QiOcrInterface*
};

using PFQiOcrInterfaceVersion = size_t(*)();
using PFQiOcrInterfaceInit = QiOcrInterface * (*)(const std::wstring&, const std::wstring&, const std::wstring&, size_t);
using PFQiOcrInterfaceInitFromMemory = QiOcrInterface * (*)(void*, size_t, void*, size_t, void*, size_t, size_t);

class QiOcrModule
{
#ifdef QIOCR_SHARED
	HMODULE dll = nullptr;
#endif
	QiOcrInterface* ocr = nullptr;
public:
#ifdef QIOCR_SHARED
	QiOcrModule(HMODULE dll = nullptr, QiOcrInterface* ocr = nullptr) : dll(dll), ocr(ocr) {}
#else
	QiOcrModule(QiOcrInterface* ocr = nullptr) : ocr(ocr) {}
#endif
	QiOcrModule(const QiOcrModule&) = delete;
	QiOcrModule(QiOcrModule&& other) { operator=(std::move(other)); }
	~QiOcrModule() { release(); }
	QiOcrModule& operator=(const QiOcrModule&) = delete;
	QiOcrModule& operator=(QiOcrModule&& other)
	{
		release();
#ifdef QIOCR_SHARED
		dll = other.dll;
		other.dll = nullptr;
#endif
		ocr = other.ocr;
		other.ocr = nullptr;
		return *this;
	}
	operator bool() const { return valid(); }
	bool valid() const
	{
		return ocr
#ifdef QIOCR_SHARED
			&& dll
#endif
			;
	}
	void release()
	{
		if (ocr)
		{
			ocr->release();
			ocr = nullptr;
		}
#ifdef QIOCR_SHARED
		if (dll)
		{
			FreeLibrary(dll);
			dll = nullptr;
		}
#endif
	}
	std::vector<std::string> scan_list(const CImage& image, bool skipDet = false, std::vector<POINT>* centers = nullptr)
	{
		if (!valid()) return {};
		return ocr->scan_list(image, skipDet, centers);
	}
	std::vector<std::string> scan_list(const RECT& rect_screen, bool skipDet = false, std::vector<POINT>* centers = nullptr)
	{
		if (!valid()) return {};
		return ocr->scan_list(rect_screen, skipDet, centers);
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
inline size_t QiOcrVersion(const std::wstring dll)
{
	HMODULE hModule = LoadLibraryW(dll.c_str());
	if (hModule)
	{
		PFQiOcrInterfaceVersion pFunction = (PFQiOcrInterfaceVersion)GetProcAddress(hModule, "QiOcrInterfaceVersionInterface");
		size_t version = 0;
		if (pFunction) version = pFunction();
		FreeLibrary(hModule);
		return version;
	}
	return 0;
}
inline QiOcrModule QiOcrInit(const std::wstring dll, const std::wstring& recFile, const std::wstring& keyFile, const std::wstring& detFile, size_t threads)
{
	HMODULE hModule = LoadLibraryW(dll.c_str());
	if (hModule)
	{
		PFQiOcrInterfaceInit pFunction = reinterpret_cast<PFQiOcrInterfaceInit>(GetProcAddress(hModule, "QiOcrInterfaceInitInterface"));
		if (pFunction)
		{
			QiOcrInterface* pInterface = pFunction(recFile, keyFile, detFile, threads);
			if (pInterface) return QiOcrModule(hModule, pInterface);
		}
		FreeLibrary(hModule);
	}
	return QiOcrModule();
}
inline QiOcrModule QiOcrInit(const std::wstring dll, void* recData, size_t recSize, void* keysData, size_t keysSize, void* detData, size_t detSize, size_t threads)
{
	HMODULE hModule = LoadLibraryW(dll.c_str());
	if (hModule)
	{
		PFQiOcrInterfaceInitFromMemory pFunction = reinterpret_cast<PFQiOcrInterfaceInitFromMemory>(GetProcAddress(hModule, "QiOcrInterfaceInitInterfaceFromMemory"));
		if (pFunction)
		{
			QiOcrInterface* pInterface = pFunction(recData, recSize, keysData, keysSize, detData, detSize, threads);
			if (pInterface) return QiOcrModule(hModule, pInterface);
		}
		FreeLibrary(hModule);
	}
	return QiOcrModule();
}
#else
extern "C" size_t _stdcall QiOcrInterfaceVersion();
extern "C" QiOcrInterface* _stdcall QiOcrInterfaceInit(const std::wstring& recFile, const std::wstring& keyFile, const std::wstring& detFile, size_t threads);
extern "C" QiOcrInterface* _stdcall QiOcrInterfaceInitFromMemory(void* recData, size_t recSize, void* keysData, size_t keysSize, void* detData, size_t detSize, size_t threads);
inline QiOcrModule _stdcall QiOcrInit(const std::wstring& recFile, const std::wstring& keyFile, const std::wstring& detFile, size_t threads)
{
	QiOcrInterface* pInterface = QiOcrInterfaceInit(recFile, keyFile, detFile, threads);
	if (pInterface) return QiOcrModule(pInterface);
	return QiOcrModule();
}
inline QiOcrModule _stdcall QiOcrInit(void* recData, size_t recSize, void* keysData, size_t keysSize, void* detData, size_t detSize, size_t threads)
{
	QiOcrInterface* pInterface = QiOcrInterfaceInitFromMemory(recData, recSize, keysData, keysSize, detData, detSize, threads);
	if (pInterface) return QiOcrModule(pInterface);
	return QiOcrModule();
}
#endif
#endif