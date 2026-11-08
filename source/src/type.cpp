#include "type.h"
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
			json.insert("ocr_current", (QString)set.ocr_current);
			json.insert("language", (QString)set.lang);
			json.insert("saveType", (int)Qi::set.save_type);
			json.insert("key", (int)(static_cast<int>(set.key1) | (static_cast<int>(set.key2) << 16)));
			json.insert("key1", (int)set.key1);
			json.insert("key2", (int)set.key2);
			json.insert("recKey", (int)set.recKey);
#ifdef Q_DRIVER
			json.insert("driver", (bool)Qi::set.driver);
#endif
			json.insert("recTrack", (bool)set.recTrack);
			json.insert("defOn", (bool)set.defOn);
			json.insert("showTips", (bool)set.showTips);
			json.insert("audFx", (bool)set.audFx);
			json.insert("minMode", (bool)set.minMode);
			json.insert("pad", (bool)set.pad);
			json.insert("tabLock", (bool)set.tabLock);
			json.insert("tabHideTip", (bool)set.tabHideTip);
			json.insert("markPoint", (bool)set.markPoint);
			json.insert("mainWidth", (int)set.mainSize.width());
			json.insert("mainHeight", (int)set.mainSize.height());
			json.insert("editWidth", (int)set.editSize.width());
			json.insert("editHeight", (int)set.editSize.height());
			json.insert("quickClickKey", (int)fun.quickClick.key);
			json.insert("quickClickState", (bool)fun.quickClick.state);
			json.insert("quickClickDelay", (int)fun.quickClick.delay);
			json.insert("quickClickMode", (int)fun.quickClick.mode);
			json.insert("showClockKey", (int)fun.showClock.key);
			json.insert("showClockState", (bool)fun.showClock.state);
			json.insert("wndActiveState", (bool)fun.wndActive.state);
			json.insert("wndActiveName", (QString)fun.wndActive.wndInfo.name);
			json.insert("wndActiveClass", (QString)fun.wndActive.wndInfo.clas);
			json.insert("wndActiveProc", (QString)fun.wndActive.wndInfo.proc);
		}
		if ("click")
		{
			QJsonArray clicks;
			for (const auto& i : Qi::click.clickers)
			{
				QJsonObject click;
				click.insert("state", (bool)i.state);
				click.insert("key1", (int)i.key1);
				click.insert("key2", (int)i.key2);
				click.insert("intv", (int)i.interval);
				click.insert("mode", (int)i.mode);
				clicks.append(std::move(click));
			}
			json.insert("click", clicks);
		}
		if ("pop config")
		{
			QJsonObject pop;
			pop.insert("px", (int)ui.pop.x);
			pop.insert("py", (int)ui.pop.y);
			pop.insert("size", (int)ui.pop.size);
			pop.insert("time", (int)ui.pop.time);
			pop.insert("qe", SavePopTextInfo(ui.pop.qe_));
			pop.insert("qd", SavePopTextInfo(ui.pop.qd_));
			pop.insert("we", SavePopTextInfo(ui.pop.we_));
			pop.insert("wd", SavePopTextInfo(ui.pop.wd_));
			pop.insert("qce", SavePopTextInfo(ui.pop.qce_));
			pop.insert("qcd", SavePopTextInfo(ui.pop.qcd_));
			pop.insert("swe", SavePopTextInfo(ui.pop.swe_));
			pop.insert("swd", SavePopTextInfo(ui.pop.swd_));
			pop.insert("dwe", SavePopTextInfo(ui.pop.dwe_));
			pop.insert("dwd", SavePopTextInfo(ui.pop.dwd_));
			pop.insert("upe", SavePopTextInfo(ui.pop.upe_));
			pop.insert("upd", SavePopTextInfo(ui.pop.upd_));
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
		if (!json.isEmpty())
		{
			if ("settings")
			{
				set.theme = json.value("theme").toInt();
				set.ocr_thread = std::clamp(json.value("ocr_thread").toInt(), 0, 16);
				set.ocr_current = json.value("ocr_current").toString();
				set.lang = json.value("language").toString();
				set.save_type = json.value("saveType").toInt();

				if (json.contains("key1"))
				{
					set.key1 = json.value("key1").toInt();
					set.key2 = json.value("key2").toInt();
				}
				else
				{
					int key = json.value("key").toInt();
					set.key1 = key & 0xFFFF;
					set.key2 = key >> 16;
				}

				set.recKey = json.value("recKey").toInt();
#ifdef Q_DRIVER
				set.driver = json.value("driver").toBool();
#endif
				set.recTrack = json.value("recTrack").toBool();
				set.defOn = json.value("defOn").toBool();
				set.showTips = json.value("showTips").toBool();
				set.audFx = json.value("audFx").toBool();
				set.minMode = json.value("minMode").toBool();
				set.pad = json.value("pad").toBool();
				set.tabLock = json.value("tabLock").toBool();
				set.tabHideTip = json.value("tabHideTip").toBool();
				set.markPoint = json.value("markPoint").toBool();
				set.mainSize = QSize(json.value("mainWidth").toInt(), json.value("mainHeight").toInt());
				set.editSize = QSize(json.value("editWidth").toInt(), json.value("editHeight").toInt());
				fun.quickClick.state = json.value("quickClickState").toBool();
				fun.quickClick.key = json.value("quickClickKey").toInt();
				fun.quickClick.delay = json.value("quickClickDelay").toInt();
				fun.quickClick.mode = json.value("quickClickMode").toInt();
				fun.showClock.state = json.value("showClockState").toBool();
				fun.showClock.key = json.value("showClockKey").toInt();
				fun.wndActive.state = json.value("wndActiveState").toBool();
				fun.wndActive.wndInfo.name = json.value("wndActiveName").toString();
				fun.wndActive.wndInfo.clas = json.value("wndActiveClass").toString();
				fun.wndActive.wndInfo.proc = json.value("wndActiveProc").toString();
			}
			if ("click")
			{
				Qi::click.clickers.clear();
				for (const auto& i : json.value("click").toArray())
				{
					const QJsonObject click = i.toObject();
					Clicker clicker;
					clicker.state = click.value("state").toBool();
					clicker.key1 = click.value("key1").toInt();
					clicker.key2 = click.value("key2").toInt();
					clicker.interval = click.value("intv").toInt();
					clicker.mode = click.value("mode").toInt();
					Qi::click.clickers.append(std::move(clicker));
				}

			}
			if ("pop config")
			{
				const QJsonObject pop = json.value("popbox").toObject();
				if (!pop.isEmpty())
				{
					ui.pop.qe = ui.pop.qe_ = LoadPopTextInfo(pop.value("qe").toObject());
					ui.pop.qd = ui.pop.qd_ = LoadPopTextInfo(pop.value("qd").toObject());
					ui.pop.we = ui.pop.we_ = LoadPopTextInfo(pop.value("we").toObject());
					ui.pop.wd = ui.pop.wd_ = LoadPopTextInfo(pop.value("wd").toObject());
					ui.pop.qce = ui.pop.qce_ = LoadPopTextInfo(pop.value("qce").toObject());
					ui.pop.qcd = ui.pop.qcd_ = LoadPopTextInfo(pop.value("qcd").toObject());
					ui.pop.swe = ui.pop.swe_ = LoadPopTextInfo(pop.value("swe").toObject());
					ui.pop.swd = ui.pop.swd_ = LoadPopTextInfo(pop.value("swd").toObject());
					ui.pop.dwe = ui.pop.dwe_ = LoadPopTextInfo(pop.value("dwe").toObject());
					ui.pop.dwd = ui.pop.dwd_ = LoadPopTextInfo(pop.value("dwd").toObject());
					ui.pop.upe = ui.pop.upe_ = LoadPopTextInfo(pop.value("upe").toObject());
					ui.pop.upd = ui.pop.upd_ = LoadPopTextInfo(pop.value("upd").toObject());
					ui.pop.time = pop.value("time").toInt();
					ui.pop.size = pop.value("size").toInt();
					ui.pop.x = pop.value("px").toInt();
					ui.pop.y = pop.value("py").toInt();
				}
			}
			if ("group fold")
			{
				group.fold.clear();
				for (const auto& i : json.value("groupFold").toArray())
				{
					const QJsonObject groupFold = i.toObject();
					group.fold[groupFold.value("name").toString()] = groupFold.value("fold").toBool();
				}
			}
			if ("group sort")
			{
				group.sort.clear();
				for (const auto& i : json.value("groupSort").toArray())
				{
					group.sort.append(i.toString());
				}
			}
		}
	}
}