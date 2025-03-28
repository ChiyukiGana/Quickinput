#include "func.h"

namespace QiFn
{
	// Pos convert
	POINT RTA(POINT rel) { return { (long)(((float)rel.x / (float)(Qi::screen.cx - 1)) * 10000.0f), (long)(((float)rel.y / (float)(Qi::screen.cy - 1)) * 10000.0f) }; }
	RECT RTAR(RECT rel) {
		if (rel.right - rel.left == 0)
		{
			if (rel.right + 1 < Qi::screen.cx) rel.right += 1;
			else rel.left -= 1;
		}
		if (rel.bottom - rel.top == 0)
		{
			if (rel.bottom + 1 < Qi::screen.cy) rel.bottom += 1;
			else rel.top -= 1;
		}
		return { (long)(((float)rel.left / (float)(Qi::screen.cx - 1)) * 10000.0f), (long)(((float)rel.top / (float)(Qi::screen.cy - 1)) * 10000.0f), (long)(((float)rel.right / (float)(Qi::screen.cx - 1)) * 10000.0f), (long)(((float)rel.bottom / (float)(Qi::screen.cy - 1)) * 10000.0f) };
	}
	POINT ATR(POINT abs) { return { (long)((float)Qi::screen.cx / 10000.0f * (float)abs.x), (long)((float)Qi::screen.cy / 10000.0f * (float)abs.y) }; }
	RECT ATRR(RECT abs) {
		RECT rect = { (long)((float)Qi::screen.cx / 10000.0f * (float)abs.left), (long)((float)Qi::screen.cy / 10000.0f * (float)abs.top), (long)((float)Qi::screen.cx / 10000.0f * (float)abs.right), (long)((float)Qi::screen.cy / 10000.0f * (float)abs.bottom) };
		if (rect.right - rect.left == 0)
		{
			if (rect.right + 1 < Qi::screen.cx) rect.right += 1;
			else rect.left -= 1;
		}
		if (rect.bottom - rect.top == 0)
		{
			if (rect.bottom + 1 < Qi::screen.cy) rect.bottom += 1;
			else rect.top -= 1;
		}
		return rect;
	}
	POINT WRTA(POINT rel, HWND wnd) { SIZE size = Window::size(wnd); return { (long)(((float)rel.x / ((float)size.cx)) * 10000.0f), (long)(((float)rel.y / ((float)size.cy)) * 10000.0f) }; }
	RECT WRTAR(RECT rel, HWND wnd) {
		SIZE size = Window::size(wnd);
		if (rel.right - rel.left == 0)
		{
			if (rel.right + 1 < size.cx) rel.right += 1;
			else rel.left -= 1;
		}
		if (rel.bottom - rel.top == 0)
		{
			if (rel.bottom + 1 < size.cy) rel.bottom += 1;
			else rel.top -= 1;
		}
		return { (long)(((float)rel.left / ((float)size.cx)) * 10000.0f), (long)(((float)rel.top / ((float)size.cy)) * 10000.0f), (long)(((float)rel.right / ((float)size.cx)) * 10000.0f), (long)(((float)rel.bottom / ((float)size.cy)) * 10000.0f) };
	}
	POINT WATR(POINT abs, HWND wnd) { SIZE size = Window::size(wnd); return { (long)((float)(size.cx + 1) / 10000.0f * (float)abs.x), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.y) }; }
	RECT WATRR(RECT abs, HWND wnd) {
		SIZE size = Window::size(wnd);
		RECT rect = { (long)((float)(size.cx + 1) / 10000.0f * (float)abs.left), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.top), (long)((float)(size.cx + 1) / 10000.0f * (float)abs.right), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.bottom) };
		if (rect.right - rect.left == 0)
		{
			if (rect.right + 1 < size.cx) rect.right += 1;
			else rect.left -= 1;
		}
		if (rect.bottom - rect.top == 0)
		{
			if (rect.bottom + 1 < size.cy) rect.bottom += 1;
			else rect.top -= 1;
		}
		return rect;
	}
	// Pop text
	QString ParseCustom(QString text, QString name, QString number) { text.replace("@", name); return text.replace("$", number); }
	QString ParseState(QString text) { return text.replace("@", Qi::folder); }
	QString ParseWindow(QString text, QString window) { return text.replace("@", window); }
	QString ParseQuickClick(QString text, short key) { return text.replace("@", QKeyEdit::keyName(key)); }
	QString ParseMacro(QString text, QString macro, int count) { if (count) text.replace("$", QString::number(count)); else text.replace("$", "∞"); return text.replace("@", macro); }
	void StatePop(bool state)
	{
		if (state) Qi::popText->Popup(Qi::ui.pop.time, ParseState(Qi::ui.pop.qe.t), Qi::ui.pop.qe.c);
		else Qi::popText->Popup(Qi::ui.pop.time, ParseState(Qi::ui.pop.qd.t), Qi::ui.pop.qd.c);
	}
	void WindowPop(QString window, bool state)
	{
		if (state) Qi::popText->Popup(Qi::ui.pop.time, ParseWindow(Qi::ui.pop.we.t, window), Qi::ui.pop.we.c);
		else Qi::popText->Popup(Qi::ui.pop.time, ParseWindow(Qi::ui.pop.wd.t, window), Qi::ui.pop.wd.c);
	}
	void QuickClickPop(bool state)
	{
		if (state) Qi::popText->Popup(Qi::ui.pop.time, ParseQuickClick(Qi::ui.pop.qce.t, Qi::fun.quickClick.key), Qi::ui.pop.qce.c);
		else Qi::popText->Popup(Qi::ui.pop.time, ParseQuickClick(Qi::ui.pop.qcd.t, Qi::fun.quickClick.key), Qi::ui.pop.qcd.c);
	}
	void MacroPop(Macro* macro, bool state)
	{
		if (macro->mode == Macro::sw)
		{
			if (state) Qi::popText->Popup(Qi::ui.pop.time, ParseMacro(Qi::ui.pop.swe.t, macro->name, 0), Qi::ui.pop.swe.c);
			else Qi::popText->Popup(Qi::ui.pop.time, ParseMacro(Qi::ui.pop.swd.t, macro->name, 0), Qi::ui.pop.swd.c);
		}
		else if (macro->mode == Macro::down)
		{
			if (state) Qi::popText->Popup(Qi::ui.pop.time, ParseMacro(Qi::ui.pop.dwe.t, macro->name, macro->count), Qi::ui.pop.dwe.c);
			else Qi::popText->Popup(Qi::ui.pop.time, ParseMacro(Qi::ui.pop.dwd.t, macro->name, macro->count), Qi::ui.pop.dwd.c);
		}
		else if (macro->mode == Macro::up)
		{
			if (state) Qi::popText->Popup(Qi::ui.pop.time, ParseMacro(Qi::ui.pop.upe.t, macro->name, macro->count), Qi::ui.pop.upe.c);
			else Qi::popText->Popup(Qi::ui.pop.time, ParseMacro(Qi::ui.pop.upd.t, macro->name, macro->count), Qi::ui.pop.upd.c);
		}
	}
	// State
	void Trigger(short vk, const bool* state)
	{
		// state swtich
		if (vk == (Qi::set.key & 0xFFFF) || vk == (Qi::set.key >> 16))
		{
			bool k1 = state[Qi::set.key & 0xFFFF];
			bool k2 = true;
			if (Qi::set.key >> 16) k2 = state[Qi::set.key >> 16];
			if (k1 && k2) QiState(!Qi::state);
		}
		// show clock
		if (Qi::fun.showClock.state && Qi::fun.showClock.key == vk && state[Qi::fun.showClock.key])
		{
			Qi::popText->Popup(QDateTime::currentDateTime().toString(L"hh:mm:ss"));
		}
		if (!Qi::run) return;
		// quick click
		if (Qi::fun.quickClick.state && Qi::fun.quickClick.key == vk)
		{
			if (state[Qi::fun.quickClick.key]) // key down
			{
				if (Qi::fun.quickClick.mode) // switch mode
				{
					if (QiThread::QuickClickActive())
					{
						QiThread::ExitQuickClick();
						if (Qi::set.showTips) QuickClickPop(false);
					}
					else
					{
						QiThread::StartQuickClick();
						if (Qi::set.showTips) QuickClickPop(true);
					}
				}
				else // press mode
				{
					if (!QiThread::QuickClickActive())
					{
						QiThread::StartQuickClick();
						if (Qi::set.showTips) QuickClickPop(true);
					}
				}
			}
			else // key up
			{
				if (!Qi::fun.quickClick.mode) // press mode
				{
					if (QiThread::QuickClickActive())
					{
						QiThread::ExitQuickClick();
						if (Qi::set.showTips) QuickClickPop(false);
					}
				}
			}
		}
		// macros
		for (auto& m : Qi::macroActive)
		{
			Macro& macro = *m;
			if (vk == (macro.key & 0xFFFF) || vk == (macro.key >> 16))
			{
				if (macro.state)
				{
					bool k1 = state[macro.key & 0xFFFF];
					bool k2 = true;
					if (macro.key >> 16) k2 = state[macro.key >> 16];
					switch (macro.mode)
					{
					case Macro::sw:
					{
						if (k1 && k2)
						{
							if (QiThread::MacroRunActive(&macro))
							{
								QiThread::ExitMacroRun(&macro);
								QiThread::StartMacroEnd(&macro);
								if (Qi::set.showTips) MacroPop(&macro, false);
							}
							else
							{
								QiThread::ExitMacroEnd(&macro);
								QiThread::StartMacroRun(&macro);
								if (Qi::set.showTips) MacroPop(&macro, true);
							}
						}
					} break;
					case Macro::down:
					{
						if (k1 && k2)
						{
							QiThread::ExitMacroRun(&macro);
							QiThread::ExitMacroEnd(&macro);
							QiThread::StartMacroRun(&macro);
							if (Qi::set.showTips) MacroPop(&macro, true);
						}
						else
						{
							if (macro.count == 0 && QiThread::MacroRunActive(&macro))
							{
								QiThread::ExitMacroRun(&macro);
								QiThread::StartMacroEnd(&macro);
								if (Qi::set.showTips) MacroPop(&macro, false);
							}
						}
					} break;
					case Macro::up:
					{
						if (k1 && k2)
						{
							macro.active = true;
							if (macro.count == 0 && QiThread::MacroRunActive(&macro))
							{
								QiThread::ExitMacroRun(&macro);
								QiThread::StartMacroEnd(&macro);
								if (Qi::set.showTips) MacroPop(&macro, false);
							}
						}
						else
						{
							if (macro.active)
							{
								macro.active = false;

								QiThread::ExitMacroRun(&macro);
								QiThread::ExitMacroEnd(&macro);
								QiThread::StartMacroRun(&macro);
								if (Qi::set.showTips) MacroPop(&macro, true);
							}
						}
					} break;
					}
				}
			}
		}
	}
	void XBoxPadProc(short keyCode, short state)
	{
		Qi::keyState[keyCode] = (bool)state;
		Trigger(keyCode, Qi::keyState);
	}
	void QiHook(bool state)
	{
		if (state)
		{
			memset(Qi::keyState, 0, sizeof(Qi::keyState));
			if (!InputHook::State())
			{
				timeBeginPeriod(1); // set clock accuracy
				if (!InputHook::Start()) MsgBox::Error(L"创建输入Hook失败，检查是否管理员身份运行 或 是否被安全软件拦截。");
				Qi::xboxpad.setStateEvent(XBoxPadProc, true);
			}
		}
		else
		{
			if (InputHook::State())
			{
				timeEndPeriod(1); // reset clock accuracy
				InputHook::Close();
				Qi::xboxpad.closeStateEvent();
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
					if (m.state && m.key)
					{
						if (m.keyBlock)
						{
							if (m.key & 0xFFFF) Qi::keyBlock[m.key & 0xFFFF] = true;
							if (m.key >> 16) Qi::keyBlock[m.key >> 16] = true;
						}
						Qi::macroActive.append(&m);
					}
				}
			}

			Qi::state = true;
			StatePop(true);
			if (Qi::set.audFx) Sound::WavePlay(sound_on);
			if (Qi::fun.wndActive.state) { if (!QiThread::WindowStateActive()) QiThread::StartWindowState(); }
			else Qi::run = true;
		}
		else
		{
			Qi::state = false, Qi::run = false;
			StatePop(false);
			if (Qi::fun.wndActive.state) { if (QiThread::WindowStateActive()) QiThread::ExitWindowState(); }
			if (Qi::set.audFx)Sound::WavePlay(sound_off);
		}
	}

	bool SelfActive() { return !(Qi::widget.mainActive || Qi::widget.dialogActive || Qi::widget.moreActive); }
	void SmoothMove(const int sx, const int sy, const int dx, const int dy, const int speed, std::function<void(int x, int y, int stepx, int stepy)> CallBack)
	{
		int cx = dx - sx;
		int cy = dy - sy;

		int px = 0;
		int py = 0;

		float step = (float)speed / 300.0f;
		if (step < 0.01f) step = 0.01f;
		if (step > 0.99f) step = 0.99f;

		for (float i = 0.0f; true; i += step)
		{
			if (i > 1.0f) i = 1.0f;
			int x = sx + (int)((float)cx * i);
			int y = sy + (int)((float)cy * i);
			if (i < 1.0f)
			{
				CallBack(x, y, x - px, y - py);
				px = x, py = y;
			}
			else
			{
				CallBack(dx, dy, dx - px, dy - py);
				break;
			}
		}
	}
	WndInfo WindowSelection()
	{
		WndInfo wndInfo;
		wndInfo.wnd = Qi::windowSelection->Start();
		if (wndInfo.wnd)
		{
			wndInfo.wndClass = Window::className(wndInfo.wnd);
			wndInfo.wndName = Window::text(wndInfo.wnd);
		}
		return wndInfo;
	}
	QiBlock* FindBlock(Actions& actions, int id)
	{
		for (size_t i = 0; (i < actions.size()); i++)
		{
			if (actions[i].index() == QiType::block)
			{
				QiBlock& block = (QiBlock&)actions[i].base();
				if (block.id == id) return &block;
			}
		}
		return nullptr;
	}
	const QiBlock* FindBlock(const Actions& actions, int id)
	{
		for (size_t i = 0; (i < actions.size()); i++)
		{
			if (actions[i].index() == QiType::block)
			{
				const QiBlock& block = (const QiBlock&)actions[i].base();
				if (block.id == id) return &block;
			}
		}
		return nullptr;
	}
}