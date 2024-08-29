#pragma once
#include <QString>
#include <QFont>
#include <QWidget>
#include <QApplication>
#include "../cg/cg.h"

// Change to your Qt directory
#ifdef DEBUG
#pragma comment(lib, "C:/Qt/32/lib/Qt5WinExtrasd.lib")
#else
#pragma comment(lib, "C:/Qt/32s/lib/Qt5WinExtras.lib")
#endif

#define MacroPath L"macro\\"
#define MacroType L".json"

enum QiEvent
{
	recStart = QEvent::User + 1,
	recStop = QEvent::User + 2,
	recClose = QEvent::User + 3,
	setTheme = QEvent::User + 4
};

////////////////// Window
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
	static inline HANDLE thread;
	static DWORD _stdcall LockThread(PVOID wnd)
	{
		while (IsWindowVisible((HWND)wnd)) {
			RECT rect = Window::rect((HWND)wnd);
			ClipCursor(&rect);
			Thread::Sleep(10);
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
////////////////// #Window

////////////////// Actions
extern struct Action; typedef List<Action> Actions;
struct QiDelay { uint32 tmin; uint32 tmax; };
struct QiKey { enum { up, down, click }; uint32 vk = 0; uint32 state = down; };
struct QiMouse { int32 x = 0; int32 y = 0; uint32 ex = 0; bool move = false; bool track = false; };
struct QiText
{
	void release() { str.release(); }
	wcstring str;
};
struct QiColor { Rgba rgbe = 0; RECT rect = {}; bool unfind = false; bool move = false; };
struct QiLoop { uint32 cmin = 0; uint32 cmax = 0; };
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
		_popText,
		_rememberPos
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
		case _rememberPos: type = ActionType::_rememberPos;
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
////////////////// #Actions

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
	std::wstring name;

	WndInfo wi;
	POINT origin;

	Actions acRun;
	Actions acEnd;

	HANDLE thRun = nullptr;
	HANDLE thEnd = nullptr;
};
typedef List<Macro> Macros;

////////////////// Datas
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
	uint32 theme = 0;
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
struct Widget
{
	QWidget* main = 0;
	QWidget* record = 0;
};
////////////////// #Datas

struct Theme { QString name; QString style; }; typedef List<Theme> Themes;
struct QuickInputUi
{
	QString syOn;
	QString syOff;
	QString syOk;
	QString syYes;
	QString syNot;
	QString syStop;
	QString syShow;
	QString syHide;
	QString syOption;
	QString syLink;
	QString syEdit;
	QString syUp;
	QString syDown;
	QString syTurn;
	QString syLeft;
	QString syTop;
	QString syRight;
	QString syMove;
	QString syTime;
	QString syText;
	QString syLoop;
	QString syColor;
	QString syImage;

	std::wstring qiOn;
	std::wstring qiOff;
	QString muOn;
	QString muOff;
	QString muShow;
	QString muHide;
	QString muExit;
	QString acWait;
	QString acDown;
	QString acUp;
	QString acClick;
	QString acPos;
	QString acMove;
	QString acLoop;
	QString acText;
	QString acColor;
	QString acEnd;
	QString acEndLoop;
	QString acKeyState;
	QString acRecoverPos;
	QString acImage;
	QString acPopText;
	QString acRememberPos;
	QString trOn;
	QString trOff;
	QString etChange;
	QString etAdd;
	QString etDel;
	QString etEdit;
	QString etFunc;
	QString etParam;
	QString rcStart;
	QString rcStop;
	QString rcClose;
};
struct QuickInputStruct
{
	bool state = false;
	bool run = false;

	bool recordState = false;
	bool recording = false;
	clock_t recordClock = 0;
	HWND recordWindow = 0;

	Actions record;
	Actions clipboard;
	Macros macros;

	FuncData fun;
	SettingsData set;

	Widget widget;

	SIZE screen = {};

	byte keyState[255];
	List<byte> blockKeys;

	Themes themes;
	QuickInputUi ui;
};

inline QuickInputStruct qis;