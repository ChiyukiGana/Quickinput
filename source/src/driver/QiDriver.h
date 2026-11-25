#pragma once
#include <windows.h>
#include <string>

struct QiDriverInfo
{
	int size;
	int version;
	int isConfiguration;
};

struct QiDriver
{
	enum Status : DWORD
	{
		failed,
		not_load,
		not_install,
		ok
	};
	virtual Status init(const std::wstring& subDir) = 0;
	virtual bool isInit() = 0;
	virtual void onload() = 0;
	virtual bool setup(const std::wstring& subDir) = 0;
	virtual void configure() = 0;
	virtual void key_up(char key, ULONG_PTR info = 0) = 0;
	virtual void key_down(char key, ULONG_PTR info = 0) = 0;
	virtual void move(int x, int y, ULONG_PTR info = 0) = 0; // use pixel
	virtual void move_to(int x, int y, ULONG_PTR info = 0) = 0; // use screen size
	virtual void release() = 0; // do not use delete QiQiDriver*
};

using PFQiDriverInfoQueryInterface = void(*)(QiDriverInfo*);
using PFQiDriverInitInterface = QiDriver * (*)();

class QiDriverModule
{
	HMODULE dll = nullptr;
	QiDriver* driver = nullptr;
public:
	QiDriverModule(HMODULE dll = nullptr, QiDriver* driver = nullptr) : dll(dll), driver(driver) {}
	QiDriverModule(const QiDriverModule&) = delete;
	QiDriverModule(QiDriverModule&& other) { operator=(std::move(other)); }
	~QiDriverModule() { release(); }
	QiDriverModule& operator=(const QiDriverModule&) = delete;
	QiDriverModule& operator=(QiDriverModule&& other)
	{
		release();
		dll = other.dll;
		driver = other.driver;
		other.dll = nullptr;
		other.driver = nullptr;
		return *this;
	}
	bool valid() const { return dll && driver; }
	QiDriver::Status init(const std::wstring& subDir) const { if (valid()) return driver->init(subDir); }
	bool isInit() const { return dll && driver && driver->isInit(); }
	void onload() const { driver->onload(); }
	bool setup(const std::wstring& subDir) const { return driver->setup(subDir); }
	void configure() const { if (isInit()) driver->configure(); }
	void key_up(char key, ULONG_PTR info = 0) const { if (isInit()) driver->key_up(key, info); }
	void key_down(char key, ULONG_PTR info = 0) const { if (isInit()) driver->key_down(key, info); }
	void move(int x, int y, ULONG_PTR info = 0) const { if (isInit()) driver->move(x, y, info); }
	void move_to(int x, int y, ULONG_PTR info = 0) const { if (isInit()) driver->move_to(x, y, info); }
	void release()
	{
		if (driver)
		{
			driver->release();
			driver = nullptr;
		}
		if (dll)
		{
			FreeLibrary(dll);
			dll = nullptr;
		}
	}
};

inline bool QiDriverInfoQuery(const std::wstring& dll, QiDriverInfo& qiDriverInfo)
{
	HMODULE hModule = LoadLibraryW(dll.c_str());
	if (hModule)
	{
		PFQiDriverInfoQueryInterface pFunction = reinterpret_cast<PFQiDriverInfoQueryInterface>(GetProcAddress(hModule, "QiDriverInfoQueryInterface"));
		if (pFunction)
		{
			qiDriverInfo.size = sizeof(QiDriverInfo);
			pFunction(&qiDriverInfo);
		}
		FreeLibrary(hModule);
		return pFunction;
	}
	return false;
}

inline QiDriverModule QiDriverInit(const std::wstring& dll)
{
	HMODULE hModule = LoadLibraryW(dll.c_str());
	if (hModule)
	{
		PFQiDriverInitInterface pFunction = reinterpret_cast<PFQiDriverInitInterface>(GetProcAddress(hModule, "QiDriverInitInterface"));
		if (pFunction)
		{
			QiDriver* driver = pFunction();
			if (driver) return QiDriverModule(hModule, driver);
		}
		FreeLibrary(hModule);
	}
	return QiDriverModule();
}