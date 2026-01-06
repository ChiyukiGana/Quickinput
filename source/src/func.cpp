#include "func.h"

namespace QiFn
{
	void Key(char key, bool press)
	{
#ifdef Q_RAWINPUT
		if (Qi::set.rawInput && Qi::rawInput.isInit())
		{
			press ? Qi::rawInput.key_down(key, Qi::key_info) : Qi::rawInput.key_up(key, Qi::key_info);
		}
		else
#endif
		{
			Input::State(key, press, Qi::key_info);
		}
	}
	void Move(int x, int y)
	{
#ifdef Q_RAWINPUT
		if (Qi::set.rawInput && Qi::rawInput.isInit())
		{
			Qi::rawInput.move(x, y, Qi::key_info);
		}
		else
#endif
		{
			Input::Move(x, y, Qi::key_info);
		}
	}
	void MoveTo(int x, int y)
	{
#ifdef Q_RAWINPUT
		if (Qi::set.rawInput && Qi::rawInput.isInit())
		{
			Qi::rawInput.move_to(x, y, Qi::key_info);
		}
		else
#endif
		{
			Input::MoveTo(x, y, Qi::key_info);
		}
	}
	void MoveToA(int x, int y)
	{
#ifdef Q_RAWINPUT
		if (Qi::set.rawInput && Qi::rawInput.isInit())
		{
			POINT pt = QiCvt::SP_AtR({ x, y });
			Qi::rawInput.move_to(pt.x, pt.y, Qi::key_info);
		}
		else
#endif
		{
			Input::MoveToA(x * 6.5535f, y * 6.5535f, Qi::key_info);
		}
	}

