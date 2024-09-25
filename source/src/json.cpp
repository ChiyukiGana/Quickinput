#pragma execution_character_set("utf-8")
#include "json.h"

namespace QiJson
{
	void LoadDefault()
	{
		qis.set.defOn = true;
		qis.set.key = VK_F8;
		qis.set.recKey = VK_F8;
		qis.set.showTips = true;
		qis.set.audFx = false;
		qis.set.minMode = false;
		qis.set.zoomBlock = false;
		qis.fun.quickClick.state = false;
		qis.fun.quickClick.key = VK_LBUTTON;
		qis.fun.quickClick.delay = 200;
		qis.fun.quickClick.mode = 0;
		qis.fun.showClock.state = false;
		qis.fun.showClock.key = VK_MENU;
		qis.fun.wndActive.state = false;
		qis.fun.wndActive.wi.wndName = L"";
	}
	void LoadDefaultPopBox()
	{
		qis.ui.pop.qe.t = L"@ 启用";
		qis.ui.pop.qd.t = L"@ 禁用";
		qis.ui.pop.we.t = L"@ 窗口启用";
		qis.ui.pop.wd.t = L"@ 窗口禁用";
		qis.ui.pop.qce.t = L"连点 @";
		qis.ui.pop.qcd.t = L"停止 @";
		qis.ui.pop.swe.t = L"执行 @";
		qis.ui.pop.swd.t = L"停止 @";
		qis.ui.pop.dwe.t = L"执行$次 @";
		qis.ui.pop.dwd.t = L"停止 @";
		qis.ui.pop.upe.t = L"执行$次 @";
		qis.ui.pop.upd.t = L"停止 @";
		qis.ui.pop.qe.c = RGB(0xCC, 0xEE, 0xFF);
		qis.ui.pop.qd.c = RGB(0xFF, 0x50, 0x50);
		qis.ui.pop.we.c = RGB(0xAA, 0xBB, 0xFF);
		qis.ui.pop.wd.c = RGB(0xFF, 0xA0, 0xA0);
		qis.ui.pop.qce.c = RGB(0x20, 0xFF, 0x40);
		qis.ui.pop.qcd.c = RGB(0xFF, 0xFF, 0x60);
		qis.ui.pop.swe.c = RGB(0x20, 0xFF, 0x40);
		qis.ui.pop.swd.c = RGB(0xFF, 0xFF, 0x60);
		qis.ui.pop.dwe.c = RGB(0x20, 0xFF, 0x40);
		qis.ui.pop.dwd.c = RGB(0xFF, 0xFF, 0x60);
		qis.ui.pop.upe.c = RGB(0x20, 0xFF, 0x40);
		qis.ui.pop.upd.c = RGB(0xFF, 0xFF, 0x60);
		qis.ui.pop.p = { 5000, 0 };
	}

	void MakeBasePop(neb::CJsonObject& json, const QiUi::PopBoxBase& base)
	{
		json.Clear();
		json.Add("t", String::toString(base.t));
		json.Add("c", (int32)base.c);
	}
	void GetBasePop(const neb::CJsonObject& json, QiUi::PopBoxBase& base)
	{
		std::string str;
		json.Get("c", (int32&)base.c);
		json.Get("t", str);
		base.t = String::toWString(str);
	}

