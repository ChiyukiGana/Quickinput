#pragma once
#include <string>
#include <windows.h>
class OcrLiteOnnxInterface
{
public:
	virtual bool setModelsDetPath(const std::string& modelDetPath) = 0;
	virtual bool setModelsClsPath(const std::string& modelClsPath) = 0;
	virtual bool setModelsRecPath(const std::string& modelRecPath) = 0;
	virtual bool setModelsKeysPath(const std::string& keysPath) = 0;
	virtual void setThreads(unsigned int threads) = 0;
	virtual void setPadding(unsigned int padding) = 0;
	virtual void setSplitSize(unsigned int splitSize) = 0;
	virtual void setBoxScoreThresh(float boxScoreThresh) = 0;
	virtual void setBoxThresh(float boxThresh) = 0;
	virtual void setUnClipRatio(float unClipRatio) = 0;
	virtual void setDoAngle(bool doAngle) = 0;
	virtual void setMostAngle(bool mostAngle) = 0;
	virtual void setOutputConsole(bool outputConsole) = 0;
	virtual void setOutputImage(bool outputImage) = 0;
	virtual void setOutputResult(bool outputResult) = 0;
	virtual bool init() = 0;
	virtual bool scanScreen(RECT rect, std::string& text) = 0;
	virtual bool scan(std::string image, std::string& text) = 0;
	virtual bool scan(HDC hdc, RECT rect, std::string& text) = 0;
	virtual bool scan(HBITMAP hBitmap, std::string& text) = 0;
	virtual int getVersion() const = 0;
	virtual bool isFailed() const = 0;
	virtual std::string getFailed() const = 0;
};

typedef bool (_stdcall *PLoadOcrLiteOnnInterface)(OcrLiteOnnxInterface**);

inline bool QueryOcrLiteOnnInterface(OcrLiteOnnxInterface** ppOcrLiteOnnxInterface)
{
	HMODULE hModule = LoadLibraryW(L"OcrLiteOnnx.dll");
	if (!hModule)
	{
		hModule = LoadLibraryW(L"OcrLiteOnnx\\OcrLiteOnnx.dll");
		if (!hModule) return false;
	}
	PLoadOcrLiteOnnInterface pFunction = (PLoadOcrLiteOnnInterface)GetProcAddress(hModule, "LoadOcrLiteOnnInterface");
	if (!pFunction)
	{
		FreeLibrary(hModule);
		return false;
	}
	if (!pFunction(ppOcrLiteOnnxInterface))
	{
		FreeLibrary(hModule);
		return false;
	}
	return true;
}