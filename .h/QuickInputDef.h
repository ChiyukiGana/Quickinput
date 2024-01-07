#pragma once
#include <QString>
#include <QFont>
#include "../header/cg.h"
#include "../header/CJsonObject.h"

struct UI {
	static float zoomRote;
	static QString syOn;
	static QString syOff;
	static QString syOk;
	static QString syYes;
	static QString syNot;
	static QString syStop;
	static QString syShow;
	static QString syHide;
	static QString syOption;
	static QString syLink;
	static QString syEdit;
	static QString syUp;
	static QString syDown;
	static QString syTurn;
	static QString syLeft;
	static QString syTop;
	static QString syRight;
	static QString syMove;
	static QString syTime;
	static QString syText;
	static QString syLoop;
	static QString syColor;

	static std::wstring qiOn;
	static std::wstring qiOff;
	static QString muOn;
	static QString muOff;
	static QString muShow;
	static QString muHide;
	static QString muExit;
	static QString acWait;
	static QString acDown;
	static QString acUp;
	static QString acClick;
	static QString acPos;
	static QString acMove;
	static QString acLoop;
	static QString acText;
	static QString acColor;
	static QString acEnd;
	static QString acEndLoop;
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

struct WndInput
{
	HWND wnd = 0;
	POINT pt = { 0 };
	WORD mk = 0;
};
struct WndInfo
{
	HWND wnd = 0;
	std::wstring wndName;
	std::wstring wndClass;
	bool Update() { return wnd = FindWindowW(wndClass.c_str(), wndName.c_str()); }
};
struct WndLock
{
	static HANDLE thread;
	static DWORD CALLBACK LockThread(PVOID wnd)
	{
		while (1) {
			RECT rect = Window::rect((HWND)wnd);
			ClipCursor(&rect);
			sleep(10);
		}
		return 0;
	}
	static void Lock(HWND wnd)
	{
		UnLock();
		thread = Thread::Start(LockThread, wnd);
	}
	static void UnLock()
	{
		if (thread)
		{
			TerminateThread(thread, 0);
			thread = 0;
		}
		ClipCursor(0);
	}
};

struct Action;
typedef List<Action> Actions;
struct QiDelay { uint32 ms; uint32 ex; };
struct QiKey { enum { up, down, click }; uint32 vk = 0; uint32 state = up; };
struct QiMouse { int32 x = 0; int32 y = 0; uint32 ex = 0; bool move = 0; };
struct QiText { wcstring str; };
struct QiColor { Rgba rgbe = 0; RECT rect = { 0 }; bool unfind = 0; bool move = 0; Actions next; };
struct QiLoop { uint32 count; Actions next; };
struct Action
{
	enum
	{
		_none,
		_end,
		_delay,
		_key,
		_mouse,
		_text,
		_color,
		_loop,
		_loopEnd
	};

	union
	{
		byte memsize[36];
		QiDelay delay;
		QiKey key;
		QiMouse mouse;
		QiText text;
		QiColor color;
		QiLoop loop;
	};

	uint32 type = _none;
	std::wstring mark;

	Action() { Emp(); }
	Action(const uint32 actionType) { Emp(); type = actionType; }
	Action(const Action& action) { Cpy(action); }
	~Action() { Emp(); }

	void operator=(const Action& action) { Cpy(action); }

	void Cpy(const Action& action)
	{
		Emp();
		mark = action.mark;
		switch (action.type)
		{
		case _end: type = _end; break;
		case _delay: type = _delay; delay = action.delay; break;
		case _key: type = _key; key = action.key; break;
		case _mouse: type = _mouse; mouse = action.mouse; break;
		case _text: type = _text; text.str.cpy(action.text.str); break;
		case _color: type = _color; color = action.color; break;
		case _loop: type = _loop; loop = action.loop; break;
		case _loopEnd: type = _loopEnd; break;
		default: type = _none; break;
		}
	}

	void Emp() {
		if (type == _text) text.str.emp();
		type = _none;
		mark = L""; memset(&memsize, 0, sizeof(memsize));
	}
};

struct Macro
{
	enum { sw, down, up };
	bool state = 0;
	bool block = 0;
	bool wndState = 0;
	bool k1 = 0, k2 = 0, active = 0;
	uint32 key = 0;
	uint32 mode = 0;
	uint32 count = 0;
	std::wstring name;
	WndInfo wi;
	HANDLE thread = 0;
	Actions actions;
};
typedef List<Macro> Macros;

struct QuickClick
{
	bool state = 0;
	uint32 mode = 0;
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
	bool state = 0;
	WndInfo wi;
	HANDLE thread = 0;
};
struct SettingsData
{
	uint32 key = 0;
	uint32 recKey = 0;
	bool shift = 0, f12 = 0;
	bool k1 = 0, k2 = 0;
	bool defOn = 0;
	bool showTips = 0;
	bool audFx = 0;
	bool minMode = 0;
	bool zoomBlock = 0;
};
struct FuncData
{
	QuickClick quickClick;
	ShowClock showClock;
	WndActive wndActive;
};

struct QuickInputStruct
{
	bool state = 0;
	bool run = 0;
	Macros macros;
	FuncData fun;
	SettingsData set;
	PVOID rec = 0;

