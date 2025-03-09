#include "thread.h"
namespace QiThread
{
	ThreadQueue releaseKeyQueue;
	void PrecSleep(clock_t ms)
	{
		clock_t begin = clock();
		if (ms > 5)
		{
			begin--;
			while ((begin + ms) > clock()) Sleep(1);
		}
		else while ((begin + ms) > clock()) Sleep(0);
	}

	bool PeekExitMsg()
	{
		return PeekMessageW(&Qi::msg, 0, msg_exit, msg_exit, PM_NOREMOVE);
	}
	bool PeekSleep(clock_t ms)
	{
		clock_t begin = clock();
		if (ms > 5)
		{
			begin--;
			while ((begin + ms) > clock())
			{
				if (PeekExitMsg()) return true;
				Sleep(1);
			}
			return false;
		}
		else while ((begin + ms) > clock()) Sleep(0);
		return PeekExitMsg();
	}
	DWORD _stdcall MacroRun(PVOID pParam)
	{
		Macro* pMacro = (Macro*)pParam;
		srand(clock());
		WndInput* pWi = nullptr;
		if (pMacro->wndState)
		{
			pMacro->wp.wnd = pMacro->wi.wnd;
			pMacro->wp.child = pMacro->wi.child;
			// window not found
			if (!pMacro->wp.active())
			{
				pMacro->wp.wnd = pMacro->wi.wnd = FindWindowW((LPCWSTR)(pMacro->wi.wndClass.utf16()), (LPCWSTR)(pMacro->wi.wndName.utf16()));
				if (!pMacro->wp.wnd) pMacro->wp.wnd = (pMacro->wi = QiFn::WindowSelection()).wnd;
				if (!pMacro->wp.wnd)
				{
					2000, Qi::popText->Popup("窗口失效");
					return -1;
				}
			}

			pWi = &pMacro->wp;
		}

		GetCursorPos(&pMacro->cursor);
		int count = 0;
		int jumpId = 0;
		QiInterpreter interpreter(pMacro->acRun, pMacro->speed, pWi, pMacro->cursor);
		Qi::curBlock += pMacro->curBlock;
		while (Qi::run && !PeekExitMsg())
		{
			if (pMacro->count && pMacro->mode != Macro::sw) { count++; if (count > pMacro->count) break; } // if count = 0 then while is infinite
			if (interpreter.ActionInterpreter(pMacro->acRun, jumpId) != r_continue) break;
		}
		Qi::curBlock -= pMacro->curBlock;
		return 0;
	}
	DWORD _stdcall MacroEnd(PVOID pParam)
	{
		Macro* pMacro = (Macro*)pParam;
		srand(clock());
		WndInput* pWi = nullptr;
		if (pMacro->wndState)
		{
			pMacro->wp.wnd = pMacro->wi.wnd;
			// window not found
			if (!pMacro->wp.active())
			{
				pMacro->wp.wnd = pMacro->wi.wnd = FindWindowW((LPCWSTR)(pMacro->wi.wndClass.utf16()), (LPCWSTR)(pMacro->wi.wndName.utf16()));
				if (!pMacro->wp.wnd) pMacro->wp.wnd = (pMacro->wi = QiFn::WindowSelection()).wnd;
				if (!pMacro->wp.wnd)
				{
					2000, Qi::popText->Popup("窗口失效");
					return -1;
				}
			}

			pWi = &pMacro->wp;
		}
		int jumpId = 0;
		QiInterpreter interpreter(pMacro->acEnd, pMacro->speed, pWi, pMacro->cursor);
		Qi::curBlock += pMacro->curBlock;
		interpreter.ActionInterpreter(pMacro->acEnd, jumpId);
		Qi::curBlock -= pMacro->curBlock;
		return 0;
	}
	DWORD _stdcall QuickClick(PVOID)
	{
		srand(clock());
		int dmax = 0, dmin = 0, umax = 0, umin = 0;
		if (Qi::fun.quickClick.delay > 200) dmax = 70, dmin = 30, umax = umin = Qi::fun.quickClick.delay - 200;
		else if (Qi::fun.quickClick.delay > 1) dmax = umax = (Qi::fun.quickClick.delay >> 1) + (Qi::fun.quickClick.delay >> 2), dmin = umin = (Qi::fun.quickClick.delay >> 1) - (Qi::fun.quickClick.delay >> 2);
		else if (Qi::fun.quickClick.delay == 1) dmax = umax = 1, dmin = umin = 0;
		while (Qi::run && !PeekExitMsg())
		{
			Input::State(Qi::fun.quickClick.key, true, key_info);
			PrecSleep(Rand(dmax, dmin));
			Input::State(Qi::fun.quickClick.key, false, key_info);
			PrecSleep(Rand(umax, umin));
		}
		return 0;
	}
	DWORD _stdcall WindowState(PVOID)
	{
		while (Qi::state)
		{
			Qi::fun.wndActive.wi.update();
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
			Sleep(100);
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
		releaseKeyQueue.enqueue([](byte key) { Input::State(key, false, key_info); }, key);
	}
}