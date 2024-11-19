#pragma once
#include <windows.h>
#include <string>
#include <codecvt>
#include "base.h"
#include "list.h"

#define CP_GB2312 10008

namespace CG {
	using wstringList = List<std::wstring>;

	class String
	{
	public:
		static void GlobalUTF8() { std::locale::global(std::locale(".UTF8")); }

		static size_t Length(const char* str) { return strlen(str); }
		static size_t Length(const wchar_t* str) { return wcslen(str); }
		static int Compare(const char* str1, const char* str2) { return strcmp(str1, str2); }
		static int Compare(const wchar_t* str1, const wchar_t* str2) { return wcscmp(str1, str2); }
		static errno_t Copy(char* buffer, rsize_t size, const char* res) { return strcpy_s(buffer, size, res); }
		static errno_t Copy(wchar_t* buffer, rsize_t size, const wchar_t* res) { return wcscpy_s(buffer, size, res); }
		static errno_t Append(char* buffer, rsize_t size, const char* res) { return strcat_s(buffer, size, res); }
		static errno_t Append(wchar_t* buffer, rsize_t size, const wchar_t* res) { return wcscat_s(buffer, size, res); }
		static int Printf(char* buffer, size_t size, const char* format, ...) { va_list vas; va_start(vas, format); int count = _vsprintf_p(buffer, size, format, vas); va_end(vas); return count; }
		static int Printf(wchar_t* buffer, size_t size, const wchar_t* format, ...) { va_list vas; va_start(vas, format); int count = _vswprintf_p(buffer, size, format, vas); va_end(vas); return count; }

