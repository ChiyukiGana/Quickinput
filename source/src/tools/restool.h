#pragma once
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <windows.h>
namespace QiTools
{
	class ResourceTool
	{
	public:
		static bool exist(const std::wstring& type, LPCWSTR name, HMODULE hModule = GetModuleHandleW(nullptr))
		{
			return FindResourceW(hModule, name, type.c_str());
		}
		static char* find(const std::wstring& type, LPCWSTR name, size_t& size, HMODULE hModule = GetModuleHandleW(nullptr))
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
		static std::vector<char> find(const std::wstring& type, LPCWSTR name, HMODULE hModule = GetModuleHandleW(nullptr))
		{
			size_t size = 0;
			char* data = find(type, name, size, hModule);
			if (data && size) return std::vector<char>(data, data + size);
			return std::vector<char>();
		}
		static std::vector<std::pair<std::vector<char>, std::wstring>> find(const std::wstring& type)
		{
			std::vector<std::pair<std::vector<char>, std::wstring>> resources;
			find(type, [&resources](std::wstring name, LPCWSTR, char* data, size_t size) -> bool { resources.push_back(std::pair<std::vector<char>, std::wstring>(std::vector<char>(data, data + size), name)); return true; });
			return resources;
		}
		using FindCallBack = std::function<bool(std::wstring name, LPWSTR name_res, char* data, size_t size)>;
		static bool find(const std::wstring& type, FindCallBack call, HMODULE hModule = GetModuleHandleW(nullptr))
		{
			return EnumResourceNamesW(hModule, type.c_str(), [](HMODULE hModule, LPCWSTR type, LPWSTR name, LONG_PTR param) -> BOOL {
				size_t size = 0;
				char* data = find(type, name, size, hModule);
				if (!data || !size) return TRUE;
				return (*reinterpret_cast<FindCallBack*>(param))(IS_INTRESOURCE(name) ? std::to_wstring((size_t)name) : name, name, data, size);
				}, (LONG_PTR)(&call));
		}
		using SearchCallBack = std::function<bool(std::wstring name, LPWSTR name_res)>;
		static bool search(const std::wstring& type, SearchCallBack call, HMODULE hModule = GetModuleHandleW(nullptr))
		{
			return EnumResourceNamesW(hModule, type.c_str(), [](HMODULE, LPCWSTR, LPWSTR name, LONG_PTR param) -> BOOL {
				return (*reinterpret_cast<SearchCallBack*>(param))(IS_INTRESOURCE(name) ? std::to_wstring((size_t)name) : name, name);
				}, (LONG_PTR)(&call));
		}
		static bool append(const std::wstring& file, const std::wstring& type, LPCWSTR name, LPVOID data, DWORD size)
		{
			HANDLE hUpdate = BeginUpdateResourceW(file.c_str(), FALSE);
			if (!hUpdate || hUpdate == INVALID_HANDLE_VALUE) return false;
			if (UpdateResourceW(hUpdate, type.c_str(), name, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), data, size)) return EndUpdateResourceW(hUpdate, FALSE);
			EndUpdateResourceW(hUpdate, TRUE);
			return false;
		}
		struct Resource
		{
			LPCWSTR name;
			LPVOID data;
			DWORD size;
		};
		static bool append(const std::wstring& file, const std::wstring& type, std::vector<Resource> res)
		{
			if (res.empty()) return true;
			HANDLE hUpdate = BeginUpdateResourceW(file.c_str(), FALSE);
			if (!hUpdate || hUpdate == INVALID_HANDLE_VALUE) return false;
			bool success = false;
			for (const auto& i : res)
			{
				if (UpdateResourceW(hUpdate, type.c_str(), i.name, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), i.data, i.size)) success = true;
			}
			if (success) return EndUpdateResourceW(hUpdate, FALSE);
			EndUpdateResourceW(hUpdate, TRUE);
			return false;
		}
		static bool remove(const std::wstring& file, const std::wstring& type, std::vector<LPCWSTR> name)
		{
			if (name.empty()) return true;
			HANDLE hUpdate = BeginUpdateResourceW(file.c_str(), FALSE);
			if (!hUpdate || hUpdate == INVALID_HANDLE_VALUE) return false;
			bool success = false;
			for (const auto& i : name)
			{
				if (UpdateResourceW(hUpdate, type.c_str(), i, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), 0, 0)) success = true;
			}
			if (success) return EndUpdateResourceW(hUpdate, FALSE);
			EndUpdateResourceW(hUpdate, TRUE);
			return false;
		}
		static bool remove_all(const std::wstring& file, const std::wstring& type)
		{
			HMODULE hModule = LoadLibraryExW(file.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
			if (!hModule || hModule == INVALID_HANDLE_VALUE) return false;

			std::vector<std::pair<std::wstring, LPCWSTR>> names;
			search(type, [&names](std::wstring name, LPWSTR name_res) -> bool {
				names.push_back(std::pair<std::wstring, LPCWSTR>(name, IS_INTRESOURCE(name_res) ? name_res : 0));
				return true;
				}, hModule);
			FreeLibrary(hModule);

			std::vector<LPCWSTR> name_ptrs;
			for (const auto& name : names) name_ptrs.push_back(name.second ? name.second : name.first.c_str());

			return remove(file, type, name_ptrs);
		}
		static bool release(const std::wstring& file, const std::wstring& type, LPCWSTR name, HMODULE hModule = GetModuleHandleW(nullptr))
		{
			size_t size;
			char* data = find(type, name, size, hModule);
			if (!data || !size) return false;

			DeleteFileW(file.c_str());
			HANDLE hFile = CreateFileW(file.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (!hFile || hFile == INVALID_HANDLE_VALUE) return false;

			DWORD writeSize = 0;
			BOOL bWriteFile = WriteFile(hFile, data, size, &writeSize, NULL);
			CloseHandle(hFile);
			return bWriteFile && writeSize;
		}
		static bool release_all(const std::wstring& type, std::function<bool(const std::wstring res_name, std::wstring& file_name, bool& skip)> rename, HMODULE hModule = GetModuleHandleW(nullptr))
		{
			return find(type, [&](std::wstring name, LPCWSTR, char* data, size_t size) -> bool {
				if (!data || !size) return true;

				std::wstring file_name = name;
				bool skip = false;
				if (!rename(name, file_name, skip)) return false;
				if (skip || file_name.empty()) return true;

				DeleteFileW(file_name.c_str());
				HANDLE hFile = CreateFileW(file_name.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (!hFile || hFile == INVALID_HANDLE_VALUE) return true;

				DWORD writeSize = 0;
				WriteFile(hFile, data, size, &writeSize, NULL);
				CloseHandle(hFile);
				return true;
				}, hModule);
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
		static bool version(VersionInfo& info, HMODULE hModule = GetModuleHandleW(nullptr))
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