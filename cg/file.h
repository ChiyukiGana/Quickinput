#pragma once

#include <windows.h>
#include <fstream>
#include <ShlObj.h> //FindFolderWindow
#include <codecvt>
#include <io.h>

#include "base.h"
#include "list.h"
#include "string.h"

namespace CG {
	class File
	{
	public:

		static bool FileState(std::wstring path) {
			FILE* file = 0;
			_wfopen_s(&file, path.c_str(), L"r");
			if (file)
			{
				fclose(file);
				return 1;
			}
			return 0;
		}

		static bool FolderState(std::wstring path) {
			DWORD file = GetFileAttributesW(path.c_str());
			if (file == FILE_ATTRIBUTE_DIRECTORY) return 1;
			return 0;
		}

		static bool Rename(LPCWSTR path, LPCWSTR name) { if (_wrename(path, name)) return 0; return 1; }
		
		static bool FileDelete(LPCWSTR path) { if (_wremove(path)) return 0; return 1; }
		static bool FolderDelete(LPCWSTR path) { if (_wrmdir(path)) return 0; return 1; }

		static bool FolderCreate(LPCWSTR path) { if (CreateDirectoryW(path, 0)) return 1; if (GetLastError() == ERROR_ALREADY_EXISTS) return 1; return 0; }

		static bool FileCreate(LPCWSTR path) {
			if (!FileState(path)) {
				HANDLE handle = CreateFileW(path, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
				if (handle == INVALID_HANDLE_VALUE) return 0;
				CloseHandle(handle);
			}
			return 1;
		}

		static DWORD FileSize(LPCWSTR path) {
			HANDLE handle = CreateFileW(path, FILE_READ_EA, FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0);
			if (handle != INVALID_HANDLE_VALUE) {
				DWORD size = GetFileSize(handle, 0);
				CloseHandle(handle);
				return size;
			}
		}

		static bool FileNameUsable(std::wstring file)
		{
			for (UINT n = 0; n < file.length(); n++) {
				if (file[n] == L'\\' || file[n] == L'/' || file[n] == L':' || file[n] == L'*' || file[n] == L'?' || file[n] == L'\"' || file[n] == L'<' || file[n] == L'>' || file[n] == L'|') return false;
			}
			return true;
		}

		static bool CreateShortcut(std::wstring path, std::wstring srcPath, std::wstring workPath = L"", std::wstring iconPath = L"")
		{
			CoInitialize(0);
			IShellLinkW* link = 0;
			HRESULT hr = 0;
			hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_ALL, IID_IShellLinkW, (void**)&link);
			if (FAILED(hr)) return false;
			link->SetPath(srcPath.c_str());
			link->SetDescription(path.c_str());
			if (iconPath.size()) link->SetIconLocation(iconPath.c_str(), 0);
			if (workPath.size()) link->SetWorkingDirectory(workPath.c_str());
			else link->SetWorkingDirectory(Path::RemoveFile(srcPath).c_str());
			IPersistFile* pers = 0;
			hr = link->QueryInterface(IID_IPersistFile, (void**)&pers);
			if (FAILED(hr))
			{
				link->Release();
				return false;
			}
			pers->Save(path.c_str(), TRUE);
			pers->Release();
			link->Release();
			return true;
		}

		static bool TextSave(std::string path, std::string str, const char* locale = ".UTF8")
		{
			std::ofstream ofs;
			ofs.imbue(std::locale(locale));
			ofs.open(path, std::ios::out);
			if (ofs.good())
			{
				ofs << str;
				ofs.close();
				return 1;
			}
			return 0;
		}

		static bool TextSave(std::wstring path, std::wstring str, const char* locale = ".UTF8") {
			std::wofstream ofs;
			ofs.imbue(std::locale(locale));
			ofs.open(path, std::ios::out);
			if (ofs.good())
			{
				ofs << str;
				ofs.close();
				return 1;
			}
			return 0;
		}

		static std::string TextLoad(std::string path, const char* locale = ".UTF8")
		{
			std::ifstream ifs;
			ifs.imbue(std::locale(locale));
			ifs.open(path, std::ios::in);
			if (ifs.is_open())
			{
				return std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			}
			return "";
		}

		static std::wstring TextLoad(std::wstring path, const char* locale = ".UTF8") {
			std::wifstream ifs;
			ifs.imbue(std::locale(locale));
			ifs.open(path, std::ios::in);
			if (ifs.is_open())
			{
				return std::wstring((std::istreambuf_iterator<wchar_t>(ifs)), std::istreambuf_iterator<wchar_t>());
			}
			return L"";
		}

		static void OpenFileLocation(std::wstring path) {
			HRESULT hr = CoInitialize(0);
			LPITEMIDLIST item = ILCreateFromPathW(path.c_str());
			if (item)
			{
				SHOpenFolderAndSelectItems(item, 0, 0, 0);
				ILFree(item);
			}
			if (hr == S_FALSE) CoUninitialize();
		}

		static LPCWSTR FindFileWindow(HWND mainWnd, LPCWSTR title = L"Select File", LPCWSTR type = 0) {
			WCHAR path[MAX_PATH] = { 0 };
			OPENFILENAMEW ofn = { 0 };
			ofn.lStructSize = sizeof(OPENFILENAMEW);
			ofn.hwndOwner = mainWnd;
			ofn.lpstrTitle = title;
			ofn.lpstrFile = path;
			ofn.nMaxFile = sizeof(path);
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			ofn.lpstrFilter = type;

			if (!GetOpenFileNameW(&ofn)) path[0] = L'\0';

			return path;
		}

		static LPCWSTR FindFolderWindow(HWND mainWnd, LPCWSTR title = L"Select Folder") {
			WCHAR path[MAX_PATH] = { 0 };
			BROWSEINFOW bi = { 0 };

			bi.hwndOwner = mainWnd;
			bi.lpszTitle = title;
			bi.ulFlags = BIF_USENEWUI | BIF_UAHINT | BIF_NONEWFOLDERBUTTON;

			LPITEMIDLIST lpId;
			lpId = SHBrowseForFolderW(&bi);
			if (lpId) SHGetPathFromIDListW(lpId, path);
			else path[0] = L'\0';

			return path;
		}

		typedef List<_wfinddata32_t> FileList;
		static FileList FindFile(LPCWSTR path) {
			FileList files;
			_wfinddata32_t file;
			intptr_t pFind = _wfindfirst32(path, &file);
			intptr_t state = pFind;

			while (state != -1) {
				files.Add(file);
				state = _wfindnext32(pFind, &file);
			}
			return files;
		}

		// fileName: exclude .exe, findPath: D:\*.exe
		static std::wstring FileNameNrep(std::wstring fileName, std::wstring path, std::wstring blank = L" ") {
			if (!FileNameUsable(fileName)) return L"";
			FileList files = FindFile(path.c_str());
			if (!files.size()) return fileName;
			List<std::wstring> strs;
			strs.resize(files.size());
			for (uint32 u = 0; u < files.size(); u++) strs[u] = Path::RemoveExtension(Path::Last(files[u].name));
			std::wstring r = fileName;
			uint32 p = 0;
			while (true)
			{
				for (uint32 u = 0;; u++)
				{
					if (p) r = fileName + blank + std::to_wstring(p);
					if (r == strs[u]) break;
					if (u >= files.size() - 1) return r;
				}
				p++;
			}
		}
	};
}