#pragma execution_character_set("utf-8")
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
////////////////// ~Window



////////////////// Actions
struct QiType
{
	enum
	{
		none,
		end,
		delay,
		key,
		mouse,
		text,
		color,
		loop,
		loopEnd,
		keyState,
		recoverPos,
		image,
		popText,
		rememberPos,
		timer
	};
};

union Action;
using Actions = List<Action>;

struct QiBase
{
	uint32 type;
	std::wstring mark;
	Actions next;
	QiBase(uint32 qiType = QiType::none) noexcept : type(qiType) {}
	QiBase(const QiBase& r) noexcept { operator=(r); }
	QiBase(QiBase&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiBase& r) noexcept { type = r.type; mark = r.mark; next = r.next; }
	void operator=(QiBase&& r) noexcept { type = r.type; mark = std::move(r.mark); next = std::move(r.next); }
};

struct QiEnd : QiBase
{
	QiEnd() noexcept : QiBase(QiType::end) {}
	QiEnd(const QiEnd& r) noexcept { operator=(r); }
	QiEnd(QiEnd&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiEnd& r) noexcept { QiBase::operator=(r); }
	void operator=(QiEnd&& r) noexcept { QiBase::operator=(std::move(r)); }
};
struct QiDelay : QiBase
{
	uint32 min; uint32 max;
	QiDelay() noexcept : QiBase(QiType::delay) {}
	QiDelay(const QiDelay& r) noexcept { operator=(r); }
	QiDelay(QiDelay&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiDelay& r) noexcept { QiBase::operator=(r); min = r.min; max = r.max; }
	void operator=(QiDelay&& r) noexcept { QiBase::operator=(std::move(r)); min = r.min; max = r.max; }
};
struct QiKey : QiBase
{
	enum { up, down, click };
	uint32 vk = 0; uint32 state = down;
	QiKey() noexcept : QiBase(QiType::key) {}
	QiKey(const QiKey& r) noexcept { operator=(r); }
	QiKey(QiKey&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiKey& r) noexcept { QiBase::operator=(r); vk = r.vk; state = r.state; }
	void operator=(QiKey&& r) noexcept { QiBase::operator=(std::move(r)); vk = r.vk; state = r.state; }
};
struct QiMouse : QiBase
{
	int32 x = 0; int32 y = 0; uint32 ex = 0; uint32 speed = 0; bool move = false; bool track = false;
	QiMouse() noexcept : QiBase(QiType::mouse) {}
	QiMouse(const QiMouse& r) noexcept { operator=(r); }
	QiMouse(QiMouse&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiMouse& r) noexcept { QiBase::operator=(r); x = r.x; y = r.y; ex = r.ex; speed = r.speed; move = r.move; track = r.track; }
	void operator=(QiMouse&& r) noexcept { QiBase::operator=(std::move(r)); x = r.x; y = r.y; ex = r.ex; speed = r.speed; move = r.move; track = r.track; }
};
struct QiText : QiBase
{
	std::wstring str;
	QiText() noexcept : QiBase(QiType::text) {}
	QiText(const QiText& r) noexcept { operator=(r); }
	QiText(QiText&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiText& r) noexcept { QiBase::operator=(r); str = r.str; }
	void operator=(QiText&& r) noexcept { QiBase::operator=(std::move(r)); str = std::move(r.str); }
};
struct QiColor : QiBase
{
	Rgba rgbe = 0; RECT rect = {}; bool unfind = false; bool move = false;
	QiColor() noexcept : QiBase(QiType::color) {}
	QiColor(const QiColor& r) noexcept { operator=(r); }
	QiColor(QiColor&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiColor& r) noexcept { QiBase::operator=(r); rgbe = r.rgbe; rect = r.rect; unfind = r.unfind; move = r.move; }
	void operator=(QiColor&& r) noexcept { QiBase::operator=(std::move(r)); rgbe = std::move(r.rgbe); rect = r.rect; unfind = r.unfind; move = r.move; }
};
struct QiLoop : QiBase
{
	uint32 min = 0; uint32 max = 0;
	QiLoop() noexcept : QiBase(QiType::loop) {}
	QiLoop(const QiLoop& r) noexcept { operator=(r); }
	QiLoop(QiLoop&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiLoop& r) noexcept { QiBase::operator=(r); min = r.min; max = r.max; }
	void operator=(QiLoop&& r) noexcept { QiBase::operator=(std::move(r)); min = r.min; max = r.max; }
};
struct QiLoopEnd : QiBase
{
	QiLoopEnd() noexcept : QiBase(QiType::loopEnd) {}
	QiLoopEnd(const QiLoopEnd& r) noexcept { operator=(r); }
	QiLoopEnd(QiLoopEnd&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiLoopEnd& r) noexcept { QiBase::operator=(r); }
	void operator=(QiLoopEnd&& r) noexcept { QiBase::operator=(std::move(r)); }
};
struct QiKeyState : QiBase
{
	uint32 vk = 0; bool state = true;
	QiKeyState() noexcept : QiBase(QiType::keyState) {}
	QiKeyState(const QiKeyState& r) noexcept { operator=(r); }
	QiKeyState(QiKeyState&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiKeyState& r) noexcept { QiBase::operator=(r); vk = r.vk; state = r.state; }
	void operator=(QiKeyState&& r) noexcept { QiBase::operator=(std::move(r)); vk = r.vk; state = r.state; }
};
struct QiRecoverPos : QiBase
{
	QiRecoverPos() noexcept : QiBase(QiType::recoverPos) {}
	QiRecoverPos(const QiRecoverPos& r) noexcept { operator=(r); }
	QiRecoverPos(QiRecoverPos&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiRecoverPos& r) noexcept { QiBase::operator=(r); }
	void operator=(QiRecoverPos&& r) noexcept { QiBase::operator=(std::move(r)); }
};
struct QiImage : QiBase
{
	RgbMap map; uint32 sim; RECT rect = {}; bool unfind = false; bool move = false;
	QiImage() noexcept : QiBase(QiType::image) {}
	QiImage(const QiImage& r) noexcept { operator=(r); }
	QiImage(QiImage&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiImage& r) noexcept { QiBase::operator=(r); map = r.map; sim = r.sim; rect = r.rect; unfind = r.unfind; move = r.move; }
	void operator=(QiImage&& r) noexcept { QiBase::operator=(std::move(r)); map = std::move(r.map); sim = r.sim; rect = r.rect; unfind = r.unfind; move = r.move; }
};
struct QiPopText : QiBase
{
	std::wstring str; uint32 time = 0;
	QiPopText() noexcept : QiBase(QiType::popText) {}
	QiPopText(const QiPopText& r) noexcept { operator=(r); }
	QiPopText(QiPopText&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiPopText& r) noexcept { QiBase::operator=(r); time = r.time; str = r.str; }
	void operator=(QiPopText&& r) noexcept { QiBase::operator=(std::move(r)); time = r.time; str = std::move(r.str); }
};
struct QiRememberPos : QiBase
{
	QiRememberPos() noexcept : QiBase(QiType::rememberPos) {}
	QiRememberPos(const QiRememberPos& r) noexcept { operator=(r); }
	QiRememberPos(QiRememberPos&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiRememberPos& r) noexcept { QiBase::operator=(r); }
	void operator=(QiRememberPos&& r) noexcept { QiBase::operator=(std::move(r)); }
};
struct QiTimer : QiBase
{
	uint32 min = 0; uint32 max = 0;
	QiTimer() noexcept : QiBase(QiType::loop) {}
	QiTimer(const QiTimer& r) noexcept { operator=(r); }
	QiTimer(QiTimer&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiTimer& r) noexcept { QiBase::operator=(r); min = r.min; max = r.max; }
	void operator=(QiTimer&& r) noexcept { QiBase::operator=(std::move(r)); min = r.min; max = r.max; }
};

