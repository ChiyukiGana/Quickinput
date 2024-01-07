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

		static std::wstring PathLast(std::wstring path) {
			size_t p = path.length() - 1;
			while ((p > 0) && (path[p] != L'\\')) p--;
			return path.substr(p + 1);
		}

		static std::wstring PathPrev(std::wstring path) {
			size_t p = path.length() - 1;
			while ((p > 0) && (path[p] != L'\\')) p--;
			return path.substr(0, p);
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

		struct CmdLine
		{
			std::wstring path;
			List<std::wstring> param;
		};

		static CmdLine ParseCmd(std::wstring cmd, bool qmark = 1)
		{
			CmdLine cl;
			uint32 p = 0, pv = 0, c = 0;
			bool pt = 0;
			while (p < cmd.length() && cmd[p] == L' ') p++;
			while (1)
			{
				if (cmd[p] == L' ' || p >= cmd.length())
				{
					if (pt) cl.param.Add(cmd.substr(pv, p - pv)); else cl.path = cmd.substr(pv, p - pv), pt = 1;
					while (p < cmd.length() && cmd[p] == L' ') p++; // remove space
					if (p < cmd.length()) pv = p;
					else break;
				}
				else if (cmd[p] == L'\"')
				{
					pv = p, p++;
					if (!qmark) pv = p;
					if (p < cmd.length())
					{
						while (p < cmd.length() && cmd[p] != L'\"') p++; // to "
						if (qmark) c = p - pv + 1;
						else c = p - pv;
						if (pt) cl.param.Add(cmd.substr(pv, c)); else cl.path = cmd.substr(pv, c), pt = 1;
						if (p < cmd.length())
						{
							p++; // skip "
							while (p < cmd.length() && cmd[p] == L' ') p++; // remove space
							pv = p;
						}
					}
				}
				else if (cmd[p] == L'\'')
				{
					pv = p, p++;
					if (!qmark) pv = p;
					if (p < cmd.length())
					{
						while (p < cmd.length() && cmd[p] != L'\'') p++; // to '
						if (qmark) c = p - pv + 1;
						else c = p - pv;
						if (pt) cl.param.Add(cmd.substr(pv, c)); else cl.path = cmd.substr(pv, c), pt = 1;
						if (p < cmd.length())
						{
							p++; // skip '
							while (p < cmd.length() && cmd[p] == L' ') p++; // remove space
							pv = p;
						}
					}
				}
				else p++;
			}
			return cl;
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
				ifs.imbue(std::locale(locale));
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

		typedef List<_wfinddata32_t> FindFileStruct;
		static FindFileStruct FindFile(LPCWSTR path) {
			FindFileStruct files;
			_wfinddata32_t file;
			intptr_t pFind = _wfindfirst32(path, &file);
			intptr_t state = pFind;

			while (state != -1) {
				files.Add(file);
				state = _wfindnext32(pFind, &file);
			}
			return files;
		}
	};
}