	void SaveAction(neb::CJsonObject& jActions, const Actions& actions)
	{
		for (uint32 u = 0; u < actions.size(); u++)
		{
			const Action& action = actions[u];
			neb::CJsonObject jItem;
			neb::CJsonObject jNext;

			std::string str = String::toString(action.mark.str());
			jItem.Add("mark", str);
			jItem.Add("type", action.type);

			switch (action.type)
			{
			case Action::_delay:
			{
				jItem.Add("ms", action.d.delay.tmin);
				jItem.Add("ex", action.d.delay.tmax);
				break;
			}

			case Action::_key:
			{
				jItem.Add("state", action.d.key.state);
				jItem.Add("vk", action.d.key.vk);
				break;
			}

			case Action::_mouse:
			{
				jItem.Add("move", action.d.mouse.move, true);
				jItem.Add("x", action.d.mouse.x);
				jItem.Add("y", action.d.mouse.y);
				jItem.Add("ex", action.d.mouse.ex);
				jItem.Add("trk", action.d.mouse.track, true);
				jItem.Add("spd", action.d.mouse.speed);
				break;
			}

			case Action::_text:
			{
				jItem.Add("text", String::toString(action.d.text.str.str()));
				break;
			}

			case Action::_color:
			{
				jItem.Add("unfind", action.d.color.unfind, true);
				jItem.Add("move", action.d.color.move, true);
				jItem.Add("left", (int32)action.d.color.rect.left);
				jItem.Add("top", (int32)action.d.color.rect.top);
				jItem.Add("right", (int32)action.d.color.rect.right);
				jItem.Add("bottom", (int32)action.d.color.rect.bottom);
				jItem.Add("rgbe", (uint32)action.d.color.rgbe.toCOLORREF());
				SaveAction(jNext, action.next), jItem.Add("next", jNext);
				break;
			}

			case Action::_loop:
			{
				jItem.Add("count", action.d.loop.cmin);
				jItem.Add("rand", action.d.loop.cmax);
				SaveAction(jNext, action.next), jItem.Add("next", jNext);
				break;
			}

			case Action::_keyState:
			{
				jItem.Add("state", action.d.keyState.state, true);
				jItem.Add("vk", action.d.keyState.vk);
				SaveAction(jNext, action.next), jItem.Add("next", jNext);
				break;
			}

			case Action::_image:
			{
				jItem.Add("unfind", action.d.image.unfind, true);
				jItem.Add("move", action.d.image.move, true);
				jItem.Add("left", (int32)(action.d.image.rect.left));
				jItem.Add("top", (int32)(action.d.image.rect.top));
				jItem.Add("right", (int32)(action.d.image.rect.right));
				jItem.Add("bottom", (int32)(action.d.image.rect.bottom));
				jItem.Add("sim", action.d.image.sim);

				jItem.Add("width", action.d.image.map.width());
				jItem.Add("height", action.d.image.map.height());
				size_t size = Base64::EncodedLength(action.d.image.map.bytes());
				std::string data = base64_encode((byte*)action.d.image.map.data(), action.d.image.map.bytes());
				if (data.size()) jItem.Add("data", data);
				SaveAction(jNext, action.next), jItem.Add("next", jNext);
				break;
			}

			case Action::_popText:
			{
				jItem.Add("text", String::toString(action.d.popText.str.str()));
				jItem.Add("time", action.d.popText.time);
				break;
			}

			case Action::_timer:
			{
				jItem.Add("min", action.d.timer.tmin);
				jItem.Add("max", action.d.timer.tmax);
				SaveAction(jNext, action.next), jItem.Add("next", jNext);
				break;
			}

			}
			jActions.Add(jItem);
		}
	}
	void SaveMacro(Macro& macro)
	{
		neb::CJsonObject json;
		neb::CJsonObject jActions;
		neb::CJsonObject jActionsEnding;
		SaveAction(jActions, macro.acRun);
		SaveAction(jActionsEnding, macro.acEnd);
		json.Add("document_ charset", std::string("UTF8"));
		json.Add("type", std::string("QuickInputMacro"));
		json.Add("wndState", macro.wndState, true);
		json.Add("wndChild", macro.wi.child, true);
		json.Add("wndName", String::toString(macro.wi.wndName));
		json.Add("wndClass", String::toString(macro.wi.wndClass));
		json.Add("state", macro.state, true);
		json.Add("block", macro.block, true);
		json.Add("mode", macro.mode);
		json.Add("key", macro.key);
		json.Add("count", macro.count);
		json.Add("actions", jActions);
		json.Add("actionsEnding", jActionsEnding);
		std::string jstr = json.ToString();
		File::FolderCreate(L"macro");
		std::wstring path = Path::Append(Path::Append(Process::runPath(), L"macro"), macro.name + L".json");
		File::TextSaveU(path, jstr);
	}
	void SaveJson()
	{
		neb::CJsonObject cfg;
		std::string str;
		cfg.Add("document_charset", std::string("UTF8"));
		cfg.Add("type", std::string("QuickInputConfig"));
		cfg.Add("theme", qis.set.theme);
		cfg.Add("key", qis.set.key);
		cfg.Add("recKey", qis.set.recKey);
		cfg.Add("defOn", qis.set.defOn, true);
		cfg.Add("showTips", qis.set.showTips, true);
		cfg.Add("audFx", qis.set.audFx, true);
		cfg.Add("minMode", qis.set.minMode, true);
		cfg.Add("zoomBlock", qis.set.zoomBlock, true);
		cfg.Add("quickClickKey", qis.fun.quickClick.key);
		cfg.Add("quickClickState", qis.fun.quickClick.state, true);
		cfg.Add("quickClickDelay", qis.fun.quickClick.delay);
		cfg.Add("quickClickMode", qis.fun.quickClick.mode);
		cfg.Add("showClockKey", qis.fun.showClock.key);
		cfg.Add("showClockState", qis.fun.showClock.state, true);
		cfg.Add("wndActiveState", qis.fun.wndActive.state, true);
		cfg.Add("wndActiveName", String::toString(qis.fun.wndActive.wi.wndName));
		{
			neb::CJsonObject pbox;
			neb::CJsonObject buf;
			pbox.Add("px", (uint32)qis.ui.pop.p.x);
			pbox.Add("py", (uint32)qis.ui.pop.p.y);
			MakeBasePop(buf, qis.ui.pop.qe);
			pbox.Add("qe", buf);
			MakeBasePop(buf, qis.ui.pop.qd);
			pbox.Add("qd", buf);
			MakeBasePop(buf, qis.ui.pop.we);
			pbox.Add("we", buf);
			MakeBasePop(buf, qis.ui.pop.wd);
			pbox.Add("wd", buf);
			MakeBasePop(buf, qis.ui.pop.qce);
			pbox.Add("qce", buf);
			MakeBasePop(buf, qis.ui.pop.qcd);
			pbox.Add("qcd", buf);
			MakeBasePop(buf, qis.ui.pop.swe);
			pbox.Add("swe", buf);
			MakeBasePop(buf, qis.ui.pop.swd);
			pbox.Add("swd", buf);
			MakeBasePop(buf, qis.ui.pop.dwe);
			pbox.Add("dwe", buf);
			MakeBasePop(buf, qis.ui.pop.dwd);
			pbox.Add("dwd", buf);
			MakeBasePop(buf, qis.ui.pop.upe);
			pbox.Add("upe", buf);
			MakeBasePop(buf, qis.ui.pop.upd);
			pbox.Add("upd", buf);
			cfg.Add("popbox", pbox);
		}
		File::TextSaveU(L"QuickInput.json", cfg.ToString());
	}