		// string convert
		static std::string toString(char val) { char str[] = { val, '\0' }; return str; }
		static std::string toString(wchar_t val) { char str[] = { (char)val, '\0' }; return str; }
		static std::string toString(const char* val) { return val; }
		static std::string toString(std::string val) { return val; }
		static std::string toString(const wchar_t* val, UINT codePage = CP_UTF8)
		{
			int length = WideCharToMultiByte(codePage, 0, val, -1, 0, 0, 0, 0);
			if (length)
			{
				char* buffer = new char[length];
				length = WideCharToMultiByte(codePage, 0, val, -1, buffer, length, 0, 0);
				if (length)
				{
					std::string result(buffer, length);
					delete[] buffer;
					return result;
				}
				delete[] buffer;
			}
			return "";
		}
		static std::string toString(std::wstring val, UINT codePage = CP_UTF8)
		{
			int length = WideCharToMultiByte(codePage, 0, val.c_str(), val.size(), 0, 0, 0, 0);
			if (length)
			{
				char* buffer = new char[length];
				length = WideCharToMultiByte(codePage, 0, val.c_str(), val.size(), buffer, length, 0, 0);
				if (length)
				{
					std::string result(buffer, length);
					delete[] buffer;
					return result;
				}
				delete[] buffer;
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
		static std::string toString(float val) { std::string str = std::to_string(val); size_t p = str.find_last_not_of('0'); if (p != std::string::npos) { if (str[p] == L'.') return str.substr(0, p + 2); else return str.substr(0, p + 1); } return str; }
		static std::string toString(double val) { std::string str = std::to_string(val); size_t p = str.find_last_not_of('0'); if (p != std::string::npos) { if (str[p] == L'.') return str.substr(0, p + 2); else return str.substr(0, p + 1); } return str; }
		static std::string toString(long double val) { std::string str = std::to_string(val); size_t p = str.find_last_not_of('0'); if (p != std::string::npos) { if (str[p] == L'.') return str.substr(0, p + 2); else return str.substr(0, p + 1); } return str; }
		static std::string toString(void* val) { char str[] = "0xFFFFFFFFFFFFFFFF"; sprintf_s(str, 19, "%p", val); return str; }
		static std::string toString(SIZE& val) { return std::to_string(val.cx) + std::string(", ") + std::to_string(val.cy); }
		static std::string toString(POINT& val) { return std::to_string(val.x) + std::string(", ") + std::to_string(val.y); }
		static std::string toString(RECT& val) { return std::to_string(val.left) + std::string(", ") + std::to_string(val.top) + std::string(", ") + std::to_string(val.right) + std::string(", ") + std::to_string(val.bottom); }
		template<typename _CHAR_T>
		static std::string toString(_CHAR_T* arr, uint32 len) { std::string str; for (uint32 p = 0; p < len - 1; p++) str += toString(arr[p]) + ", "; str += toString(arr[len - 1]); return str; }

		// wstring convert
		static std::wstring toWString(char val) { wchar_t str[] = { (wchar_t)val, L'\0' }; return str; }
		static std::wstring toWString(wchar_t val) { wchar_t str[] = { val, L'\0' }; return str; }
		static std::wstring toWString(const char* val, UINT codePage = CP_UTF8)
		{
			int length = MultiByteToWideChar(codePage, 0, val, -1, 0, 0);
			if (length)
			{
				wchar_t* buffer = new wchar_t[length];
				length = MultiByteToWideChar(codePage, 0, val, -1, buffer, length);
				if (length)
				{
					std::wstring result(buffer, length);
					delete[] buffer;
					return result;
				}
				delete[] buffer;
			}
			return L"";
		}
		static std::wstring toWString(std::string val, DWORD codePage = CP_UTF8)
		{
			int length = MultiByteToWideChar(codePage, 0, val.c_str(), val.size(), 0, 0);
			if (length)
			{
				wchar_t* buffer = new wchar_t[length];
				length = MultiByteToWideChar(codePage, 0, val.c_str(), val.size(), buffer, length);
				if (length)
				{
					std::wstring result(buffer, length);
					delete[] buffer;
					return result;
				}
				delete[] buffer;
			}
			return L"";
		}
		static std::wstring toWString(const wchar_t* val) { return val; }
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
		static std::wstring toWString(float val) { std::wstring str = std::to_wstring(val); size_t p = str.find_last_not_of(L'0'); if (p != std::wstring::npos) { if (str[p] == L'.') return str.substr(0, p + 2); else return str.substr(0, p + 1); } return str; }
		static std::wstring toWString(double val) { std::wstring str = std::to_wstring(val); size_t p = str.find_last_not_of(L'0'); if (p != std::wstring::npos) { if (str[p] == L'.') return str.substr(0, p + 2); else return str.substr(0, p + 1); } return str; }
		static std::wstring toWString(long double val) { std::wstring str = std::to_wstring(val); size_t p = str.find_last_not_of(L'0'); if (p != std::wstring::npos) { if (str[p] == L'.') return str.substr(0, p + 2); else return str.substr(0, p + 1); } return str; }
		static std::wstring toWString(void* val) { wchar_t str[] = L"0xFFFFFFFFFFFFFFFF"; swprintf_s(str, 19, L"%p", val); return str; }
		static std::wstring toWString(const SIZE& val) { return std::to_wstring(val.cx) + std::wstring(L", ") + std::to_wstring(val.cy); }
		static std::wstring toWString(const POINT& val) { return std::to_wstring(val.x) + std::wstring(L", ") + std::to_wstring(val.y); }
		static std::wstring toWString(const RECT& val) { return std::to_wstring(val.left) + std::wstring(L", ") + std::to_wstring(val.top) + std::wstring(L", ") + std::to_wstring(val.right) + std::wstring(L", ") + std::to_wstring(val.bottom); }
		
		template<typename _Ty>
		static std::wstring toWString(_Ty* arr, uint32 len) { std::wstring str; for (uint32 p = 0; p < len - 1; p++) str += toWString(arr[p]) + L", "; str += toWString(arr[len - 1]); return str; }

		static std::string toLower(std::string str) { std::string result; for (size_t s = 0; s < str.length(); s++) result += tolower(str[s]); return result; }
		static std::wstring toLower(std::wstring str) { std::wstring result; for (size_t s = 0; s < str.length(); s++) result += towlower(str[s]); return result; }

		static std::wstring markNumber(int64 n) { bool sig = (n < 0); if (sig) n = ~n + 1; std::wstring str = toWString(n); if (n > 999) { for (size_t i = 1;; i++) { int p = str.length() - (i * 3) - i + 1; if (p > 0) str.insert(p, 1, L','); else break; } } if (sig) str.insert(0, 1, L'-'); return str; }
		static std::wstring markNumber(double n) { return markNumber((int64)n) + std::to_wstring(n - (int64)n).substr(1); }
	};
}