	QString FoldText(QString str, int len, bool back)
	{
		int size = str.size();
		str.remove('\r').replace('\n', ' ').replace('\t', ' ');
		if (back)
		{
			str = str.size() - len < 0 ? str.mid(0, len) : str.mid(str.size() - len, len);
			if (size > len) str = QString("...") + str;
		}
		else
		{
			str = str.mid(0, len);
			if (size > len) str = str + QString("...");
		}
		return str;
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
	void WindowPop(const QString& window, bool state)
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
	void SoundPlay(const QString& sound, bool sync)
	{
		Sound::SoundClose();
		PlaySoundW(nullptr, GetModuleHandleW(nullptr), SND_ASYNC);
		if (!sound.isEmpty())
		{
			int index = Qi::ui.sounds.indexOf(sound);
			if (index != -1)
			{
				PlaySoundW((const wchar_t*)Qi::ui.sounds.at(index).utf16(), GetModuleHandleW(nullptr), (sync ? SND_SYNC : SND_ASYNC) | SND_RESOURCE);
			}
			else if (sound.indexOf(".wav") == -1)
			{
				Sound::SoundClose();
				Sound::SoundOpen((const wchar_t*)sound.utf16());
				Sound::SoundPlay(sync);
			}
			else
			{
				Sound::WavePlay((const wchar_t*)sound.utf16(), sync);
			}
		}
	}
	// State
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
						if (Qi::set.showTips && !Qi::ui.pop.qcd.t.isEmpty()) QuickClickPop(false);
						if (Qi::set.audFx) SoundPlay(Qi::ui.pop.qcd.s, false);
					}
					else
					{
						Qi::fun.quickClick.thread.start();
						if (Qi::set.showTips && !Qi::ui.pop.qce.t.isEmpty()) QuickClickPop(true);
						if (Qi::set.audFx) SoundPlay(Qi::ui.pop.qce.s, false);
					}
				}
				else // press mode
				{
					if (!Qi::fun.quickClick.thread.active())
					{
						Qi::fun.quickClick.thread.start();
						if (Qi::set.showTips && !Qi::ui.pop.qce.t.isEmpty()) QuickClickPop(true);
						if (Qi::set.audFx) SoundPlay(Qi::ui.pop.qce.s, false);
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
						if (Qi::set.showTips && !Qi::ui.pop.qcd.t.isEmpty()) QuickClickPop(false);
						if (Qi::set.audFx) SoundPlay(Qi::ui.pop.qcd.s, false);
					}
				}
			}
		}
		// macros
		for (auto& m : Qi::macroActive)
		{
			Macro& macro = *m;
			if (vk == macro.key1 || vk == macro.key2)
			{
				bool k1 = macro.key1 ? state[macro.key1] : true;
				bool k2 = macro.key2 ? state[macro.key2] : true;
				switch (macro.mode)
				{
				case Macro::sw:
				{
					if (k1 && k2)
					{
						if (macro.thread.run_active())
						{
							macro.thread.end_start(&macro);
							if (Qi::set.showTips && !Qi::ui.pop.swd.t.isEmpty()) MacroPop(&macro, false);
							if (Qi::set.audFx) SoundPlay(Qi::ui.pop.swd.s, false);
						}
						else
						{
							macro.thread.run_start(&macro);
							if (Qi::set.showTips && !Qi::ui.pop.swe.t.isEmpty()) MacroPop(&macro, true);
							if (Qi::set.audFx) SoundPlay(Qi::ui.pop.swe.s, false);
						}
					}
				} break;
				case Macro::down:
				{
					if (k1 && k2)
					{
						macro.thread.run_start(&macro);
						if (Qi::set.showTips && !Qi::ui.pop.dwe.t.isEmpty()) MacroPop(&macro, true);
						if (Qi::set.audFx) SoundPlay(Qi::ui.pop.dwe.s, false);
					}
					else
					{
						if (macro.count == 0 && macro.thread.run_active())
						{
							macro.thread.end_start(&macro);
							if (Qi::set.showTips && !Qi::ui.pop.dwd.t.isEmpty()) MacroPop(&macro, false);
							if (Qi::set.audFx) SoundPlay(Qi::ui.pop.dwd.s, false);
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
							if (Qi::set.showTips && !Qi::ui.pop.upd.t.isEmpty()) MacroPop(&macro, false);
							if (Qi::set.audFx) SoundPlay(Qi::ui.pop.upd.s, false);
						}
					}
					else
					{
						if (macro.active)
						{
							macro.active = false;
							macro.thread.run_start(&macro);
							if (Qi::set.showTips && !Qi::ui.pop.upe.t.isEmpty()) MacroPop(&macro, true);
							if (Qi::set.audFx) SoundPlay(Qi::ui.pop.upe.s, false);
						}
					}
				} break;
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
#ifndef DEBUGx
				if (!InputHook::Start()) MsgBox::Error(L"创建输入Hook失败，检查是否管理员身份运行 或 是否被安全软件拦截。");
#endif
#ifdef Q_KEYEDIT_PAD_ENABLED
				if (Qi::set.pad) Qi::xboxpad.setStateEvent(XBoxPadProc, true);
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
			if (Qi::set.showTips && !Qi::ui.pop.qe.t.isEmpty()) StatePop(true);
			if (Qi::set.audFx) SoundPlay(Qi::ui.pop.qe.s, false);
		}
		else
		{
			Qi::state = false, Qi::run = false;
			if (Qi::fun.wndActive.state) { if (Qi::fun.wndActive.thread.active()) Qi::fun.wndActive.thread.stop(); }
			if (Qi::set.showTips && !Qi::ui.pop.qd.t.isEmpty()) StatePop(false);
			if (Qi::set.audFx) SoundPlay(Qi::ui.pop.qd.s, false);
		}
	}
	void UnBlock()
	{
		Qi::curBlock = 0;
		memset(Qi::keyBlock, 0, sizeof Qi::keyBlock);
	}

	void InitOcr(bool warning)
	{
		std::wstring dll = L"OCR\\qiocr.dll";
		if (!File::PathState(dll))
		{
			if (warning) MsgBox::Warning(L"没有安装文字识别功能");
			return;
		}
		Qi::ocr_ver = QiOcrInterfaceVersion(dll);
		if (Qi::ocr_ver < 3)
		{
			MsgBox::Warning(L"文字识别版本低于3，需要更新");
			return;
		}
		std::wstring rec = L"ppocr.onnx";
		std::wstring keys = L"ppocr.keys";
		std::wstring det = L"OCR\\ppdet.onnx";
		if (Qi::set.ocr_current.isEmpty())
		{
			rec = std::wstring(L"OCR\\") + rec;
			keys = std::wstring(L"OCR\\") + keys;
		}
		else
		{
			std::wstring lang = Qi::set.ocr_current.toStdWString() + L"\\";
			rec = std::wstring(L"OCR\\") + lang + rec;
			keys = std::wstring(L"OCR\\") + lang + keys;
		}
		Qi::ocr = QiOcrInterfaceInit(dll, rec, keys, det, Qi::set.ocr_thread);
		if (!Qi::ocr.valid()) MsgBox::Error(L"文字识别加载失败");
	}
#ifdef Q_RAWINPUT
	void InitRawInput(bool warning)
	{
		std::wstring dll = L"RAWIN\\QiRawInput.dll";
		if (!File::PathState(dll))
		{
			if (warning) MsgBox::Warning(L"没有安装驱动");
			return;
		}
		QiRawInputInfo info;
		if (!QiRawInputInfoQuery(dll, info))
		{
			MsgBox::Error(L"加载驱动失败");
			return;
		}
		if (info.version < 1)
		{
			MsgBox::Error(L"驱动信息错误");
			return;
		}
		Qi::rawInput = QiRawInputInit(dll);
		if (!Qi::rawInput.valid())
		{
			MsgBox::Error(L"驱动无效");
			return;
		}
		if (!Qi::rawInput.init(L"RAWIN"))
		{
			MsgBox::Error(L"初始化驱动错误");
			return;
		}
		if (!Qi::rawInput.isInit()) MsgBox::Error(L"初始化驱动失败");
	}
#endif

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

	Macro* FindMacro(const QString& path)
	{
		int i = path.indexOf("/");
		MacroGroup* group = nullptr;
		Macro* macro = nullptr;
		QString macro_name;
		if (i == -1)
		{
			macro_name = path;
			group = &Qi::macroGroups.front();
		}
		else
		{
			macro_name = path.mid(i + 1);
			group = Qi::macroGroups.find([group_name = path.mid(0, i)](const MacroGroup& group) -> bool { return group_name == group.name; });
		}
		if (group)
		{
			macro = group->macros.find([&macro_name](Macro& macro) -> bool { return macro_name == macro.name; });
		}
		return macro;
	}
}