	LPCWSTR path = L"macro\\";
	HDC hdc = GetDC(0);
	SIZE screen = System::screenSize();

	void ReScreen()
	{
		ReleaseDC(0, hdc);
		hdc = GetDC(0);
		screen = System::screenSize();
	}
};

struct Global {
	static QuickInputStruct qi;
	static List<byte> trBlock;
	static byte blockRep[255];
};

static POINT RTA(POINT rel) { return { (long)(((float)rel.x / (float)(Global::qi.screen.cx - 1)) * 10000.0f), (long)(((float)rel.y / (float)(Global::qi.screen.cy - 1)) * 10000.0f) }; }
static RECT RTAR(RECT rel) { return { (long)(((float)rel.left / (float)(Global::qi.screen.cx - 1)) * 10000.0f), (long)(((float)rel.top / (float)(Global::qi.screen.cy - 1)) * 10000.0f), (long)(((float)rel.right / (float)(Global::qi.screen.cx - 1)) * 10000.0f), (long)(((float)rel.bottom / (float)(Global::qi.screen.cy - 1)) * 10000.0f) }; }
static POINT ATR(POINT abs) { return { (long)((float)Global::qi.screen.cx / 10000.0f * (float)abs.x), (long)((float)Global::qi.screen.cy / 10000.0f * (float)abs.y) }; }
static RECT ATRR(RECT abs) { return { (long)((float)Global::qi.screen.cx / 10000.0f * (float)abs.left), (long)((float)Global::qi.screen.cy / 10000.0f * (float)abs.top), (long)((float)Global::qi.screen.cx / 10000.0f * (float)abs.right), (long)((float)Global::qi.screen.cy / 10000.0f * (float)abs.bottom) }; }
static POINT WRTA(POINT rel, HWND wnd) { SIZE size = Window::size(wnd); return { (long)(((float)rel.x / ((float)size.cx)) * 10000.0f), (long)(((float)rel.y / ((float)size.cy)) * 10000.0f) }; }
static RECT WRTAR(RECT rel, HWND wnd) { SIZE size = Window::size(wnd); return { (long)(((float)rel.left / ((float)size.cx)) * 10000.0f), (long)(((float)rel.top / ((float)size.cy)) * 10000.0f), (long)(((float)rel.right / ((float)size.cx)) * 10000.0f), (long)(((float)rel.bottom / ((float)size.cy)) * 10000.0f) }; }
static POINT WATR(POINT abs, HWND wnd) { SIZE size = Window::size(wnd); return { (long)((float)(size.cx + 1) / 10000.0f * (float)abs.x), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.y) }; }
static RECT WATRR(RECT abs, HWND wnd) { SIZE size = Window::size(wnd); return { (long)((float)(size.cx + 1) / 10000.0f * (float)abs.left), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.top), (long)((float)(size.cx + 1) / 10000.0f * (float)abs.right), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.bottom) }; }

