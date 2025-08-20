#include "type.h"

void Action::fromJson(const QJsonObject& json)
{
	int type = json.value("type").toInt();
	switch (type)
	{
	case QiType::none:
	{
		QiBase a;
		a.fromJson(json);
		emplace<QiBase>(a);
	}
	break;
	case QiType::end:
	{
		QiEnd a;
		a.fromJson(json);
		emplace<QiEnd>(a);
	}
	break;
	case QiType::delay:
	{
		QiDelay a;
		a.fromJson(json);
		emplace<QiDelay>(a);
	}
	break;
	case QiType::key:
	{
		QiKey a;
		a.fromJson(json);
		emplace<QiKey>(a);
	}
	break;
	case QiType::mouse:
	{
		QiMouse a;
		a.fromJson(json);
		emplace<QiMouse>(a);
	}
	break;
	case QiType::copyText:
	{
		QiCopyText a;
		a.fromJson(json);
		emplace<QiCopyText>(a);
	}
	break;
	case QiType::color:
	{
		QiColor a;
		a.fromJson(json);
		emplace<QiColor>(a);
	}
	break;
	case QiType::loop:
	{
		QiLoop a;
		a.fromJson(json);
		emplace<QiLoop>(a);
	}
	break;
	case QiType::loopEnd:
	{
		QiLoopEnd a;
		a.fromJson(json);
		emplace<QiLoopEnd>(a);
	}
	break;
	case QiType::keyState:
	{
		QiKeyState a;
		a.fromJson(json);
		emplace<QiKeyState>(a);
	}
	break;
	case QiType::resetPos:
	{
		QiResetPos a;
		a.fromJson(json);
		emplace<QiResetPos>(a);
	}
	break;
	case QiType::image:
	{
		QiImage a;
		a.fromJson(json);
		emplace<QiImage>(a);
	}
	break;
	case QiType::popText:
	{
		QiPopText a;
		a.fromJson(json);
		emplace<QiPopText>(a);
	}
	break;
	case QiType::savePos:
	{
		QiSavePos a;
		a.fromJson(json);
		emplace<QiSavePos>(a);
	}
	break;
	case QiType::timer:
	{
		QiTimer a;
		a.fromJson(json);
		emplace<QiTimer>(a);
	}
	break;
	case QiType::jump:
	{
		QiJump a;
		a.fromJson(json);
		emplace<QiJump>(a);
	}
	break;
	case QiType::jumpPoint:
	{
		QiJumpPoint a;
		a.fromJson(json);
		emplace<QiJumpPoint>(a);
	}
	break;
	case QiType::dialog:
	{
		QiDialog a;
		a.fromJson(json);
		emplace<QiDialog>(a);
	}
	break;
	case QiType::block:
	{
		QiBlock a;
		a.fromJson(json);
		emplace<QiBlock>(a);
	}
	break;
	case QiType::blockExec:
	{
		QiBlockExec a;
		a.fromJson(json);
		emplace<QiBlockExec>(a);
	}
	break;
	case QiType::quickInput:
	{
		QiQuickInput a;
		a.fromJson(json);
		emplace<QiQuickInput>(a);
	}
	break;
	case QiType::keyBlock:
	{
		QiKeyBlock a;
		a.fromJson(json);
		emplace<QiKeyBlock>(a);
	}
	break;
	case QiType::clock:
	{
		QiClock a;
		a.fromJson(json);
		emplace<QiClock>(a);
	}
	break;
	case QiType::ocr:
	{
		QiOcr a;
		a.fromJson(json);
		emplace<QiOcr>(a);
	}
	break;
	case QiType::varOperator:
	{
		QiVarOperator a;
		a.fromJson(json);
		emplace<QiVarOperator>(a);
	}
	break;
	case QiType::varCondition:
	{
		QiVarCondition a;
		a.fromJson(json);
		emplace<QiVarCondition>(a);
	}
	break;
	case QiType::mouseTrack:
	{
		QiMouseTrack a;
		a.fromJson(json);
		emplace<QiMouseTrack>(a);
	}
	break;
	case QiType::open:
	{
		QiOpen a;
		a.fromJson(json);
		emplace<QiOpen>(a);
	}
	break;
	case QiType::textPad:
	{
		QiTextPad a;
		a.fromJson(json);
		emplace<QiTextPad>(a);
	}
	break;
	case QiType::editDialog:
	{
		QiEditDialog a;
		a.fromJson(json);
		emplace<QiEditDialog>(a);
	}
	break;
	case QiType::volume:
	{
		QiVolume a;
		a.fromJson(json);
		emplace<QiVolume>(a);
	}
	break;
	case QiType::soundPlay:
	{
		QiSoundPlay a;
		a.fromJson(json);
		emplace<QiSoundPlay>(a);
	}
	break;
	default:
	{
		QiBase a;
		a.fromJson(json);
		emplace<QiBase>(a);
	}
	}
}

