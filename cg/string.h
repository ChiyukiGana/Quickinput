#pragma once

#include <windows.h>
#include <math.h>
#include <string>
#include <codecvt>
#include "base.h"

#define CP_GB2312 10008

namespace CG {
	class String
	{
	public:
		static void GlobalU8() { std::locale::global(std::locale(".UTF8")); }

		static size_t Length(const char* str) { return strlen(str); }
		static size_t Length(const wchar_t* str) { return wcslen(str); }
		static int Compare(const char* str1, const char* str2) { return strcmp(str1, str2); }
		static int Compare(const wchar_t* str1, const wchar_t* str2) { return wcscmp(str1, str2); }
		static errno_t Copy(char* buffer, uint32 size, const char* res) { return strcpy_s(buffer, size, res); }
		static errno_t Copy(wchar_t* buffer, uint32 size, const wchar_t* res) { return wcscpy_s(buffer, size, res); }
		static errno_t Cat(char* buffer, uint32 size, const char* res) { return strcat_s(buffer, size, res); }
		static errno_t Cat(wchar_t* buffer, uint32 size, const wchar_t* res) { return wcscat_s(buffer, size, res); }
		static int Printf(char* buffer, uint32 size, const char* format, ...) { va_list vas; va_start(vas, format); int count = _vsprintf_p(buffer, size, format, vas); va_end(vas); return count; }
		static int Printf(wchar_t* buffer, uint32 size, const wchar_t* format, ...) { va_list vas; va_start(vas, format); int count = _vswprintf_p(buffer, size, format, vas); va_end(vas); return count; }

		// string convert
		static std::string toString(char val) { char str[] = { val, '\0' }; return str; }
		static std::string toString(wchar_t val) { char str[] = { (char)val, '\0' }; return str; }
		static std::string toString(const char* val) { return val; }
		static std::string toString(const wchar_t* val, UINT codePage = CP_UTF8)
		{
			int length = WideCharToMultiByte(codePage, 0, val, -1, 0, 0, 0, 0);
			if (length > 0)
			{
				char* tar = new char[length + 1];
				if (WideCharToMultiByte(codePage, 0, val, -1, tar, length, 0, 0) > 0)
				{
					tar[length] = 0;
					std::string result(tar);
					delete[] tar;
					return result;
				}
			}
			return "";
		}
		static std::string toString(std::string val) { return val; }
		static std::string toString(std::wstring val, UINT codePage = CP_UTF8)
		{
			int length = WideCharToMultiByte(codePage, 0, val.c_str(), val.size(), 0, 0, 0, 0);
			if (length > 0)
			{
				char* tar = new char[length];
				if (WideCharToMultiByte(codePage, 0, val.c_str(), val.size(), tar, length, 0, 0) > 0)
				{
					std::string result(tar);
					delete[] tar;
					return result.substr(0, length);
				}
			}
			return "";
		}
		static std::string toString(bool val) { if (val) return "true"; return "false"; }
		static std::string toString(int8 val) { return std::to_string((int32)val); }
		static std::string toString(uint8 val) { return std::to_string((uint32)val); }
		static std::string toString(int16 val) { return std::to_string((int32)val); }
		static std::string toString(uint16 val) { return std::to_string((uint32)val); }
		static std::string toString(int32 val) { return std::to_string(val); }
		static std::string toString(uint32 val) { return std::to_string(val); }
		static std::string toString(signed long val) { return std::to_string(val); }
		static std::string toString(unsigned long val) { return std::to_string(val); }
		static std::string toString(int64 val) { return std::to_string(val); }
		static std::string toString(uint64 val) { return std::to_string(val); }
		static std::string toString(float val) { return std::to_string(val); }
		static std::string toString(double val) { return std::to_string(val); }
		static std::string toString(long double val) { return std::to_string(val); }
		static std::string toString(void* val) { char str[] = "0xFFFFFFFFFFFFFFFF"; sprintf_s(str, 19, "%p", val); return str; }
		static std::string toString(SIZE& val) { return std::to_string(val.cx) + std::string(", ") + std::to_string(val.cy); }
		static std::string toString(POINT& val) { return std::to_string(val.x) + std::string(", ") + std::to_string(val.y); }
		static std::string toString(RECT& val) { return std::to_string(val.left) + std::string(", ") + std::to_string(val.top) + std::string(", ") + std::to_string(val.right) + std::string(", ") + std::to_string(val.bottom); }
		template<typename T>
		static std::string toString(T* arr, uint32 len) { std::string str; for (uint32 p = 0; p < len - 1; p++) str += toString(arr[p]) + ", "; str += toString(arr[len - 1]); return str; }