static void _stdcall SaveAction(neb::CJsonObject& jActions, const Actions& actions)
{
	for (uint32 u = 0; u < actions.size(); u++)
	{
		neb::CJsonObject jItem;
		neb::CJsonObject jNext;

		std::string str = String::toString(actions[u].mark);
		jItem.Add("mark", str);

		switch (actions[u].type)
		{
		case Action::_end:
		{
			jItem.Add("type", Action::_end);
		}
		break;
		case Action::_delay:
		{
			jItem.Add("type", Action::_delay);
			jItem.Add("ms", actions[u].delay.ms);
			jItem.Add("ex", actions[u].delay.ex);
		}
		break;
		case Action::_key:
		{
			jItem.Add("type", Action::_key);
			jItem.Add("state", actions[u].key.state);
			jItem.Add("vk", actions[u].key.vk);
		}
		break;
		case Action::_mouse:
		{
			jItem.Add("type", Action::_mouse);
			jItem.Add("move", actions[u].mouse.move);
			jItem.Add("x", actions[u].mouse.x);
			jItem.Add("y", actions[u].mouse.y);
			jItem.Add("ex", actions[u].mouse.ex);
		}
		break;
		case Action::_text:
		{
			jItem.Add("type", Action::_text);
			jItem.Add("text", String::toString(actions[u].text.str.str));
		}
		break;
		case Action::_color:
		{
			jItem.Add("type", Action::_color);
			jItem.Add("unfind", actions[u].color.unfind);
			jItem.Add("move", actions[u].color.move);
			jItem.Add("left", (int32)actions[u].color.rect.left);
			jItem.Add("top", (int32)actions[u].color.rect.top);
			jItem.Add("right", (int32)actions[u].color.rect.right);
			jItem.Add("bottom", (int32)actions[u].color.rect.bottom);
			jItem.Add("rgbe", (uint32)actions[u].color.rgbe.toCOLORREF());
			SaveAction(jNext, actions[u].color.next);
			jItem.Add("next", jNext);
		}
		break;
		case Action::_loop:
		{
			jItem.Add("type", Action::_loop);
			jItem.Add("count", actions[u].loop.count);
			SaveAction(jNext, actions[u].loop.next);
			jItem.Add("next", jNext);
		}
		break;
		case Action::_loopEnd:
			jItem.Add("type", Action::_loopEnd);
			break;
		}
		jActions.Add(jItem);
	}
}
static void SaveMacro(Macro& macro)
{
	neb::CJsonObject json;
	neb::CJsonObject jActions;
	SaveAction(jActions, macro.actions);
	json.Add("documen_ charset", std::string("UTF8"));
	json.Add("wndState", macro.wndState);
	json.Add("wndName", String::toString(macro.wi.wndName));
	json.Add("wndClass", String::toString(macro.wi.wndClass));
	json.Add("state", macro.state);
	json.Add("block", macro.block);
	json.Add("mode", macro.mode);
	json.Add("key", macro.key);
	json.Add("count", macro.count);
	json.Add("actions", jActions);
	File::FolderCreate(L"macro");
	File::TextSave(String::toString(std::wstring(L"macro\\") + macro.name + std::wstring(L".json")), json.ToString());
}
static void SaveJson()
{
	neb::CJsonObject cfg;
	std::string str;
	cfg.Add("document_charset", std::string("UTF8"));
	cfg.Add("defOn", Global::qi.set.defOn);
	cfg.Add("key", Global::qi.set.key);
	cfg.Add("recKey", Global::qi.set.recKey);
	cfg.Add("showTips", Global::qi.set.showTips);
	cfg.Add("audFx", Global::qi.set.audFx);
	cfg.Add("minMode", Global::qi.set.minMode);
	cfg.Add("zoomBlock", Global::qi.set.zoomBlock);
	cfg.Add("quickClickKey", Global::qi.fun.quickClick.key);
	cfg.Add("quickClickState", Global::qi.fun.quickClick.state);
	cfg.Add("quickClickDelay", Global::qi.fun.quickClick.delay);
	cfg.Add("quickClickMode", Global::qi.fun.quickClick.mode);
	cfg.Add("showClockKey", Global::qi.fun.showClock.key);
	cfg.Add("showClockState", Global::qi.fun.showClock.state);
	cfg.Add("wndActiveState", Global::qi.fun.wndActive.state);
	cfg.Add("wndActiveName", String::toString(Global::qi.fun.wndActive.wi.wndName));
	File::TextSave("QuickInput.json", cfg.ToString());
}

