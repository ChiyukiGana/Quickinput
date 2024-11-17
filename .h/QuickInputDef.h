#pragma once
#include <QString>
#include <QFont>
#include <QWidget>
#include "../cg/cg.h"

// Change to your Qt directory
#ifdef DEBUG
#pragma comment(lib, "C:/Qt32/lib/Qt5WinExtrasd.lib")
#else
#pragma comment(lib, "C:/Qt32s/lib/Qt5WinExtras.lib")
#endif

struct UI
{
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
	static QString syImage;

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
	static QString acKeyState;
	static QString acRecoverPos;
	static QString acImage;
	static QString acPopText;
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

struct Style
{
	QString name;
	QString style;
};
typedef List<Style> Styles;

struct ChildWindow
{
	HWND wnd; // child
	RECT rect; // child rect
};
typedef List<ChildWindow> ChildWindows;
struct WndInput
{
	HWND wnd = 0; // parent
	HWND current = 0; // currented (pt in rect)
	ChildWindows children;
	POINT pt = {}; // prev point
	WORD mk = 0; // prev key
};
struct WndInfo
{
	HWND wnd = 0;
	bool child = false;
	std::wstring wndName;
	std::wstring wndClass;
	bool Update() { return wnd = FindWindowW(wndClass.c_str(), wndName.c_str()); }
};
struct WndLock
{
	static HANDLE thread;
	static DWORD CALLBACK LockThread(PVOID wnd)
	{
		while (IsWindowVisible((HWND)wnd)) {
			RECT rect = Window::rect((HWND)wnd);
			ClipCursor(&rect);
			sleep(10);
		}
		thread = 0;
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

extern struct Action;
typedef List<Action> Actions;
struct QiDelay { uint32 ms; uint32 ex; };
struct QiKey { enum { up, down, click }; uint32 vk = 0; uint32 state = down; };
struct QiMouse { int32 x = 0; int32 y = 0; uint32 ex = 0; bool move = false; };
struct QiText 
{
	void release() { str.release(); }
	wcstring str;
};
struct QiColor { Rgba rgbe = 0; RECT rect = {}; bool unfind = false; bool move = false; };
struct QiLoop { uint32 count = 0; uint32 rand = 0; };
struct QiKeyState { uint32 vk = 0; bool state = true; };
struct QiImage
{
	void release() { map.release(); }
	RgbMap map; uint32 sim; RECT rect = {}; bool unfind = false; bool move = false;
};
struct QiPopText
{
	void release() { str.release(); }
	wcstring str; uint32 time = 0;
};

struct Action
{
	typedef enum
	{
		_none,
		_end,
		_delay,
		_key,
		_mouse,
		_text,
		_color,
		_loop,
		_loopEnd,
		_keyState,
		_revocerPos,
		_image,
		_popText
	} ActionType;
	
	ActionType type = _none;
	typedef union _Data
	{
		_Data() {}
		~_Data() {}
		QiDelay delay;
		QiKey key;
		QiMouse mouse;
		QiText text;
		QiColor color;
		QiLoop loop;
		QiKeyState keyState;
		QiImage image;
		QiPopText popText;
	} Data;
	Data d;
	Actions next;
	wcstring mark;

	Action() { release(); }
	Action(const ActionType actionType) { release(); type = actionType; }
	Action(const Action& action) { release(); cpy(action); }
	~Action() { release(); }

	void operator=(const Action& action) { cpy(action); }

	void cpy(const Action& action)
	{
		release();
		mark.cpy(action.mark);
		switch (action.type)
		{
		case _end: type = ActionType::_end;
			break;
		case _delay: type = ActionType::_delay;
			d.delay = action.d.delay;
			break;
		case _key: type = ActionType::_key;
			d.key = action.d.key;
			break;
		case _mouse: type = ActionType::_mouse;
			d.mouse = action.d.mouse;
			break;
		case _text: type = ActionType::_text;
			d.text = action.d.text;
			break;
		case _color: type = ActionType::_color;
			d.color = action.d.color;
			next = action.next;
			break;
		case _loop: type = ActionType::_loop;
			d.loop = action.d.loop;
			next = action.next;
			break;
		case _loopEnd: type = ActionType::_loopEnd;
			break;
		case _keyState: type = ActionType::_keyState;
			d.keyState = action.d.keyState;
			next = action.next;
			break;
		case _revocerPos: type = ActionType::_revocerPos;
			break;
		case _image: type = ActionType::_image;
			d.image = action.d.image;
			next = action.next;
			break;
		case _popText: type = ActionType::_popText;
			d.popText = action.d.popText;
			break;
		default: type = ActionType::_none;
		}
	}

	void release() {
		switch (type)
		{
		case _text: d.text.release(); break;
		case _image: d.image.release(); break;
		case _popText: d.popText.release(); break;
		}
		mark.release();
		type = _none;
		memset(&d, 0, sizeof(Data));
	}
};

struct Macro
{
	enum { sw, down, up };
	bool state = false; // enable | disable
	bool block = false; // block this trigger key
	bool wndState = false; // window mode enable | disable
	bool active = false; // state of release trigger
	uint32 key = 0;
	uint32 mode = 0;
	uint32 count = 0;
	
	POINT originPos = {};

	std::wstring name;
	
	WndInfo wi;

	HANDLE thread = 0;
	HANDLE threadEnding = 0;
	Actions actions;
	Actions actionsEnding;
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
	uint32 style = 0;
	uint32 key = 0;
	uint32 recKey = 0;
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
	Actions clipboard;
	Macros macros;
	FuncData fun;
	SettingsData set;
	QWidget* main = 0;
	QWidget* rec = 0;

	Styles styles;

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
	static byte keyState[255];
};