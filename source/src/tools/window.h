#pragma once
#include <windows.h>
#include <qstring.h>
#include <qlist.h>
#include "base.h"
namespace QiTools {
	class Window
	{
	public:
		static void Register(LPCWSTR className, WNDPROC wndProc, bool transparent = 0, COLORREF color = RGB(255, 255, 255), HICON icon = 0, INT menu = 0) {
			WNDCLASSW wndClass = { sizeof(WNDCLASSW) };
			wndClass.lpszClassName = className;
			wndClass.lpfnWndProc = wndProc;
			wndClass.hIcon = icon;
			wndClass.lpszMenuName = MAKEINTRESOURCEW(menu);
			wndClass.style = CS_HREDRAW | CS_VREDRAW;
			if (!transparent) wndClass.hbrBackground = CreateSolidBrush(color);
			RegisterClassW(&wndClass);
		}

		static HWND Create(
			LPCWSTR wndName, LPCWSTR className, DWORD style = WS_OVERLAPPED, DWORD exStyle = 0,
			int width = CW_USEDEFAULT, int height = CW_USEDEFAULT, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
			HWND parent = 0, HMENU menu = 0, HINSTANCE instance = GetModuleHandleW(0)
		) {
			return CreateWindowExW(exStyle, className, wndName, style, x, y, width, height, parent, menu, instance, 0);
		}

		static HWND CreateRect(
			LPCWSTR wndName, LPCWSTR className, DWORD style = WS_OVERLAPPED, DWORD exStyle = 0, RECT rect = { CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT },
			HWND parent = 0, HMENU menu = 0, HINSTANCE instance = GetModuleHandleW(0)
		) {
			if (rect.left == CW_USEDEFAULT && rect.top == CW_USEDEFAULT && rect.right == CW_USEDEFAULT && rect.bottom == CW_USEDEFAULT)
				return CreateWindowExW(exStyle, className, wndName, style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent, menu, instance, 0);
			return CreateWindowExW(exStyle, className, wndName, style, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, parent, menu, instance, 0);
		}
		static QString text(HWND wnd)
		{
			int size = GetWindowTextLengthW(wnd) + 1;
			std::wstring str(size, 0);
			GetWindowTextW(wnd, str.data(), size);
			str.pop_back();
			return QString::fromStdWString(str);
		}
		static QString className(HWND wnd)
		{
			wchar_t buf[1024];
			GetClassNameW(wnd, buf, 1024);
			return QString::fromWCharArray(buf);
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

	struct WindowItem;
	struct WindowItems : public std::vector<WindowItem>
	{
		WindowItem* iter(std::function<bool(WindowItem& item)> f);
		const WindowItem* iter(std::function<bool(const WindowItem& item)> f) const;
	};
	struct WindowItem
	{
		HWND wnd;
		WindowItems child;
		WindowItem* iter(std::function<bool(WindowItem& item)> f);
		const WindowItem* iter(std::function<bool(const WindowItem& item)> f) const;
		static WindowItem find(HWND parent)
		{
			WindowItem current;
			current.wnd = parent;
			auto children = Window::FindChild(current.wnd);
			for (auto i : children) current.child.push_back(find(i));
			return current;
		}
	};

	inline WindowItem* WindowItem::iter(std::function<bool(WindowItem& item)> f)
	{
		return child.iter(f);
	}
	inline const WindowItem* WindowItem::iter(std::function<bool(const WindowItem& item)> f) const
	{
		return child.iter(f);
	}
	inline WindowItem* WindowItems::iter(std::function<bool(WindowItem& item)> f)
	{
		for (auto& i : *this)
		{
			if (f(i)) return &i;
			auto* r = i.iter(f);
			if (r) return r;
		}
		return nullptr;
	}
	inline const WindowItem* WindowItems::iter(std::function<bool(const WindowItem& item)> f) const
	{
		for (const auto& i : *this)
		{
			if (f(i)) return &i;
			const auto* r = i.iter(f);
			if (r) return r;
		}
		return nullptr;
	}
}