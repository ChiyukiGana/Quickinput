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

	void InitOcr(bool warning)
	{
		std::wstring dll = L"OCR\\qiocr.dll";
		if (!File::FileExist(dll))
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
		if (!File::FileExist(dll))
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
		wndInfo.update_fromHwnd();
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