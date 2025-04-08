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
		Qi::set.tabLock = false;
		Qi::set.tabHideTip = false;
		Qi::fun.quickClick.state = false;
		Qi::fun.quickClick.key = VK_LBUTTON;
		Qi::fun.quickClick.delay = 200;
		Qi::fun.quickClick.mode = 0;
		Qi::fun.showClock.state = false;
		Qi::fun.showClock.key = VK_MENU;
		Qi::fun.wndActive.state = false;
		Qi::fun.wndActive.wndInfo.wndName = "";
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
			QJsonObject jAction;
			const Action& var = actions[i];

			jAction.insert("dis", var.base().disable);
			jAction.insert("type", (int)var.base().type);
			jAction.insert("mark", var.base().mark);

			bool success = true;
			switch (var.index())
			{
			case QiType::end:
			{
			} break;
			case QiType::delay:
			{
				const QiDelay& ref = std::get<QiDelay>(var);
				jAction.insert("ms", (int)ref.min);
				jAction.insert("ex", (int)ref.max);
			} break;
			case QiType::key:
			{
				const QiKey& ref = std::get<QiKey>(var);
				jAction.insert("state", (int)ref.state);
				jAction.insert("vk", (int)ref.vk);
			} break;
			case QiType::mouse:
			{
				const QiMouse& ref = std::get<QiMouse>(var);
				jAction.insert("move", (bool)ref.move);
				jAction.insert("x", (int)ref.x);
				jAction.insert("y", (int)ref.y);
				jAction.insert("ex", (int)ref.ex);
				jAction.insert("trk", (bool)ref.track);
				jAction.insert("spd", (int)ref.speed);
			} break;
			case QiType::copyText:
			{
				const QiCopyText& ref = std::get<QiCopyText>(var);
				jAction.insert("text", ref.text);
			} break;
			case QiType::color:
			{
				const QiColor& ref = std::get<QiColor>(var);
				jAction.insert("move", (bool)ref.move);
				jAction.insert("left", (int)ref.rect.left);
				jAction.insert("top", (int)ref.rect.top);
				jAction.insert("right", (int)ref.rect.right);
				jAction.insert("bottom", (int)ref.rect.bottom);
				jAction.insert("rgbe", (int)ref.rgbe.toCOLORREF());
				jAction.insert("next", SaveAction(ref.next));
				jAction.insert("next2", SaveAction(ref.next2));
			} break;
			case QiType::loop:
			{
				const QiLoop& ref = std::get<QiLoop>(var);
				jAction.insert("count", (int)ref.min);
				jAction.insert("rand", (int)ref.max);
				jAction.insert("next", SaveAction(ref.next));
				jAction.insert("next2", SaveAction(ref.next2));
			} break;
			case QiType::loopEnd:
			{
			} break;
			case QiType::keyState:
			{
				const QiKeyState& ref = std::get<QiKeyState>(var);
				jAction.insert("vk", (int)ref.vk);
				jAction.insert("next", SaveAction(ref.next));
				jAction.insert("next2", SaveAction(ref.next2));
			} break;
			case QiType::resetPos:
			{
			} break;
			case QiType::image:
			{
				const QiImage& ref = std::get<QiImage>(var);
				jAction.insert("move", (bool)ref.move);
				jAction.insert("left", (int)(ref.rect.left));
				jAction.insert("top", (int)(ref.rect.top));
				jAction.insert("right", (int)(ref.rect.right));
				jAction.insert("bottom", (int)(ref.rect.bottom));
				jAction.insert("sim", (int)ref.sim);
				jAction.insert("width", (int)ref.map.width());
				jAction.insert("height", (int)ref.map.height());
				jAction.insert("data", ref.toBase64());
				jAction.insert("next", SaveAction(ref.next));
				jAction.insert("next2", SaveAction(ref.next2));
			} break;
			case QiType::popText:
			{
				const QiPopText& ref = std::get<QiPopText>(var);
				jAction.insert("text", ref.text);
				jAction.insert("time", (int)ref.time);
				jAction.insert("sync", (bool)ref.sync);
			} break;
			case QiType::savePos:
			{
			} break;
			case QiType::timer:
			{
				const QiTimer& ref = std::get<QiTimer>(var);
				jAction.insert("min", (int)ref.min);
				jAction.insert("max", (int)ref.max);
				jAction.insert("next", SaveAction(ref.next));
				jAction.insert("next2", SaveAction(ref.next2));
			} break;
			case QiType::jump:
			{
				const QiJump& ref = std::get<QiJump>(var);
				jAction.insert("id", (int)ref.id);
			} break;
			case QiType::jumpPoint:
			{
				const QiJumpPoint& ref = std::get<QiJumpPoint>(var);
				jAction.insert("id", (int)ref.id);
			} break;
			case QiType::dialog:
			{
				const QiDialog& ref = std::get<QiDialog>(var);
				jAction.insert("style", (int)ref.style);
				jAction.insert("title", ref.title);
				jAction.insert("text", ref.text);
				jAction.insert("next", SaveAction(ref.next));
				jAction.insert("next2", SaveAction(ref.next2));
			} break;
			case QiType::block:
			{
				const QiBlock& ref = std::get<QiBlock>(var);
				jAction.insert("id", (int)ref.id);
				jAction.insert("next", SaveAction(ref.next));
			} break;
			case QiType::blockExec:
			{
				const QiBlockExec& ref = std::get<QiBlockExec>(var);
				jAction.insert("id", (int)ref.id);
			} break;
			case QiType::quickInput:
			{
				const QiQuickInput& ref = std::get<QiQuickInput>(var);
				QJsonArray chars;
				for (auto& i : ref.chars) chars.append((int)i);
				jAction.insert("c", chars);
			} break;
			case QiType::keyBlock:
			{
				const QiKeyBlock& ref = std::get<QiKeyBlock>(var);
				jAction.insert("k", (int)ref.vk);
				jAction.insert("b", ref.block);
			} break;
			case QiType::clock:
			{
				const QiClock& ref = std::get<QiClock>(var);
				jAction.insert("t", ref.time);
				jAction.insert("next", SaveAction(ref.next));
				jAction.insert("next2", SaveAction(ref.next2));
			} break;
			case QiType::ocr:
			{
				const QiOcr& ref = std::get<QiOcr>(var);
				jAction.insert("left", (int)(ref.rect.left));
				jAction.insert("top", (int)(ref.rect.top));
				jAction.insert("right", (int)(ref.rect.right));
				jAction.insert("bottom", (int)(ref.rect.bottom));
				jAction.insert("text", ref.text);
				jAction.insert("var", ref.var);
				jAction.insert("match", ref.match);
				jAction.insert("next", SaveAction(ref.next));
				jAction.insert("next2", SaveAction(ref.next2));
			} break;
			case QiType::varOperator:
			{
				const QiVarOperator& ref = std::get<QiVarOperator>(var);
				jAction.insert("t", ref.script);
			} break;
			case QiType::varCondition:
			{
				const QiVarCondition& ref = std::get<QiVarCondition>(var);
				jAction.insert("t", ref.script);
				jAction.insert("next", SaveAction(ref.next));
				jAction.insert("next2", SaveAction(ref.next2));
			} break;
			case QiType::mouseTrack:
			{
				const QiMouseTrack& ref = std::get<QiMouseTrack>(var);
				jAction.insert("size", (int)ref.s.size());
				jAction.insert("data", ref.toBase64());
			} break;
			case QiType::open:
			{
				const QiOpen& ref = std::get<QiOpen>(var);
				jAction.insert("url", ref.url);
			} break;
			case QiType::textPad:
			{
				const QiTextPad& ref = std::get<QiTextPad>(var);
				jAction.insert("text", ref.text);
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
		jMacro.insert("app", "QuickInput");
		jMacro.insert("type", "QuickInputMacro");
		jMacro.insert("name", macro.name);
		jMacro.insert("wndState", (bool)macro.wndState);
		jMacro.insert("wndChild", (bool)macro.wndInput.child);
		jMacro.insert("wndName", macro.wndInfo.wndName);
		jMacro.insert("wndClass", macro.wndInfo.wndClass);
		jMacro.insert("state", (bool)macro.state);
		jMacro.insert("keyBlock", (bool)macro.keyBlock);
		jMacro.insert("curBlock", (bool)macro.curBlock);
		jMacro.insert("mode", (int)macro.mode);
		jMacro.insert("key", (int)macro.key);
		jMacro.insert("count", (int)macro.count);
		jMacro.insert("speed", (double)macro.speed);
		jMacro.insert("timer", (bool)macro.timer);
		jMacro.insert("timerStart", (int)macro.timerStart);
		jMacro.insert("timerEnd", (int)macro.timerEnd);
		jMacro.insert("moveScaleX", (double)macro.moveScaleX);
		jMacro.insert("moveScaleY", (double)macro.moveScaleY);
		jMacro.insert("posScaleX", (double)macro.posScaleX);
		jMacro.insert("posScaleY", (double)macro.posScaleY);
		jMacro.insert("actions", SaveAction(macro.acRun));
		jMacro.insert("actionsEnding", SaveAction(macro.acEnd));

		QJsonDocument json(jMacro);

		QString path = Qi::macroDir;
		if (!QDir(path).exists() && !QDir(path).mkdir(path))
		{
			MsgBox::Error(L"创建宏目录失败");
			return;
		}
		if (!macro.groupBase)
		{
			path += macro.groupName + QString("/");
			if (!QDir(path).exists() && !QDir(path).mkdir(path))
			{
				MsgBox::Error(L"创建分组目录失败");
				return;
			}
		}
		path += macro.name + Qi::macroType;

		if (!File::SaveText(path, json.toJson())) MsgBox::Error((const wchar_t*)macro.name.utf16(), L"保存宏失败");
	}
	void SaveJson()
	{
		QJsonObject cfg;
		cfg.insert("document_charset", "UTF8");
		cfg.insert("app", "QuickInput");
		cfg.insert("type", "QuickInputConfig");
		cfg.insert("theme", (int)Qi::set.theme);
		cfg.insert("key", (int)Qi::set.key);
		cfg.insert("recKey", (int)Qi::set.recKey);
		cfg.insert("recTrack", (bool)Qi::set.recTrack);
		cfg.insert("defOn", (bool)Qi::set.defOn);
		cfg.insert("showTips", (bool)Qi::set.showTips);
		cfg.insert("audFx", (bool)Qi::set.audFx);
		cfg.insert("minMode", (bool)Qi::set.minMode);
		cfg.insert("tabLock", (bool)Qi::set.tabLock);
		cfg.insert("tabHideTip", (bool)Qi::set.tabHideTip);
		cfg.insert("quickClickKey", (int)Qi::fun.quickClick.key);
		cfg.insert("quickClickState", (bool)Qi::fun.quickClick.state);
		cfg.insert("quickClickDelay", (int)Qi::fun.quickClick.delay);
		cfg.insert("quickClickMode", (int)Qi::fun.quickClick.mode);
		cfg.insert("showClockKey", (int)Qi::fun.showClock.key);
		cfg.insert("showClockState", (bool)Qi::fun.showClock.state);
		cfg.insert("wndActiveState", (bool)Qi::fun.wndActive.state);
		cfg.insert("wndActiveName", Qi::fun.wndActive.wndInfo.wndName);
		cfg.insert("wndActiveClass", Qi::fun.wndActive.wndInfo.wndClass);
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
			if (type)
			{
				bool dis = jAction.value("dis").toBool();
				QString mark = jAction.value("mark").toString();
				switch (type)
				{
				case QiType::end:
				{
					QiEnd var; var.disable = dis, var.mark = mark;
					actions.append(var);
				} break;
				case QiType::delay:
				{
					QiDelay var; var.disable = dis, var.mark = mark;
					var.min = jAction.value("ms").toInt();
					var.max = jAction.value("ex").toInt();
					if (var.max < var.min) var.max = var.min;
					actions.append(var);
				} break;
				case QiType::key:
				{
					QiKey var; var.disable = dis, var.mark = mark;
					var.state = jAction.value("state").toInt();
					var.vk = jAction.value("vk").toInt();
					actions.append(var);
				} break;
				case QiType::mouse:
				{
					QiMouse var; var.disable = dis, var.mark = mark;
					var.move = jAction.value("move").toBool();
					var.x = jAction.value("x").toInt();
					var.y = jAction.value("y").toInt();
					var.ex = jAction.value("ex").toInt();
					var.track = jAction.value("trk").toBool();
					var.speed = jAction.value("spd").toInt();
					if (!var.speed) var.speed = 1;
					if (var.speed > 99) var.speed = 99;
					actions.append(var);
				} break;
				case QiType::copyText:
				{
					QiCopyText var; var.disable = dis, var.mark = mark;
					var.text = jAction.value("text").toString();
					actions.append(var);
				} break;
				case QiType::color:
				{
					QiColor var; var.disable = dis, var.mark = mark;
					var.move = jAction.value("move").toBool();
					var.rect.left = jAction.value("left").toInt();
					var.rect.top = jAction.value("top").toInt();
					var.rect.right = jAction.value("right").toInt();
					var.rect.bottom = jAction.value("bottom").toInt();
					var.rgbe.set(jAction.value("rgbe").toInt());
					var.next = LoadAction(jAction.value("next").toArray());
					var.next2 = LoadAction(jAction.value("next2").toArray());
					actions.append(var);
				} break;
				case QiType::loop:
				{
					QiLoop var; var.disable = dis, var.mark = mark;
					var.min = jAction.value("count").toInt();
					var.max = jAction.value("rand").toInt();
					if (var.max < var.min) var.max = var.min;
					var.next = LoadAction(jAction.value("next").toArray());
					actions.append(var);
				} break;
				case QiType::loopEnd:
				{
					QiLoopEnd var; var.disable = dis, var.mark = mark;

					actions.append(var);
				} break;
				case QiType::keyState:
				{
					QiKeyState var; var.disable = dis, var.mark = mark;
					var.vk = jAction.value("vk").toInt();
					var.next = LoadAction(jAction.value("next").toArray());
					var.next2 = LoadAction(jAction.value("next2").toArray());
					actions.append(var);
				} break;
				case QiType::resetPos:
				{
					QiResetPos var; var.disable = dis, var.mark = mark;
					actions.append(var);
				} break;
				case QiType::image:
				{
					QiImage var; var.disable = dis, var.mark = mark;
					var.move = jAction.value("move").toBool();
					var.rect.left = jAction.value("left").toInt();
					var.rect.top = jAction.value("top").toInt();
					var.rect.right = jAction.value("right").toInt();
					var.rect.bottom = jAction.value("bottom").toInt();
					var.sim = jAction.value("sim").toInt();
					var.fromBase64(jAction.value("data").toString(), jAction.value("width").toInt(), jAction.value("height").toInt());
					var.next = LoadAction(jAction.value("next").toArray());
					var.next2 = LoadAction(jAction.value("next2").toArray());
					actions.append(var);
				} break;
				case QiType::popText:
				{
					QiPopText var; var.disable = dis, var.mark = mark;
					var.text = jAction.value("text").toString();
					var.time = jAction.value("time").toInt();
					var.sync = jAction.value("sync").toBool();
					actions.append(var);
				} break;
				case QiType::savePos:
				{
					QiSavePos var; var.disable = dis, var.mark = mark;
					actions.append(var);
				} break;
				case QiType::timer:
				{
					QiTimer var; var.disable = dis, var.mark = mark;
					var.min = jAction.value("min").toInt();
					var.max = jAction.value("max").toInt();
					if (var.max < var.min) var.max = var.min;
					var.next = LoadAction(jAction.value("next").toArray());
					actions.append(var);
				} break;
				case QiType::jump:
				{
					QiJump var; var.disable = dis, var.mark = mark;
					var.id = jAction.value("id").toInt();
					actions.append(var);
				} break;
				case QiType::jumpPoint:
				{
					QiJumpPoint var; var.disable = dis, var.mark = mark;
					var.id = jAction.value("id").toInt();
					actions.append(var);
				} break;
				case QiType::dialog:
				{
					QiDialog var; var.disable = dis, var.mark = mark;
					var.style = jAction.value("style").toInt();
					var.title = jAction.value("title").toString();
					var.text = jAction.value("text").toString();
					var.next = LoadAction(jAction.value("next").toArray());
					var.next2 = LoadAction(jAction.value("next2").toArray());
					actions.append(var);
				} break;
				case QiType::block:
				{
					QiBlock var; var.disable = dis, var.mark = mark;
					var.id = jAction.value("id").toInt();
					var.next = LoadAction(jAction.value("next").toArray());
					actions.append(var);
				} break;
				case QiType::blockExec:
				{
					QiBlockExec var; var.disable = dis, var.mark = mark;
					var.id = jAction.value("id").toInt();
					actions.append(var);
				} break;
				case QiType::quickInput:
				{
					QiQuickInput var; var.disable = dis, var.mark = mark;
					QJsonArray chars = jAction.value("c").toArray();
					for (auto c : chars) var.chars.append(c.toInt());
					actions.append(var);
				} break;
				case QiType::keyBlock:
				{
					QiKeyBlock var; var.disable = dis, var.mark = mark;
					var.vk = jAction.value("k").toInt();
					var.block = jAction.value("b").toBool();
					actions.append(var);
				} break;
				case QiType::clock:
				{
					QiClock var; var.disable = dis, var.mark = mark;
					var.time = jAction.value("t").toInt();
					var.next = LoadAction(jAction.value("next").toArray());
					var.next2 = LoadAction(jAction.value("next2").toArray());
					actions.append(var);
				} break;
				case QiType::ocr:
				{
					QiOcr var; var.disable = dis, var.mark = mark;
					var.rect.left = jAction.value("left").toInt();
					var.rect.top = jAction.value("top").toInt();
					var.rect.right = jAction.value("right").toInt();
					var.rect.bottom = jAction.value("bottom").toInt();
					var.text = jAction.value("text").toString();
					var.var = jAction.value("var").toString();
					var.match = jAction.value("match").toBool();
					var.next = LoadAction(jAction.value("next").toArray());
					var.next2 = LoadAction(jAction.value("next2").toArray());
					actions.append(var);
				} break;
				case QiType::varOperator:
				{
					QiVarOperator var; var.disable = dis, var.mark = mark;
					var.script = jAction.value("t").toString();
					actions.append(var);
				} break;
				case QiType::varCondition:
				{
					QiVarCondition var; var.disable = dis, var.mark = mark;
					var.script = jAction.value("t").toString();
					var.next = LoadAction(jAction.value("next").toArray());
					var.next2 = LoadAction(jAction.value("next2").toArray());
					actions.append(var);
				} break;
				case QiType::mouseTrack:
				{
					QiMouseTrack var; var.disable = dis, var.mark = mark;
					var.fromBase64(jAction.value("data").toString(), jAction.value("size").toInt());
					actions.append(var);
				} break;
				case QiType::open:
				{
					QiOpen var; var.disable = dis, var.mark = mark;
					var.url = jAction.value("url").toString();
					actions.append(var);
				} break;
				case QiType::textPad:
				{
					QiTextPad var; var.disable = dis, var.mark = mark;
					var.text = jAction.value("text").toString();
					actions.append(var);
				} break;
				default: actions.append(QiBase(type)); break;
				}
			}
		}
		return actions;
	}
	bool LoadMacro(Macro& macro, const QString path, const QString name)
	{
		QByteArray text;
		if (File::LoadText(path, text))
		{
			QJsonDocument json(QJsonDocument::fromJson(text));
			if (json.isObject())
			{
				macro.name = name;
				QJsonObject jMacro(json.object());
				macro.wndState = jMacro.value("wndState").toBool();
				macro.wndInfo.child = jMacro.value("wndChild").toBool();
				macro.wndInfo.wndName = jMacro.value("wndName").toString();
				macro.wndInfo.wndClass = jMacro.value("wndClass").toString();
				macro.state = jMacro.value("state").toBool();
				macro.keyBlock = jMacro.value("keyBlock").toBool();
				macro.curBlock = jMacro.value("curBlock").toBool();
				macro.timer = jMacro.value("timer").toBool();

				macro.key = jMacro.value("key").toInt();
				macro.mode = QiRange::Restricted(jMacro.value("mode").toInt(), QiRange::macro_mode_max);
				macro.count = QiRange::Restricted(jMacro.value("count").toInt(), QiRange::macro_count_max);

				macro.timerStart = QiRange::Restricted((time_t)jMacro.value("timerStart").toInt(), QiRange::macro_timer_max, QiRange::macro_timer_min);
				macro.timerEnd = QiRange::Restricted((time_t)jMacro.value("timerEnd").toInt(), QiRange::macro_timer_max, QiRange::macro_timer_min);

				macro.speed = jMacro.value("speed").toDouble();
				if (macro.speed == 0) macro.speed = 1.0f;
				else macro.speed = QiRange::Restricted(macro.speed, QiRange::macro_speed_max, QiRange::macro_speed_min);

				macro.moveScaleX = jMacro.value("moveScaleX").toDouble();
				if (macro.moveScaleX == 0) macro.moveScaleX = 1.0f;
				else macro.moveScaleX = QiRange::Restricted(macro.moveScaleX, QiRange::macro_moveScale_max, QiRange::macro_moveScale_min);

				macro.moveScaleY = jMacro.value("moveScaleY").toDouble();
				if (macro.moveScaleY == 0) macro.moveScaleY = 1.0f;
				else macro.moveScaleY = QiRange::Restricted(macro.moveScaleY, QiRange::macro_moveScale_max, QiRange::macro_moveScale_min);

				macro.posScaleX = QiRange::Restricted(jMacro.value("posScaleX").toDouble(), QiRange::macro_posScale_max, QiRange::macro_posScale_min);
				macro.posScaleY = QiRange::Restricted(jMacro.value("posScaleY").toDouble(), QiRange::macro_posScale_max, QiRange::macro_posScale_min);

				macro.acRun = LoadAction(jMacro.value("actions").toArray());
				macro.acEnd = LoadAction(jMacro.value("actionsEnding").toArray());
				return true;
			}
		}
		MsgBox::Error((const wchar_t*)name.utf16(), L"加载宏失败");
		return false;
	}
	void LoadMacro()
	{
		Qi::macroGroups.clear();
		Qi::macroGroups.append(MacroGroup(true, "默认分组"));

		QDir dir(Qi::macroDir);
		dir.setFilter(QDir::Filter::Dirs);
		QFileInfoList dirInfos = dir.entryInfoList();
		if (dirInfos.isEmpty()) return;

		QList<QString> dirs;
		dirs.append(Qi::macroDir);
		for (auto& i : dirInfos) if (i.fileName() != "." && i.fileName() != "..") dirs.append(i.fileName());

		bool base = true;
		for (auto& i : dirs)
		{
			QString path = Qi::macroDir;
			MacroGroup* group;

			if (base)
			{
				group = &Qi::macroGroups.front();
				base = false;
			}
			else
			{
				group = &Qi::macroGroups.append(MacroGroup(false, i));
				group->name = i;
				path += i;
			}
			for (const QFileInfo& file : File::Find(path, QString("*") + Qi::macroType))
			{
				Macro macro;
				macro.groupName = group->name;
				macro.groupBase = group->base;
				if (LoadMacro(macro, file.filePath(), file.baseName())) group->macros.append(std::move(macro));
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
				Qi::set.tabLock = cfg.value("tabLock").toBool();
				Qi::set.tabHideTip = cfg.value("tabHideTip").toBool();
				Qi::fun.quickClick.state = cfg.value("quickClickState").toBool();
				Qi::fun.quickClick.key = cfg.value("quickClickKey").toInt();
				Qi::fun.quickClick.delay = cfg.value("quickClickDelay").toInt();
				Qi::fun.quickClick.mode = cfg.value("quickClickMode").toInt();
				Qi::fun.showClock.state = cfg.value("showClockState").toBool();
				Qi::fun.showClock.key = cfg.value("showClockKey").toInt();
				Qi::fun.wndActive.state = cfg.value("wndActiveState").toBool();
				Qi::fun.wndActive.wndInfo.wndName = cfg.value("wndActiveName").toString();
				Qi::fun.wndActive.wndInfo.wndClass = cfg.value("wndActiveClass").toString();
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