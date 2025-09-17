#include "func.h"

namespace QiFn
{
	// Pos convert
	POINT P_RTA(const POINT& rel, const SIZE& size)
	{
		return { (long)(((float)rel.x / (float)size.cx) * 10000.0f), (long)(((float)rel.y / (float)size.cy) * 10000.0f) };
	}
	POINT P_ATR(const POINT& abs, const SIZE& size)
	{
		return { (long)((float)size.cx / 10000.0f * (float)abs.x), (long)((float)size.cy / 10000.0f * (float)abs.y) };
	}
	QPointF PF_RTA(const POINT& rel, const SIZE& size)
	{
		return { (float)rel.x / (float)(size.cx), (float)rel.y / (float)(size.cy) };
	}
	POINT PF_ATR(const QPointF& abs, const SIZE& size)
	{
		return { (long)((float)size.cx * (float)abs.x()), (long)((float)size.cy * (float)abs.y()) };
	}
	RECT R_RTA(const RECT& rel, const SIZE& size)
	{
		RECT rel_s(rel);
		if (rel_s.right - rel_s.left <= 0)
		{
			if (rel_s.right + 1 < size.cx) rel_s.right += 1;
			else rel_s.left -= 1;
		}
		if (rel_s.bottom - rel_s.top <= 0)
		{
			if (rel_s.bottom + 1 < size.cy) rel_s.bottom += 1;
			else rel_s.top -= 1;
		}
		return { (long)(((float)rel.left / (float)(size.cx - 1)) * 10000.0f), (long)(((float)rel.top / (float)(size.cy - 1)) * 10000.0f), (long)(((float)rel.right / (float)(Qi::screen.cx - 1)) * 10000.0f), (long)(((float)rel.bottom / (float)(Qi::screen.cy - 1)) * 10000.0f) };
	}
	RECT R_ATR(const RECT& abs, const SIZE& size)
	{
		RECT rect = { (long)((float)size.cx / 10000.0f * (float)abs.left), (long)((float)size.cy / 10000.0f * (float)abs.top), (long)((float)size.cx / 10000.0f * (float)abs.right), (long)((float)size.cy / 10000.0f * (float)abs.bottom) };
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
	QRectF RF_RTA(const RECT& rel, const SIZE& size)
	{
		RECT rel_s(rel);
		if (rel_s.right - rel_s.left <= 0)
		{
			if (rel_s.right + 1 < size.cx) rel_s.right += 1;
			else rel_s.left -= 1;
		}
		if (rel_s.bottom - rel_s.top <= 0)
		{
			if (rel_s.bottom + 1 < size.cy) rel_s.bottom += 1;
			else rel_s.top -= 1;
		}
		return { (float)rel.left / (float)size.cx, (float)rel.top / (float)size.cy, (float)rel.right / (float)Qi::screen.cx, (float)rel.bottom / (float)Qi::screen.cy };
	}
	RECT RF_ATR(const QRectF& abs, const SIZE& size)
	{
		RECT rect = { (long)((float)size.cx * (float)abs.left()), (long)((float)size.cy * (float)abs.top()), (long)((float)size.cx * (float)abs.right()), (long)((float)size.cy * (float)abs.bottom()) };
		if (rect.right - rect.left <= 0)
		{
			if (rect.right + 1 < size.cx) rect.right += 1;
			else rect.left -= 1;
		}
		if (rect.bottom - rect.top <= 0)
		{
			if (rect.bottom + 1 < size.cy) rect.bottom += 1;
			else rect.top -= 1;
		}
		return rect;
	}

	POINT P_ATA(const QPointF& abs)
	{
		return { (long)(abs.x() * 10000.0f), (long)(abs.y() * 10000.0f) };
	}
	QPointF P_ATA(const POINT& abs)
	{
		return { abs.x / 10000.0f, abs.y / 10000.0f };
	}

	POINT P_SRTA(const POINT& rel) { return P_RTA(rel, Qi::screen); }
	POINT P_SATR(const POINT& abs) { return P_ATR(abs, Qi::screen); }
	QPointF PF_SRTA(const POINT& abs) { return PF_RTA(abs, Qi::screen); }
	POINT PF_SATR(const QPointF& abs) { return PF_ATR(abs, Qi::screen); }
	RECT R_SRTA(const RECT& rel) { return R_RTA(rel, Qi::screen); }
	RECT R_SATR(const RECT& abs) { return R_ATR(abs, Qi::screen); }
	QRectF RF_SRTA(const RECT& rel) { return RF_RTA(rel, Qi::screen); }
	RECT RF_SATR(const QRectF& abs) { return RF_ATR(abs, Qi::screen); }

	POINT P_WRTA(const POINT& rel, const HWND& wnd) { return P_RTA(rel, Window::size(wnd)); }
	POINT P_WATR(const POINT& abs, const HWND& wnd) { return P_ATR(abs, Window::size(wnd)); }
	QPointF PF_WATR(const POINT& rel, const HWND& wnd) { return PF_RTA(rel, Window::size(wnd)); }
	POINT PF_WATR(const QPointF& abs, const HWND& wnd) { return PF_ATR(abs, Window::size(wnd)); }
	RECT R_WRTA(const RECT& rel, const HWND& wnd) { return R_RTA(rel, Window::size(wnd)); }
	RECT R_WATR(const RECT& abs, const HWND& wnd) { return R_ATR(abs, Window::size(wnd)); }
	QRectF RF_WRTA(const RECT& rel, const HWND& wnd) { return RF_RTA(rel, Window::size(wnd)); }
	RECT RF_WATR(const QRectF& abs, const HWND& wnd) { return RF_ATR(abs, Window::size(wnd)); }

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
						Qi::fun.quickClick.thread.exit();
						if (Qi::set.showTips) QuickClickPop(false);
						if (Qi::set.audFx) SoundPlay(Qi::ui.pop.qcd.s, false);
					}
					else
					{
						Qi::fun.quickClick.thread.start();
						if (Qi::set.showTips) QuickClickPop(true);
						if (Qi::set.audFx) SoundPlay(Qi::ui.pop.qce.s, false);
					}
				}
				else // press mode
				{
					if (!Qi::fun.quickClick.thread.active())
					{
						Qi::fun.quickClick.thread.start();
						if (Qi::set.showTips) QuickClickPop(true);
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
						Qi::fun.quickClick.thread.exit();
						if (Qi::set.showTips) QuickClickPop(false);
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
							if (Qi::set.showTips) MacroPop(&macro, false);
							if (Qi::set.audFx) SoundPlay(Qi::ui.pop.swd.s, false);
						}
						else
						{
							macro.thread.run_start(&macro);
							if (Qi::set.showTips) MacroPop(&macro, true);
							if (Qi::set.audFx) SoundPlay(Qi::ui.pop.swe.s, false);
						}
					}
				} break;
				case Macro::down:
				{
					if (k1 && k2)
					{
						macro.thread.run_start(&macro);
						if (Qi::set.showTips) MacroPop(&macro, true);
						if (Qi::set.audFx) SoundPlay(Qi::ui.pop.dwe.s, false);
					}
					else
					{
						if (macro.count == 0 && macro.thread.run_active())
						{
							macro.thread.end_start(&macro);
							if (Qi::set.showTips) MacroPop(&macro, false);
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
							if (Qi::set.showTips) MacroPop(&macro, false);
							if (Qi::set.audFx) SoundPlay(Qi::ui.pop.upd.s, false);
						}
					}
					else
					{
						if (macro.active)
						{
							macro.active = false;
							macro.thread.run_start(&macro);
							if (Qi::set.showTips) MacroPop(&macro, true);
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
#ifndef DEBUG
				if (!InputHook::Start()) MsgBox::Error(L"创建输入Hook失败，检查是否管理员身份运行 或 是否被安全软件拦截。");
#endif
				if (Qi::set.pad) Qi::xboxpad.setStateEvent(XBoxPadProc, true);
			}
		}
		else
		{
			if (InputHook::State())
			{
				timeEndPeriod(1); // reset clock accuracy
				InputHook::Close();
				if (Qi::set.pad) Qi::xboxpad.closeStateEvent();
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
						Qi::macroActive.append(&m);
					}
				}
			}

			Qi::state = true;
			if (Qi::fun.wndActive.state) { if (!Qi::fun.wndActive.thread.active()) Qi::fun.wndActive.thread.start(); }
			else Qi::run = true;
			StatePop(true);
			if (Qi::set.audFx) SoundPlay(Qi::ui.pop.qe.s, false);
		}
		else
		{
			Qi::state = false, Qi::run = false;
			if (Qi::fun.wndActive.state) { if (Qi::fun.wndActive.thread.active()) Qi::fun.wndActive.thread.exit(); }
			StatePop(false);
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
		if (File::PathState(dll))
		{
			Qi::ocr_ver = QiOcrInterfaceVersion(dll);
			if (Qi::ocr_ver > 2)
			{
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
			else MsgBox::Warning(L"文字识别版本低于2，需要更新");
		}
		else if (warning) MsgBox::Warning(L"没有安装文字识别功能");
	}

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