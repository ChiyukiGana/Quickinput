#pragma execution_character_set("utf-8")
#include "json.h"

namespace QiJson
{
	void LoadDefault()
	{
		Qi::set.key = VK_F8;
		Qi::set.recKey = VK_F8;
		Qi::set.recTrack = true;
		Qi::set.defOn = true;
		Qi::set.showTips = true;
		Qi::set.audFx = false;
		Qi::set.minMode = false;
		Qi::set.scaleBlock = false;
		Qi::fun.quickClick.state = false;
		Qi::fun.quickClick.key = VK_LBUTTON;
		Qi::fun.quickClick.delay = 200;
		Qi::fun.quickClick.mode = 0;
		Qi::fun.showClock.state = false;
		Qi::fun.showClock.key = VK_MENU;
		Qi::fun.wndActive.state = false;
		Qi::fun.wndActive.wi.wndName = L"";
	}
	void LoadDefaultPopBox()
	{
		Qi::ui.pop.qe.t = "@ 启用";
		Qi::ui.pop.qd.t = "@ 禁用";
		Qi::ui.pop.we.t = "@ 窗口启用";
		Qi::ui.pop.wd.t = "@ 窗口禁用";
		Qi::ui.pop.qce.t = "连点 @";
		Qi::ui.pop.qcd.t = "停止 @";
		Qi::ui.pop.swe.t = "执行 @";
		Qi::ui.pop.swd.t = "停止 @";
		Qi::ui.pop.dwe.t = "执行$次 @";
		Qi::ui.pop.dwd.t = "停止 @";
		Qi::ui.pop.upe.t = "执行$次 @";
		Qi::ui.pop.upd.t = "停止 @";
		Qi::ui.pop.qe.c = QColor(0xC0, 0xE0, 0xFF);
		Qi::ui.pop.qd.c = QColor(0xFF, 0x50, 0x50);
		Qi::ui.pop.we.c = QColor(0xAA, 0xBB, 0xFF);
		Qi::ui.pop.wd.c = QColor(0xFF, 0xA0, 0xA0);
		Qi::ui.pop.qce.c = QColor(0x20, 0xFF, 0x40);
		Qi::ui.pop.qcd.c = QColor(0xFF, 0xFF, 0x60);
		Qi::ui.pop.swe.c = QColor(0x20, 0xFF, 0x40);
		Qi::ui.pop.swd.c = QColor(0xFF, 0xFF, 0x60);
		Qi::ui.pop.dwe.c = QColor(0x20, 0xFF, 0x40);
		Qi::ui.pop.dwd.c = QColor(0xFF, 0xFF, 0x60);
		Qi::ui.pop.upe.c = QColor(0x20, 0xFF, 0x40);
		Qi::ui.pop.upd.c = QColor(0xFF, 0xFF, 0x60);
		Qi::ui.pop.p = { 5000, 0 };
		Qi::ui.pop.size = 20;
		Qi::ui.pop.time = 1000;
	}

	void SavePopTextInfo(neb::CJsonObject& json, const PopTextInfo& p)
	{
		json.Clear();
		json.Add("t", (const char*)p.t.toUtf8());
		json.Add("c", (uint32)RGB(p.c.red(), p.c.green(), p.c.blue()));
	}
	void LoadPopTextInfo(const neb::CJsonObject& json, PopTextInfo& p)
	{
		std::string str;
		json.Get("t", str);
		p.t = str.c_str();

		uint32 color;
		json.Get("c", color);
		p.c = QColor(GetRValue(color), GetGValue(color), GetBValue(color));
	}

