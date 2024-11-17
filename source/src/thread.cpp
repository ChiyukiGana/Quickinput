#pragma execution_character_set("utf-8")
#include "thread.h"

namespace QiThread
{
	ThreadPool releaseKeyPool(1);
	void _stdcall ReleaseKey(byte key)
	{
		Input::State(key, false, key_info);
	}

	bool PeekExitMsg()
	{
		return PeekMessageW(&Qi::msg, 0, msg_exit, msg_exit, PM_REMOVE);
	}

	DWORD _stdcall MacroRun(PVOID pParam)
	{
		Macro* pMacro = (Macro*)pParam;
		srand(clock());
		WndInput wi, * pWi = nullptr;
		if (pMacro->wndState)
		{
			// window is not found
			pMacro->wi.wnd = FindWindowW(pMacro->wi.wndClass.c_str(), pMacro->wi.wndName.c_str());
			if (!pMacro->wi.wnd) pMacro->wi = QiFn::WindowSelection();
			if (!pMacro->wi.wnd) return -1;

			wi.wnd = pMacro->wi.wnd;

			// select child windows
			if (pMacro->wi.child)
			{
				Window::HWNDS wnds; Window::FindChild(wi.wnd, wnds);
				wi.children.resize(wnds.size());
				for (uint32 i = 0; i < wnds.size(); i++)
				{
					if (IsWindowVisible(wnds.at(1)))
					{
						wi.children.at(i).wnd = wnds[i];
						wi.children.at(i).rect = Window::childRect(wi.wnd, wi.children.at(i).wnd);
					}
				}
			}

			pWi = &wi;
		}

		POINT cursor;
		GetCursorPos(&cursor);
		uint32 count = 0;
		while (Qi::run && !PeekExitMsg())
		{
			if (pMacro->count) { count++; if (count > pMacro->count) break; } // if count = 0 then while is infinite
			for (uint32 i = 0; i < pMacro->acRun.size(); i++)
			{
				if (QiFn::ActionExecute(pMacro->acRun.at(i), cursor, pWi) != r_continue)
				{
					return 0;
				}
			}
		}
	}
	DWORD _stdcall MacroEnd(PVOID pParam)
	{
		Macro* pMacro = (Macro*)pParam;
		srand(clock());
		WndInput wi, * pWi = 0;
		if (pMacro->wndState)
		{
			// window is not found
			pMacro->wi.wnd = FindWindowW(pMacro->wi.wndClass.c_str(), pMacro->wi.wndName.c_str());
			if (!pMacro->wi.wnd)
			{
				return -1;
			}
		}

		POINT cursor;
		GetCursorPos(&cursor);
		for (uint32 i = 0; i < pMacro->acEnd.size(); i++) if (QiFn::ActionExecute(pMacro->acEnd.at(i), cursor, pWi) != r_continue) break;
	}

	DWORD _stdcall QuickClick(PVOID)
	{
		srand(clock());
		size_t max, min;
		if (Qi::fun.quickClick.delay > 99) max = 70, min = 30;
		else if (Qi::fun.quickClick.delay > 1) max = (Qi::fun.quickClick.delay >> 1) + (Qi::fun.quickClick.delay >> 2), min = (Qi::fun.quickClick.delay >> 1) - (Qi::fun.quickClick.delay >> 2);
		else if (Qi::fun.quickClick.delay == 1) max = 1, min = 0;
		else max = 0, min = 0;
		while (Qi::run && !PeekExitMsg())
		{
			Input::State(Qi::fun.quickClick.key, true, key_info);
			Thread::Sleep(Rand(max, min));
			Input::State(Qi::fun.quickClick.key, false, key_info);
			Thread::Sleep(Rand(max, min));
		}
		return 0;
	}
	
	DWORD _stdcall WindowState(PVOID)
	{
		while (Qi::state)
		{
			Qi::fun.wndActive.wi.wnd = FindWindowW(0, Qi::fun.wndActive.wi.wndName.c_str());
			if (Qi::fun.wndActive.wi.wnd)
			{
				bool active = (GetForegroundWindow() == Qi::fun.wndActive.wi.wnd);
				if (!Qi::run && active)
				{
					Qi::run = true;
					if (Qi::set.showTips) QiFn::WindowPop(Qi::fun.wndActive.wi.wndName, true);
				}
				else if (Qi::run && !active)
				{
					Qi::run = false;
					if (Qi::set.showTips) QiFn::WindowPop(Qi::fun.wndActive.wi.wndName, false);
				}
			}
			else if (Qi::run)
			{
				Qi::run = false;
				if (Qi::set.showTips) QiFn::WindowPop(Qi::fun.wndActive.wi.wndName, false);
			}
			Thread::Sleep(100);
		}
		Qi::fun.wndActive.thread = 0;
		return 0;
	}

	void StartMacroRun(Macro* pMacro)
	{
		pMacro->thRun = Thread::Start(MacroRun, pMacro);
	}
	void StartMacroEnd(Macro* pMacro)
	{
		pMacro->thEnd = Thread::Start(MacroEnd, pMacro);
	}
	void StartQuickClick()
	{
		Qi::fun.quickClick.thread = Thread::Start(QuickClick, nullptr);
	}
	void StartWindowState()
	{
		Qi::fun.wndActive.thread = Thread::Start(WindowState, nullptr);
	}

	void ExitMacroRun(Macro* pMacro)
	{
		PostThreadMessageW(GetThreadId(pMacro->thRun), msg_exit, 0, 0);
		pMacro->thRun = nullptr;
	}
	void ExitMacroEnd(Macro* pMacro)
	{
		PostThreadMessageW(GetThreadId(pMacro->thEnd), msg_exit, 0, 0);
		pMacro->thEnd = nullptr;
	}
	void ExitQuickClick()
	{
		PostThreadMessageW(GetThreadId(Qi::fun.quickClick.thread), msg_exit, 0, 0);
		Qi::fun.quickClick.thread = nullptr;
	}
	void ExitWindowState()
	{
		PostThreadMessageW(GetThreadId(Qi::fun.wndActive.thread), msg_exit, 0, 0);
		Qi::fun.wndActive.thread = nullptr;
	}

	bool MacroRunActive(Macro* pMacro)
	{
		if (pMacro->thRun)
		{
			DWORD ecode; GetExitCodeThread(pMacro->thRun, &ecode);
			if (ecode == STILL_ACTIVE) return true;
		}
		return false;
	}
	bool MacroEndActive(Macro* pMacro)
	{
		if (pMacro->thEnd)
		{
			DWORD ecode; GetExitCodeThread(pMacro->thEnd, &ecode);
			if (ecode == STILL_ACTIVE) return true;
		}
		return false;
	}
	bool QuickClickActive()
	{
		if (Qi::fun.quickClick.thread)
		{
			DWORD ecode; GetExitCodeThread(Qi::fun.quickClick.thread, &ecode);
			if (ecode == STILL_ACTIVE) return true;
		}
		return false;
	}
	bool WindowStateActive()
	{
		if (Qi::fun.wndActive.thread)
		{
			DWORD ecode; GetExitCodeThread(Qi::fun.wndActive.thread, &ecode);
			if (ecode == STILL_ACTIVE) return true;
		}
		return false;
	}

	void AddReleaseKey(byte key)
	{
		releaseKeyPool.enqueue(ReleaseKey, key);
	}
}