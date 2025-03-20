#pragma once
#include <string>
#include <windows.h>
#include <atlimage.h>
class OnnxOcrInterface
{
public:
	virtual bool isInit() const = 0;
	virtual std::string scan(const CImage& image) = 0;
	virtual std::string scan(const RECT& rect) = 0;
};

using PLoadOnnxOcrModule = OnnxOcrInterface * (*)();
inline OnnxOcrInterface* LoadOcrLiteOnnInterface()
{
	HMODULE hModule = LoadLibraryW(L"OnnxOcr.dll");
	if (!hModule)
	{
		hModule = LoadLibraryW(L"OCR\\OnnxOcr.dll");
		if (!hModule) return nullptr;
	}
	PLoadOnnxOcrModule pFunction = (PLoadOnnxOcrModule)GetProcAddress(hModule, "CreateOcrLiteOnnInterface");
	if (!pFunction)
	{
		FreeLibrary(hModule);
		return nullptr;
	}
	OnnxOcrInterface* pInterface = pFunction();
	if (!pInterface)
	{
		FreeLibrary(hModule);
		return nullptr;
	}
	return pInterface;
}