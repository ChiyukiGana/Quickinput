﻿#include "thread.h"
namespace QiThread
{
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
	bool IsInvalid(const Macro* macro)
	{
		if (Qi::debug) return PeekExitMsg();
		return !Qi::run || PeekExitMsg() || (macro->timer && !QiTime::in(macro->timerStart, macro->timerEnd));
	}
	DWORD _stdcall MacroExec(PVOID pParam)
	{
		ThreadParam* param = (ThreadParam*)pParam;
		Macro* pMacro = param->macro;
		bool isRunning = param->run;
		QiInterpreter interpreter(*pMacro, isRunning);
		param->load.notify_all();
		Sleep(1);
		if (isRunning)
		{
			if (Qi::run)
			{
				if (pMacro->timer && QiTime::compare(pMacro->timerStart, pMacro->timerEnd) == 1) Qi::popText->Popup(QString("宏：") + pMacro->name + QString("已超过时间范围"));
				else
				{
					if (pMacro->timer && QiTime::compare(pMacro->timerStart, pMacro->timerEnd) == -1)
					{
						Qi::popText->Popup(QString("宏：") + pMacro->name + QString("等待运行"));
						while (Qi::run && !PeekExitMsg() && (pMacro->timer && !(QiTime::in(pMacro->timerStart, pMacro->timerEnd)))) Sleep(1);
					}
					Qi::curBlock += pMacro->curBlock;
					if (pMacro->count && pMacro->mode != Macro::sw)
					{
						for (size_t i = 0; i < pMacro->count && !IsInvalid(pMacro); i++) if (interpreter.ActionInterpreter(pMacro->acRun) != r_continue) break;
					}
					else if (!pMacro->count || pMacro->mode == Macro::sw)
					{
						while (!IsInvalid(pMacro)) if (interpreter.ActionInterpreter(pMacro->acRun) != r_continue) break;
					}
					if (pMacro->timer && !QiTime::in(pMacro->timerStart, pMacro->timerEnd))
					{
						Qi::popText->Popup(QString("宏：") + pMacro->name + QString("已超时结束"));
					}
					Qi::curBlock -= pMacro->curBlock;
				}
			}
			else if (Qi::debug)
			{
				Qi::curBlock += pMacro->curBlock;
				interpreter.ActionInterpreter(pMacro->acRun);
				Qi::curBlock -= pMacro->curBlock;
			}
		}
		else
		{
			Qi::curBlock += pMacro->curBlock;
			if (Qi::run || Qi::debug) interpreter.ActionInterpreter(pMacro->acEnd);
			Qi::curBlock -= pMacro->curBlock;
		}
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
			Qi::fun.wndActive.wndInfo.update();
			if (Qi::fun.wndActive.wndInfo.wnd)
			{
				bool active = (GetForegroundWindow() == Qi::fun.wndActive.wndInfo.wnd);
				if (!Qi::run && active)
				{
					Qi::run = true;
					if (Qi::set.showTips) QiFn::WindowPop(Qi::fun.wndActive.wndInfo.wndName, true);
				}
				else if (Qi::run && !active)
				{
					Qi::run = false;
					if (Qi::set.showTips) QiFn::WindowPop(Qi::fun.wndActive.wndInfo.wndName, false);
				}
			}
			else if (Qi::run)
			{
				Qi::run = false;
				if (Qi::set.showTips) QiFn::WindowPop(Qi::fun.wndActive.wndInfo.wndName, false);
			}
			Sleep(100);
		}
		Qi::fun.wndActive.thread = 0;
		return 0;
	}
	void StartMacro(Macro* pMacro, bool run)
	{
		if (pMacro->wndState)
		{
			pMacro->wndInput.wnd = pMacro->wndInfo.wnd;
			pMacro->wndInput.child = pMacro->wndInfo.child;
			if (!pMacro->wndInput.active())
			{
				pMacro->wndInput.wnd = pMacro->wndInfo.wnd = FindWindowW((LPCWSTR)(pMacro->wndInfo.wndClass.utf16()), (LPCWSTR)(pMacro->wndInfo.wndName.utf16()));
				if (!pMacro->wndInput.wnd) pMacro->wndInput.wnd = (pMacro->wndInfo = QiFn::WindowSelection()).wnd;
				if (!pMacro->wndInput.wnd)
				{
					Qi::popText->Popup("窗口失效");
					return;
				}
			}
		}
		ThreadParam tp;
		tp.run = run;
		tp.macro = pMacro;
		if (run) pMacro->thRun = Thread::Start(MacroExec, &tp);
		else pMacro->thEnd = Thread::Start(MacroExec, &tp);
		std::unique_lock<std::mutex> lock(tp.mutex);
		tp.load.wait(lock);
	}
	void StartMacroRun(Macro* pMacro)
	{
		StartMacro(pMacro, true);
	}
	void StartMacroEnd(Macro* pMacro)
	{
		StartMacro(pMacro, false);
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
		if (pMacro->thRun)
		{
			PostThreadMessageW(GetThreadId(pMacro->thRun), msg_exit, 0, 0);
			pMacro->thRun = nullptr;
		}
	}
	void ExitMacroEnd(Macro* pMacro)
	{
		if (pMacro->thEnd)
		{
			PostThreadMessageW(GetThreadId(pMacro->thEnd), msg_exit, 0, 0);
			pMacro->thEnd = nullptr;
		}
	}
	void ExitQuickClick()
	{
		if (Qi::fun.quickClick.thread)
		{
			PostThreadMessageW(GetThreadId(Qi::fun.quickClick.thread), msg_exit, 0, 0);
			Qi::fun.quickClick.thread = nullptr;
		}
	}
	void ExitWindowState()
	{
		if (Qi::fun.wndActive.thread)
		{
			PostThreadMessageW(GetThreadId(Qi::fun.wndActive.thread), msg_exit, 0, 0);
			Qi::fun.wndActive.thread = nullptr;
		}
	}
	bool MacroRunActive(const Macro* pMacro)
	{
		if (pMacro->thRun)
		{
			DWORD ecode; GetExitCodeThread(pMacro->thRun, &ecode);
			if (ecode == STILL_ACTIVE) return true;
		}
		return false;
	}
	bool MacroEndActive(const Macro* pMacro)
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
}