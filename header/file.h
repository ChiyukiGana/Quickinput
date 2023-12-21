//Charset⭐

#pragma once

#ifndef __AFXWIN_H__
#include <windows.h>
#endif

#include <fstream>
#include <direct.h> //_wgetcwd
#include <ShlObj.h> //FindFolderWindow
#include <codecvt>
#include <io.h>

#include "base.h"
#include "string.h"

namespace CG {
	class File
	{
	public:

		static bool PathState(std::wstring path) { if (_waccess(path.c_str(), 0)) return 0; return 1; }

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

		static bool FileDelete(LPCWSTR path) { if (!_wremove(path)) return 1; return 0; }

		static bool FolderDelete(LPCWSTR path) { if (!_wrmdir(path)) return 1; return 0; }

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

		static std::wstring FileName(std::wstring path) {
			size_t len = path.length() - 1;
			while ((len > 0) && (path[len] != L'\\')) len--;
			return path.substr(len + 1);
		}

		static LPCWSTR PathChange(LPCWSTR absPath, LPCWSTR relPath) {
			WCHAR path[MAX_PATH];
			if (String::Compare(relPath, L"..")) swprintf_s(path, MAX_PATH, L"%s", PathLast(absPath).c_str());
			else {
				swprintf_s(path, MAX_PATH, L"%s%s", absPath, relPath);
			}
			return path;
		}

		static std::wstring PathLast(std::wstring absPath) {
			size_t len = absPath.length() - 2;
			while ((len > 0) && (absPath[len] != L'\\')) len--;
			return absPath.substr(0, len);
		}

		static bool FileNameOk(std::wstring path)
		{
			for (UINT n = 0; n < path.length(); n++) {
				if (path[n] == L'\\' || path[n] == L'/' || path[n] == L':' || path[n] == L'*' || path[n] == L'?' || path[n] == L'\"' || path[n] == L'<' || path[n] == L'>' || path[n] == L'|') return 0;
			}
			return 1;
		}

		static std::wstring NameFilter(std::wstring path) {
			if (!FileNameOk(path)) return L"";
			std::wstring result;
			bool state = 1;
			for (UINT n = 0; n < path.length(); n++) {
				if (state && path[n] == L' ');
				else
				{
					state = 0;
					result += path[n];
				}
			}
			return result;
		}

		static std::wstring PathToUrl(std::wstring path)
		{
			WCHAR str[MAX_PATH];
			wcscpy_s(str, path.c_str());

			for (UINT u = 0; u < MAX_PATH; u++)
			{
				if (str[u] == L'\\')
				{
					str[u] = L'/';
				}
				else if (str[u] == L'\0')
				{
					break;
				}
			}

			return str;
		}

		static std::wstring UrlToPath(std::wstring url)
		{
			WCHAR str[MAX_PATH];
			wcscpy_s(str, url.c_str());

			for (UINT u = 0; u < MAX_PATH; u++)
			{
				if (str[u] == L'/')
				{
					str[u] = L'\\';
				}
				else if (str[u] == L'\0')
				{
					break;
				}
			}

			return str;
		}

		typedef List<std::wstring> CmdLine;
		static CmdLine toCmdLine(std::wstring cmd)
		{
			CmdLine cmdLine;
			size_t pos = cmd.find(L".exe");
			if (pos > cmd.length()) return cmdLine;
			pos += 4;
			if (pos > cmd.length()) return cmdLine;
			cmdLine.Add(cmd.substr(0, pos));
			for (size_t s = pos; s < cmd.length(); s++)
			{
				if (cmd[s] == L' ')
				{
					while (s < cmd.length() && cmd[s] == L' ') s++;
					if (s >= cmd.length()) return cmdLine;
					if (cmd[s] != L'\"')
					{
						pos = s;
						while (s < cmd.length() && cmd[s] != L' ') s++;
						if (s >= cmd.length()) return cmdLine;
						cmdLine.Add(cmd.substr(pos, s - pos));
						s--;
					}
					else
					{
						pos = s;
						s++;
						while (s < cmd.length() && cmd[s] != L'\"') s++;
						if (s >= cmd.length()) return cmdLine;
						cmdLine.Add(cmd.substr(pos, s - pos + 1));
					}

				}
			}
			return cmdLine;
		}

		//zh_CN.UTF8
		static void TextSave(std::wstring path, std::wstring str, LPCSTR locale = "chinese") {
			std::wofstream ofs(path, std::ios::out);
			if (ofs.good())
			{
				ofs.imbue(std::locale(locale));
				ofs << str;
				ofs.close();
			}
		}

		//zh_CN.UTF8
		static std::wstring TextLoad(std::wstring path, LPCSTR locale = "chinese") {
			std::wifstream ifs(path, std::ios::in);
			if (ifs.is_open())
			{
				ifs.imbue(std::locale(locale));
				std::wstring text((std::istreambuf_iterator<wchar_t>(ifs)), std::istreambuf_iterator<wchar_t>());
				return text;
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
			WCHAR path[MAX_PATH];
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
			WCHAR path[MAX_PATH];
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

		typedef List<_wfinddata32_t> FindFileStruct;
		static FindFileStruct FindFile(LPCWSTR path) {
			FindFileStruct files;
			_wfinddata32_t file;
			intptr_t pFind = _wfindfirst32(path, &file);
			intptr_t state = pFind;

			while (state != -1) {
				files.Add();
				files[files.len() - 1] = file;
				state = _wfindnext32(pFind, &file);
			}

			return files;
		}
	};
}