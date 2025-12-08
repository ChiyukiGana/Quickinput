#pragma once
#include <windows.h>
#include <string>
#define CP_GB2312 10008
namespace QiTools {
	class String
	{
	public:
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
		static std::string toString(short val) { return std::to_string((int)val); }
		static std::string toString(unsigned short val) { return std::to_string((unsigned int)val); }
		static std::string toString(int val) { return std::to_string(val); }
		static std::string toString(unsigned int val) { return std::to_string(val); }
		static std::string toString(signed long val) { return std::to_string(val); }
		static std::string toString(unsigned long val) { return std::to_string(val); }
		static std::string toString(long long val) { return std::to_string(val); }
		static std::string toString(unsigned long long val) { return std::to_string(val); }
		static std::string toString(float val) { std::string str = std::to_string(val); size_t p = str.find_last_not_of('0'); if (p != std::string::npos) { if (str[p] == L'.') return str.substr(0, p + 2); else return str.substr(0, p + 1); } return str; }
		static std::string toString(double val) { std::string str = std::to_string(val); size_t p = str.find_last_not_of('0'); if (p != std::string::npos) { if (str[p] == L'.') return str.substr(0, p + 2); else return str.substr(0, p + 1); } return str; }
		static std::string toString(long double val) { std::string str = std::to_string(val); size_t p = str.find_last_not_of('0'); if (p != std::string::npos) { if (str[p] == L'.') return str.substr(0, p + 2); else return str.substr(0, p + 1); } return str; }
		static std::string toString(void* val) { char str[] = "0xFFFFFFFFFFFFFFFF"; sprintf_s(str, 19, "%p", val); return str; }
		static std::string toString(const SIZE& val) { return std::to_string(val.cx) + std::string(", ") + std::to_string(val.cy); }
		static std::string toString(const POINT& val) { return std::to_string(val.x) + std::string(", ") + std::to_string(val.y); }
		static std::string toString(const RECT& val) { return std::to_string(val.left) + std::string(", ") + std::to_string(val.top) + std::string(", ") + std::to_string(val.right) + std::string(", ") + std::to_string(val.bottom); }
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
		static std::wstring toWString(short val) { return std::to_wstring((int)val); }
		static std::wstring toWString(unsigned short val) { return std::to_wstring((unsigned int)val); }
		static std::wstring toWString(int val) { return std::to_wstring(val); }
		static std::wstring toWString(unsigned int val) { return std::to_wstring(val); }
		static std::wstring toWString(signed long val) { return std::to_wstring(val); }
		static std::wstring toWString(unsigned long val) { return std::to_wstring(val); }
		static std::wstring toWString(long long val) { return std::to_wstring(val); }
		static std::wstring toWString(unsigned long long val) { return std::to_wstring(val); }
		static std::wstring toWString(float val) { std::wstring str = std::to_wstring(val); size_t p = str.find_last_not_of(L'0'); if (p != std::wstring::npos) { if (str[p] == L'.') return str.substr(0, p + 2); else return str.substr(0, p + 1); } return str; }
		static std::wstring toWString(double val) { std::wstring str = std::to_wstring(val); size_t p = str.find_last_not_of(L'0'); if (p != std::wstring::npos) { if (str[p] == L'.') return str.substr(0, p + 2); else return str.substr(0, p + 1); } return str; }
		static std::wstring toWString(long double val) { std::wstring str = std::to_wstring(val); size_t p = str.find_last_not_of(L'0'); if (p != std::wstring::npos) { if (str[p] == L'.') return str.substr(0, p + 2); else return str.substr(0, p + 1); } return str; }
		static std::wstring toWString(void* val) { wchar_t str[] = L"0xFFFFFFFFFFFFFFFF"; swprintf_s(str, 19, L"%p", val); return str; }
		static std::wstring toWString(const SIZE& val) { return std::to_wstring(val.cx) + std::wstring(L", ") + std::to_wstring(val.cy); }
		static std::wstring toWString(const POINT& val) { return std::to_wstring(val.x) + std::wstring(L", ") + std::to_wstring(val.y); }
		static std::wstring toWString(const RECT& val) { return std::to_wstring(val.left) + std::wstring(L", ") + std::to_wstring(val.top) + std::wstring(L", ") + std::to_wstring(val.right) + std::wstring(L", ") + std::to_wstring(val.bottom); }

		template<typename... Args>
		static std::string connect(Args&&... args) {
			std::string result;
			(result += ... += toString(std::forward<Args>(args)));
			return result;
		}

		template<typename... Args>
		static std::wstring connectw(Args&&... args) {
			std::wstring result;
			(result += ... += toWString(std::forward<Args>(args)));
			return result;
		}

		static std::string toLower(std::string str) { std::string result; for (size_t s = 0; s < str.length(); s++) result += tolower(str[s]); return result; }
		static std::wstring toLower(std::wstring str) { std::wstring result; for (size_t s = 0; s < str.length(); s++) result += towlower(str[s]); return result; }
		static std::string toUpper(std::string str) { std::string result; for (size_t s = 0; s < str.length(); s++) result += toupper(str[s]); return result; }
		static std::wstring toUpper(std::wstring str) { std::wstring result; for (size_t s = 0; s < str.length(); s++) result += towupper(str[s]); return result; }
	};
}