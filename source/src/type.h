﻿#pragma execution_character_set("utf-8")
#pragma once
#include "minc.h"

inline const std::wstring macroPath = L"macro\\";
inline const std::wstring macroType = L".json";

struct PopTextInfo
{
	QString t;
	QColor c;
};

// theme and text
namespace QiUi
{
	constexpr int event_restyle = QEvent::User + 1;

	struct Theme
	{
		QString name;
		QString style;
	};
	using Themes = List<Theme>;

	struct Text
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
		QString acTimer;
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

	struct PopText
	{
		PopTextInfo qe;
		PopTextInfo qd;
		PopTextInfo we;
		PopTextInfo wd;
		PopTextInfo qce;
		PopTextInfo qcd;
		PopTextInfo swe;
		PopTextInfo swd;
		PopTextInfo dwe;
		PopTextInfo dwd;
		PopTextInfo upe;
		PopTextInfo upd;
		int time = 1000;
		POINT p = {};
	};

	struct QuickInputUi
	{
		QString dialogStyle = "QPushButton{width:60px;border:2px solid blue}";
		Themes themes;
		Text text;
		PopText pop;
	};
}

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
extern struct Action;
using Actions = List<Action>;
struct QiDelay { uint32 tmin; uint32 tmax; };
struct QiKey { enum { up, down, click }; uint32 vk = 0; uint32 state = down; };
struct QiMouse { int32 x = 0; int32 y = 0; uint32 ex = 0; uint32 speed = 0; bool move = false; bool track = false; };
struct QiText { u16string str; void release() { str.release(); } };
struct QiColor { Rgba rgbe = 0; RECT rect = {}; bool unfind = false; bool move = false; };
struct QiLoop { uint32 cmin = 0; uint32 cmax = 0; };
struct QiKeyState { uint32 vk = 0; bool state = true; };
struct QiImage { RgbMap map; uint32 sim; RECT rect = {}; bool unfind = false; bool move = false; void release() { map.release(); } };
struct QiPopText { u16string str; uint32 time = 0; void release() { str.release(); } };
struct QiTimer { uint32 tmin = 0; uint32 tmax = 0; };
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
		_rememberPos,
		_timer
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
		QiTimer timer;
	} Data;
	Data d;
	Actions next;
	u16string mark;

	Action() { release(); }
	Action(const ActionType actionType) { release(); type = actionType; }
	Action(const Action& action) { release(); cpy(action); }
	~Action() { release(); }

	void operator=(const Action& action) { cpy(action); }

	void cpy(const Action& action)
	{
		release();
		mark = action.mark;
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
		case _timer: type = ActionType::_timer;
			d.timer = action.d.timer;
			next = action.next;
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

////////////////// Macros
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

	Actions acRun;
	Actions acEnd;

	HANDLE thRun = nullptr;
	HANDLE thEnd = nullptr;
};
using Macros = List<Macro>;
////////////////// #Macros

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
	bool state = false;
	uint32 key = 0;
};
struct WndActive
{
	bool state = false;
	WndInfo wi;
	HANDLE thread = 0;
};
struct SettingsData
{
	uint32 theme = 0;
	uint32 key = 0;
	uint32 recKey = 0;
	bool recTrack = false;
	bool defOn = false;
	bool showTips = false;
	bool audFx = false;
	bool minMode = false;
	bool scaleBlock = false;
};
struct FuncData
{
	QuickClick quickClick;
	ShowClock showClock;
	WndActive wndActive;
};
struct Widget
{
	bool dialogActive = false;
	bool mainActive = false;
	bool moreActive = false;
	QWidget* main = nullptr;
	QWidget* record = nullptr;
};
////////////////// #Datas

struct QuickInputStruct
{
	QApplication* application = nullptr;

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
	QPopText* popText = nullptr;

	bool keyState[XBoxPad::key_end];
	List<byte> blockKeys;

	XBoxPad xboxpad;

	SIZE screen = {};

	std::wstring dir = Path::Last(Process::runPath());

	MSG msg;

	QiUi::QuickInputUi ui;
};

extern QuickInputStruct qis;