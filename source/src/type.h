#pragma execution_character_set("utf-8")
#pragma once
#include "minc.h"

inline const std::wstring macroPath = L"macro\\";
inline const std::wstring macroType = L".json";

struct DataRole
{
	enum
	{
		id = Qt::UserRole
	};
};

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
		QString syJump;
		QString syPoint;

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
		QString acJump;
		QString acJumpPoint;
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
		POINT p = {};
		uint32 size = 100;
		uint32 time = 1000;
	};

	struct QuickInputUi
	{
		QString dialogStyle = R"(*{color:black}QPushButton{width:60px;border:2px solid blue})";
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
		timer,
		jump,
		jumpPoint
	};
};

class Action;
using Actions = List<Action>;

class QiBase
{
public:
	uint32 type;
	std::wstring mark;
	Actions next;
	QiBase(uint32 qiType = QiType::none) noexcept : type(qiType), mark(std::wstring()), next(Actions()) {}
	QiBase(const QiBase& r) noexcept { operator=(r); }
	QiBase(QiBase&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiBase& r) noexcept { type = r.type; mark = r.mark; next = r.next; }
	void operator=(QiBase&& r) noexcept { type = r.type; mark = std::move(r.mark); next = std::move(r.next); }
};
class QiEnd : public QiBase
{
public:
	QiEnd() noexcept : QiBase(QiType::end) {}
	QiEnd(const QiEnd& r) noexcept { operator=(r); }
	QiEnd(QiEnd&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiEnd& r) noexcept { QiBase::operator=(r); }
	void operator=(QiEnd&& r) noexcept { QiBase::operator=(std::move(r)); }
};
class QiDelay : public QiBase
{
public:
	uint32 min; uint32 max;
	QiDelay() noexcept : QiBase(QiType::delay) {}
	QiDelay(const QiDelay& r) noexcept { operator=(r); }
	QiDelay(QiDelay&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiDelay& r) noexcept { QiBase::operator=(r); min = r.min; max = r.max; }
	void operator=(QiDelay&& r) noexcept { QiBase::operator=(std::move(r)); min = r.min; max = r.max; }
};
class QiKey : public QiBase
{
public:
	enum { up, down, click };
	uint32 vk = 0; uint32 state = down;
	QiKey() noexcept : QiBase(QiType::key) {}
	QiKey(const QiKey& r) noexcept { operator=(r); }
	QiKey(QiKey&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiKey& r) noexcept { QiBase::operator=(r); vk = r.vk; state = r.state; }
	void operator=(QiKey&& r) noexcept { QiBase::operator=(std::move(r)); vk = r.vk; state = r.state; }
};
class QiMouse : public QiBase
{
public:
	int32 x = 0; int32 y = 0; uint32 ex = 0; uint32 speed = 0; bool move = false; bool track = false;
	QiMouse() noexcept : QiBase(QiType::mouse) {}
	QiMouse(const QiMouse& r) noexcept { operator=(r); }
	QiMouse(QiMouse&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiMouse& r) noexcept { QiBase::operator=(r); x = r.x; y = r.y; ex = r.ex; speed = r.speed; move = r.move; track = r.track; }
	void operator=(QiMouse&& r) noexcept { QiBase::operator=(std::move(r)); x = r.x; y = r.y; ex = r.ex; speed = r.speed; move = r.move; track = r.track; }
};
class QiText : public QiBase
{
public:
	std::wstring str;
	QiText() noexcept : QiBase(QiType::text) {}
	QiText(const QiText& r) noexcept { operator=(r); }
	QiText(QiText&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiText& r) noexcept { QiBase::operator=(r); str = r.str; }
	void operator=(QiText&& r) noexcept { QiBase::operator=(std::move(r)); str = std::move(r.str); }
};
class QiColor : public QiBase
{
public:
	Rgba rgbe = 0; RECT rect = {}; bool unfind = false; bool move = false;
	QiColor() noexcept : QiBase(QiType::color) {}
	QiColor(const QiColor& r) noexcept { operator=(r); }
	QiColor(QiColor&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiColor& r) noexcept { QiBase::operator=(r); rgbe = r.rgbe; rect = r.rect; unfind = r.unfind; move = r.move; }
	void operator=(QiColor&& r) noexcept { QiBase::operator=(std::move(r)); rgbe = std::move(r.rgbe); rect = r.rect; unfind = r.unfind; move = r.move; }
};
class QiLoop : public QiBase
{
public:
	uint32 min = 0; uint32 max = 0;
	QiLoop() noexcept : QiBase(QiType::loop) {}
	QiLoop(const QiLoop& r) noexcept { operator=(r); }
	QiLoop(QiLoop&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiLoop& r) noexcept { QiBase::operator=(r); min = r.min; max = r.max; }
	void operator=(QiLoop&& r) noexcept { QiBase::operator=(std::move(r)); min = r.min; max = r.max; }
};
class QiLoopEnd : public QiBase
{
public:
	QiLoopEnd() noexcept : QiBase(QiType::loopEnd) {}
	QiLoopEnd(const QiLoopEnd& r) noexcept { operator=(r); }
	QiLoopEnd(QiLoopEnd&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiLoopEnd& r) noexcept { QiBase::operator=(r); }
	void operator=(QiLoopEnd&& r) noexcept { QiBase::operator=(std::move(r)); }
};
class QiKeyState : public QiBase
{
public:
	uint32 vk = 0; bool state = true;
	QiKeyState() noexcept : QiBase(QiType::keyState) {}
	QiKeyState(const QiKeyState& r) noexcept { operator=(r); }
	QiKeyState(QiKeyState&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiKeyState& r) noexcept { QiBase::operator=(r); vk = r.vk; state = r.state; }
	void operator=(QiKeyState&& r) noexcept { QiBase::operator=(std::move(r)); vk = r.vk; state = r.state; }
};
class QiRecoverPos : public QiBase
{
public:
	QiRecoverPos() noexcept : QiBase(QiType::recoverPos) {}
	QiRecoverPos(const QiRecoverPos& r) noexcept { operator=(r); }
	QiRecoverPos(QiRecoverPos&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiRecoverPos& r) noexcept { QiBase::operator=(r); }
	void operator=(QiRecoverPos&& r) noexcept { QiBase::operator=(std::move(r)); }
};
class QiImage : public QiBase
{
public:
	RgbMap map; uint32 sim; RECT rect = {}; bool unfind = false; bool move = false;
	QiImage() noexcept : QiBase(QiType::image) {}
	QiImage(const QiImage& r) noexcept { operator=(r); }
	QiImage(QiImage&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiImage& r) noexcept { QiBase::operator=(r); map = r.map; sim = r.sim; rect = r.rect; unfind = r.unfind; move = r.move; }
	void operator=(QiImage&& r) noexcept { QiBase::operator=(std::move(r)); map = std::move(r.map); sim = r.sim; rect = r.rect; unfind = r.unfind; move = r.move; }
};
class QiPopText : public QiBase
{
public:
	std::wstring str; uint32 time = 0; bool sync = false;
	QiPopText() noexcept : QiBase(QiType::popText) {}
	QiPopText(const QiPopText& r) noexcept { operator=(r); }
	QiPopText(QiPopText&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiPopText& r) noexcept { QiBase::operator=(r); time = r.time; str = r.str; sync = r.sync; }
	void operator=(QiPopText&& r) noexcept { QiBase::operator=(std::move(r)); time = r.time; str = std::move(r.str); sync = r.sync; }
};
class QiRememberPos : public QiBase
{
public:
	QiRememberPos() noexcept : QiBase(QiType::rememberPos) {}
	QiRememberPos(const QiRememberPos& r) noexcept { operator=(r); }
	QiRememberPos(QiRememberPos&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiRememberPos& r) noexcept { QiBase::operator=(r); }
	void operator=(QiRememberPos&& r) noexcept { QiBase::operator=(std::move(r)); }
};
class QiTimer : public QiBase
{
public:
	uint32 min = 0; uint32 max = 0;
	QiTimer() noexcept : QiBase(QiType::loop) {}
	QiTimer(const QiTimer& r) noexcept { operator=(r); }
	QiTimer(QiTimer&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiTimer& r) noexcept { QiBase::operator=(r); min = r.min; max = r.max; }
	void operator=(QiTimer&& r) noexcept { QiBase::operator=(std::move(r)); min = r.min; max = r.max; }
};
class QiJump : public QiBase
{
public:
	int32 id = 0;
	QiJump() noexcept : QiBase(QiType::jump) {}
	QiJump(const QiJump& r) noexcept { operator=(r); }
	QiJump(QiJump&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiJump& r) noexcept { QiBase::operator=(r); id = r.id; }
	void operator=(QiJump&& r) noexcept { QiBase::operator=(std::move(r)); id = r.id; }
};
class QiJumpPoint : public QiBase
{
public:
	int32 id = 0;
	QiJumpPoint() noexcept : QiBase(QiType::jumpPoint) {}
	QiJumpPoint(const QiJumpPoint& r) noexcept { operator=(r); }
	QiJumpPoint(QiJumpPoint&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiJumpPoint& r) noexcept { QiBase::operator=(r); id = r.id; }
	void operator=(QiJumpPoint&& r) noexcept { QiBase::operator=(std::move(r)); id = r.id; }
};

