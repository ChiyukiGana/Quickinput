#pragma once
#include <windows.h>
#include <qstring.h>
#include <qlist.h>
#include "base.h"
namespace QiTools {
	class Window
	{
	public:
		static QString text(HWND wnd)
		{
			int length = GetWindowTextLengthW(wnd) + 1;
			LPWSTR name = new WCHAR[length];
			GetWindowTextW(wnd, name, length);
			QString result(QString::fromWCharArray(name));
			delete[] name;
			return result;
		}
		static QString className(HWND wnd)
		{
			LPWSTR name = new WCHAR[256];
			GetClassNameW(wnd, name, 256);
			QString result(QString::fromWCharArray(name));
			delete[] name;
			return result;
		}
		static POINT pos(HWND wnd) { RECT rect = {}; GetWindowRect(wnd, &rect); return { rect.left, rect.top }; }
		static SIZE size(HWND wnd) { RECT rect = {}; GetWindowRect(wnd, &rect); return { rect.right - rect.left, rect.bottom - rect.top }; }
		static RECT sizeRect(HWND wnd) { RECT rect = {}; GetWindowRect(wnd, &rect); return { 0, 0, rect.right - rect.left, rect.bottom - rect.top }; }
		static RECT rect(HWND wnd) { RECT rect = {}; GetWindowRect(wnd, &rect); return rect; }
		static RECT childRect(HWND parent, HWND child) { RECT rParent = {}; RECT rChild = {}; GetWindowRect(parent, &rParent); GetWindowRect(child, &rChild); return { rChild.left - rParent.left, rChild.top - rParent.top, rChild.right - rParent.left, rChild.bottom - rParent.top }; }
		static SIZE clientSize(HWND wnd) { RECT rect; GetClientRect(wnd, &rect); return { rect.right, rect.bottom }; }
		static bool setTop(HWND wnd, bool top = true)
		{
			if (top) return SetWindowPos(wnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			return SetWindowPos(wnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		}
		static bool setTopMost(HWND wnd) { if (SetWindowPos(wnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE)) return SetWindowPos(wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE); return 0; }
		static QList<HWND> FindChild(HWND parent)
		{
			QList<HWND> children;
			HWND child = nullptr;
			do
			{
				child = FindWindowExW(parent, child, 0, 0);
				if (child) children.append(child);
			} while (child);
			return children;
		}
	};
}