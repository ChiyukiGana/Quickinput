#pragma once
#pragma execution_character_set("utf-8")
#include <QString>
#include "X:/#Lib/C++/cg.h"
#include "CJsonObject.h"
#include "TipsWindow.h"

struct UI {
	static QFont font1;
	static QFont font2;
	static QFont font3;
	static QFont font4;
	static std::wstring qiOn;
	static std::wstring qiOff;
	static QString qiWait;
	static QString qiDown;
	static QString qiUp;
	static QString qiClick;
	static QString qiMove;
	static QString qiMoveTo;
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
	int type = 0; //-2: endLoop, -1: end 0: delay, 1: down, 2: up, 3: click, 4: moveTo, 5: move, 6: text, 7: color
	int a = 0;
	int b = 0;
	int c = 0;
	std::wstring text;
	List<Item> next;
};

struct Script
{
	List<Item> items;
	std::wstring name;
	int state = 0;
	int key = 0;
	bool k1 = 0, k2 = 0;
	int mode = 0; //0: Loop, 1: Switch, 2: Once
	HANDLE thread = 0;
};

struct QuickClick
{
	UINT key = 0;
	UINT state = 0;
	UINT delay = 10;
	UINT mode = 0;
	HANDLE thread = 0;
};

struct ShowClock
{
	UINT key = 0;
	UINT state = 0;
};

struct WndActive
{
	bool active = 0;
	UINT state = 0;
	HWND wnd = 0;
	HANDLE thread = 0;
	std::wstring name;
};

struct SettingsData
{
	UINT key = 0;
	bool k1 = 0, k2 = 0;
	UINT minMode = 0;
	UINT showTips = 0;
	UINT wndZoom = 0;
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

static void SaveItem(neb::CJsonObject& jItems, List<Item> items)
{
	for (uint32 u = 0; u < items.len(); u++)
	{
		neb::CJsonObject jItem;
		jItem.Add(items[u].type);
		jItem.Add(items[u].a);
		jItem.Add(items[u].b);
		jItem.Add(items[u].c);
		jItem.Add(String::toString(items[u].text));

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
	SaveItem(jItems, script.items);
	json.Add("state", script.state);
	json.Add("mode", script.mode);
	json.Add("key", script.key);
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
	cfg.Add("key", qis->set.key);
	cfg.Add("minMode", qis->set.minMode);
	cfg.Add("showTips", qis->set.showTips);
	cfg.Add("wndZoom", qis->set.wndZoom);
	cfg.Add("quickClickKey", qis->fun.quickClick.key);
	cfg.Add("quickClickState", qis->fun.quickClick.state);
	cfg.Add("quickClickDelay", qis->fun.quickClick.delay);
	cfg.Add("quickClickMode", qis->fun.quickClick.mode);
	cfg.Add("showClockKey", qis->fun.showClock.key);
	cfg.Add("showClockState", qis->fun.showClock.state);
	cfg.Add("wndActiveState", qis->fun.wndActive.state);
	cfg.Add("wndActiveName", String::toString(qis->fun.wndActive.name));

	File::TextSave(L"QuickInput.json", String::String::toWString(cfg.ToString()), "chinese");
}

static void LoadItem(const neb::CJsonObject jItems, List<Item>& items)
{
	for (uint32 u = 0; u < jItems.GetArraySize(); u++)
	{
		Item item;
		neb::CJsonObject jItem;
		jItems.Get(u, jItem);
		jItem.Get(0, item.type);
		jItem.Get(1, item.a);
		jItem.Get(2, item.b);
		jItem.Get(3, item.c);
		std::string text;
		jItem.Get(4, text);
		item.text = String::toWString(text);

		if (item.type == 7 || item.type == 8)
		{
			neb::CJsonObject jNext;
			jItem.Get(5, jNext);
			LoadItem(jNext, item.next);
		}
		items.Add(item);
	}
}

static void LoadJson(QuickInputStruct* qis)
{
	qis->scripts.Emp();
	neb::CJsonObject cfg(String::toString(File::TextLoad(L"QuickInput.json")));
	cfg.Get("key", qis->set.key);
	cfg.Get("minMode", qis->set.minMode);
	cfg.Get("showTips", qis->set.showTips);
	cfg.Get("wndZoom", qis->set.wndZoom);
	cfg.Get("quickClickKey", qis->fun.quickClick.key);
	cfg.Get("quickClickState", qis->fun.quickClick.state);
	cfg.Get("quickClickDelay", qis->fun.quickClick.delay);
	cfg.Get("quickClickMode", qis->fun.quickClick.mode);
	cfg.Get("showClockKey", qis->fun.showClock.key);
	cfg.Get("showClockState", qis->fun.showClock.state);
	cfg.Get("wndActiveState", qis->fun.wndActive.state);

	std::string wndName;
	cfg.Get("wndActiveName", wndName);
	qis->fun.wndActive.name = String::toWString(wndName);

	File::FindFileStruct files = File::FindFile(L"json\\*.json");

	for (UINT u = 0; u < files.len(); u++) {
		qis->scripts.Add();
		Script& script = qis->scripts[qis->scripts.len() - 1];
		script.name = ((std::wstring)files[u].name).substr(0, wcslen(files[u].name) - 5);

		neb::CJsonObject jScript(String::toString(File::TextLoad(((std::wstring)L"json\\" + files[u].name), "chinese")));

		jScript.Get("state", script.state);
		jScript.Get("mode", script.mode);
		jScript.Get("key", script.key);

		neb::CJsonObject jItems;
		jScript.Get("items", jItems);

		LoadItem(jItems, script.items);
	}
}

static std::wstring NameFilter(QuickInputStruct* qis, std::wstring name)
{
	for (UINT n = 0;; n++)
	{
		for (UINT nx = 0; nx < qis->scripts.len(); nx++)
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