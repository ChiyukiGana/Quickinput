#include <windows.h>
#include <string>

struct QiRawInputInfo
{
	int size;
	int version;
	int isConfiguration;
};

struct QiRawInput
{
	virtual bool init(const std::wstring& subDir) = 0;
	virtual bool isInit() = 0;
	virtual void onload() = 0;
	virtual void release() = 0; // do not use delete QiOcrInterface*
	virtual void configure() = 0;
	virtual void key_up(char key, ULONG_PTR info = 0) = 0;
	virtual void key_down(char key, ULONG_PTR info = 0) = 0;
	virtual void move(int x, int y, ULONG_PTR info = 0) = 0; // use pixel
	virtual void move_to(int x, int y, ULONG_PTR info = 0) = 0; // use screen size
};

using PFQiRawInputInfoQueryInterface = void(*)(QiRawInputInfo*);
using PFQiRawInputInitInterface = QiRawInput * (*)();

class QiRawInputModule
{
	HMODULE dll = nullptr;
	QiRawInput* rawInput = nullptr;
public:
	QiRawInputModule(HMODULE dll = nullptr, QiRawInput* rawInput = nullptr) : dll(dll), rawInput(rawInput) {}
	QiRawInputModule(const QiRawInputModule&) = delete;
	QiRawInputModule(QiRawInputModule&& other) { operator=(std::move(other)); }
	~QiRawInputModule() { release(); }
	QiRawInputModule& operator=(const QiRawInputModule&) = delete;
	QiRawInputModule& operator=(QiRawInputModule&& other)
	{
		release();
		dll = other.dll;
		rawInput = other.rawInput;
		other.dll = nullptr;
		other.rawInput = nullptr;
		return *this;
	}
	bool valid() const { return dll && rawInput; }
	bool init(const std::wstring& subDir) { if (valid()) return rawInput->init(subDir); }
	bool isInit() const { return dll && rawInput && rawInput->isInit(); }
	void onload() { rawInput->onload(); }
	void release()
	{
		if (rawInput)
		{
			rawInput->release();
			rawInput = nullptr;
		}
		if (dll)
		{
			FreeLibrary(dll);
			dll = nullptr;
		}
	}
	void configure() { if (isInit()) rawInput->configure(); }
	void key_up(char key, ULONG_PTR info = 0) { if (isInit()) rawInput->key_up(key, info); }
	void key_down(char key, ULONG_PTR info = 0) { if (isInit()) rawInput->key_down(key, info); }
	void move(int x, int y, ULONG_PTR info = 0) { if (isInit()) rawInput->move(x, y, info); }
	void move_to(int x, int y, ULONG_PTR info = 0) { if (isInit()) rawInput->move_to(x, y, info); }
};

inline bool QiRawInputInfoQuery(const std::wstring& dll, QiRawInputInfo& qiRawInputInfo)
{
	HMODULE hModule = LoadLibraryW(dll.c_str());
	if (hModule)
	{
		PFQiRawInputInfoQueryInterface pFunction = reinterpret_cast<PFQiRawInputInfoQueryInterface>(GetProcAddress(hModule, "QiRawInputInfoQueryInterface"));
		if (pFunction)
		{
			qiRawInputInfo.size = sizeof(QiRawInputInfo);
			pFunction(&qiRawInputInfo);
		}
		FreeLibrary(hModule);
		return pFunction;
	}
	return false;
}

inline QiRawInputModule QiRawInputInit(const std::wstring& dll)
{
	HMODULE hModule = LoadLibraryW(dll.c_str());
	if (hModule)
	{
		PFQiRawInputInitInterface pFunction = reinterpret_cast<PFQiRawInputInitInterface>(GetProcAddress(hModule, "QiRawInputInitInterface"));
		if (pFunction)
		{
			QiRawInput* rawInput = pFunction();
			if (rawInput) return QiRawInputModule(hModule, rawInput);
		}
		FreeLibrary(hModule);
	}
	return QiRawInputModule();
}