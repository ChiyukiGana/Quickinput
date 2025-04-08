#pragma once
#include <memory>
#include <vector>
#include <functional>
#include <windows.h>
namespace QiTools
{
	class ResourceTool
	{
	public:
		static auto find(std::wstring type, LPCWSTR name, size_t& size, HMODULE hModule = GetModuleHandleW(nullptr)) -> char*
		{
			HRSRC hResInfo = FindResourceW(hModule, name, type.c_str());
			if (!hResInfo) return (char*)(size = (size_t)nullptr);

			DWORD dwResSize = SizeofResource(hModule, hResInfo);
			if (!dwResSize) return (char*)(size = (size_t)nullptr);

			HGLOBAL hResData = LoadResource(hModule, hResInfo);
			if (!hResData) return (char*)(size = (size_t)nullptr);

			char* pResData = (char*)LockResource(hResData);
			if (!pResData) return (char*)(size = (size_t)nullptr);

			size = dwResSize;
			return pResData;
		}
		static auto find(std::wstring type, LPCWSTR name, HMODULE hModule = GetModuleHandleW(nullptr)) -> std::vector<char>
		{
			size_t size = 0;
			char* data = find(type, name, size, hModule);
			if (data && size) return std::vector<char>(data, data + size);
			return std::vector<char>();
		}
		static auto find(std::wstring type) -> std::vector<std::pair<std::vector<char>, std::wstring>>
		{
			std::vector<std::pair<std::vector<char>, std::wstring>> resources;
			find(type, [&resources](char* data, size_t size, std::wstring name) -> bool { resources.push_back(std::pair<std::vector<char>, std::wstring>(std::vector<char>(data, data + size), name)); return true; });
			return resources;
		}
		static auto find(std::wstring type, std::function<bool(char* data, size_t size, std::wstring name)> call) -> bool
		{
			return EnumResourceNamesW(GetModuleHandleW(nullptr), type.c_str(), [](HMODULE hModule, LPCWSTR type, LPWSTR name, LONG_PTR param) -> BOOL {
				size_t size = 0;
				char* data = find(type, name, size, hModule);
				if (data && size)
				{
					std::function<bool(char*, size_t, std::wstring)>* call = (std::function<bool(char*, size_t, std::wstring)>*)param;
					if ((size_t)name > (size_t)MAKEINTRESOURCEW(~size_t(0))) return (*call)(data, size, name);
					else return (*call)(data, size, std::to_wstring((size_t)name));
				}
				return TRUE;
				}, (LONG_PTR)(&call));
		}
		struct VersionInfo
		{
			std::wstring CompanyName;
			std::wstring FileDescription;
			std::wstring FileVersion;
			std::wstring InternalName;
			std::wstring LegalCopyright;
			std::wstring OriginalFilename;
			std::wstring ProductName;
			std::wstring ProductVersion;
		};
        static auto version(VersionInfo& info, HMODULE hModule = GetModuleHandleW(nullptr)) -> bool
        {
            WCHAR file[0x7FFF];
            if (GetModuleFileNameW(hModule, file, 0x7FFF) == 0) return false;

            DWORD dummyHandle = 0;
            DWORD infoSize = GetFileVersionInfoSizeW(file, &dummyHandle);
            if (!infoSize) return false;

            std::unique_ptr<BYTE[]> infoData(new BYTE[infoSize]);
            if (!GetFileVersionInfoW(file, dummyHandle, infoSize, infoData.get())) return false;

            VS_FIXEDFILEINFO* pFileInfo = nullptr;
            UINT valueLength = 0;
            if (!VerQueryValueW(infoData.get(), L"\\", (LPVOID*)&pFileInfo, &valueLength)) return false;
            if (!pFileInfo) return false;

            struct LANGANDCODEPAGE { WORD wLanguage; WORD wCodePage; } *lpTranslate;

            UINT translateLen = 0;
            VerQueryValueW(infoData.get(), L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &translateLen);

            for (UINT i = 0; i < (translateLen / sizeof(LANGANDCODEPAGE)); i++)
            {
                WCHAR subBlockName[256];
                wsprintfW(subBlockName, L"\\StringFileInfo\\%04x%04x\\", lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);

                auto queryString = [&](const wchar_t* name, std::wstring& output) {
                    WCHAR value[256];
                    WCHAR fullPath[512];
                    wsprintfW(fullPath, L"%s%s", subBlockName, name);

                    LPVOID lpBuffer = nullptr;
                    UINT dwBytes = 0;
                    if (VerQueryValueW(infoData.get(), fullPath, &lpBuffer, &dwBytes) && dwBytes > 0) output = (LPWSTR)lpBuffer;
				};

                queryString(L"CompanyName", info.CompanyName);
                queryString(L"FileDescription", info.FileDescription);
                queryString(L"FileVersion", info.FileVersion);
                queryString(L"InternalName", info.InternalName);
                queryString(L"LegalCopyright", info.LegalCopyright);
                queryString(L"OriginalFilename", info.OriginalFilename);
                queryString(L"ProductName", info.ProductName);
                queryString(L"ProductVersion", info.ProductVersion);

                if (!info.CompanyName.empty() || !info.FileDescription.empty() || !info.ProductName.empty() || !info.ProductVersion.empty()) return true;
            }
            return false;
        }
		static auto string(UINT id, HMODULE hModule = GetModuleHandleW(nullptr)) -> std::wstring
		{
			wchar_t text[1024];
			if (LoadStringW(hModule, id, text, 1024) > 0) return text;
			return std::wstring();
		}
	};
}