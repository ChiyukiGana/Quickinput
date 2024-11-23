#pragma once
#include "inc_project.h"
constexpr int key_info = 214;
constexpr int msg_exit = (WM_USER + 0xFF);
constexpr int keySize = XBoxPad::key_end;
namespace QiUi
{
	constexpr int event_restyle = QEvent::User;
	struct Theme
	{
		QString name;
		QString style;
	};
	using Themes = QList<Theme>;
	struct Text
	{
		// symbols
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
		QString syBlock;
		QString syBlockExec;
		// menu
		QString muOn;
		QString muOff;
		QString muShow;
		QString muHide;
		QString muExit;
		// action
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
		QString acDialog;
		QString acBlock;
		QString acBlockExec;
		// state
		QString trOn;
		QString trOff;
		// edit
		QString etChange;
		QString etAdd;
		QString etDel;
		QString etEdit;
		QString etFunc;
		QString etParam;
		// record
		QString rcStart;
		QString rcStop;
		QString rcClose;
	};
	struct PopTextInfo
	{
		QString t;
		QColor c;
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
		int size = 100;
		int time = 1000;
	};
	struct QuickInputUi
	{
		QString dialogStyle = R"(*{color:black}QPushButton{width:60px;border:2px solid blue})";
		Themes themes;
		Text text;
		PopText pop;
	};
}
enum InterpreterResult
{
	r_exit,
	r_continue,
	r_break,
	r_top
};
struct DataRole
{
	enum
	{
		id = Qt::UserRole
	};
};
////////////////// Window
struct WndInput
{
	HWND wnd = nullptr; // parent
	QList<HWND> children;
	bool child = false;
	HWND current = nullptr;
	POINT pt = {}; // prev point
	WORD mk = 0; // prev key
	bool active() { return IsWindow(wnd); }
	HWND find(const POINT& pt, POINT& ptrel, bool child)
	{
		if (!active()) return nullptr;
		if (child)
		{
			children = Window::FindChild(wnd);
			QList<HWND> inrect;
			for (int u = (int)children.size() - 1; u > -1; u--) if (InRect(Window::childRect(wnd, children[u]), pt)) inrect.append(children[u]);
			if (inrect.size())
			{
				// select minimum window
				HWND min = nullptr;
				int minArea = (int)((~(0x8000000))-1);
				for (int i = 0; i < inrect.size(); i++)
				{
					int area = RectArea(Window::rect(inrect[i]));
					if (area < minArea) min = inrect[i], minArea = area;
				}
				if (min)
				{
					ptrel = InRectPos(Window::childRect(wnd, min), pt);
					return min;
				}
			}
		}
		ptrel = InRectPos(Window::sizeRect(wnd), pt);
		return wnd;
	}
};
struct WndInfo
{
	HWND wnd = nullptr;
	bool child = false;
	QString wndName;
	QString wndClass;
	bool update() {
		if (!IsWindow(wnd))
		{
			return wnd = FindWindowW((LPCWSTR)(wndClass.utf16()), (LPCWSTR)(wndName.utf16()));
		}
		return true;
	}
};
struct WndLock
{
	static HANDLE thread;
	static DWORD _stdcall LockThread(PVOID wnd)
	{
		while (IsWindowVisible((HWND)wnd)) {
			RECT rect = Window::rect((HWND)wnd);
			ClipCursor(&rect);
			Sleep(10);
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
////////////////// Action
struct QiType
{
	enum
	{
		none,
		end,
		delay,
		key,
		mouse,
		copyText,
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
		jumpPoint,
		dialog,
		block,
		blockExec
	};
};
using Actions = QiVector<class Action>;
class QiBase
{
public:
	int type;
	QString mark;
	Actions next;
	Actions next2;
	QiBase(int qiType = QiType::none) noexcept : type(qiType), mark(QString()), next(Actions()), next2(Actions()) {}
	QiBase(const QiBase& r) { operator=(r); }
	QiBase(QiBase&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiBase& r) noexcept { type = r.type; mark = r.mark; next.copy(r.next); next2.copy(r.next2); }
	void operator=(QiBase&& r) noexcept { type = r.type; mark = std::move(r.mark); next = std::move(r.next); next2 = std::move(r.next2); }
};
class QiEnd : public QiBase
{
public:
	QiEnd() : QiBase(QiType::end) {}
	QiEnd(const QiEnd& r) { operator=(r); }
	QiEnd(QiEnd&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiEnd& r) { QiBase::operator=(r); }
	void operator=(QiEnd&& r) noexcept { QiBase::operator=(std::move(r)); }
};
class QiDelay : public QiBase
{
public:
	int min; int max;
	QiDelay() : QiBase(QiType::delay) {}
	QiDelay(const QiDelay& r) { operator=(r); }
	QiDelay(QiDelay&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiDelay& r) { QiBase::operator=(r); min = r.min; max = r.max; }
	void operator=(QiDelay&& r) noexcept { QiBase::operator=(std::move(r)); min = r.min; max = r.max; }
};
class QiKey : public QiBase
{
public:
	enum { up, down, click };
	int vk = 0; int state = down;
	QiKey() : QiBase(QiType::key) {}
	QiKey(const QiKey& r) { operator=(r); }
	QiKey(QiKey&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiKey& r) { QiBase::operator=(r); vk = r.vk; state = r.state; }
	void operator=(QiKey&& r) noexcept { QiBase::operator=(std::move(r)); vk = r.vk; state = r.state; }
};
class QiMouse : public QiBase
{
public:
	int x = 0; int y = 0; int ex = 0; int speed = 0; bool move = false; bool track = false;
	QiMouse() : QiBase(QiType::mouse) {}
	QiMouse(const QiMouse& r) { operator=(r); }
	QiMouse(QiMouse&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiMouse& r) { QiBase::operator=(r); x = r.x; y = r.y; ex = r.ex; speed = r.speed; move = r.move; track = r.track; }
	void operator=(QiMouse&& r) noexcept { QiBase::operator=(std::move(r)); x = r.x; y = r.y; ex = r.ex; speed = r.speed; move = r.move; track = r.track; }
};
class QiCopyText : public QiBase
{
public:
	QString text;
	QiCopyText() : QiBase(QiType::copyText) {}
	QiCopyText(const QiCopyText& r) { operator=(r); }
	QiCopyText(QiCopyText&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiCopyText& r) { QiBase::operator=(r); text = r.text; }
	void operator=(QiCopyText&& r) noexcept { QiBase::operator=(std::move(r)); text = std::move(r.text); }
};
class QiColor : public QiBase
{
public:
	Rgba rgbe = 0; RECT rect = {}; bool move = false;
	QiColor() : QiBase(QiType::color) {}
	QiColor(const QiColor& r) { operator=(r); }
	QiColor(QiColor&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiColor& r) { QiBase::operator=(r); rgbe = r.rgbe; rect = r.rect; move = r.move; }
	void operator=(QiColor&& r) noexcept { QiBase::operator=(std::move(r)); rgbe = std::move(r.rgbe); rect = r.rect; move = r.move; }
};
class QiLoop : public QiBase
{
public:
	int min = 0; int max = 0;
	QiLoop() : QiBase(QiType::loop) {}
	QiLoop(const QiLoop& r) { operator=(r); }
	QiLoop(QiLoop&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiLoop& r) { QiBase::operator=(r); min = r.min; max = r.max; }
	void operator=(QiLoop&& r) noexcept { QiBase::operator=(std::move(r)); min = r.min; max = r.max; }
};
class QiLoopEnd : public QiBase
{
public:
	QiLoopEnd() : QiBase(QiType::loopEnd) {}
	QiLoopEnd(const QiLoopEnd& r) { operator=(r); }
	QiLoopEnd(QiLoopEnd&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiLoopEnd& r) { QiBase::operator=(r); }
	void operator=(QiLoopEnd&& r) noexcept { QiBase::operator=(std::move(r)); }
};
class QiKeyState : public QiBase
{
public:
	int vk = 0;
	QiKeyState() : QiBase(QiType::keyState) {}
	QiKeyState(const QiKeyState& r) { operator=(r); }
	QiKeyState(QiKeyState&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiKeyState& r) { QiBase::operator=(r); vk = r.vk; }
	void operator=(QiKeyState&& r) noexcept { QiBase::operator=(std::move(r)); vk = r.vk; }
};
class QiRecoverPos : public QiBase
{
public:
	QiRecoverPos() : QiBase(QiType::recoverPos) {}
	QiRecoverPos(const QiRecoverPos& r) { operator=(r); }
	QiRecoverPos(QiRecoverPos&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiRecoverPos& r) { QiBase::operator=(r); }
	void operator=(QiRecoverPos&& r) noexcept { QiBase::operator=(std::move(r)); }
};
class QiImage : public QiBase
{
public:
	RgbMap map; int sim; RECT rect = {}; bool move = false;
	QiImage() : QiBase(QiType::image) {}
	QiImage(const QiImage& r) { operator=(r); }
	QiImage(QiImage&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiImage& r) { QiBase::operator=(r); map = r.map; sim = r.sim; rect = r.rect; move = r.move; }
	void operator=(QiImage&& r) noexcept { QiBase::operator=(std::move(r)); map = std::move(r.map); sim = r.sim; rect = r.rect; move = r.move; }
};
class QiPopText : public QiBase
{
public:
	QString text; int time = 0; bool sync = false;
	QiPopText() : QiBase(QiType::popText) {}
	QiPopText(const QiPopText& r) { operator=(r); }
	QiPopText(QiPopText&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiPopText& r) { QiBase::operator=(r); time = r.time; text = r.text; sync = r.sync; }
	void operator=(QiPopText&& r) noexcept { QiBase::operator=(std::move(r)); time = r.time; text = std::move(r.text); sync = r.sync; }
};
class QiRememberPos : public QiBase
{
public:
	QiRememberPos() : QiBase(QiType::rememberPos) {}
	QiRememberPos(const QiRememberPos& r) { operator=(r); }
	QiRememberPos(QiRememberPos&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiRememberPos& r) { QiBase::operator=(r); }
	void operator=(QiRememberPos&& r) noexcept { QiBase::operator=(std::move(r)); }
};
class QiTimer : public QiBase
{
public:
	int min = 0; int max = 0;
	QiTimer() : QiBase(QiType::loop) {}
	QiTimer(const QiTimer& r) { operator=(r); }
	QiTimer(QiTimer&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiTimer& r) { QiBase::operator=(r); min = r.min; max = r.max; }
	void operator=(QiTimer&& r) noexcept { QiBase::operator=(std::move(r)); min = r.min; max = r.max; }
};
class QiJump : public QiBase
{
public:
	int id = 0;
	QiJump() : QiBase(QiType::jump) {}
	QiJump(const QiJump& r) { operator=(r); }
	QiJump(QiJump&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiJump& r) { QiBase::operator=(r); id = r.id; }
	void operator=(QiJump&& r) noexcept { QiBase::operator=(std::move(r)); id = r.id; }
};
class QiJumpPoint : public QiBase
{
public:
	int id = 0;
	QiJumpPoint() : QiBase(QiType::jumpPoint) {}
	QiJumpPoint(const QiJumpPoint& r) { operator=(r); }
	QiJumpPoint(QiJumpPoint&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiJumpPoint& r) { QiBase::operator=(r); id = r.id; }
	void operator=(QiJumpPoint&& r) noexcept { QiBase::operator=(std::move(r)); id = r.id; }
};
class QiDialog : public QiBase
{
public:
	enum Style
	{
		none,
		warning,
		error
	};
	int style;
	QString title;
	QString text;
	QiDialog() : QiBase(QiType::dialog) {}
	QiDialog(const QiDialog& r) { operator=(r); }
	QiDialog(QiDialog&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiDialog& r) { QiBase::operator=(r); style = r.style; title = r.title; text = r.text; }
	void operator=(QiDialog&& r) noexcept { QiBase::operator=(std::move(r)); style = r.style; title = std::move(r.title); text = std::move(r.text); }
};
class QiBlock : public QiBase
{
public:
	int id;
	QiBlock() : QiBase(QiType::block) {}
	QiBlock(const QiBlock& r) { operator=(r); }
	QiBlock(QiBlock&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiBlock& r) { QiBase::operator=(r); id = r.id; }
	void operator=(QiBlock&& r) noexcept { QiBase::operator=(std::move(r)); id = r.id; }
};
class QiBlockExec : public QiBase
{
public:
	int id;
	QiBlockExec() : QiBase(QiType::blockExec) {}
	QiBlockExec(const QiBlockExec& r) { operator=(r); }
	QiBlockExec(QiBlockExec&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiBlockExec& r) { QiBase::operator=(r); id = r.id; }
	void operator=(QiBlockExec&& r) noexcept { QiBase::operator=(std::move(r)); id = r.id; }
};
using ActionVariant = std::variant
<
	QiBase,
	QiEnd,
	QiDelay,
	QiKey,
	QiMouse,
	QiCopyText,
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
	QiJumpPoint,
	QiDialog,
	QiBlock,
	QiBlockExec
> ;
class Action : public ActionVariant
{
public:
	using ActionVariant::ActionVariant;
	Action() : ActionVariant(QiBase()) {}
	Action(const Action& v)
	{
		operator=(v);
	}
	Action(Action&& r) noexcept
	{
		operator=(std::move(r));
	}
	void operator=(const Action& v)
	{
		ActionVariant::operator=(v);
	}
	void operator=(Action&& r) noexcept
	{
		ActionVariant::operator=(std::move(r));
	}
	QiBase& base()
	{
		QiBase* base;
		std::visit([&base](auto&& var) { base = &var; }, *this);
		return *base;
	}
	const QiBase& base() const
	{
		const QiBase* base;
		std::visit([&base](auto&& var) { base = &var; }, *this);
		return *base;
	}
};
/*
Action& var = actions->at(i);
switch (var.index())
{
case QiType::end:
{
	QiEnd& end = std::get<QiEnd>(var);
} break;
case QiType::delay:
{
	QiDelay& delay = std::get<QiDelay>(var);
} break;
case QiType::key:
{
	QiKey& key = std::get<QiKey>(var);
} break;
case QiType::mouse:
{
	QiMouse& mouse = std::get<QiMouse>(var);
} break;
case QiType::copyText:
{
	QiCopyText& text = std::get<QiCopyText>(var);
} break;
case QiType::color:
{
	QiColor& color = std::get<QiColor>(var);
} break;
case QiType::loop:
{
	QiLoop& loop = std::get<QiLoop>(var);
} break;
case QiType::loopEnd:
{
	QiLoopEnd& loopEnd = std::get<QiLoopEnd>(var);
} break;
case QiType::keyState:
{
	QiKeyState& keyState = std::get<QiKeyState>(var);
} break;
case QiType::recoverPos:
{
	QiRecoverPos& recoverPos = std::get<QiRecoverPos>(var);
} break;
case QiType::image:
{
	QiImage& image = std::get<QiImage>(var);
} break;
case QiType::popText:
{
	QiPopText& popText = std::get<QiPopText>(var);
} break;
case QiType::rememberPos:
{
	QiRememberPos& rememberPos = std::get<QiRememberPos>(var);
} break;
case QiType::timer:
{
	QiTimer& timer = std::get<QiTimer>(var);
} break;
case QiType::jump:
{
	QiJump& jump = std::get<QiJump>(var);
} break;
case QiType::jumpPoint:
{
	QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(var);
} break;
case QiType::dialog:
{
	QiDialog& dialog = std::get<QiDialog>(var);
} break;
case QiType::block:
{
	QiBlock& block = std::get<QiBlock>(var);
} break;
case QiType::blockExec:
{
	QiBlockExec& blockExec = std::get<QiBlockExec>(var);
} break;
}
----------------------------------------
const Action& var = actions->at(i);
switch (var.index())
{
case QiType::end:
{
	const QiEnd& end = std::get<QiEnd>(var);
} break;
case QiType::delay:
{
	const QiDelay& delay = std::get<QiDelay>(var);
} break;
case QiType::key:
{
	const QiKey& key = std::get<QiKey>(var);
} break;
case QiType::mouse:
{
	const QiMouse& mouse = std::get<QiMouse>(var);
} break;
case QiType::copyText:
{
	const QiCopyText& text = std::get<QiCopyText>(var);
} break;
case QiType::color:
{
	const QiColor& color = std::get<QiColor>(var);
} break;
case QiType::loop:
{
	const QiLoop& loop = std::get<QiLoop>(var);
} break;
case QiType::loopEnd:
{
	const QiLoopEnd& loopEnd = std::get<QiLoopEnd>(var);
} break;
case QiType::keyState:
{
	const QiKeyState& keyState = std::get<QiKeyState>(var);
} break;
case QiType::recoverPos:
{
	const QiRecoverPos& recoverPos = std::get<QiRecoverPos>(var);
} break;
case QiType::image:
{
	const QiImage& image = std::get<QiImage>(var);
} break;
case QiType::popText:
{
	const QiPopText& popText = std::get<QiPopText>(var);
} break;
case QiType::rememberPos:
{
	const QiRememberPos& rememberPos = std::get<QiRememberPos>(var);
} break;
case QiType::timer:
{
	const QiTimer& timer = std::get<QiTimer>(var);
} break;
case QiType::jump:
{
	const QiJump& jump = std::get<QiJump>(var);
} break;
case QiType::jumpPoint:
{
	const QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(var);
} break;
case QiType::dialog:
{
	const QiDialog& dialog = std::get<QiDialog>(var);
} break;
case QiType::block:
{
	const QiBlock& block = std::get<QiBlock>(var);
} break;
case QiType::blockExec:
{
	const QiBlockExec& blockExec = std::get<QiBlockExec>(var);
} break;
}
*/
////////////////// Macro
struct Macro
{
	Macro() {}
	Macro(const Macro& v)
	{
		operator=(v);
	}
	Macro(Macro&& r) noexcept
	{
		operator=(std::move(r));
	}
	void operator=(const Macro& v)
	{
		state = v.state;
		block = v.block;
		wndState = v.wndState;
		active = v.active;
		key = v.key;
		mode = v.mode;
		count = v.count;
		name = v.name;
		acRun.copy(v.acRun);
		acEnd.copy(v.acEnd);
		cursor = v.cursor;
		wi = v.wi;
		wp = v.wp;
		thRun = v.thRun;
		thEnd = v.thEnd;
	}
	void operator=(Macro&& r) noexcept
	{
		state = r.state;
		block = r.block;
		wndState = r.wndState;
		active = r.active;
		key = r.key;
		mode = r.mode;
		count = r.count;
		name = r.name;
		acRun = std::move(r.acRun);
		acEnd = std::move(r.acEnd);
		cursor = r.cursor;
		wi = r.wi;
		wp = r.wp;
		thRun = r.thRun;
		thEnd = r.thEnd;
	}
	enum { sw, down, up };
	bool state = false; // enable | disable
	bool block = false; // block this trigger key
	bool wndState = false; // window mode enable | disable
	bool active = false; // state of release trigger
	int key = 0;
	int mode = 0;
	int count = 0;
	QString name;
	Actions acRun;
	Actions acEnd;
	POINT cursor = {};
	WndInfo wi;
	WndInput wp;
	HANDLE thRun = nullptr;
	HANDLE thEnd = nullptr;
};
using Macros = QiVector<Macro>;
////////////////// Datas
struct QuickClick
{
	bool state = 0;
	int mode = 0;
	int key = 0;
	int delay = 10;
	HANDLE thread = 0;
};
struct ShowClock
{
	bool state = false;
	int key = 0;
};
struct WndActive
{
	bool state = false;
	WndInfo wi;
	HANDLE thread = 0;
};
struct SettingsData
{
	int theme = 0;
	int key = 0;
	int recKey = 0;
	bool recTrack = false;
	bool defOn = false;
	bool showTips = false;
	bool audFx = false;
	bool minMode = false;
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
namespace Qi
{
	// for setStyle
	extern QApplication* application;
	extern QiUi::QuickInputUi ui;
	// state
	extern bool state;
	extern bool run;
	// record
	extern bool recordState;
	extern bool recording;
	extern clock_t recordClock;
	extern HWND recordWindow;
	// macro
	extern Actions record;
	extern Actions clipboard;
	extern Macros macros;
	// data
	extern FuncData fun;
	extern SettingsData set;
	extern Widget widget;
	extern QPopText* popText;
	extern QWindowSelection* windowSelection;
	// input
	extern bool keyState[keySize];
	extern QList<byte> blockKeys;
	extern XBoxPad xboxpad;
	// path
	extern const QString dir;
	extern const QString folder;
	extern const QString macroDir;
	extern const QString macroType;
	extern const QString configFile;
	// other
	extern SIZE screen;
	extern MSG msg;
}