union Action
{
	QiBase base;
	QiEnd end;
	QiDelay delay;
	QiKey key;
	QiMouse mouse;
	QiText text;
	QiColor color;
	QiLoop loop;
	QiLoopEnd loopEnd;
	QiKeyState keyState;
	QiRecoverPos recoverPos;
	QiImage image;
	QiPopText popText;
	QiRememberPos rememberPos;
	QiTimer timer;

	Action() noexcept : base() {}
	Action(const Action& r) noexcept { operator=(r); }
	Action(Action&& r) noexcept { operator=(std::move(r)); }
	Action(QiEnd&& r) noexcept : end(std::move(r)) {}
	Action(QiDelay&& r) noexcept : delay(std::move(r)) {}
	Action(QiKey&& r) noexcept : key(std::move(r)) {}
	Action(QiMouse&& r) noexcept : mouse(std::move(r)) {}
	Action(QiText&& r) noexcept : text(std::move(r)) {}
	Action(QiColor&& r) noexcept : color(std::move(r)) {}
	Action(QiLoop&& r) noexcept : loop(std::move(r)) {}
	Action(QiLoopEnd&& r) noexcept : loopEnd(std::move(r)) {}
	Action(QiKeyState&& r) noexcept : keyState(std::move(r)) {}
	Action(QiRecoverPos&& r) noexcept : recoverPos(std::move(r)) {}
	Action(QiImage&& r) noexcept : image(std::move(r)) {}
	Action(QiPopText&& r) noexcept : popText(std::move(r)) {}
	Action(QiRememberPos&& r) noexcept : rememberPos(std::move(r)) {}
	Action(QiTimer&& r) noexcept : timer(std::move(r)) {}
	~Action() { Release(); }

