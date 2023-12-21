#pragma once

#include <windows.h>
#include <string>
#include <sstream>
#include <math.h>

namespace CG {
	class String
	{
	public:

		static std::string toString(char val) { char str[] = { val, '\0' }; return str; }
		static std::string toString(wchar_t val) { char str[] = { (char)val, '\0' }; return str; }
		static std::string toString(const char* val) { return val; }
		static std::string toString(const wchar_t* val) {
			int len = WideCharToMultiByte(CP_ACP, 0, val, -1, 0, 0, 0, 0);
			char* cache = new char[len];
			WideCharToMultiByte(CP_ACP, 0, val, -1, cache, len, 0, 0);
			std::string str = cache;
			delete[] cache;
			return str;
		}
		static std::string toString(std::string val) { return val; }
		static std::string toString(std::wstring val) {
			int len = WideCharToMultiByte(CP_ACP, 0, val.c_str(), -1, 0, 0, 0, 0);
			char* cache = new char[len];
			WideCharToMultiByte(CP_ACP, 0, val.c_str(), -1, cache, len, 0, 0);
			std::string str = cache;
			delete[] cache;
			return str;
		}
		static std::string toString(bool val) { if (val) return "true"; return "false"; }
		static std::string toString(int8 val) { return std::to_string((int32)val); }
		static std::string toString(uint8 val) { return std::to_string((uint32)val); }
		static std::string toString(int16 val) { return std::to_string((int32)val); }
		static std::string toString(uint16 val) { return std::to_string((uint32)val); }
		static std::string toString(int32 val) { return std::to_string(val); }
		static std::string toString(uint32 val) { return std::to_string(val); }
		static std::string toString(int64 val) { return std::to_string(val); }
		static std::string toString(uint64 val) { return std::to_string(val); }
		static std::string toString(float val) { return std::to_string(val); }
		static std::string toString(double val) { return std::to_string(val); }
		static std::string toString(long double val) { return std::to_string(val); }
		static std::string toString(void* val) { std::stringstream ss; ss << val; return ss.str(); }

		static std::wstring toWString(char val) { wchar_t str[] = { (wchar_t)val, L'\0' }; return str; }
		static std::wstring toWString(wchar_t val) { wchar_t str[] = { val, L'\0' }; return str; }
		static std::wstring toWString(const char* val) {
			int len = MultiByteToWideChar(CP_ACP, 0, val, -1, 0, 0);
			wchar_t* cache = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, val, -1, cache, len);
			std::wstring str = cache;
			delete[] cache;
			return str;
		}
		static std::wstring toWString(const wchar_t* val) { return val; }
		static std::wstring toWString(std::string val) {
			int len = MultiByteToWideChar(CP_ACP, 0, val.c_str(), -1, 0, 0);
			wchar_t* cache = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, val.c_str(), -1, cache, len);
			std::wstring str = cache;
			delete[] cache;
			return str;
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
		static std::wstring toWString(void* val) { std::wstringstream ss; ss << val; return ss.str(); }

		static std::wstring toLower(std::wstring str)
		{
			std::wstring result;
			for (size_t s = 0; s < str.length(); s++)
			{
				result += towlower(str[s]);
			}
			return result;
		}

		static std::string toLower(std::string str)
		{
			std::string result;
			for (size_t s = 0; s < str.length(); s++)
			{
				result += tolower(str[s]);
			}
			return result;
		}

		static LPCWSTR toWString(int* arr, UINT len) {
			WCHAR str[1024];
			swprintf_s(str, 1024, L"%d", *arr);
			for (UINT n = 1; n < len; n++) {
				swprintf_s(str, 1024, L"%s, %d", str, *(arr + n));
			}
			return str;
		}

		static std::wstring toHex(UINT hex)
		{
			wchar_t result[] = L"0x00000000";
			UINT c = 2, b = 8, n, r;
			do
			{
				n = (BYTE)((hex & ((UINT)0xF << (UINT)((b - 1) * 4u))) >> (UINT)((b - 1) * 4u)), r = 0;
				while (1) { if (r == n) break; r++; }
				if (r)
				{
					if (r < 10) result[c] = L'0' + r;
					else result[c] = 55 + r;
					c++;
				}
				b--;
			} while (b > 0);
			result[c] = L'\0';
			return result;
		}

		static UINT HexToNum(LPCWSTR hex)
		{
			UINT len = (UINT)wcslen(hex);
			if (len < 3 || len > 10) return 0;
			if (hex[0] != '0') return 0;
			if (hex[1] != 'x' && hex[1] != 'X') return 0;

			UINT result = 0;
			UINT c = len - 1, b = 0, r;
			while (c > 1)
			{
				while (1)
				{
					r = 0;
					if (hex[c] >= '0' && hex[c] <= '9')
					{
						if (r == hex[c] - '0') break;
					}
					else if (hex[c] >= 'A' && hex[c] <= 'F')
					{
						if (r == hex[c] - 55) break;
					}
					r++;
				}
				result |= r << (b * 4);
				c--, b++;
			};
			return result;
		}

		static bool Compare(LPCWSTR str1, LPCWSTR str2) { return !wcscmp(str1, str2); }

		static bool Compare(LPCSTR str1, LPCSTR str2) { return !strcmp(str1, str2); }

		static UINT Find(LPCWSTR str, LPCWSTR strFind) {
			for (UINT len = (UINT)wcslen(str), lenF = (UINT)wcslen(strFind), n = 0, f = 0; n < len; n++) {
				if (str[n] == strFind[f]) {
					f++;
					if (f == lenF) return n - lenF + 1;
				}
				else f = 0;
			}
			return 0;
		}
	};
}