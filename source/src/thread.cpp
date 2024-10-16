﻿#pragma execution_character_set("utf-8")
#include "thread.h"

namespace QiThread
{
	ThreadPool releaseKeyPool(1);
	void _stdcall ReleaseKey(byte key)
	{
		Input::State(key, false, 214);
	}

	bool PeekExitMsg()
	{
		return PeekMessageW(&qis.msg, 0, msg_exit, msg_exit, PM_REMOVE);
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
		while (qis.run && !PeekExitMsg())
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
		if (qis.fun.quickClick.delay > 99) max = 70, min = 30;
		else if (qis.fun.quickClick.delay > 1) max = (qis.fun.quickClick.delay >> 1) + (qis.fun.quickClick.delay >> 2), min = (qis.fun.quickClick.delay >> 1) - (qis.fun.quickClick.delay >> 2);
		else if (qis.fun.quickClick.delay == 1) max = 1, min = 0;
		else max = 0, min = 0;
		while (qis.run && !PeekExitMsg())
		{
			Input::State(qis.fun.quickClick.key, true, 214);
			Thread::Sleep(Rand(max, min));
			Input::State(qis.fun.quickClick.key, false, 214);
			Thread::Sleep(Rand(max, min));
		}
		return 0;
	}
	
	DWORD _stdcall WindowState(PVOID)
	{
		while (qis.state)
		{
			qis.fun.wndActive.wi.wnd = FindWindowW(0, qis.fun.wndActive.wi.wndName.c_str());
			if (qis.fun.wndActive.wi.wnd)
			{
				bool active = (GetForegroundWindow() == qis.fun.wndActive.wi.wnd);
				if (!qis.run && active)
				{
					qis.run = true;
					if (qis.set.showTips) QiFn::WindowPop(qis.fun.wndActive.wi.wndName, true);
				}
				else if (qis.run && !active)
				{
					qis.run = false;
					if (qis.set.showTips) QiFn::WindowPop(qis.fun.wndActive.wi.wndName, false);
				}
			}
			else if (qis.run)
			{
				qis.run = false;
				if (qis.set.showTips) QiFn::WindowPop(qis.fun.wndActive.wi.wndName, false);
			}
			Thread::Sleep(100);
		}
		qis.fun.wndActive.thread = 0;
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
		qis.fun.quickClick.thread = Thread::Start(QuickClick, nullptr);
	}
	void StartWindowState()
	{
		qis.fun.wndActive.thread = Thread::Start(WindowState, nullptr);
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
		PostThreadMessageW(GetThreadId(qis.fun.quickClick.thread), msg_exit, 0, 0);
		qis.fun.quickClick.thread = nullptr;
	}
	void ExitWindowState()
	{
		PostThreadMessageW(GetThreadId(qis.fun.wndActive.thread), msg_exit, 0, 0);
		qis.fun.wndActive.thread = nullptr;
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
		if (qis.fun.quickClick.thread)
		{
			DWORD ecode; GetExitCodeThread(qis.fun.quickClick.thread, &ecode);
			if (ecode == STILL_ACTIVE) return true;
		}
		return false;
	}
	bool WindowStateActive()
	{
		if (qis.fun.wndActive.thread)
		{
			DWORD ecode; GetExitCodeThread(qis.fun.wndActive.thread, &ecode);
			if (ecode == STILL_ACTIVE) return true;
		}
		return false;
	}

	void AddReleaseKey(byte key)
	{
		releaseKeyPool.enqueue(ReleaseKey, key);
	}
}