using ActionVariant = std::variant
<
	QiBase,
	QiEnd,
	QiDelay,
	QiKey,
	QiMouse,
	QiText,
	QiColor,
	QiLoop,
	QiLoopEnd,
	QiKeyState,
	QiRecoverPos,
	QiImage,
	QiPopText,
	QiRememberPos,
	QiTimer,
	QiJump,
	QiJumpPoint
>;

class Action : public ActionVariant
{
public:
	using ActionVariant::ActionVariant;

	Action() : ActionVariant(QiBase()) { }

	QiBase& base()
	{
		QiBase* base;
		std::visit([&base](auto&& var)
			{
				base = &var;
			}, *this
		);
		return *base;
	}
};
/*
std::visit([](auto&& var)
	{
		using T = std::decay_t<decltype(var)>;
		if constexpr (std::is_same_v<T, QiEnd>)
		{
			const QiEnd& end = var;
		}
		else if constexpr (std::is_same_v<T, QiDelay>)
		{
			const QiDelay& delay = var;
		}
		else if constexpr (std::is_same_v<T, QiKey>)
		{
			const QiKey& key = var;
		}
		else if constexpr (std::is_same_v<T, QiMouse>)
		{
			const QiMouse& mouse = var;
		}
		else if constexpr (std::is_same_v<T, QiText>)
		{
			const QiText& text = var;
		}
		else if constexpr (std::is_same_v<T, QiColor>)
		{
			const QiColor& color = var;
		}
		else if constexpr (std::is_same_v<T, QiLoop>)
		{
			const QiLoop& loop = var;
		}
		else if constexpr (std::is_same_v<T, QiLoopEnd>)
		{
			const QiLoopEnd& loopEnd = var;
		}
		else if constexpr (std::is_same_v<T, QiKeyState>)
		{
			const QiKeyState& keyState = var;
		}
		else if constexpr (std::is_same_v<T, QiRecoverPos>)
		{
			const QiRecoverPos& recoverPos = var;
		}
		else if constexpr (std::is_same_v<T, QiImage>)
		{
			const QiImage& image = var;
		}
		else if constexpr (std::is_same_v<T, QiPopText>)
		{
			const QiPopText& popText = var;
		}
		else if constexpr (std::is_same_v<T, QiRememberPos>)
		{
			const QiRememberPos& rememberPos = var;
		}
		else if constexpr (std::is_same_v<T, QiTimer>)
		{
			const QiTimer& timer = var;
		}
		else if constexpr (std::is_same_v<T, QiJump>)
		{
			const QiJump& jump = var;
		}
		else if constexpr (std::is_same_v<T, QiJumpPoint>)
		{
			const QiJumpPoint& jumpPoint = var;
		}
		else
		{
		}
	}, var
);
*/
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

	POINT cursor = {};

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
	constexpr size_t keySize = XBoxPad::key_end;

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

	inline bool keyState[keySize];
	inline List<byte> blockKeys;

	inline XBoxPad xboxpad;

	inline SIZE screen = {};

	inline std::wstring dir = Path::Last(Process::runPath());

	inline MSG msg;

	inline QiUi::QuickInputUi ui;
}