static void _stdcall LoadAction(const neb::CJsonObject jActions, Actions& actions)
{
	for (uint32 u = 0; u < jActions.GetArraySize(); u++)
	{
		Action action;
		neb::CJsonObject jItem;
		neb::CJsonObject jNext;
		jActions.Get(u, jItem);

		uint32 ui32 = 0;
		jItem.Get("type", ui32);
		if (ui32)
		{
			switch (ui32)
			{
			case Action::_end: action.type = Action::_end; break;
			case Action::_delay:
			{
				action.type = Action::_delay;
				jItem.Get("ms", action.delay.ms);
				jItem.Get("ex", action.delay.ex);
			}
			break;
			case Action::_key:
			{
				action.type = Action::_key;
				jItem.Get("state", action.key.state);
				jItem.Get("vk", action.key.vk);
			}
			break;
			case Action::_mouse:
			{
				action.type = Action::_mouse;
				jItem.Get("move", action.mouse.move);
				jItem.Get("x", action.mouse.x);
				jItem.Get("y", action.mouse.y);
				jItem.Get("ex", action.mouse.ex);
			}
			break;
			case Action::_text:
			{
				std::string str;
				jItem.Get("text", str);
				action.type = Action::_text;
				action.text.str = String::toWString(str).c_str();
			}
			break;
			case Action::_color:
			{
				action.type = Action::_color;
				jItem.Get("unfind", action.color.unfind);
				jItem.Get("move", action.color.move);
				jItem.Get("left", (int32&)(action.color.rect.left));
				jItem.Get("top", (int32&)(action.color.rect.top));
				jItem.Get("right", (int32&)(action.color.rect.right));
				jItem.Get("bottom", (int32&)(action.color.rect.bottom));
				jItem.Get("rgbe", ui32); action.color.rgbe.set(ui32);
				jItem.Get("next", jNext);
				LoadAction(jNext, action.color.next);
			}
			break;
			case Action::_loop:
			{
				action.type = Action::_loop;
				jItem.Get("count", action.loop.count);
				jItem.Get("next", jNext);
				LoadAction(jNext, action.loop.next);
			}
			break;
			case Action::_loopEnd: action.type = Action::_loopEnd; break;
			default: action.type = Action::_none; break;
			}

			std::string str;
			jItem.Get("mark", str);
			action.mark = String::toWString(str);
			actions.Add(action);
		}
	}
}
static void LoadMacro()
{
	File::FindFileStruct files = File::FindFile(L"macro\\*.json");
	for (uint32 u = 0; u < files.size(); u++) {
		Global::qi.macros.AddNull();
		Macro& macro = Global::qi.macros.Get();
		macro.name = (std::wstring(files[u].name)).substr(0, wcslen(files[u].name) - 5);

		neb::CJsonObject jMacro(File::TextLoad(std::string("macro\\") + String::toString(files[u].name)));
		std::string str;
		jMacro.Get("wndState", macro.wndState);
		jMacro.Get("wndName", str); macro.wi.wndName = String::toWString(str);
		jMacro.Get("wndClass", str); macro.wi.wndClass = String::toWString(str);
		jMacro.Get("state", macro.state);
		jMacro.Get("block", macro.block);
		jMacro.Get("mode", macro.mode);
		jMacro.Get("key", macro.key);
		jMacro.Get("count", macro.count);

		neb::CJsonObject jActions;
		jMacro.Get("actions", jActions);

		LoadAction(jActions, macro.actions);
	}
}
static void LoadJson()
{
	Global::qi.macros.clear();
	if (File::FileState(L"QuickInput.json"))
	{
		neb::CJsonObject cfg(File::TextLoad("QuickInput.json"));
		std::string str;
		cfg.Get("defOn", Global::qi.set.defOn);
		cfg.Get("key", Global::qi.set.key);
		cfg.Get("recKey", Global::qi.set.recKey);
		cfg.Get("showTips", Global::qi.set.showTips);
		cfg.Get("audFx", Global::qi.set.audFx);
		cfg.Get("minMode", Global::qi.set.minMode);
		cfg.Get("zoomBlock", Global::qi.set.zoomBlock);
		cfg.Get("quickClickState", Global::qi.fun.quickClick.state);
		cfg.Get("quickClickKey", Global::qi.fun.quickClick.key);
		cfg.Get("quickClickDelay", Global::qi.fun.quickClick.delay);
		cfg.Get("quickClickMode", Global::qi.fun.quickClick.mode);
		cfg.Get("showClockState", Global::qi.fun.showClock.state);
		cfg.Get("showClockKey", Global::qi.fun.showClock.key);
		cfg.Get("wndActiveState", Global::qi.fun.wndActive.state);
		cfg.Get("wndActiveName", str); Global::qi.fun.wndActive.wi.wndName = String::toWString(str);
	}
	else
	{
		Global::qi.set.defOn = 1;
		Global::qi.set.key = VK_F8;
		Global::qi.set.recKey = VK_F8;
		Global::qi.set.showTips = 1;
		Global::qi.set.audFx = 0;
		Global::qi.set.minMode = 0;
		Global::qi.set.zoomBlock = 0;
		Global::qi.fun.quickClick.state = 1;
		Global::qi.fun.quickClick.key = VK_LBUTTON;
		Global::qi.fun.quickClick.delay = 200;
		Global::qi.fun.quickClick.mode = 0;
		Global::qi.fun.showClock.state = 0;
		Global::qi.fun.showClock.key = VK_MENU;
		Global::qi.fun.wndActive.state = 0;
		Global::qi.fun.wndActive.wi.wndName = L"";
	}
	LoadMacro();
}

static std::wstring NameFilter(std::wstring name)
{
	for (uint32 n = 0;; n++)
	{
		for (uint32 nx = 0; nx < Global::qi.macros.size(); nx++)
		{
			std::wstring find = name + L" " + String::toWString(n + 1);
			if (Global::qi.macros[nx].name == find)
			{
				break;
			}
			if (nx >= Global::qi.macros.size() - 1)
			{
				return find;
			}
		}
	}
	return L"";
}