		// wstring convert
		static std::wstring toWString(char val) { wchar_t str[] = { (wchar_t)val, L'\0' }; return str; }
		static std::wstring toWString(wchar_t val) { wchar_t str[] = { val, L'\0' }; return str; }
		static std::wstring toWString(const char* val, UINT codePage = CP_UTF8)
		{
			int length = MultiByteToWideChar(codePage, 0, val, -1, 0, 0);
			if (length > 0)
			{
				wchar_t* tar = new wchar_t[length];
				if (MultiByteToWideChar(codePage, 0, val, -1, tar, length) > 0)
				{
					std::wstring result(tar);
					delete[] tar;
					return result.substr(0, length);
				}
			}
			return L"";
		}
		static std::wstring toWString(const wchar_t* val) { return val; }
		static std::wstring toWString(std::string val, UINT codePage = CP_UTF8)
		{
			int length = MultiByteToWideChar(codePage, 0, val.c_str(), val.size(), 0, 0);
			if (length > 0)
			{
				wchar_t* tar = new wchar_t[length + 1];
				if (MultiByteToWideChar(codePage, 0, val.c_str(), val.size(), tar, length) > 0)
				{
					tar[length] = 0;
					std::wstring result(tar);
					delete[] tar;
					return result;
				}
			}
			return L"";
		}
		static std::wstring toWString(std::wstring val) { return val; }
		static std::wstring toWString(bool val) { if (val) return L"true"; return L"false"; }
		static std::wstring toWString(int8 val) { return std::to_wstring((int32)val); }
		static std::wstring toWString(uint8 val) { return std::to_wstring((uint32)val); }
		static std::wstring toWString(int16 val) { return std::to_wstring((int32)val); }
		static std::wstring toWString(uint16 val) { return std::to_wstring((uint32)val); }
		static std::wstring toWString(int32 val) { return std::to_wstring(val); }
		static std::wstring toWString(uint32 val) { return std::to_wstring(val); }
		static std::wstring toWString(signed long val) { return std::to_wstring(val); }
		static std::wstring toWString(unsigned long val) { return std::to_wstring(val); }
		static std::wstring toWString(int64 val) { return std::to_wstring(val); }
		static std::wstring toWString(uint64 val) { return std::to_wstring(val); }
		static std::wstring toWString(float val) { return std::to_wstring(val); }
		static std::wstring toWString(double val) { return std::to_wstring(val); }
		static std::wstring toWString(long double val) { return std::to_wstring(val); }
		static std::wstring toWString(void* val) { wchar_t str[] = L"0xFFFFFFFFFFFFFFFF"; swprintf_s(str, 19, L"%p", val); return str; }
		static std::wstring toWString(SIZE& val) { return std::to_wstring(val.cx) + std::wstring(L", ") + std::to_wstring(val.cy); }
		static std::wstring toWString(POINT& val) { return std::to_wstring(val.x) + std::wstring(L", ") + std::to_wstring(val.y); }
		static std::wstring toWString(RECT& val) { return std::to_wstring(val.left) + std::wstring(L", ") + std::to_wstring(val.top) + std::wstring(L", ") + std::to_wstring(val.right) + std::wstring(L", ") + std::to_wstring(val.bottom); }
		template<typename T>
		static std::wstring toWString(T* arr, uint32 len) { std::wstring str; for (uint32 p = 0; p < len - 1; p++) str += toWString(arr[p]) + L", "; str += toWString(arr[len - 1]); return str; }

		static std::string toLower(std::string str) { std::string result; for (size_t s = 0; s < str.length(); s++) result += tolower(str[s]); return result; }
		static std::wstring toLower(std::wstring str) { std::wstring result; for (size_t s = 0; s < str.length(); s++) result += towlower(str[s]); return result; }
	};

	template<class T>
	struct xcstring
	{
		T* str = 0;
		uint32 len = 0;
		~xcstring() { emp(); }

		void operator=(const T* res) { cpy(res); }
		void cpy(const T* res)
		{
			emp();
			len = String::Length(res) + 1;
			str = new T[len];
			String::Copy(str, len, res);
		}
		void cpy(const xcstring& res)
		{
			emp();
			len = res.len;
			str = new T[len];
			String::Copy(str, len, res.str);
		}
		void emp()
		{
			if (str) { delete[] str; str = 0; }
			len = 0;
		}
		void resize(uint32 size)
		{
			emp();
			len = size;
			new T[len];
		}
	};

	typedef xcstring<char> cstring;
	typedef xcstring<wchar_t> wcstring;
}