	void LoadAction(const neb::CJsonObject jActions, Actions& actions)
	{
		actions.resize(jActions.GetArraySize());
		for (uint32 i = 0; i < jActions.GetArraySize(); i++)
		{
			Action& action = actions.at(i);
			neb::CJsonObject jItem;
			neb::CJsonObject jNext;
			jActions.Get(i, jItem);

			uint32 ui32 = 0;
			std::string str;
			jItem.Get("type", ui32);
			if (ui32)
			{
				switch (ui32)
				{
				case Action::_end: action.type = Action::_end; break;

				case Action::_delay: action.type = Action::_delay;
				{
					jItem.Get("ms", action.d.delay.tmin);
					jItem.Get("ex", action.d.delay.tmax);
					if (action.d.delay.tmax < action.d.delay.tmin) action.d.delay.tmax = action.d.delay.tmin;
					break;
				}

				case Action::_key: action.type = Action::_key;
				{
					jItem.Get("state", action.d.key.state);
					jItem.Get("vk", action.d.key.vk);
					break;
				}

				case Action::_mouse: action.type = Action::_mouse;
				{
					jItem.Get("move", action.d.mouse.move);
					jItem.Get("x", action.d.mouse.x);
					jItem.Get("y", action.d.mouse.y);
					jItem.Get("ex", action.d.mouse.ex);
					jItem.Get("trk", action.d.mouse.track);
					jItem.Get("spd", action.d.mouse.speed);
					if (!action.d.mouse.speed) action.d.mouse.speed = 1;
					break;
				}

				case Action::_text: action.type = Action::_text;
				{
					jItem.Get("text", str);
					action.d.text.str.copy(String::toWString(str).c_str());
					break;
				}

				case Action::_color: action.type = Action::_color;
				{
					jItem.Get("unfind", action.d.color.unfind);
					jItem.Get("move", action.d.color.move);
					jItem.Get("left", (int32&)(action.d.color.rect.left));
					jItem.Get("top", (int32&)(action.d.color.rect.top));
					jItem.Get("right", (int32&)(action.d.color.rect.right));
					jItem.Get("bottom", (int32&)(action.d.color.rect.bottom));
					jItem.Get("rgbe", ui32); action.d.color.rgbe.set(ui32);
					jItem.Get("next", jNext);
					LoadAction(jNext, action.next);
					break;
				}

				case Action::_loop: action.type = Action::_loop;
				{
					jItem.Get("count", action.d.loop.cmin);
					jItem.Get("rand", action.d.loop.cmax);
					if (action.d.loop.cmax < action.d.loop.cmin) action.d.loop.cmax = action.d.loop.cmin;
					jItem.Get("next", jNext);
					LoadAction(jNext, action.next);
					break;
				}

				case Action::_loopEnd: action.type = Action::_loopEnd; break;

				case Action::_keyState: action.type = Action::_keyState;
				{
					jItem.Get("state", action.d.keyState.state);
					jItem.Get("vk", action.d.keyState.vk);
					jItem.Get("next", jNext);
					LoadAction(jNext, action.next);
					break;
				}

				case Action::_revocerPos: action.type = Action::_revocerPos; break;

				case Action::_image: action.type = Action::_image;
				{
					jItem.Get("unfind", action.d.image.unfind);
					jItem.Get("move", action.d.image.move);
					jItem.Get("left", (int32&)(action.d.image.rect.left));
					jItem.Get("top", (int32&)(action.d.image.rect.top));
					jItem.Get("right", (int32&)(action.d.image.rect.right));
					jItem.Get("bottom", (int32&)(action.d.image.rect.bottom));
					jItem.Get("sim", action.d.image.sim);

					uint32 width, height;
					jItem.Get("width", width);
					jItem.Get("height", height);
					if (width && height)
					{
						action.d.image.map.create(width, height);
						std::string str; jItem.Get("data", str);
						if (!Base64::Decode(str.c_str(), str.size(), (char*)action.d.image.map.data(), action.d.image.map.bytes())) action.d.image.map.release();
					}
					jItem.Get("next", jNext);
					LoadAction(jNext, action.next);
					break;
				}

				case Action::_popText: action.type = Action::_popText;
				{
					jItem.Get("text", str);
					jItem.Get("time", action.d.popText.time);
					action.d.popText.str.copy(String::toWString(str).c_str());
					break;
				}

				case Action::_rememberPos: action.type = Action::_rememberPos; break;

				case Action::_timer: action.type = Action::_timer;
				{
					jItem.Get("min", action.d.timer.tmin);
					jItem.Get("max", action.d.timer.tmax);
					if (action.d.timer.tmax < action.d.timer.tmin) action.d.timer.tmax = action.d.timer.tmin;
					jItem.Get("next", jNext);
					LoadAction(jNext, action.next);
					break;
				}

				default: action.type = Action::_none; break;
				}

				jItem.Get("mark", str);
				action.mark.copy(String::toWString(str).c_str());
			}
		}
	}
	void LoadMacro()
	{
		qis.macros.clear();
		File::FileList files = File::FindFile(L"macro\\*.json");
		for (uint32 i = 0; i < files.size(); i++) {
			std::wstring path = Path::Append(Path::Append(Process::runPath(), L"macro"), files[i].name);
			Macro& macro = qis.macros.AddNull(); macro.name = Path::RemoveExtension(files[i].name);

			std::string jstr = File::TextReadU(path);
			if (jstr.size())
			{
				neb::CJsonObject jMacro(jstr);
				std::string str;
				jMacro.Get("wndState", macro.wndState);
				jMacro.Get("wndChild", macro.wi.child);
				jMacro.Get("wndName", str); macro.wi.wndName = String::toWString(str);
				jMacro.Get("wndClass", str); macro.wi.wndClass = String::toWString(str);
				jMacro.Get("state", macro.state);
				jMacro.Get("block", macro.block);
				jMacro.Get("mode", macro.mode);
				jMacro.Get("key", macro.key);
				jMacro.Get("count", macro.count);

				neb::CJsonObject jActions; jMacro.Get("actions", jActions);
				neb::CJsonObject jActionsEnding; jMacro.Get("actionsEnding", jActionsEnding);
				LoadAction(jActions, macro.acRun);
				LoadAction(jActionsEnding, macro.acEnd);
			}
		}
	}
	void LoadJson()
	{
		std::wstring path = Path::Append(Process::runPath(), L"QuickInput.json");
		if (File::FileState(path))
		{
			std::string jstr = File::TextReadU(path);
			if (jstr.size())
			{
				neb::CJsonObject cfg(jstr);
				std::string str;
				cfg.Get("theme", qis.set.theme);
				cfg.Get("key", qis.set.key);
				cfg.Get("recKey", qis.set.recKey);
				cfg.Get("defOn", qis.set.defOn);
				cfg.Get("showTips", qis.set.showTips);
				cfg.Get("audFx", qis.set.audFx);
				cfg.Get("minMode", qis.set.minMode);
				cfg.Get("zoomBlock", qis.set.zoomBlock);
				cfg.Get("quickClickState", qis.fun.quickClick.state);
				cfg.Get("quickClickKey", qis.fun.quickClick.key);
				cfg.Get("quickClickDelay", qis.fun.quickClick.delay);
				cfg.Get("quickClickMode", qis.fun.quickClick.mode);
				cfg.Get("showClockState", qis.fun.showClock.state);
				cfg.Get("showClockKey", qis.fun.showClock.key);
				cfg.Get("wndActiveState", qis.fun.wndActive.state);
				cfg.Get("wndActiveName", str); qis.fun.wndActive.wi.wndName = String::toWString(str);
				{
					neb::CJsonObject pbox;
					cfg.Get("popbox", pbox);
					if (pbox.IsEmpty()) LoadDefaultPopBox();
					else
					{
						neb::CJsonObject buf;
						pbox.Get("qe", buf);
						GetBasePop(buf, qis.ui.pop.qe);
						pbox.Get("qd", buf);
						GetBasePop(buf, qis.ui.pop.qd);
						pbox.Get("we", buf);
						GetBasePop(buf, qis.ui.pop.we);
						pbox.Get("wd", buf);
						GetBasePop(buf, qis.ui.pop.wd);
						pbox.Get("qce", buf);
						GetBasePop(buf, qis.ui.pop.qce);
						pbox.Get("qcd", buf);
						GetBasePop(buf, qis.ui.pop.qcd);
						pbox.Get("swe", buf);
						GetBasePop(buf, qis.ui.pop.swe);
						pbox.Get("swd", buf);
						GetBasePop(buf, qis.ui.pop.swd);
						pbox.Get("dwe", buf);
						GetBasePop(buf, qis.ui.pop.dwe);
						pbox.Get("dwd", buf);
						GetBasePop(buf, qis.ui.pop.dwd);
						pbox.Get("upe", buf);
						GetBasePop(buf, qis.ui.pop.upe);
						pbox.Get("upd", buf);
						GetBasePop(buf, qis.ui.pop.upd);
						pbox.Get("px", (uint32&)qis.ui.pop.p.x);
						pbox.Get("py", (uint32&)qis.ui.pop.p.y);
					}
				}
				LoadMacro();
				return;
			}
		}
		LoadDefault();
		LoadDefaultPopBox();
		LoadMacro();
	}
}