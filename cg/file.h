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

		static bool Rename(std::wstring path, std::wstring name) { if (_wrename(path.c_str(), name.c_str())) return false; return true; }
		
		static bool FileDelete(std::wstring path) { if (_wremove(path.c_str())) return false; return true; }
		static bool FolderDelete(std::wstring path) { if (_wrmdir(path.c_str())) return false; return true; }

		static bool FolderCreate(std::wstring path) { if (CreateDirectoryW(path.c_str(), 0)) return true; if (GetLastError() == ERROR_ALREADY_EXISTS) return true; return false; }

		static bool FileCreate(std::wstring path) {
			if (!FileState(path)) {
				HANDLE handle = CreateFileW(path.c_str(), GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
				if (handle == INVALID_HANDLE_VALUE) return 0;
				CloseHandle(handle);
			}
			return 1;
		}

		static DWORD FileSize(std::wstring path) {
			HANDLE handle = CreateFileW(path.c_str(), FILE_READ_EA, FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0);
			if (handle != INVALID_HANDLE_VALUE) {
				DWORD size = GetFileSize(handle, 0);
				CloseHandle(handle);
				return size;
			}
			return 0;
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

		static bool FileSave(std::wstring path, const void* data, size_t bytes)
		{
			FileDelete(path);
			HANDLE hFile = CreateFileW(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile)
			{
				DWORD bytesWrited;
				BOOL b = WriteFile(hFile, data, bytes, &bytesWrited, 0);
				CloseHandle(hFile);
				return b;
			}
			return false;
		}

		static bool FileRead(std::wstring path, void* data, size_t bytes)
		{
			HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile)
			{
				DWORD bytesReaded;
				BOOL b = ReadFile(hFile, data, bytes, &bytesReaded, 0);
				CloseHandle(hFile);
				return b;
			}
			return false;
		}

		static bool TextSaveU(std::wstring path, std::string str)
		{
			return FileSave(String::toWString(path), str.c_str(), str.size());
		}

		static bool TextSaveW(std::wstring path, std::wstring str)
		{
			return FileSave(path, str.c_str(), str.size() * sizeof(wchar_t));
		}

		static std::string TextReadU(std::wstring path)
		{
			size_t size = FileSize(String::toWString(path));
			if (size)
			{
				size++;
				char* buffer = new char[size];
				FileRead(String::toWString(path), buffer, size);
				std::string str = buffer;
				delete[] buffer;
				return str;
			}
			return "";
		}

		static std::wstring TextReadW(std::wstring path) {
			size_t size = FileSize(path);
			if (size)
			{
				wchar_t* buffer = new wchar_t[size >> 1];
				FileRead(String::toWString(path), buffer, size);
				std::wstring str(buffer, size >> 1);
				delete[] buffer;
				return str;
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
			WCHAR path[MAX_PATH_NEW] = {};
			OPENFILENAMEW ofn = {};
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
			WCHAR path[MAX_PATH_NEW] = {};
			BROWSEINFOW bi = {};

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