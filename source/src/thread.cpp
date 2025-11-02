#include "inc_header.h"

struct QiMacroWorker : public QiWorkerWithArgs<bool,Macro*,std::condition_variable&>
{
	static inline std::mutex worker_mutex;
	bool running = true;
	Macro* macro = nullptr;
	using QiWorkerWithArgs::QiWorkerWithArgs;
	bool invalid()
	{
		if (Qi::debug) return m_stop;
		return !Qi::run || m_stop || (macro->timer && !QiTime::in(macro->timerStart, macro->timerEnd));
	}
	void run(bool isRunning, Macro* macro, std::condition_variable& ready) override
	{
		QiInterpreter interpreter(*macro, isRunning, *this);
		this->running = isRunning;
		this->macro = macro;
		ready.notify_all();

		{
			std::unique_lock<std::mutex> lock(worker_mutex);
			macro->script_interpreter.setWorker(this);
			macro->script_interpreter.setValue(QiScriptInterpreter::var_macro_name, macro->name.toStdString());
		}

		srand(clock());
		try {
			if (isRunning)
			{
				if (Qi::run)
				{
					bool timerOnce = false;
					do
					{
						if (macro->timer)
						{
							Qi::popText->Popup(QString("宏：") + macro->name + QString(timerOnce ? "已超时结束，等待下一次运行" : "等待运行"));
							while (Qi::run && !m_stop && (macro->timer && !(QiTime::in(macro->timerStart, macro->timerEnd)))) Sleep(1);
						}

						macro->script_interpreter.interpretAll(macro->script.toStdString());

						Qi::curBlock += macro->curBlock;
						if (macro->count && macro->mode != Macro::sw)
						{
							for (size_t i = 0; i < macro->count && !invalid(); i++) if (interpreter.ActionInterpreter(macro->acRun) != InterpreterResult::r_continue) break;
						}
						else if (!macro->count || macro->mode == Macro::sw)
						{
							while (!invalid()) if (interpreter.ActionInterpreter(macro->acRun) != InterpreterResult::r_continue) break;
						}
						Qi::curBlock -= macro->curBlock;
						
						if (!invalid()) macro->thread.end_start(macro);

						timerOnce = true;
					} while (Qi::run && !m_stop && macro->timer);
				}
				else if (Qi::debug)
				{
					Qi::curBlock += macro->curBlock;
					macro->script_interpreter.interpretAll(macro->script.toStdString());
					interpreter.ActionInterpreter(macro->acRun);
					Qi::curBlock -= macro->curBlock;
				}
			}
			else
			{
				Qi::curBlock += macro->curBlock;
				if (Qi::run || Qi::debug) interpreter.ActionInterpreter(macro->acEnd);
				Qi::curBlock -= macro->curBlock;
			}
		}
		catch (std::runtime_error e) { QiFn::UnBlock(); macro->script_interpreter.showError(e.what(), std::string("位于初始化脚本")); }

		{
			std::unique_lock<std::mutex> lock(worker_mutex);
			if (macro->script_interpreter.worker() == this) macro->script_interpreter.setWorker(nullptr);
		}
	}
};
void QiMacroThread::start(Macro* macro, bool running)
{
	if (macro->wndState)
	{
		macro->wndInput.wnd = macro->wndInfo.wnd;
		macro->wndInput.child = macro->wndInfo.child;
		if (!macro->wndInput.active())
		{
			macro->wndInput.wnd = macro->wndInfo.wnd = FindWindowW((LPCWSTR)(macro->wndInfo.wndClass.utf16()), (LPCWSTR)(macro->wndInfo.wndName.utf16()));
			if (!macro->wndInput.wnd) macro->wndInput.wnd = (macro->wndInfo = QiFn::WindowSelection()).wnd;
			if (!macro->wndInput.wnd)
			{
				Qi::popText->Popup("窗口失效");
				return;
			}
		}
	}
	std::mutex mutex;
	std::condition_variable cv;
	std::unique_lock<std::mutex> lock(mutex);
	QiThreadManager::start<QiMacroWorker>(static_cast<bool>(running), reinterpret_cast<Macro*>(macro), reinterpret_cast<std::condition_variable&>(cv));
	cv.wait(lock);
}
void QiMacroThread::run_start(Macro* macro)
{
	start(macro, true);
}
void QiMacroThread::end_start(Macro* macro)
{
	if (macro->acEnd.not_empty()) start(macro, false);
	else stop();
}
bool QiMacroThread::run_active()
{
	lock();
	const QiWorker* worker = worker_last();
	bool active = worker && !worker->m_stop && reinterpret_cast<const QiMacroWorker*>(worker)->running;
	unlock();
	return active;
}
bool QiMacroThread::end_active()
{
	lock();
	const QiWorker* worker = worker_last();
	bool active = worker && !worker->m_stop && !reinterpret_cast<const QiMacroWorker*>(worker)->running;
	unlock();
	return active;
}


struct QiQuickClickWorker : QiWorker
{
	void run() override
	{
		srand(clock());
		int dmax = 0, dmin = 0, umax = 0, umin = 0;
		if (Qi::fun.quickClick.delay > 200) dmax = 70, dmin = 30, umax = umin = Qi::fun.quickClick.delay - 200;
		else if (Qi::fun.quickClick.delay > 1) dmax = umax = (Qi::fun.quickClick.delay >> 1) + (Qi::fun.quickClick.delay >> 2), dmin = umin = (Qi::fun.quickClick.delay >> 1) - (Qi::fun.quickClick.delay >> 2);
		else if (Qi::fun.quickClick.delay == 1) dmax = umax = 1, dmin = umin = 0;
		while (Qi::run && !m_stop)
		{
			Input::State(Qi::fun.quickClick.key, true, Qi::key_info);
			sleep(Rand(dmax, dmin));
			Input::State(Qi::fun.quickClick.key, false, Qi::key_info);
			sleep(Rand(umax, umin));
		}
	}
};
void QiQuickClickThread::start()
{
	QiThreadManager::start<QiQuickClickWorker>();
}


struct QiWindowBindWorker : QiWorker
{
	void run() override
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
					if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.we.s, false);
				}
				else if (Qi::run && !active)
				{
					Qi::run = false;
					if (Qi::set.showTips) QiFn::WindowPop(Qi::fun.wndActive.wndInfo.wndName, false);
					if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.wd.s, false);
				}
			}
			else if (Qi::run)
			{
				Qi::run = false;
				if (Qi::set.showTips) QiFn::WindowPop(Qi::fun.wndActive.wndInfo.wndName, false);
				if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.wd.s, false);
			}
			Sleep(100);
		}
	}
};
void QiWindowBindThread::start()
{
	QiThreadManager::start<QiWindowBindWorker>();
}


struct QiDisplayUpdateWorker : QiWorker
{
	static LRESULT _stdcall DisplayUpdateProcess(HWND w, UINT m, WPARAM wp, LPARAM lp)
	{
		if (m == WM_DISPLAYCHANGE) Qi::screen = System::screenSize();
		return DefWindowProcW(w, m, wp, lp);
	}
	void run() override
	{
		Window::Register(L"QuickInputDisplayUpdateClass", DisplayUpdateProcess);
		HWND wnd = Window::Create(L"QuickInputDisplayUpdate", L"QuickInputDisplayUpdateClass", WS_OVERLAPPED, 0, 100, 100, 0, 0);
		MSG msg;
		while (GetMessageW(&msg, wnd, 0, 0)) DispatchMessageW(&msg);
	}
};
void QiDisplayUpdateThread::start()
{
	QiThreadManager::start<QiDisplayUpdateWorker>();
}