	void operator=(const Action& r) noexcept
	{
		Release();
		switch (r.base.type)
		{
		case QiType::end: end = r.end; break;
		case QiType::delay: delay = r.delay; break;
		case QiType::key: key = r.key; break;
		case QiType::mouse: mouse = r.mouse; break;
		case QiType::text: text = r.text; break;
		case QiType::color: color = r.color; break;
		case QiType::loop: loop = r.loop; break;
		case QiType::loopEnd: loopEnd = r.loopEnd; break;
		case QiType::keyState: keyState = r.keyState; break;
		case QiType::recoverPos: recoverPos = r.recoverPos; break;
		case QiType::image: image = r.image; break;
		case QiType::popText: popText = r.popText; break;
		case QiType::rememberPos: rememberPos = r.rememberPos; break;
		case QiType::timer: timer = r.timer; break;
		case QiType::none: base = r.base;
		}
	}
	void operator=(Action&& r) noexcept
	{
		Release();
		switch (r.base.type)
		{
		case QiType::end: end = std::move(r.end); break;
		case QiType::delay: delay = std::move(r.delay); break;
		case QiType::key: key = std::move(r.key); break;
		case QiType::mouse: mouse = std::move(r.mouse); break;
		case QiType::text: text = std::move(r.text); break;
		case QiType::color: color = std::move(r.color); break;
		case QiType::loop: loop = std::move(r.loop); break;
		case QiType::loopEnd: loopEnd = std::move(r.loopEnd); break;
		case QiType::keyState: keyState = std::move(r.keyState); break;
		case QiType::recoverPos: recoverPos = std::move(r.recoverPos); break;
		case QiType::image: image = std::move(r.image); break;
		case QiType::popText: popText = std::move(r.popText); break;
		case QiType::rememberPos: rememberPos = std::move(r.rememberPos); break;
		case QiType::timer: timer = std::move(r.timer); break;
		case QiType::none: base = std::move(r.base);
		}
	}

private:
	void Release()
	{
		switch (base.type)
		{
		case QiType::end: end.~QiEnd(); break;
		case QiType::delay: delay.~QiDelay(); break;
		case QiType::key: key.~QiKey(); break;
		case QiType::mouse: mouse.~QiMouse(); break;
		case QiType::text: text.~QiText(); break;
		case QiType::color: color.~QiColor(); break;
		case QiType::loop: loop.~QiLoop(); break;
		case QiType::loopEnd: loopEnd.~QiLoopEnd(); break;
		case QiType::keyState: keyState.~QiKeyState(); break;
		case QiType::recoverPos: recoverPos.~QiRecoverPos(); break;
		case QiType::image: image.~QiImage(); break;
		case QiType::popText: popText.~QiPopText(); break;
		case QiType::rememberPos: rememberPos.~QiRememberPos(); break;
		case QiType::timer: timer.~QiTimer(); break;
		case QiType::none: base.~QiBase(); break;
		}
		memset(this, 0, sizeof(Action));
	}
};
////////////////// ~Actions



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
////////////////// ~Macros



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
////////////////// ~Datas



namespace Qi
{
	inline QApplication* application = nullptr;

	inline bool state = false;
	inline bool run = false;

	inline bool recordState = false;
	inline bool recording = false;
	inline clock_t recordClock = 0;
	inline HWND recordWindow = 0;

	inline Actions record;
	inline Actions clipboard;
	inline Macros macros;

	inline FuncData fun;
	inline SettingsData set;
	inline Widget widget;
	inline QPopText* popText = nullptr;

	inline bool keyState[XBoxPad::key_end];
	inline List<byte> blockKeys;

	inline XBoxPad xboxpad;

	inline SIZE screen = {};

	inline std::wstring dir = Path::Last(Process::runPath());

	inline MSG msg;

	inline QiUi::QuickInputUi ui;
}