QJsonArray Actions::toJson() const
{
	QJsonArray array;
	for (const auto& i : *this) array.append(i.base().toJson());
	return array;
}
void Actions::fromJson(const QJsonArray& json)
{
	clear();
	for (const auto& i : json)
	{
		Action a;
		a.fromJson(i.toObject());
		append(std::move(a));
	}
}

QJsonObject Macro::toJson() const
{
	QJsonObject json;
	json.insert("document_charset", "UTF8");
	json.insert("app", "QuickInput");
	json.insert("type", "QuickInputMacro");
	json.insert("ver", (QString)Qi::version);
	json.insert("name", (QString)name);
	json.insert("wndState", (bool)wndState);
	json.insert("wndChild", (bool)wndInput.child);
	json.insert("wndName", (QString)wndInfo.wndName);
	json.insert("wndClass", (QString)wndInfo.wndClass);
	json.insert("script", (QString)script);
	json.insert("state", (bool)state);
	json.insert("keyBlock", (bool)keyBlock);
	json.insert("curBlock", (bool)curBlock);
	json.insert("key", (int)key);
	json.insert("mode", (int)mode);
	json.insert("count", (int)count);
	json.insert("timer", (bool)timer);
	json.insert("timerStart", (int)timerStart);
	json.insert("timerEnd", (int)timerEnd);
	json.insert("speed", (double)speed);
	json.insert("moveScaleX", (double)moveScaleX);
	json.insert("moveScaleY", (double)moveScaleY);
	json.insert("posScaleX", (double)posScaleX);
	json.insert("posScaleY", (double)posScaleY);
	json.insert("actions", acRun.toJson());
	json.insert("actionsEnding", acEnd.toJson());
	return json;
}
void Macro::fromJson(const QJsonObject& json)
{
	if (name.isEmpty()) name = json.value("name").toString();

	wndState = json.value("wndState").toBool();
	wndInput.child = json.value("wndChild").toBool();
	wndInfo.wndName = json.value("wndName").toString();
	wndInfo.wndClass = json.value("wndClass").toString();
	script = json.value("script").toString();

	state = json.value("state").toBool();
	keyBlock = json.value("keyBlock").toBool();
	curBlock = json.value("curBlock").toBool();

	key = json.value("key").toInt();

	mode = QiRange::Restricted(json.value("mode").toInt(), Macro::range_mode);
	count = QiRange::Restricted(json.value("count").toInt(), Macro::range_count);

	timer = json.value("timer").toBool();

	timerStart = QiRange::Restricted((int)json.value("timerStart").toInt(), Macro::range_timer);
	timerEnd = QiRange::Restricted((int)json.value("timerEnd").toInt(), Macro::range_timer);

	speed = json.value("speed").toDouble();
	if (speed == 0) speed = 1.0f;
	else speed = QiRange::Restricted(speed, Macro::range_speed);

	moveScaleX = json.value("moveScaleX").toDouble();
	if (moveScaleX == 0) moveScaleX = 1.0f;
	else moveScaleX = QiRange::Restricted(moveScaleX, Macro::range_moveScale);

	moveScaleY = json.value("moveScaleY").toDouble();
	if (moveScaleY == 0) moveScaleY = 1.0f;
	else moveScaleY = QiRange::Restricted(moveScaleY, Macro::range_moveScale);

	posScaleX = QiRange::Restricted(json.value("posScaleX").toDouble(), Macro::range_posScale);
	posScaleY = QiRange::Restricted(json.value("posScaleY").toDouble(), Macro::range_posScale);

	acRun.fromJson(json.value("actions").toArray());
	acEnd.fromJson(json.value("actionsEnding").toArray());
}

