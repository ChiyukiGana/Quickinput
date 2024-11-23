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
		Qi::fun.quickClick.state = false;
		Qi::fun.quickClick.key = VK_LBUTTON;
		Qi::fun.quickClick.delay = 200;
		Qi::fun.quickClick.mode = 0;
		Qi::fun.showClock.state = false;
		Qi::fun.showClock.key = VK_MENU;
		Qi::fun.wndActive.state = false;
		Qi::fun.wndActive.wi.wndName = "";
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
	QJsonObject SavePopTextInfo(const QiUi::PopTextInfo& p)
	{
		QJsonObject json;
		json.insert("t", p.t);
		json.insert("c", (int)RGB(p.c.red(), p.c.green(), p.c.blue()));
		return json;
	}
	QiUi::PopTextInfo LoadPopTextInfo(const QJsonObject& json)
	{
		QiUi::PopTextInfo info;
		info.t = json.value("t").toString();
		int c = json.value("c").toInt();
		info.c = QColor(GetRValue(c), GetGValue(c), GetBValue(c));
		return info;
	}
	QJsonArray SaveAction(const Actions& actions)
	{
		QJsonArray jActions;
		for (int i = 0; i < actions.size(); i++)
		{
			const Action& action = actions[i];
			QJsonObject jAction;

			bool success = true;

			const Action& var = actions[i];

			jAction.insert("type", (int)var.base().type);
			jAction.insert("mark", var.base().mark);

			switch (var.index())
			{
			case QiType::end:
			{
			} break;
			case QiType::delay:
			{
				const QiDelay& delay = std::get<QiDelay>(var);
				jAction.insert("ms", (int)delay.min);
				jAction.insert("ex", (int)delay.max);
			} break;
			case QiType::key:
			{
				const QiKey& key = std::get<QiKey>(var);
				jAction.insert("state", (int)key.state);
				jAction.insert("vk", (int)key.vk);
			} break;
			case QiType::mouse:
			{
				const QiMouse& mouse = std::get<QiMouse>(var);
				jAction.insert("move", (bool)mouse.move);
				jAction.insert("x", (int)mouse.x);
				jAction.insert("y", (int)mouse.y);
				jAction.insert("ex", (int)mouse.ex);
				jAction.insert("trk", (bool)mouse.track);
				jAction.insert("spd", (int)mouse.speed);
			} break;
			case QiType::copyText:
			{
				const QiCopyText& text = std::get<QiCopyText>(var);
				jAction.insert("text", text.text);
			} break;
			case QiType::color:
			{
				const QiColor& color = std::get<QiColor>(var);
				jAction.insert("move", (bool)color.move);
				jAction.insert("left", (int)color.rect.left);
				jAction.insert("top", (int)color.rect.top);
				jAction.insert("right", (int)color.rect.right);
				jAction.insert("bottom", (int)color.rect.bottom);
				jAction.insert("rgbe", (int)color.rgbe.toCOLORREF());
				jAction.insert("next", SaveAction(color.next));
				jAction.insert("next2", SaveAction(color.next2));
			} break;
			case QiType::loop:
			{
				const QiLoop& loop = std::get<QiLoop>(var);
				jAction.insert("count", (int)loop.min);
				jAction.insert("rand", (int)loop.max);
				jAction.insert("next", SaveAction(loop.next));
				jAction.insert("next2", SaveAction(loop.next2));
			} break;
			case QiType::loopEnd:
			{
			} break;
			case QiType::keyState:
			{
				const QiKeyState& keyState = std::get<QiKeyState>(var);
				jAction.insert("vk", (int)keyState.vk);
				jAction.insert("next", SaveAction(keyState.next));
				jAction.insert("next2", SaveAction(keyState.next2));
			} break;
			case QiType::recoverPos:
			{
			} break;
			case QiType::image:
			{
				const QiImage& image = std::get<QiImage>(var);
				jAction.insert("move", (bool)image.move);
				jAction.insert("left", (int)(image.rect.left));
				jAction.insert("top", (int)(image.rect.top));
				jAction.insert("right", (int)(image.rect.right));
				jAction.insert("bottom", (int)(image.rect.bottom));
				jAction.insert("sim", (int)image.sim);
				jAction.insert("width", (int)image.map.width());
				jAction.insert("height", (int)image.map.height());

				QByteArray data((const char*)image.map.data(), image.map.bytes());
				
				jAction.insert("data", data.toBase64().data());

				jAction.insert("next", SaveAction(image.next));
				jAction.insert("next2", SaveAction(image.next2));
			} break;
			case QiType::popText:
			{
				const QiPopText& popText = std::get<QiPopText>(var);
				jAction.insert("text", popText.text);
				jAction.insert("time", (int)popText.time);
				jAction.insert("sync", (bool)popText.sync);
			} break;
			case QiType::rememberPos:
			{
			} break;
			case QiType::timer:
			{
				const QiTimer& timer = std::get<QiTimer>(var);
				jAction.insert("min", (int)timer.min);
				jAction.insert("max", (int)timer.max);
				jAction.insert("next", SaveAction(timer.next));
				jAction.insert("next2", SaveAction(timer.next2));
			} break;
			case QiType::jump:
			{
				const QiJump& jump = std::get<QiJump>(var);
				jAction.insert("id", (int)jump.id);
			} break;
			case QiType::jumpPoint:
			{
				const QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(var);
				jAction.insert("id", (int)jumpPoint.id);
			} break;
			case QiType::dialog:
			{
				const QiDialog& dialog = std::get<QiDialog>(var);
				jAction.insert("style", (int)dialog.style);
				jAction.insert("title", dialog.title);
				jAction.insert("text", dialog.text);
				jAction.insert("next", SaveAction(dialog.next));
				jAction.insert("next2", SaveAction(dialog.next2));
			} break;
			case QiType::block:
			{
				const QiBlock& block = std::get<QiBlock>(var);
				jAction.insert("id", (int)block.id);
				jAction.insert("next", SaveAction(block.next));
			} break;
			case QiType::blockExec:
			{
				const QiBlockExec& blockExec = std::get<QiBlockExec>(var);
				jAction.insert("id", (int)blockExec.id);
			} break;
			default: success = false;
			}

			if (success) jActions.append(jAction);
		}
		return jActions;
	}
	void SaveMacro(const Macro& macro)
	{
		QJsonObject jMacro;
		jMacro.insert("document_charset", "UTF8");
		jMacro.insert("type", "QuickInputMacro");
		jMacro.insert("wndState", (bool)macro.wndState);
		jMacro.insert("wndChild", (bool)macro.wi.child);
		jMacro.insert("wndName", macro.wi.wndName);
		jMacro.insert("wndClass", macro.wi.wndClass);
		jMacro.insert("state", (bool)macro.state);
		jMacro.insert("block", (bool)macro.block);
		jMacro.insert("mode", (int)macro.mode);
		jMacro.insert("key", (int)macro.key);
		jMacro.insert("count", (int)macro.count);
		jMacro.insert("actions", SaveAction(macro.acRun));
		jMacro.insert("actionsEnding", SaveAction(macro.acEnd));

		QJsonDocument json(jMacro);
		if (!QDir(Qi::macroDir).exists() && !QDir(Qi::macroDir).mkdir(Qi::macroDir)) MsgBox::Error(L"创建宏目录失败");
		if (!File::SaveText(Qi::macroDir + macro.name + Qi::macroType, json.toJson())) MsgBox::Error((const wchar_t*)macro.name.utf16(), L"保存宏失败");
	}
	void SaveJson()
	{
		QJsonObject cfg;
		cfg.insert("document_charset", "UTF8");
		cfg.insert("type", "QuickInputConfig");
		cfg.insert("theme", (int)Qi::set.theme);
		cfg.insert("key", (int)Qi::set.key);
		cfg.insert("recKey", (int)Qi::set.recKey);
		cfg.insert("recTrack", (bool)Qi::set.recTrack);
		cfg.insert("defOn", (bool)Qi::set.defOn);
		cfg.insert("showTips", (bool)Qi::set.showTips);
		cfg.insert("audFx", (bool)Qi::set.audFx);
		cfg.insert("minMode", (bool)Qi::set.minMode);
		cfg.insert("quickClickKey", (int)Qi::fun.quickClick.key);
		cfg.insert("quickClickState", (bool)Qi::fun.quickClick.state);
		cfg.insert("quickClickDelay", (int)Qi::fun.quickClick.delay);
		cfg.insert("quickClickMode", (int)Qi::fun.quickClick.mode);
		cfg.insert("showClockKey", (int)Qi::fun.showClock.key);
		cfg.insert("showClockState", (bool)Qi::fun.showClock.state);
		cfg.insert("wndActiveState", (bool)Qi::fun.wndActive.state);
		cfg.insert("wndActiveName", Qi::fun.wndActive.wi.wndName);
		cfg.insert("wndActiveClass", Qi::fun.wndActive.wi.wndClass);
		if ("pop config")
		{
			QJsonObject pop;
			pop.insert("px", (int)Qi::ui.pop.p.x);
			pop.insert("py", (int)Qi::ui.pop.p.y);
			pop.insert("size", (int)Qi::ui.pop.size);
			pop.insert("time", (int)Qi::ui.pop.time);
			pop.insert("qe", SavePopTextInfo(Qi::ui.pop.qe));
			pop.insert("qd", SavePopTextInfo(Qi::ui.pop.qd));
			pop.insert("we", SavePopTextInfo(Qi::ui.pop.we));
			pop.insert("wd", SavePopTextInfo(Qi::ui.pop.wd));
			pop.insert("qce", SavePopTextInfo(Qi::ui.pop.qce));
			pop.insert("qcd", SavePopTextInfo(Qi::ui.pop.qcd));
			pop.insert("swe", SavePopTextInfo(Qi::ui.pop.swe));
			pop.insert("swd", SavePopTextInfo(Qi::ui.pop.swd));
			pop.insert("dwe", SavePopTextInfo(Qi::ui.pop.dwe));
			pop.insert("dwd", SavePopTextInfo(Qi::ui.pop.dwd));
			pop.insert("upe", SavePopTextInfo(Qi::ui.pop.upe));
			pop.insert("upd", SavePopTextInfo(Qi::ui.pop.upd));
			cfg.insert("popbox", pop);
		}
		QJsonDocument json(cfg);
		if (!File::SaveText(Qi::configFile, json.toJson())) MsgBox::Error(L"保存配置失败");
	}
	Actions LoadAction(QJsonArray jActions)
	{
		Actions actions;
		for (int i = 0; i < jActions.size(); i++)
		{
			QJsonObject jAction;
			jAction = jActions.at(i).toObject();

			int type = jAction.value("type").toInt();
			QString mark;

			if (type)
			{
				mark = jAction.value("mark").toString();
				switch (type)
				{
				case QiType::end:
				{
					QiEnd end; end.mark = mark;

					actions.append(std::move(end));
				} break;
				case QiType::delay:
				{
					QiDelay delay; delay.mark = mark;

					delay.min = jAction.value("ms").toInt();
					delay.max = jAction.value("ex").toInt();
					if (delay.max < delay.min) delay.max = delay.min;

					actions.append(std::move(delay));
				} break;
				case QiType::key:
				{
					QiKey key; key.mark = mark;

					key.state = jAction.value("state").toInt();
					key.vk = jAction.value("vk").toInt();

					actions.append(std::move(key));
				} break;
				case QiType::mouse:
				{
					QiMouse mouse; mouse.mark = mark;

					mouse.move = jAction.value("move").toBool();
					mouse.x = jAction.value("x").toInt();
					mouse.y = jAction.value("y").toInt();
					mouse.ex = jAction.value("ex").toInt();
					mouse.track = jAction.value("trk").toBool();
					mouse.speed = jAction.value("spd").toInt();
					if (!mouse.speed) mouse.speed = 1;
					if (mouse.speed > 99) mouse.speed = 99;

					actions.append(std::move(mouse));
				} break;
				case QiType::copyText:
				{
					QiCopyText text; text.mark = mark;

					text.text = jAction.value("text").toString();

					actions.append(std::move(text));
				} break;
				case QiType::color:
				{
					QiColor color; color.mark = mark;

					color.move = jAction.value("move").toBool();
					color.rect.left = jAction.value("left").toInt();
					color.rect.top = jAction.value("top").toInt();
					color.rect.right = jAction.value("right").toInt();
					color.rect.bottom = jAction.value("bottom").toInt();
					color.rgbe.set(jAction.value("rgbe").toInt());
					color.next = LoadAction(jAction.value("next").toArray());
					color.next2 = LoadAction(jAction.value("next2").toArray());

					actions.append(std::move(color));
				} break;
				case QiType::loop:
				{
					QiLoop loop; loop.mark = mark;

					loop.min = jAction.value("count").toInt();
					loop.max = jAction.value("rand").toInt();
					if (loop.max < loop.min) loop.max = loop.min;

					loop.next = LoadAction(jAction.value("next").toArray());

					actions.append(std::move(loop));
				} break;
				case QiType::loopEnd:
				{
					QiLoopEnd loopEnd; loopEnd.mark = mark;

					actions.append(QiLoopEnd());
				} break;
				case QiType::keyState:
				{
					QiKeyState keyState; keyState.mark = mark;

					keyState.vk = jAction.value("vk").toInt();
					keyState.next = LoadAction(jAction.value("next").toArray());
					keyState.next2 = LoadAction(jAction.value("next2").toArray());

					actions.append(std::move(keyState));
				} break;
				case QiType::recoverPos:
				{
					QiRecoverPos recoverPos; recoverPos.mark = mark;

					actions.append(std::move(recoverPos));
				} break;
				case QiType::image:
				{
					QiImage image; image.mark = mark;

					image.move = jAction.value("move").toBool();
					image.rect.left = jAction.value("left").toInt();
					image.rect.top = jAction.value("top").toInt();
					image.rect.right = jAction.value("right").toInt();
					image.rect.bottom = jAction.value("bottom").toInt();
					image.sim = jAction.value("sim").toInt();

					int width = jAction.value("width").toInt();
					int height = jAction.value("height").toInt();

					QByteArray data = QByteArray::fromBase64(jAction.value("data").toString().toUtf8());

					if (width && height && data.size())
					{
						image.map.create(width, height);
						memcpy_s(image.map.data(), image.map.bytes(), data.data(), data.size());
					}

					image.next = LoadAction(jAction.value("next").toArray());
					image.next2 = LoadAction(jAction.value("next2").toArray());

					actions.append(std::move(image));
				} break;
				case QiType::popText:
				{
					QiPopText popText; popText.mark = mark;

					popText.text = jAction.value("text").toString();
					popText.time = jAction.value("time").toInt();
					popText.sync = jAction.value("sync").toBool();

					actions.append(std::move(popText));
				} break;
				case QiType::rememberPos:
				{
					QiRememberPos rememberPos; rememberPos.mark = mark;

					actions.append(std::move(rememberPos));
				} break;
				case QiType::timer:
				{
					QiTimer timer; timer.mark = mark;

					timer.min = jAction.value("min").toInt();
					timer.max = jAction.value("max").toInt();
					if (timer.max < timer.min) timer.max = timer.min;
					timer.next = LoadAction(jAction.value("next").toArray());

					actions.append(std::move(timer));
				} break;
				case QiType::jump:
				{
					QiJump jump; jump.mark = mark;

					jump.id = jAction.value("id").toInt();
					actions.append(std::move(jump));
				} break;
				case QiType::jumpPoint:
				{
					QiJumpPoint jumpPoint; jumpPoint.mark = mark;

					jumpPoint.id = jAction.value("id").toInt();
					actions.append(std::move(jumpPoint));
				} break;
				case QiType::dialog:
				{
					QiDialog dialog; dialog.mark = mark;

					dialog.style = jAction.value("style").toInt();
					dialog.title = jAction.value("title").toString();
					dialog.text = jAction.value("text").toString();
					dialog.next = LoadAction(jAction.value("next").toArray());
					dialog.next2 = LoadAction(jAction.value("next2").toArray());

					actions.append(std::move(dialog));
				} break;
				case QiType::block:
				{
					QiBlock block; block.mark = mark;

					block.id = jAction.value("id").toInt();
					block.next = LoadAction(jAction.value("next").toArray());

					actions.append(std::move(block));
				} break;
				case QiType::blockExec:
				{
					QiBlockExec blockExec; blockExec.mark = mark;

					blockExec.id = jAction.value("id").toInt();
					actions.append(std::move(blockExec));
				} break;
				default: actions.append(QiBase(type)); break;
				}
			}
		}
		return actions;
	}
	void LoadMacro()
	{
		Qi::macros.clear();
		QFileInfoList files = File::Find(Qi::macroDir, QString("*") + Qi::macroType);
		for (const QFileInfo& file : files)
		{
			QByteArray text;
			if (File::LoadText(file.filePath(), text))
			{
				QJsonDocument json(QJsonDocument::fromJson(text));
				if (json.isObject())
				{
					Macro macro;
					macro.name = file.baseName();
					QJsonObject jMacro(json.object());
					macro.wndState = jMacro.value("wndState").toBool();
					macro.wi.child = jMacro.value("wndChild").toBool();
					macro.wi.wndName = jMacro.value("wndName").toString();
					macro.wi.wndClass = jMacro.value("wndClass").toString();
					macro.state = jMacro.value("state").toBool();
					macro.block = jMacro.value("block").toBool();
					macro.mode = jMacro.value("mode").toInt();
					macro.key = jMacro.value("key").toInt();
					macro.count = jMacro.value("count").toInt();
					macro.acRun = LoadAction(jMacro.value("actions").toArray());
					macro.acEnd = LoadAction(jMacro.value("actionsEnding").toArray());
					Qi::macros.append(std::move(macro));
				}
			}
			else
			{
				MsgBox::Error((const wchar_t*)file.baseName().utf16(), L"加载宏失败");
			}
		}
	}
	void LoadJson()
	{
		LoadDefault();
		LoadDefaultPopBox();
		QByteArray text;
		if (File::LoadText(Qi::configFile, text))
		{
			QJsonDocument json(QJsonDocument::fromJson(text));
			if (json.isObject())
			{
				QJsonObject cfg(json.object());

				Qi::set.theme = cfg.value("theme").toInt();
				Qi::set.key = cfg.value("key").toInt();
				Qi::set.recTrack = cfg.value("recTrack").toBool();
				Qi::set.recKey = cfg.value("recKey").toInt();
				Qi::set.defOn = cfg.value("defOn").toBool();
				Qi::set.showTips = cfg.value("showTips").toBool();
				Qi::set.audFx = cfg.value("audFx").toBool();
				Qi::set.minMode = cfg.value("minMode").toBool();
				Qi::fun.quickClick.state = cfg.value("quickClickState").toBool();
				Qi::fun.quickClick.key = cfg.value("quickClickKey").toInt();
				Qi::fun.quickClick.delay = cfg.value("quickClickDelay").toInt();
				Qi::fun.quickClick.mode = cfg.value("quickClickMode").toInt();
				Qi::fun.showClock.state = cfg.value("showClockState").toBool();
				Qi::fun.showClock.key = cfg.value("showClockKey").toInt();
				Qi::fun.wndActive.state = cfg.value("wndActiveState").toBool();
				Qi::fun.wndActive.wi.wndName = cfg.value("wndActiveName").toString();
				Qi::fun.wndActive.wi.wndClass = cfg.value("wndActiveClass").toString();
				{
					QJsonObject pop;
					pop = cfg.value("popbox").toObject();
					if (pop.isEmpty()) LoadDefaultPopBox();
					else
					{
						Qi::ui.pop.qe = LoadPopTextInfo(pop.value("qe").toObject());
						Qi::ui.pop.qd = LoadPopTextInfo(pop.value("qd").toObject());
						Qi::ui.pop.we = LoadPopTextInfo(pop.value("we").toObject());
						Qi::ui.pop.wd = LoadPopTextInfo(pop.value("wd").toObject());
						Qi::ui.pop.qce = LoadPopTextInfo(pop.value("qce").toObject());
						Qi::ui.pop.qcd = LoadPopTextInfo(pop.value("qcd").toObject());
						Qi::ui.pop.swe = LoadPopTextInfo(pop.value("swe").toObject());
						Qi::ui.pop.swd = LoadPopTextInfo(pop.value("swd").toObject());
						Qi::ui.pop.dwe = LoadPopTextInfo(pop.value("dwe").toObject());
						Qi::ui.pop.dwd = LoadPopTextInfo(pop.value("dwd").toObject());
						Qi::ui.pop.upe = LoadPopTextInfo(pop.value("upe").toObject());
						Qi::ui.pop.upd = LoadPopTextInfo(pop.value("upd").toObject());
						Qi::ui.pop.time = pop.value("time").toInt();
						Qi::ui.pop.size = pop.value("size").toInt();
						Qi::ui.pop.p.x = pop.value("px").toInt();
						Qi::ui.pop.p.y = pop.value("py").toInt();
					}
				}
				LoadMacro();
				return;
			}
		}
		LoadMacro();
	}
}