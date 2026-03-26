#include "trigger.h"

namespace QiTr
{
	void Trigger(short vk, const bool* state)
	{
		// state swtich
		if ((vk == Qi::set.key1 || vk == Qi::set.key2) && (Qi::set.key1 ? state[Qi::set.key1] : true) && (Qi::set.key2 ? state[Qi::set.key2] : true)) QiState(!Qi::state);
		// show clock
		if (Qi::fun.showClock.state && Qi::fun.showClock.key == vk && state[Qi::fun.showClock.key]) Qi::popText->Popup(QDateTime::currentDateTime().toString(L"hh:mm:ss"));
		if (!Qi::run) return;
		// quick click
		if (Qi::fun.quickClick.state && Qi::fun.quickClick.key == vk)
		{
			if (state[Qi::fun.quickClick.key]) // key down
			{
				if (Qi::fun.quickClick.mode) // switch mode
				{
					if (Qi::fun.quickClick.thread.active())
					{
						Qi::fun.quickClick.thread.stop();
						if (Qi::set.showTips && !Qi::ui.pop.qcd.t.isEmpty()) QiFn::QuickClickPop(false);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.qcd.s, false);
					}
					else
					{
						Qi::fun.quickClick.thread.start();
						if (Qi::set.showTips && !Qi::ui.pop.qce.t.isEmpty()) QiFn::QuickClickPop(true);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.qce.s, false);
					}
				}
				else // press mode
				{
					if (!Qi::fun.quickClick.thread.active())
					{
						Qi::fun.quickClick.thread.start();
						if (Qi::set.showTips && !Qi::ui.pop.qce.t.isEmpty()) QiFn::QuickClickPop(true);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.qce.s, false);
					}
				}
			}
			else // key up
			{
				if (!Qi::fun.quickClick.mode) // press mode
				{
					if (Qi::fun.quickClick.thread.active())
					{
						Qi::fun.quickClick.thread.stop();
						if (Qi::set.showTips && !Qi::ui.pop.qcd.t.isEmpty()) QiFn::QuickClickPop(false);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.qcd.s, false);
					}
				}
			}
		}
		// macros
		for (auto& m : Qi::macroActive)
		{
			Macro& macro = *m;
			if (vk != macro.key1 && vk != macro.key2) continue;

			if (macro.matchState)
			{
				WndMatch wm;
				wm.update();
				if (!wm.match(macro.wndMatch.name, macro.wndMatch.clas, macro.wndMatch.proc)) continue;
			}

			const bool k1 = macro.key1 ? state[macro.key1] : true;
			const bool k2 = macro.key2 ? state[macro.key2] : true;
			switch (macro.mode)
			{
			case Macro::sw:
			{
				if (k1 && k2)
				{
					if (macro.thread.run_active())
					{
						macro.thread.end_start(&macro);
						if (Qi::set.showTips && !Qi::ui.pop.swd.t.isEmpty()) QiFn::MacroPop(&macro, false);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.swd.s, false);
					}
					else
					{
						macro.thread.run_start(&macro);
						if (Qi::set.showTips && !Qi::ui.pop.swe.t.isEmpty()) QiFn::MacroPop(&macro, true);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.swe.s, false);
					}
				}
			} break;
			case Macro::down:
			{
				if (k1 && k2)
				{
					macro.thread.run_start(&macro);
					if (Qi::set.showTips && !Qi::ui.pop.dwe.t.isEmpty()) QiFn::MacroPop(&macro, true);
					if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.dwe.s, false);
				}
				else
				{
					if (macro.count == 0 && macro.thread.run_active())
					{
						macro.thread.end_start(&macro);
						if (Qi::set.showTips && !Qi::ui.pop.dwd.t.isEmpty()) QiFn::MacroPop(&macro, false);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.dwd.s, false);
					}
				}
			} break;
			case Macro::up:
			{
				if (k1 && k2)
				{
					macro.active = true;
					if (macro.count == 0 && macro.thread.run_active())
					{
						macro.thread.end_start(&macro);
						if (Qi::set.showTips && !Qi::ui.pop.upd.t.isEmpty()) QiFn::MacroPop(&macro, false);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.upd.s, false);
					}
				}
				else
				{
					if (macro.active)
					{
						macro.active = false;
						macro.thread.run_start(&macro);
						if (Qi::set.showTips && !Qi::ui.pop.upe.t.isEmpty()) QiFn::MacroPop(&macro, true);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.upe.s, false);
					}
				}
			} break;
			}
		}
	}
	void QiHook(bool state)
	{
		if (state)
		{
			memset(Qi::keyState, 0, sizeof(Qi::keyState));
			if (!InputHook::State())
			{
				timeBeginPeriod(1); // set clock accuracy
#ifndef DEBUG
				if (!InputHook::Start()) MsgBox::Error(L"创建输入Hook失败，检查是否管理员身份运行 或 是否被安全软件拦截。");
#endif
#ifdef Q_KEYEDIT_PAD_ENABLED
				if (Qi::set.pad) Qi::xboxpad.setStateEvent([](short keyCode, short state) { Qi::keyState[keyCode] = (bool)state; Trigger(keyCode, Qi::keyState); }, true);
#endif
			}
		}
		else
		{
			if (InputHook::State())
			{
				timeEndPeriod(1); // reset clock accuracy
				InputHook::Close();
#ifdef Q_KEYEDIT_PAD_ENABLED
				if (Qi::set.pad) Qi::xboxpad.closeStateEvent();
#endif
			}
		}
	}
	void QiState(bool state)
	{
		srand(clock());
		if (state)
		{
			Qi::curBlock = 0;
			memset(Qi::keyBlock, 0, sizeof(Qi::keyBlock));
			Qi::macroActive.clear();
			for (auto& g : Qi::macroGroups)
			{
				for (auto& m : g.macros)
				{
					if (m.state && (m.key1 || m.key2))
					{
						if (m.keyBlock)
						{
							if (m.key1) Qi::keyBlock[m.key1] = true;
							if (m.key2) Qi::keyBlock[m.key2] = true;
						}
						m.active = false;
						m.range = { 0,0,10000,10000 };
						Qi::macroActive.append(&m);
					}
				}
			}

			Qi::state = true;
			if (Qi::fun.wndActive.state) { if (!Qi::fun.wndActive.thread.active()) Qi::fun.wndActive.thread.start(); }
			else Qi::run = true;
			if (Qi::set.showTips && !Qi::ui.pop.qe.t.isEmpty()) QiFn::StatePop(true);
			if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.qe.s, false);
		}
		else
		{
			Qi::state = false, Qi::run = false;
			if (Qi::fun.wndActive.state) { if (Qi::fun.wndActive.thread.active()) Qi::fun.wndActive.thread.stop(); }
			if (Qi::set.showTips && !Qi::ui.pop.qd.t.isEmpty()) QiFn::StatePop(false);
			if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.qd.s, false);
		}
	}
	void UnBlock()
	{
		Qi::curBlock = 0;
		memset(Qi::keyBlock, 0, sizeof Qi::keyBlock);
	}
}