namespace Qi
{
	QJsonObject SavePopTextInfo(const QiUi::PopTextInfo& p)
	{
		QJsonObject json;
		json.insert("c", (int)RGB(p.c.red(), p.c.green(), p.c.blue()));
		json.insert("t", p.t);
		json.insert("s", p.s);
		return json;
	}
	QiUi::PopTextInfo LoadPopTextInfo(const QJsonObject& json)
	{
		QiUi::PopTextInfo info;
		int c = json.value("c").toInt();
		info.c = QColor(GetRValue(c), GetGValue(c), GetBValue(c));
		info.t = json.value("t").toString();
		info.s = json.value("s").toString();
		return info;
	}
	QJsonObject toJson()
	{
		QJsonObject json;
		if ("settings")
		{
			json.insert("document_charset", "UTF8");
			json.insert("app", "QuickInput");
			json.insert("type", "QuickInputConfig");
			json.insert("ver", (QString)version);
			json.insert("theme", (int)set.theme);
			json.insert("ocr_thread", (int)set.ocr_thread);
			json.insert("key", (int)set.key);
			json.insert("recKey", (int)set.recKey);
			json.insert("recTrack", (bool)set.recTrack);
			json.insert("defOn", (bool)set.defOn);
			json.insert("showTips", (bool)set.showTips);
			json.insert("audFx", (bool)set.audFx);
			json.insert("minMode", (bool)set.minMode);
			json.insert("pad", (bool)set.pad);
			json.insert("tabLock", (bool)set.tabLock);
			json.insert("tabHideTip", (bool)set.tabHideTip);
			json.insert("markPoint", (bool)set.markPoint);
			json.insert("editWidth", (int)set.editSize.width());
			json.insert("editHeight", (int)set.editSize.height());
			json.insert("quickClickKey", (int)fun.quickClick.key);
			json.insert("quickClickState", (bool)fun.quickClick.state);
			json.insert("quickClickDelay", (int)fun.quickClick.delay);
			json.insert("quickClickMode", (int)fun.quickClick.mode);
			json.insert("showClockKey", (int)fun.showClock.key);
			json.insert("showClockState", (bool)fun.showClock.state);
			json.insert("wndActiveState", (bool)fun.wndActive.state);
			json.insert("wndActiveName", (QString)fun.wndActive.wndInfo.wndName);
			json.insert("wndActiveClass", (QString)fun.wndActive.wndInfo.wndClass);
		}
		if ("pop config")
		{
			QJsonObject pop;
			pop.insert("px", (int)ui.pop.p.x);
			pop.insert("py", (int)ui.pop.p.y);
			pop.insert("size", (int)ui.pop.size);
			pop.insert("time", (int)ui.pop.time);
			pop.insert("qe", SavePopTextInfo(ui.pop.qe));
			pop.insert("qd", SavePopTextInfo(ui.pop.qd));
			pop.insert("we", SavePopTextInfo(ui.pop.we));
			pop.insert("wd", SavePopTextInfo(ui.pop.wd));
			pop.insert("qce", SavePopTextInfo(ui.pop.qce));
			pop.insert("qcd", SavePopTextInfo(ui.pop.qcd));
			pop.insert("swe", SavePopTextInfo(ui.pop.swe));
			pop.insert("swd", SavePopTextInfo(ui.pop.swd));
			pop.insert("dwe", SavePopTextInfo(ui.pop.dwe));
			pop.insert("dwd", SavePopTextInfo(ui.pop.dwd));
			pop.insert("upe", SavePopTextInfo(ui.pop.upe));
			pop.insert("upd", SavePopTextInfo(ui.pop.upd));
			json.insert("popbox", pop);
		}
		if ("group fold")
		{
			QJsonArray groupFolds;
			size_t groupFoldIndex = 0;
			for (const auto& i : group.fold)
			{
				if (macroGroups.find([&i](const MacroGroup& group) { if (group.name == i.first) return true; return false; }))
				{
					QJsonObject groupFold;
					groupFold.insert("name", (QString)i.first);
					groupFold.insert("fold", (bool)i.second);
					groupFolds.insert(groupFoldIndex, groupFold);
					groupFoldIndex++;
				}
			}
			json.insert("groupFold", groupFolds);
		}
		if ("group sort")
		{
			QJsonArray groupSort;
			size_t groupSortIndex = 0;
			for (const auto& i : macroGroups)
			{
				groupSort.append((QString)i.name);
				groupSortIndex++;
			}
			json.insert("groupSort", groupSort);
		}
		return json;
	}
	void fromJson(const QJsonObject& json)
	{
		std::function<void()> DefaultConfig = [] {
			set.ocr_thread = 4;
			set.key = VK_F8;
			set.recKey = VK_F8;
			set.recTrack = true;
			set.defOn = true;
			set.showTips = true;
			set.audFx = false;
			set.minMode = false;
			set.tabLock = false;
			set.tabHideTip = false;
			fun.quickClick.state = false;
			fun.quickClick.key = VK_LBUTTON;
			fun.quickClick.delay = 200;
			fun.quickClick.mode = 0;
			fun.showClock.state = false;
			fun.showClock.key = VK_MENU;
			fun.wndActive.state = false;
			fun.wndActive.wndInfo.wndName = "";
			};
		std::function<void()> DefaultPop = [] {
			ui.pop.qe.t = "@ 启用";
			ui.pop.qd.t = "@ 禁用";
			ui.pop.we.t = "@ 窗口启用";
			ui.pop.wd.t = "@ 窗口禁用";
			ui.pop.qce.t = "连点 @";
			ui.pop.qcd.t = "停止 @";
			ui.pop.swe.t = "执行 @";
			ui.pop.swd.t = "停止 @";
			ui.pop.dwe.t = "执行$次 @";
			ui.pop.dwd.t = "停止 @";
			ui.pop.upe.t = "执行$次 @";
			ui.pop.upd.t = "停止 @";
			ui.pop.qe.s = "SND_ON";
			ui.pop.qd.s = "SND_OFF";
			ui.pop.we.s = "SND_ON";
			ui.pop.wd.s = "SND_OFF";
			ui.pop.qce.s = "SND_RUN";
			ui.pop.qcd.s = "SND_STOP";
			ui.pop.swe.s = "SND_RUN";
			ui.pop.swd.s = "SND_STOP";
			ui.pop.dwe.s = "SND_RUN";
			ui.pop.dwd.s = "SND_STOP";
			ui.pop.upe.s = "SND_RUN";
			ui.pop.upd.s = "SND_STOP";
			ui.pop.qe.c = QColor(0xC0, 0xE0, 0xFF);
			ui.pop.qd.c = QColor(0xFF, 0x50, 0x50);
			ui.pop.we.c = QColor(0xAA, 0xBB, 0xFF);
			ui.pop.wd.c = QColor(0xFF, 0xA0, 0xA0);
			ui.pop.qce.c = QColor(0x20, 0xFF, 0x40);
			ui.pop.qcd.c = QColor(0xFF, 0xFF, 0x60);
			ui.pop.swe.c = QColor(0x20, 0xFF, 0x40);
			ui.pop.swd.c = QColor(0xFF, 0xFF, 0x60);
			ui.pop.dwe.c = QColor(0x20, 0xFF, 0x40);
			ui.pop.dwd.c = QColor(0xFF, 0xFF, 0x60);
			ui.pop.upe.c = QColor(0x20, 0xFF, 0x40);
			ui.pop.upd.c = QColor(0xFF, 0xFF, 0x60);
			ui.pop.p = { 5000, 0 };
			ui.pop.size = 20;
			ui.pop.time = 1000;
			};
		if (json.isEmpty())
		{
			DefaultConfig();
			DefaultPop();
		}
		else
		{
			if ("settings")
			{
				set.theme = json.value("theme").toInt();
				set.ocr_thread = std::clamp(json.value("ocr_thread").toInt(), 0, 16);
				set.key = json.value("key").toInt();
				set.recTrack = json.value("recTrack").toBool();
				set.recKey = json.value("recKey").toInt();
				set.defOn = json.value("defOn").toBool();
				set.showTips = json.value("showTips").toBool();
				set.audFx = json.value("audFx").toBool();
				set.minMode = json.value("minMode").toBool();
				set.pad = json.value("pad").toBool();
				set.tabLock = json.value("tabLock").toBool();
				set.tabHideTip = json.value("tabHideTip").toBool();
				set.markPoint = json.value("markPoint").toBool();
				set.editSize = QSize(json.value("editWidth").toInt(), json.value("editHeight").toInt());
				fun.quickClick.state = json.value("quickClickState").toBool();
				fun.quickClick.key = json.value("quickClickKey").toInt();
				fun.quickClick.delay = json.value("quickClickDelay").toInt();
				fun.quickClick.mode = json.value("quickClickMode").toInt();
				fun.showClock.state = json.value("showClockState").toBool();
				fun.showClock.key = json.value("showClockKey").toInt();
				fun.wndActive.state = json.value("wndActiveState").toBool();
				fun.wndActive.wndInfo.wndName = json.value("wndActiveName").toString();
				fun.wndActive.wndInfo.wndClass = json.value("wndActiveClass").toString();
			}
			if ("pop config")
			{
				QJsonObject pop = json.value("popbox").toObject();
				if (pop.isEmpty())
				{
					DefaultPop();
				}
				else
				{
					ui.pop.qe = LoadPopTextInfo(pop.value("qe").toObject());
					ui.pop.qd = LoadPopTextInfo(pop.value("qd").toObject());
					ui.pop.we = LoadPopTextInfo(pop.value("we").toObject());
					ui.pop.wd = LoadPopTextInfo(pop.value("wd").toObject());
					ui.pop.qce = LoadPopTextInfo(pop.value("qce").toObject());
					ui.pop.qcd = LoadPopTextInfo(pop.value("qcd").toObject());
					ui.pop.swe = LoadPopTextInfo(pop.value("swe").toObject());
					ui.pop.swd = LoadPopTextInfo(pop.value("swd").toObject());
					ui.pop.dwe = LoadPopTextInfo(pop.value("dwe").toObject());
					ui.pop.dwd = LoadPopTextInfo(pop.value("dwd").toObject());
					ui.pop.upe = LoadPopTextInfo(pop.value("upe").toObject());
					ui.pop.upd = LoadPopTextInfo(pop.value("upd").toObject());
					ui.pop.time = pop.value("time").toInt();
					ui.pop.size = pop.value("size").toInt();
					ui.pop.p.x = pop.value("px").toInt();
					ui.pop.p.y = pop.value("py").toInt();
				}
			}
			if ("group fold")
			{
				QJsonArray groupFolds = json.value("groupFold").toArray();
				for (const auto& i : groupFolds)
				{
					QJsonObject groupFold = i.toObject();
					group.fold[groupFold.value("name").toString()] = groupFold.value("fold").toBool();
				}
			}
			if ("group sort")
			{
				Qi::group.sort.clear();
				QJsonArray groupSort = json.value("groupSort").toArray();
				for (const auto& i : groupSort)
				{
					Qi::group.sort.append(i.toString());
				}
			}
		}
	}

	bool IsActive()
	{
		if (Qi::debug) return !Qi::PeekExitMsg();
		return Qi::run && !Qi::PeekExitMsg();
	}
	void PrecSleep(clock_t ms)
	{
		clock_t begin = clock();
		if (ms > 5)
		{
			begin--;
			while ((begin + ms) > clock()) Sleep(1);
		}
		else while ((begin + ms) > clock()) Sleep(0);
	}
	bool PeekExitMsg()
	{
		return PeekMessageW(&Qi::msg, 0, Qi::msg_exit, Qi::msg_exit, PM_NOREMOVE);
	}
	bool PeekSleep(clock_t ms)
	{
		clock_t begin = clock();
		if (ms > 5)
		{
			begin--;
			while ((begin + ms) > clock())
			{
				if (PeekExitMsg()) return true;
				Sleep(1);
			}
			return false;
		}
		else while ((begin + ms) > clock()) Sleep(0);
		return PeekExitMsg();
	}
}