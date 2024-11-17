#pragma execution_character_set("utf-8")
#include "func.h"

namespace QiFn
{
	POINT RTA(POINT rel) { return { (long)(((float)rel.x / (float)(Qi::screen.cx - 1)) * 10000.0f), (long)(((float)rel.y / (float)(Qi::screen.cy - 1)) * 10000.0f) }; }
	RECT RTAR(RECT rel) { return { (long)(((float)rel.left / (float)(Qi::screen.cx - 1)) * 10000.0f), (long)(((float)rel.top / (float)(Qi::screen.cy - 1)) * 10000.0f), (long)(((float)rel.right / (float)(Qi::screen.cx - 1)) * 10000.0f), (long)(((float)rel.bottom / (float)(Qi::screen.cy - 1)) * 10000.0f) }; }
	POINT ATR(POINT abs) { return { (long)((float)Qi::screen.cx / 10000.0f * (float)abs.x), (long)((float)Qi::screen.cy / 10000.0f * (float)abs.y) }; }
	RECT ATRR(RECT abs) { return { (long)((float)Qi::screen.cx / 10000.0f * (float)abs.left), (long)((float)Qi::screen.cy / 10000.0f * (float)abs.top), (long)((float)Qi::screen.cx / 10000.0f * (float)abs.right), (long)((float)Qi::screen.cy / 10000.0f * (float)abs.bottom) }; }
	POINT WRTA(POINT rel, HWND wnd) { SIZE size = Window::size(wnd); return { (long)(((float)rel.x / ((float)size.cx)) * 10000.0f), (long)(((float)rel.y / ((float)size.cy)) * 10000.0f) }; }
	RECT WRTAR(RECT rel, HWND wnd) { SIZE size = Window::size(wnd); return { (long)(((float)rel.left / ((float)size.cx)) * 10000.0f), (long)(((float)rel.top / ((float)size.cy)) * 10000.0f), (long)(((float)rel.right / ((float)size.cx)) * 10000.0f), (long)(((float)rel.bottom / ((float)size.cy)) * 10000.0f) }; }
	POINT WATR(POINT abs, HWND wnd) { SIZE size = Window::size(wnd); return { (long)((float)(size.cx + 1) / 10000.0f * (float)abs.x), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.y) }; }
	RECT WATRR(RECT abs, HWND wnd) { SIZE size = Window::size(wnd); return { (long)((float)(size.cx + 1) / 10000.0f * (float)abs.left), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.top), (long)((float)(size.cx + 1) / 10000.0f * (float)abs.right), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.bottom) }; }

	QString ParseCustom(QString text, QString name, QString number) { text.replace("@", name); return text.replace("$", number); }
	QString ParseState(QString text) { return text.replace("@", QString::fromWCharArray(Qi::dir.c_str())); }
	QString ParseWindow(QString text, QString window) { return text.replace("@", window); }
	QString ParseQuickClick(QString text, short key) { return text.replace("@", QKeyEdit::keyName(key)); }
	QString ParseMacro(QString text, QString macro, int count) { if (count) text.replace("$", QString::number(count)); else text.replace("$", "∞"); return text.replace("@", macro); }
	void StatePop(bool state)
	{
		if (state) Qi::popText->Popup(Qi::ui.pop.time, ParseState(Qi::ui.pop.qe.t), Qi::ui.pop.qe.c);
		else Qi::popText->Popup(Qi::ui.pop.time, ParseState(Qi::ui.pop.qd.t), Qi::ui.pop.qd.c);
	}
	void WindowPop(std::wstring window, bool state)
	{
		if (state) Qi::popText->Popup(Qi::ui.pop.time, ParseWindow(Qi::ui.pop.we.t, QString::fromWCharArray(window.c_str())), Qi::ui.pop.we.c);
		else Qi::popText->Popup(Qi::ui.pop.time, ParseWindow(Qi::ui.pop.wd.t, QString::fromWCharArray(window.c_str())), Qi::ui.pop.wd.c);
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
			if (state) Qi::popText->Popup(Qi::ui.pop.time, ParseMacro(Qi::ui.pop.swe.t, QString::fromWCharArray(macro->name.c_str()), 0), Qi::ui.pop.swe.c);
			else Qi::popText->Popup(Qi::ui.pop.time, ParseMacro(Qi::ui.pop.swd.t, QString::fromWCharArray(macro->name.c_str()), 0), Qi::ui.pop.swd.c);
		}
		else if (macro->mode == Macro::down)
		{
			if (state) Qi::popText->Popup(Qi::ui.pop.time, ParseMacro(Qi::ui.pop.dwe.t, QString::fromWCharArray(macro->name.c_str()), macro->count), Qi::ui.pop.dwe.c);
			else Qi::popText->Popup(Qi::ui.pop.time, ParseMacro(Qi::ui.pop.dwd.t, QString::fromWCharArray(macro->name.c_str()), macro->count), Qi::ui.pop.dwd.c);
		}
		else if (macro->mode == Macro::up)
		{
			if (state) Qi::popText->Popup(Qi::ui.pop.time, ParseMacro(Qi::ui.pop.upe.t, QString::fromWCharArray(macro->name.c_str()), macro->count), Qi::ui.pop.upe.c);
			else Qi::popText->Popup(Qi::ui.pop.time, ParseMacro(Qi::ui.pop.upd.t, QString::fromWCharArray(macro->name.c_str()), macro->count), Qi::ui.pop.upd.c);
		}
	}

	bool SelfActive() { if (Qi::widget.mainActive || Qi::widget.dialogActive || Qi::widget.moreActive) return false; }
	void SmoothMove(const int sx, const int sy, const int dx, const int dy, const int speed, void(*CallBack)(int x, int y, int stepx, int stepy))
	{
		int cx = dx - sx;
		int cy = dy - sy;

		int px = 0;
		int py = 0;

		float step = (float)speed / 300.0f;
		if (step < 0.01f) step = 0.01f;
		if (step > 0.99f) step = 0.99f;

		for (float i = 0.0f; i < 1.0f; i += step)
		{
			if (i > 1.0f) i = 1.0f;
			int x = sx + (int)((float)cx * i);
			int y = sy + (int)((float)cy * i);
			CallBack(x, y, x - px, y - py);
			px = x;
			py = y;
		}
		CallBack(dx, dy, sx - px, sy - py);
	}
	WndInfo WindowSelection()
	{
		WndInfo wi;
		QWindowSelection ws;
		wi.wnd = ws.Start();
		wi.wndClass = Window::className(wi.wnd);
		wi.wndName = Window::text(wi.wnd);
		return wi;
	}
	std::wstring AllocName(std::wstring name)
	{
		wstringList names;
		for (size_t i = 0; i < Qi::macros.size(); i++)
		{
			names.push_back(Qi::macros[i].name);
		}
		return File::Unique(names, name);
	}

	void Trigger(short vk, const bool* state)
	{
		// state swtich
		if (vk == (Qi::set.key & 0xFFFF) || vk == (Qi::set.key >> 16))
		{
			bool k1 = state[Qi::set.key & 0xFFFF];
			bool k2 = true;
			if (Qi::set.key >> 16) k2 = state[Qi::set.key >> 16];
			if (k1 && k2)
			{
				if (Qi::state) QiState(false);
				else QiState(true);
			}
		}

		// show clock
		if (Qi::fun.showClock.state && Qi::fun.showClock.key == vk && state[Qi::fun.showClock.key]) { Qi::popText->Popup(QString::fromWCharArray(Time::toWStringT().c_str())); }

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
						QiThread::AddReleaseKey(Qi::fun.quickClick.key);
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
						QiThread::AddReleaseKey(Qi::fun.quickClick.key);
						if (Qi::set.showTips) QuickClickPop(false);
					}
				}
			}
		}

		// macros
		for (uint32 i = 0; i < Qi::macros.size(); i++)
		{
			if (vk == (Qi::macros.at(i).key & 0xFFFF) || vk == (Qi::macros.at(i).key >> 16))
			{
				if (Qi::macros.at(i).state)
				{
					bool k1 = state[Qi::macros.at(i).key & 0xFFFF];
					bool k2 = true;
					if (Qi::macros.at(i).key >> 16) k2 = state[Qi::macros.at(i).key >> 16];

					switch (Qi::macros.at(i).mode)
					{
					case Macro::sw:
					{
						if (k1 && k2)
						{
							if (QiThread::MacroRunActive(&Qi::macros.at(i)))
							{
								QiThread::ExitMacroRun(&Qi::macros.at(i));
								QiThread::StartMacroEnd(&Qi::macros.at(i));
								if (Qi::set.showTips) MacroPop(&Qi::macros.at(i), false);
							}
							else
							{
								QiThread::ExitMacroEnd(&Qi::macros.at(i));
								QiThread::StartMacroRun(&Qi::macros.at(i));
								if (Qi::set.showTips) MacroPop(&Qi::macros.at(i), true);
							}
						}
					}
					break;

					case Macro::down:
					{
						if (k1 && k2)
						{
							QiThread::ExitMacroRun(&Qi::macros.at(i));
							QiThread::ExitMacroEnd(&Qi::macros.at(i));
							QiThread::StartMacroRun(&Qi::macros.at(i));
							if (Qi::set.showTips) MacroPop(&Qi::macros.at(i), true);
						}
						else
						{
							if (Qi::macros.at(i).count == 0 && QiThread::MacroRunActive(&Qi::macros.at(i)))
							{
								QiThread::ExitMacroRun(&Qi::macros.at(i));
								QiThread::StartMacroEnd(&Qi::macros.at(i));
								if (Qi::set.showTips) MacroPop(&Qi::macros.at(i), false);
							}
						}
					}
					break;

					case Macro::up:
					{
						if (k1 && k2)
						{
							Qi::macros.at(i).active = true;
							if (Qi::macros.at(i).count == 0 && QiThread::MacroRunActive(&Qi::macros.at(i)))
							{
								QiThread::ExitMacroRun(&Qi::macros.at(i));
								QiThread::StartMacroEnd(&Qi::macros.at(i));
								if (Qi::set.showTips) MacroPop(&Qi::macros.at(i), false);
							}
						}
						else
						{
							if (Qi::macros.at(i).active)
							{
								Qi::macros.at(i).active = false;

								QiThread::ExitMacroRun(&Qi::macros.at(i));
								QiThread::ExitMacroEnd(&Qi::macros.at(i));
								QiThread::StartMacroRun(&Qi::macros.at(i));
								if (Qi::set.showTips) MacroPop(&Qi::macros.at(i), true);
							}
						}
					}
					break;
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
		if (state)
		{
			// Update block keys
			{
				Qi::blockKeys.clear();
				for (uint32 u = 0; u < Qi::macros.size(); u++)
				{
					if (Qi::macros[u].state && Qi::macros[u].block)
					{
						if ((Qi::macros[u].key & 0xFFFF)) Qi::blockKeys.Add((Qi::macros[u].key & 0xFFFF));
						if (Qi::macros[u].key >> 16) Qi::blockKeys.Add(Qi::macros[u].key >> 16);
					}
				}
			}
			Qi::state = true;
			StatePop(true);
			if (Qi::set.audFx) Media::WavePlay(audfx_on);
			if (Qi::fun.wndActive.state) { if (!QiThread::WindowStateActive()) QiThread::StartWindowState(); }
			else Qi::run = true;
		}
		else
		{
			Qi::state = false, Qi::run = false;
			StatePop(false);
			if (Qi::fun.wndActive.state) { if (QiThread::WindowStateActive()) QiThread::ExitWindowState(); }
			if (Qi::set.audFx)Media::WavePlay(audfx_off);
		}
	}
}