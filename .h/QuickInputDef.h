#pragma once
#pragma execution_character_set("utf-8")
#include <QString>
#include <QFont>
#include "../header/cg.h"
#include "../header/CJsonObject.h"
#include "TipsWindow.h"

/*
	ItemParam {
		endLoop, end {
			null
		}

		delay {
			b = count
			c = random
		}

		down, up, click {
			a = key
		}

		pos, move {
			a = random
			b = y
			c = y
		}

		text {
			text = text
		}

		color {
			a = RGB + Ex
			b = left + top
			c = right + bottom
			d = isFind
		}

		loop {
			b = count
		}
	}
*/

struct UI {
	static std::wstring qiOn;
	static std::wstring qiOff;
	static QString qiWait;
	static QString qiDown;
	static QString qiUp;
	static QString qiClick;
	static QString qiPos;
	static QString qiMove;
	static QString qiLoop;
	static QString qiText;
	static QString qiColor;
	static QString qiEnd;
	static QString qiEndLoop;
	static QString trOn;
	static QString trOff;
	static QString etChange;
	static QString etAdd;
	static QString etDel;
	static QString etEdit;
	static QString etFunc;
	static QString etParam;
	static QString rcStart;
	static QString rcStop;
	static QString rcClose;
};

struct Item
{
	enum {
		endLoop = -2,
		end = -1,
		delay = 0,
		down = 1,
		up = 2,
		click = 3,
		pos = 4,
		move = 5,
		text = 6,
		color = 7,
		loop = 8
	};
	int8 type = 0;
	int32 a = 0;
	int32 b = 0;
	int32 c = 0;
	int32 d = 0;
	std::wstring s;
	List<Item> next;
};

struct Script
{
	enum {
		sw,
		down,
		up
	};
	bool state = 0;
	bool block = 0;
	bool k1 = 0, k2 = 0, active = 0;
	uint8 mode = 0;
	uint32 key = 0;
	uint32 a = 0;
	std::wstring name;
	List<Item> items;
	HANDLE thread = 0;
};

struct QuickClick
{
	bool state = 0;
	uint8 mode = 0;
	uint32 key = 0;
	uint32 delay = 10;
	HANDLE thread = 0;
};

struct ShowClock
{
	bool state = 0;
	uint32 key = 0;
};

struct WndActive
{
	bool active = 0;
	bool state = 0;
	HWND wnd = 0;
	HANDLE thread = 0;
	std::wstring name;
};

struct SettingsData
{
	uint32 key = 0;
	uint32 recKey = 0;
	bool k1 = 0, k2 = 0;
	bool showTips = 0;
	bool audFx = 0;
	bool minMode = 0;
	bool wndZoom = 0;
};

struct FuncData
{
	QuickClick quickClick;
	ShowClock showClock;
	WndActive wndActive;
};

struct QuickInputStruct
{
public:
	List<Script> scripts;
	FuncData fun;
	SettingsData set;

	HDC hdc = 0;
	SIZE screen = { 0 };
	bool state = 0;
	LPVOID rec = 0;

private:
	void (*SetHookState)(bool) = 0;

public:

	void Ptrs(void (*SetHookState)(bool))
	{
		hdc = GetDC(0);
		this->SetHookState = SetHookState;
	}

	void HookState(bool state)
	{
		SetHookState(state);
	}
};

static POINT RelToAbs(POINT rel)
{
	SIZE scr = System::screenVSize();
	return { (long)(((double)rel.x / ((double)scr.cx - 1.0)) * 10000.0), (long)(((double)rel.y / ((double)scr.cy - 1.0)) * 10000.0) };
}

static POINT AbsToRel(POINT abs)
{
	SIZE scr = System::screenVSize();
	return { (long)((double)scr.cx / 10000.0 * (double)abs.x), (long)((double)scr.cy / 10000.0 * (double)abs.y) };
}

static void SaveItem(neb::CJsonObject& jItems, List<Item>& items)
{
	for (uint32 u = 0; u < items.len(); u++)
	{
		neb::CJsonObject jItem;
		uint32 u32cache = 0;
		std::string scache;
		u32cache = (uint32)items[u].type; jItem.Add(u32cache);
		jItem.Add(items[u].a);
		jItem.Add(items[u].b);
		jItem.Add(items[u].c);
		jItem.Add(items[u].d);
		scache = String::toString(items[u].s); jItem.Add(scache);

		if (items[u].type == 7 || items[u].type == 8)
		{
			neb::CJsonObject jNext;
			SaveItem(jNext, items[u].next);
			jItem.Add(jNext);
		}
		jItems.Add(jItem);
	}
}

