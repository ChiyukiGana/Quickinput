#include "trigger.h"

#ifdef DEBUG
#define InputHookT InputState
#else
#define InputHookT InputHook
#endif

namespace QiTr
{
	void Trigger(short vk, const bool* state)
	{
		// state swtich
		if ((vk == Qi::set.key1 || vk == Qi::set.key2) && (Qi::set.key1 ? state[Qi::set.key1] : true) && (Qi::set.key2 ? state[Qi::set.key2] : true)) QiState(!Qi::state);
		// show clock
		if (Qi::fun.showClock.state && Qi::fun.showClock.key == vk && state[Qi::fun.showClock.key]) Qi::popText->Popup(QDateTime::currentDateTime().toString(L"hh:mm:ss"));
		if (!Qi::run) return;
		// click
		for (auto& c : Qi::click.clickers)
		{
			if (!c.state || !c.key1 || !c.key2) continue;
			if (c.mode == Clicker::press)
			{
				if (state[c.key1])
				{
					if (!c.thread.active())
					{
						c.thread.start(&c);
						if (Qi::set.showTips && !Qi::ui.pop.qce.t.isEmpty()) QiFn::QuickClickPop(&c, true);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.qce.s, false);
					}
				}
				else
				{
					if (c.thread.active())
					{
						c.thread.stop();
						if (Qi::set.showTips && !Qi::ui.pop.qcd.t.isEmpty()) QiFn::QuickClickPop(&c, false);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.qcd.s, false);
					}
				}
			}
			else if (c.mode == Clicker::toggle)
			{
				if (state[c.key1])
				{
					if (c.thread.active())
					{
						c.thread.stop();
						if (Qi::set.showTips && !Qi::ui.pop.qcd.t.isEmpty()) QiFn::QuickClickPop(&c, false);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.qcd.s, false);
					}
					else
					{
						c.thread.start(&c);
						if (Qi::set.showTips && !Qi::ui.pop.qce.t.isEmpty()) QiFn::QuickClickPop(&c, true);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.qce.s, false);
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

			if (macro.mode == Macro::sw)
			{
				const bool k1 = macro.key1 ? state[macro.key1] : true;
				const bool k2 = macro.key2 ? state[macro.key2] : true;
				if (k1 && k2)
				{
					if (macro.thread.run_active())
					{
						macro.thread.stop();
						if (Qi::set.showTips && !macro.hideState && !Qi::ui.pop.swd.t.isEmpty()) QiFn::MacroPop(&macro, false);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.swd.s, false);
					}
					else
					{
						macro.thread.run_start(&macro);
						if (Qi::set.showTips && !macro.hideState && !Qi::ui.pop.swe.t.isEmpty()) QiFn::MacroPop(&macro, true);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.swe.s, false);
					}
				}
			}
			else if (macro.mode == Macro::down)
			{
				const bool k1 = macro.key1 ? state[macro.key1] : true;
				const bool k2 = macro.key2 ? state[macro.key2] : true;
				if (k1 && k2)
				{
					macro.thread.run_start(&macro);
					if (Qi::set.showTips && !macro.hideState && !Qi::ui.pop.dwe.t.isEmpty()) QiFn::MacroPop(&macro, true);
					if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.dwe.s, false);
				}
				else
				{
					if (macro.count == 0 && macro.thread.run_active())
					{
						macro.thread.stop();
						if (Qi::set.showTips && !macro.hideState && !Qi::ui.pop.dwd.t.isEmpty()) QiFn::MacroPop(&macro, false);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.dwd.s, false);
					}
				}
			}
			else if (macro.mode == Macro::up)
			{
				const bool k1 = macro.key1 ? state[macro.key1] : true;
				const bool k2 = macro.key2 ? state[macro.key2] : true;
				if (k1 && k2)
				{
					macro.keyAll = true;
					if (macro.count == 0 && macro.thread.run_active())
					{
						macro.thread.stop();
						if (Qi::set.showTips && !macro.hideState && !Qi::ui.pop.upd.t.isEmpty()) QiFn::MacroPop(&macro, false);
						if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.upd.s, false);
					}
				}
				else if (macro.keyAll)
				{
					macro.keyAll = false;
					macro.thread.run_start(&macro);
					if (Qi::set.showTips && !macro.hideState && !Qi::ui.pop.upe.t.isEmpty()) QiFn::MacroPop(&macro, true);
					if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.upe.s, false);
				}
			}
		}
	}
	void QiHook(bool state)
	{
		if (state)
		{
			Qi::keyBlock.clear();
			if (!InputHookT::State())
			{
				timeBeginPeriod(1);

				if (!InputHookT::Start()) MsgBox::Error(L"创建输入Hook失败，检查是否管理员身份运行 或 是否被安全软件拦截。");

#ifdef Q_KEYEDIT_PAD_ENABLED
				if (Qi::set.pad) Qi::xboxpad.setStateEvent([](short keyCode, short state) {
					Qi::keyState[keyCode] = (bool)state;
					std::array<bool, Qi::key_size> t(Qi::keyState);
					Trigger(keyCode, t.data());
					}, true);
#endif
			}
		}
		else
		{
			if (InputHookT::State())
			{
				timeEndPeriod(1);
				InputHookT::Close();
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
			Qi::keyBlock.clear();
			Qi::macroActive.clear();
			for (auto& g : Qi::macroGroups) for (auto& m : g.macros) if (m.state && (m.key1 || m.key2))
			{
				m.keyAll = false;
				if (m.keyBlock)
				{
					if (m.key1) Qi::keyBlock.set(m.key1, true);
					if (m.key2) Qi::keyBlock.set(m.key2, true);
				}
				m.range = { 0,0,10000,10000 };
				Qi::macroActive.append(&m);
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
			for (auto& m : Qi::macroActive) m->thread.stop();
			if (Qi::fun.wndActive.state) { if (Qi::fun.wndActive.thread.active()) Qi::fun.wndActive.thread.stop(); }
			if (Qi::set.showTips && !Qi::ui.pop.qd.t.isEmpty()) QiFn::StatePop(false);
			if (Qi::set.audFx) QiFn::SoundPlay(Qi::ui.pop.qd.s, false);
			QiScriptInterpreter::clearMutex();
		}
	}
	void UnBlock()
	{
		Qi::curBlock = 0;
		Qi::keyBlock.clear();
	}
}