	void SaveAction(neb::CJsonObject& jActions, const Actions& actions)
	{
		for (uint32 u = 0; u < actions.size(); u++)
		{
			const Action& action = actions[u];
			neb::CJsonObject jItem;
			neb::CJsonObject jNext;

			bool failed = false;
			std::visit([&jItem, &jNext, &failed](auto&& var)
				{
					using T = std::decay_t<decltype(var)>;
					if constexpr (std::is_same_v<T, QiEnd>)
					{
						const QiEnd& end = var;
						jItem.Add("type", QiType::end);
						jItem.Add("mark", String::toString(end.mark));
					}
					else if constexpr (std::is_same_v<T, QiDelay>)
					{
						const QiDelay& delay = var;
						jItem.Add("type", QiType::delay);
						jItem.Add("mark", String::toString(delay.mark));
						jItem.Add("ms", delay.min);
						jItem.Add("ex", delay.max);
					}
					else if constexpr (std::is_same_v<T, QiKey>)
					{
						const QiKey& key = var;
						jItem.Add("type", QiType::key);
						jItem.Add("mark", String::toString(key.mark));
						jItem.Add("state", key.state);
						jItem.Add("vk", key.vk);
					}
					else if constexpr (std::is_same_v<T, QiMouse>)
					{
						const QiMouse& mouse = var;
						jItem.Add("type", QiType::mouse);
						jItem.Add("mark", String::toString(mouse.mark));
						jItem.Add("move", mouse.move, true);
						jItem.Add("x", mouse.x);
						jItem.Add("y", mouse.y);
						jItem.Add("ex", mouse.ex);
						jItem.Add("trk", mouse.track, true);
						jItem.Add("spd", mouse.speed);
					}
					else if constexpr (std::is_same_v<T, QiText>)
					{
						const QiText& text = var;
						jItem.Add("type", QiType::text);
						jItem.Add("mark", String::toString(text.mark));
						jItem.Add("text", String::toString(text.str));
					}
					else if constexpr (std::is_same_v<T, QiColor>)
					{
						const QiColor& color = var;
						jItem.Add("type", QiType::color);
						jItem.Add("mark", String::toString(color.mark));
						jItem.Add("unfind", color.unfind, true);
						jItem.Add("move", color.move, true);
						jItem.Add("left", (int32)color.rect.left);
						jItem.Add("top", (int32)color.rect.top);
						jItem.Add("right", (int32)color.rect.right);
						jItem.Add("bottom", (int32)color.rect.bottom);
						jItem.Add("rgbe", (uint32)color.rgbe.toCOLORREF());
						SaveAction(jNext, color.next), jItem.Add("next", jNext);
					}
					else if constexpr (std::is_same_v<T, QiLoop>)
					{
						const QiLoop& loop = var;
						jItem.Add("type", QiType::loop);
						jItem.Add("mark", String::toString(loop.mark));
						jItem.Add("count", loop.min);
						jItem.Add("rand", loop.max);
						SaveAction(jNext, loop.next), jItem.Add("next", jNext);
					}
					else if constexpr (std::is_same_v<T, QiLoopEnd>)
					{
						const QiLoopEnd& loopEnd = var;
						jItem.Add("type", QiType::loopEnd);
						jItem.Add("mark", String::toString(loopEnd.mark));
					}
					else if constexpr (std::is_same_v<T, QiKeyState>)
					{
						const QiKeyState& keyState = var;
						jItem.Add("type", QiType::keyState);
						jItem.Add("mark", String::toString(keyState.mark));
						jItem.Add("state", keyState.state, true);
						jItem.Add("vk", keyState.vk);
						SaveAction(jNext, keyState.next), jItem.Add("next", jNext);
					}
					else if constexpr (std::is_same_v<T, QiRecoverPos>)
					{
						const QiRecoverPos& recoverPos = var;
						jItem.Add("type", QiType::recoverPos);
						jItem.Add("mark", String::toString(recoverPos.mark));
					}
					else if constexpr (std::is_same_v<T, QiImage>)
					{
						const QiImage& image = var;
						jItem.Add("type", QiType::image);
						jItem.Add("mark", String::toString(image.mark));
						jItem.Add("unfind", image.unfind, true);
						jItem.Add("move", image.move, true);
						jItem.Add("left", (int32)(image.rect.left));
						jItem.Add("top", (int32)(image.rect.top));
						jItem.Add("right", (int32)(image.rect.right));
						jItem.Add("bottom", (int32)(image.rect.bottom));
						jItem.Add("sim", image.sim);
						jItem.Add("width", image.map.width());
						jItem.Add("height", image.map.height());
						std::string data(Base64::EncodedLength(image.map.bytes()), '\0');
						if (Base64::Encode((const char*)image.map.data(), image.map.bytes(), &data[0], data.size())) jItem.Add("data", data);
						SaveAction(jNext, image.next), jItem.Add("next", jNext);
					}
					else if constexpr (std::is_same_v<T, QiPopText>)
					{
						const QiPopText& popText = var;
						jItem.Add("type", QiType::popText);
						jItem.Add("mark", String::toString(popText.mark));
						jItem.Add("text", String::toString(popText.str));
						jItem.Add("time", popText.time);
					}
					else if constexpr (std::is_same_v<T, QiRememberPos>)
					{
						const QiRememberPos& rememberPos = var;
						jItem.Add("type", QiType::rememberPos);
						jItem.Add("mark", String::toString(rememberPos.mark));
					}
					else if constexpr (std::is_same_v<T, QiTimer>)
					{
						const QiTimer& timer = var;
						jItem.Add("type", QiType::timer);
						jItem.Add("mark", String::toString(timer.mark));
						jItem.Add("min", timer.min);
						jItem.Add("max", timer.max);
						SaveAction(jNext, timer.next), jItem.Add("next", jNext);
					}
					else
					{
						failed = true;
					}
				}, actions[u]);

			if (!failed) jActions.Add(jItem);
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
		cfg.Add("theme", Qi::set.theme);
		cfg.Add("key", Qi::set.key);
		cfg.Add("recKey", Qi::set.recKey);
		cfg.Add("recTrack", Qi::set.recTrack, true);
		cfg.Add("defOn", Qi::set.defOn, true);
		cfg.Add("showTips", Qi::set.showTips, true);
		cfg.Add("audFx", Qi::set.audFx, true);
		cfg.Add("minMode", Qi::set.minMode, true);
		cfg.Add("scaleBlock", Qi::set.scaleBlock, true);
		cfg.Add("quickClickKey", Qi::fun.quickClick.key);
		cfg.Add("quickClickState", Qi::fun.quickClick.state, true);
		cfg.Add("quickClickDelay", Qi::fun.quickClick.delay);
		cfg.Add("quickClickMode", Qi::fun.quickClick.mode);
		cfg.Add("showClockKey", Qi::fun.showClock.key);
		cfg.Add("showClockState", Qi::fun.showClock.state, true);
		cfg.Add("wndActiveState", Qi::fun.wndActive.state, true);
		cfg.Add("wndActiveName", String::toString(Qi::fun.wndActive.wi.wndName));
		{
			neb::CJsonObject pbox;
			neb::CJsonObject buf;
			pbox.Add("px", (uint32)Qi::ui.pop.p.x);
			pbox.Add("py", (uint32)Qi::ui.pop.p.y);
			pbox.Add("size", Qi::ui.pop.size);
			pbox.Add("time", Qi::ui.pop.time);
			SavePopTextInfo(buf, Qi::ui.pop.qe);
			pbox.Add("qe", buf);
			SavePopTextInfo(buf, Qi::ui.pop.qd);
			pbox.Add("qd", buf);
			SavePopTextInfo(buf, Qi::ui.pop.we);
			pbox.Add("we", buf);
			SavePopTextInfo(buf, Qi::ui.pop.wd);
			pbox.Add("wd", buf);
			SavePopTextInfo(buf, Qi::ui.pop.qce);
			pbox.Add("qce", buf);
			SavePopTextInfo(buf, Qi::ui.pop.qcd);
			pbox.Add("qcd", buf);
			SavePopTextInfo(buf, Qi::ui.pop.swe);
			pbox.Add("swe", buf);
			SavePopTextInfo(buf, Qi::ui.pop.swd);
			pbox.Add("swd", buf);
			SavePopTextInfo(buf, Qi::ui.pop.dwe);
			pbox.Add("dwe", buf);
			SavePopTextInfo(buf, Qi::ui.pop.dwd);
			pbox.Add("dwd", buf);
			SavePopTextInfo(buf, Qi::ui.pop.upe);
			pbox.Add("upe", buf);
			SavePopTextInfo(buf, Qi::ui.pop.upd);
			pbox.Add("upd", buf);
			cfg.Add("popbox", pbox);
		}
		File::TextSaveU(L"QuickInput.json", cfg.ToString());
	}

	void LoadAction(const neb::CJsonObject jActions, Actions& actions)
	{
		size_t size = jActions.GetArraySize();
		for (uint32 i = 0; i < size; i++)
		{
			neb::CJsonObject jItem;
			neb::CJsonObject jNext;
			jActions.Get(i, jItem);

			uint32 ui32 = 0;
			std::string str;
			jItem.Get("type", ui32);
			if (ui32)
			{
				jItem.Get("mark", str);
				switch (ui32)
				{
				case QiType::end:
				{
					QiEnd end; end.mark = String::toWString(str);

					actions.Add(std::move(end));
					break;
				}

				case QiType::delay:
				{
					QiDelay delay; delay.mark = String::toWString(str);

					jItem.Get("ms", delay.min);
					jItem.Get("ex", delay.max);
					if (delay.max < delay.min) delay.max = delay.min;

					actions.Add(std::move(delay));
					break;
				}

				case QiType::key:
				{
					QiKey key; key.mark = String::toWString(str);

					jItem.Get("state", key.state);
					jItem.Get("vk", key.vk);

					actions.Add(std::move(key));
					break;
				}

				case QiType::mouse:
				{
					QiMouse mouse; mouse.mark = String::toWString(str);

					jItem.Get("move", mouse.move);
					jItem.Get("x", mouse.x);
					jItem.Get("y", mouse.y);
					jItem.Get("ex", mouse.ex);
					jItem.Get("trk", mouse.track);
					jItem.Get("spd", mouse.speed);
					if (!mouse.speed) mouse.speed = 1;

					actions.Add(std::move(mouse));
					break;
				}

				case QiType::text:
				{
					QiText text; text.mark = String::toWString(str);

					jItem.Get("text", str);
					text.str = String::toWString(str);

					actions.Add(std::move(text));
					break;
				}

				case QiType::color:
				{
					QiColor color; color.mark = String::toWString(str);

					jItem.Get("unfind", color.unfind);
					jItem.Get("move", color.move);
					jItem.Get("left", (int32&)(color.rect.left));
					jItem.Get("top", (int32&)(color.rect.top));
					jItem.Get("right", (int32&)(color.rect.right));
					jItem.Get("bottom", (int32&)(color.rect.bottom));
					jItem.Get("rgbe", ui32); color.rgbe.set(ui32);
					jItem.Get("next", jNext);
					LoadAction(jNext, color.next);

					actions.Add(std::move(color));
					break;
				}

				case QiType::loop:
				{
					QiLoop loop; loop.mark = String::toWString(str);

					jItem.Get("count", loop.min);
					jItem.Get("rand", loop.max);
					if (loop.max < loop.min) loop.max = loop.min;

					jItem.Get("next", jNext);
					LoadAction(jNext, loop.next);

					actions.Add(std::move(loop));
					break;
				}

				case QiType::loopEnd: actions.Add(QiLoopEnd()); break;

				case QiType::keyState:
				{
					QiKeyState keyState; keyState.mark = String::toWString(str);

					jItem.Get("state", keyState.state);
					jItem.Get("vk", keyState.vk);

					jItem.Get("next", jNext);
					LoadAction(jNext, keyState.next);

					actions.Add(std::move(keyState));
					break;
				}

				case QiType::recoverPos:
				{
					QiRecoverPos recoverPos; recoverPos.mark = String::toWString(str);

					actions.Add(std::move(recoverPos));
					break;
				}

				case QiType::image:
				{
					QiImage image; image.mark = String::toWString(str);

					jItem.Get("unfind", image.unfind);
					jItem.Get("move", image.move);
					jItem.Get("left", (int32&)(image.rect.left));
					jItem.Get("top", (int32&)(image.rect.top));
					jItem.Get("right", (int32&)(image.rect.right));
					jItem.Get("bottom", (int32&)(image.rect.bottom));
					jItem.Get("sim", image.sim);

					uint32 width, height;
					jItem.Get("width", width);
					jItem.Get("height", height);
					std::string str; jItem.Get("data", str);
					if (width && height && str.size())
					{
						image.map.create(width, height);
						if (!Base64::Decode(str.c_str(), str.size(), (char*)image.map.data(), image.map.bytes())) image.map.release();
					}

					jItem.Get("next", jNext);
					LoadAction(jNext, image.next);

					actions.Add(std::move(image));
					break;
				}

				case QiType::popText:
				{
					QiPopText popText; popText.mark = String::toWString(str);

					jItem.Get("text", str);
					jItem.Get("time", popText.time);
					popText.str = String::toWString(str).c_str();

					actions.Add(std::move(popText));
					break;
				}

				case QiType::rememberPos: actions.Add(QiRememberPos()); break;

				case QiType::timer:
				{
					QiTimer timer; timer.mark = String::toWString(str);

					jItem.Get("min", timer.min);
					jItem.Get("max", timer.max);
					if (timer.max < timer.min) timer.max = timer.min;

					jItem.Get("next", jNext);
					LoadAction(jNext, timer.next);

					actions.Add(std::move(timer));
					break;
				}
				}
			}
		}
	}
	void LoadMacro()
	{
		Qi::macros.clear();
		File::FileList files = File::FindFile(L"macro\\*.json");
		for (uint32 i = 0; i < files.size(); i++) {
			std::wstring path = Path::Append(Path::Append(Process::runPath(), L"macro"), files[i].name);
			Macro& macro = Qi::macros.AddNull(); macro.name = Path::RemoveExtension(files[i].name);

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
		LoadDefault();
		LoadDefaultPopBox();
		std::wstring path = Path::Append(Process::runPath(), L"QuickInput.json");
		if (File::FileState(path))
		{
			std::string jstr = File::TextReadU(path);
			if (jstr.size())
			{
				neb::CJsonObject cfg(jstr);
				std::string str;
				cfg.Get("theme", Qi::set.theme);
				cfg.Get("key", Qi::set.key);
				cfg.Get("recTrack", Qi::set.recTrack);
				cfg.Get("recKey", Qi::set.recKey);
				cfg.Get("defOn", Qi::set.defOn);
				cfg.Get("showTips", Qi::set.showTips);
				cfg.Get("audFx", Qi::set.audFx);
				cfg.Get("minMode", Qi::set.minMode);
				cfg.Get("scaleBlock", Qi::set.scaleBlock);
				cfg.Get("quickClickState", Qi::fun.quickClick.state);
				cfg.Get("quickClickKey", Qi::fun.quickClick.key);
				cfg.Get("quickClickDelay", Qi::fun.quickClick.delay);
				cfg.Get("quickClickMode", Qi::fun.quickClick.mode);
				cfg.Get("showClockState", Qi::fun.showClock.state);
				cfg.Get("showClockKey", Qi::fun.showClock.key);
				cfg.Get("wndActiveState", Qi::fun.wndActive.state);
				cfg.Get("wndActiveName", str); Qi::fun.wndActive.wi.wndName = String::toWString(str);
				{
					neb::CJsonObject pbox;
					cfg.Get("popbox", pbox);
					if (pbox.IsEmpty()) LoadDefaultPopBox();
					else
					{
						neb::CJsonObject buf;
						pbox.Get("qe", buf);
						LoadPopTextInfo(buf, Qi::ui.pop.qe);
						pbox.Get("qd", buf);
						LoadPopTextInfo(buf, Qi::ui.pop.qd);
						pbox.Get("we", buf);
						LoadPopTextInfo(buf, Qi::ui.pop.we);
						pbox.Get("wd", buf);
						LoadPopTextInfo(buf, Qi::ui.pop.wd);
						pbox.Get("qce", buf);
						LoadPopTextInfo(buf, Qi::ui.pop.qce);
						pbox.Get("qcd", buf);
						LoadPopTextInfo(buf, Qi::ui.pop.qcd);
						pbox.Get("swe", buf);
						LoadPopTextInfo(buf, Qi::ui.pop.swe);
						pbox.Get("swd", buf);
						LoadPopTextInfo(buf, Qi::ui.pop.swd);
						pbox.Get("dwe", buf);
						LoadPopTextInfo(buf, Qi::ui.pop.dwe);
						pbox.Get("dwd", buf);
						LoadPopTextInfo(buf, Qi::ui.pop.dwd);
						pbox.Get("upe", buf);
						LoadPopTextInfo(buf, Qi::ui.pop.upe);
						pbox.Get("upd", buf);
						LoadPopTextInfo(buf, Qi::ui.pop.upd);
						pbox.Get("time", Qi::ui.pop.time);
						pbox.Get("size", Qi::ui.pop.size);
						pbox.Get("px", (uint32&)Qi::ui.pop.p.x);
						pbox.Get("py", (uint32&)Qi::ui.pop.p.y);
					}
				}
				LoadMacro();
				return;
			}
		}
		LoadMacro();
	}
}