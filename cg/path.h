#pragma once

#include "shlwapi.h"
#include "pathcch.h"
#include "string.h"
#pragma comment(lib,"pathcch.lib")

class Path
{
public:
    /* "C:\A\1.exe" - n  >  -n */
    static std::wstring args(std::wstring path)
    {
        wchar_t s[MAX_PATH]; wcscpy_s(s, MAX_PATH, path.c_str());
        return PathGetArgsW(s);
    }
    /* "C:\A\1.exe"  >  C:\A\1.exe */
    static std::wstring RemoveMark(std::wstring path)
    {
        std::wstring r = Path::RemoveBlanks(path);
        if (r[0] == L'\"')
        {
            size_t p = r.find_first_of(L'"', 1);
            if (p != std::wstring::npos) return r.substr(1, p - 1);
        }
        else if (r[0] == L'\'')
        {
            size_t p = r.find_first_of(L'\'', 1);
            if (p != std::wstring::npos) return r.substr(1, p - 1);
        }
        return path;
    }
    /* "C:\A\1.exe"  >  "C:\A" */
    static std::wstring RemoveFile(std::wstring path)
    {
        std::wstring r = RemoveMark(path);
        size_t size = r.size() + 1;
        wchar_t* s = new wchar_t[size];
        wcscpy_s(s, size, r.c_str());
        PathCchRemoveFileSpec(s, size);
        r.assign(s);
        delete[] s;
        return r;
    }
    /* "C:\A\1.exe" -n  >  "C:\A\1.exe" */
    static std::wstring RemoveArgs(std::wstring path)
    {
        wchar_t s[MAX_PATH]; wcscpy_s(s, MAX_PATH, path.c_str());
        PathRemoveArgsW(s);
        return s;
    }
    /* " C:\A\1.exe "  >  "C:\A\1.exe" */
    static std::wstring RemoveBlanks(std::wstring path)
    {
        wchar_t s[MAX_PATH]; wcscpy_s(s, MAX_PATH, path.c_str());
        PathRemoveBlanksW(s);
        return s;
    }
    /* "C:\A\"  >  "C:\A" */
    static std::wstring RemoveBackslash(std::wstring path)
    {
        size_t size = path.size() + 1;
        wchar_t* s = new wchar_t[size];
        wcscpy_s(s, size, path.c_str());
        PathCchRemoveBackslash(s, size);
        std::wstring r(s);
        delete[] s;
        return r;
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