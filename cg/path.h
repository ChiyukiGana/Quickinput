#pragma once

#include "shlwapi.h"
#include "string.h"

class Path
{
public:
    /* "C:\A\1.exe" - n  >  -n */
    static std::wstring args(std::wstring path)
    {
        size_t size = path.size();
        wchar_t* s = new wchar_t[size + 1];
        wcscpy_s(s, size + 1, path.c_str());
        s[size] = L'\0';
        path = PathGetArgsW(s);
        delete[] s;
        return path;
    }
    /* "C:\A\1.exe"  >  "C:\A" */
    static std::wstring RemoveFile(std::wstring path)
    {
        size_t size = path.size();
        wchar_t* s = new wchar_t[size + 1];
        wcscpy_s(s, size + 1, path.c_str());
        s[size] = L'\0';
        PathRemoveFileSpecW(s);
        path = s;
        delete[] s;
        return path;
    }
    /* "C:\A\1.exe" -n  >  "C:\A\1.exe" */
    static std::wstring RemoveArgs(std::wstring path)
    {
        size_t size = path.size();
        wchar_t* s = new wchar_t[size + 1];
        wcscpy_s(s, size + 1, path.c_str());
        s[size] = L'\0';
        PathRemoveArgsW(s);
        path = s;
        delete[] s;
        return path;
    }
    /* " C:\A\1.exe "  >  "C:\A\1.exe" */
    static std::wstring RemoveBlanks(std::wstring path)
    {
        size_t size = path.size();
        wchar_t* s = new wchar_t[size + 1];
        wcscpy_s(s, size + 1, path.c_str());
        s[size] = L'\0';
        PathRemoveBlanksW(s);
        path = s;
        delete[] s;
        return path;
    }
    /* "C:\A\"  >  "C:\A" */
    static std::wstring RemoveBackslash(std::wstring path)
    {
        size_t size = path.size();
        wchar_t* s = new wchar_t[size + 1];
        wcscpy_s(s, size + 1, path.c_str());
        s[size] = L'\0';
        PathRemoveBackslashW(s);
        path = s;
        delete[] s;
        return path;
    }
    /* "C:\A\1.exe"  >  "C:/A/1.exe" */
    static std::wstring PathToUrl(std::wstring path)
    {
        size_t p = 0;
        while (true)
        {
            p = path.find(L"\\");
            if (p == std::wstring::npos) break;
            path.replace(p, 1, L"/");
        }
        return path;
    }
    /* "C:/A/1.exe"  >  "C:\A\1.exe" */
    static std::wstring UrlToPath(std::wstring url)
    {
        size_t p = 0;
        while (true)
        {
            p = url.find(L"/");
            if (p == std::wstring::npos) break;
            url.replace(p, 1, L"\\");
        }
        return url;
    }

    static bool PathState(std::wstring path) { if (_waccess(path.c_str(), 0)) return false; return true; }
    /* "C:\A\1.exe"  >  "1.exe" */
    static std::wstring Last(std::wstring path) {
        size_t p =path.find_last_of(L"\\");
        if (p != std::wstring::npos) return path.substr(p + 1);
        return path;
    }
    /* "C:\A\1.exe"  >  "C:\A" */
    static std::wstring Prev(std::wstring path) {
        size_t p = path.find_last_of(L"\\");
        if (p != std::wstring::npos) return path.substr(0, p);
        return path;
    }
};