static void SaveScript(Script& script)
{
	neb::CJsonObject json;
	neb::CJsonObject jItems;
	uint32 u32cache = 0;
	SaveItem(jItems, script.items);
	json.Add("state", script.state);
	json.Add("block", script.block);
	u32cache = script.mode; json.Add("mode", u32cache);
	json.Add("key", script.key);
	json.Add("a", script.a);
	json.Add("items", jItems);

	if (!File::FolderState(L"json"))
	{
		File::FolderCreate(L"json");
	}

	std::wstring path = L"json\\";
	path += script.name;
	path += L".json";
	File::TextSave(path, String::String::toWString(json.ToString()), "chinese");
}

static void SaveJson(QuickInputStruct* qis)
{
	neb::CJsonObject cfg;
	uint32 u32cache = 0;
	std::string scache;
	cfg.Add("key", qis->set.key);
	cfg.Add("recKey", qis->set.recKey);
	cfg.Add("showTips", qis->set.showTips);
	cfg.Add("audFx", qis->set.audFx);
	cfg.Add("minMode", qis->set.minMode);
	cfg.Add("wndZoom", qis->set.wndZoom);
	cfg.Add("quickClickKey", qis->fun.quickClick.key);
	cfg.Add("quickClickState", qis->fun.quickClick.state);
	cfg.Add("quickClickDelay", qis->fun.quickClick.delay);
	u32cache = (uint32)qis->fun.quickClick.mode; cfg.Add("quickClickMode", u32cache);
	cfg.Add("showClockKey", qis->fun.showClock.key);
	cfg.Add("showClockState", qis->fun.showClock.state);
	cfg.Add("wndActiveState", qis->fun.wndActive.state);
	scache = String::toString(qis->fun.wndActive.name); cfg.Add("wndActiveName", scache);

	File::TextSave(L"QuickInput.json", String::String::toWString(cfg.ToString()), "chinese");
}

static void LoadItem(const neb::CJsonObject jItems, List<Item>& items)
{
	for (uint32 u = 0; u < jItems.GetArraySize(); u++)
	{
		Item item;
		neb::CJsonObject jItem;
		jItems.Get(u, jItem);
		uint32 u32cache;
		std::string scache;
		jItem.Get(0, u32cache); item.type = u32cache;
		jItem.Get(1, item.a);
		jItem.Get(2, item.b);
		jItem.Get(3, item.c);
		jItem.Get(4, item.d);
		jItem.Get(5, scache); item.s = String::toWString(scache);

		if (item.type == 7 || item.type == 8)
		{
			neb::CJsonObject jNext;
			jItem.Get(6, jNext);
			LoadItem(jNext, item.next);
		}
		items.Add(item);
	}
}

static void LoadJson(QuickInputStruct* qis)
{
	qis->scripts.Emp();
	neb::CJsonObject cfg(String::toString(File::TextLoad(L"QuickInput.json")));
	uint32 u32cache = 0;
	std::string scache;
	cfg.Get("key", qis->set.key);
	cfg.Get("recKey", qis->set.recKey);
	cfg.Get("showTips", qis->set.showTips);
	cfg.Get("audFx", qis->set.audFx);
	cfg.Get("minMode", qis->set.minMode);
	cfg.Get("wndZoom", qis->set.wndZoom);
	cfg.Get("quickClickKey", qis->fun.quickClick.key);
	cfg.Get("quickClickState", qis->fun.quickClick.state);
	cfg.Get("quickClickDelay", qis->fun.quickClick.delay);
	cfg.Get("quickClickMode", u32cache); qis->fun.quickClick.mode = (uint8)u32cache;
	cfg.Get("showClockKey", qis->fun.showClock.key);
	cfg.Get("showClockState", qis->fun.showClock.state);
	cfg.Get("wndActiveState", qis->fun.wndActive.state);
	cfg.Get("wndActiveName", scache); qis->fun.wndActive.name = String::toWString(scache);

	File::FindFileStruct files = File::FindFile(L"json\\*.json");

	for (uint32 u = 0; u < files.len(); u++) {
		qis->scripts.Add();
		Script& script = qis->scripts[qis->scripts.len() - 1];
		script.name = ((std::wstring)files[u].name).substr(0, wcslen(files[u].name) - 5);

		neb::CJsonObject jScript(String::toString(File::TextLoad(((std::wstring)L"json\\" + files[u].name), "chinese")));

		jScript.Get("state", script.state);
		jScript.Get("block", script.block);
		jScript.Get("mode", u32cache); script.mode = u32cache;
		jScript.Get("key", script.key);
		jScript.Get("a", script.a);

		neb::CJsonObject jItems;
		jScript.Get("items", jItems);

		LoadItem(jItems, script.items);
	}
}

static std::wstring NameFilter(QuickInputStruct* qis, std::wstring name)
{
	for (uint32 n = 0;; n++)
	{
		for (uint32 nx = 0; nx < qis->scripts.len(); nx++)
		{
			std::wstring find = name + L" " + String::toWString(n + 1);
			if (qis->scripts[nx].name == find)
			{
				break;
			}
			if (nx >= qis->scripts.len() - 1)
			{
				return find;
			}
		}
	}
	return L"";
}