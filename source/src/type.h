#pragma once
#include "inc_project.h"
constexpr int key_info = 214;
constexpr int key_size = XBoxPad::key_end;
constexpr int msg_exit = (WM_USER + 0xFF);
namespace Qi
{
	// path
	extern const QString dir;
	extern const QString folder;
	extern const QString macroDir;
	extern const QString macroType;
	extern const QString configFile;
}
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
		QString syEntry;
		QString syPause;
		QString syExit;
		QString syAny;
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
		QString syEqual;
		QString syVar;
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
		QString acResetPos;
		QString acImage;
		QString acPopText;
		QString acSavePos;
		QString acTimer;
		QString acJump;
		QString acJumpPoint;
		QString acDialog;
		QString acBlock;
		QString acBlockExec;
		QString acQuickInput;
		QString acKeyBlock;
		QString acClock;
		QString acOcr;
		QString acVarOperator;
		QString acVarCondition;
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
struct EditEvent
{
	enum
	{
		close = QEvent::User + 1,
		debug_pause
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
	inline static HANDLE thread;
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
		resetPos,
		image,
		popText,
		savePos,
		timer,
		jump,
		jumpPoint,
		dialog,
		block,
		blockExec,
		quickInput,
		keyBlock,
		clock,
		ocr,
		varOperator,
		varCondition
	};
};
using Actions = QiVector<class Action>;
class QiBase
{
public:
	bool disable;
	bool debug_entry;
	bool debug_break;
	bool debug_exit;
	int type;
	QString mark;
	Actions next;
	Actions next2;
	QiBase(int qiType = QiType::none) noexcept : disable(false), debug_entry(false), debug_break(false), debug_exit(false), type(qiType), mark(QString()), next(Actions()), next2(Actions()) {}
};
class QiEnd : public QiBase
{
public:
	QiEnd() : QiBase(QiType::end) {}
};
class QiDelay : public QiBase
{
public:
	int min; int max;
	QiDelay() : QiBase(QiType::delay) {}
};
class QiKey : public QiBase
{
public:
	enum { up, down, click };
	int vk = 0; int state = down;
	QiKey() : QiBase(QiType::key) {}
};
class QiMouse : public QiBase
{
public:
	int x = 0; int y = 0; int ex = 0; int speed = 0; bool move = false; bool track = false;
	QiMouse() : QiBase(QiType::mouse) {}
};
class QiCopyText : public QiBase
{
public:
	QString text;
	QiCopyText() : QiBase(QiType::copyText) {}
};
class QiColor : public QiBase
{
public:
	Rgba rgbe = 0; RECT rect = {}; bool move = false;
	QiColor() : QiBase(QiType::color) {}
};
class QiLoop : public QiBase
{
public:
	int min = 0; int max = 0;
	QiLoop() : QiBase(QiType::loop) {}
};
class QiLoopEnd : public QiBase
{
public:
	QiLoopEnd() : QiBase(QiType::loopEnd) {}
};
class QiKeyState : public QiBase
{
public:
	int vk = 0;
	QiKeyState() : QiBase(QiType::keyState) {}
};
class QiResetPos : public QiBase
{
public:
	QiResetPos() : QiBase(QiType::resetPos) {}
};
class QiImage : public QiBase
{
public:
	RgbMap map; int sim; RECT rect = {}; bool move = false;
	QiImage() : QiBase(QiType::image) {}
};
class QiPopText : public QiBase
{
public:
	QString text; int time = 0; bool sync = false;
	QiPopText() : QiBase(QiType::popText) {}
};
class QiSavePos : public QiBase
{
public:
	QiSavePos() : QiBase(QiType::savePos) {}
};
class QiTimer : public QiBase
{
public:
	int min = 0; int max = 0;
	QiTimer() : QiBase(QiType::timer) {}
};
class QiJump : public QiBase
{
public:
	int id = 0;
	QiJump() : QiBase(QiType::jump) {}
};
class QiJumpPoint : public QiBase
{
public:
	int id = 0;
	QiJumpPoint() : QiBase(QiType::jumpPoint) {}
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
};
class QiBlock : public QiBase
{
public:
	int id;
	QiBlock() : QiBase(QiType::block) {}
};
class QiBlockExec : public QiBase
{
public:
	int id;
	QiBlockExec() : QiBase(QiType::blockExec) {}
};
class QiQuickInput : public QiBase
{
public:
	QiVector<unsigned char> chars;
	QiQuickInput() : QiBase(QiType::quickInput) {}
};
class QiKeyBlock : public QiBase
{
public:
	int vk = 0;
	bool block = false;
	QiKeyBlock() : QiBase(QiType::keyBlock) {}
};
class QiClock : public QiBase
{
public:
	int time = 0;
	QiClock() : QiBase(QiType::clock) {}
};
class QiOcr: public QiBase
{
public:
	bool match = false;
	RECT rect = {};
	QString text;
	QString var;
	QiOcr() : QiBase(QiType::ocr) {}
};
class QiVarOperator : public QiBase
{
public:
	QString script;
	QiVarOperator() : QiBase(QiType::varOperator) {}
};
class QiVarCondition : public QiBase
{
public:
	QString script;
	QiVarCondition() : QiBase(QiType::varCondition) {}
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
	QiResetPos,
	QiImage,
	QiPopText,
	QiSavePos,
	QiTimer,
	QiJump,
	QiJumpPoint,
	QiDialog,
	QiBlock,
	QiBlockExec,
	QiQuickInput,
	QiKeyBlock,
	QiClock,
	QiOcr,
	QiVarOperator,
	QiVarCondition
> ;
class Action : public ActionVariant
{
public:
	using ActionVariant::ActionVariant;
	Action() : ActionVariant(QiBase()) {}
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
////////////////// Macro
class QiInterpreter;
class MacroGroup;
class Macro
{
public:
	enum { sw, down, up };
	bool state = false; // enable | disable
	bool keyBlock = false; // block this trigger key
	bool curBlock = false; // block cursor move
	bool wndState = false; // window mode enable | disable
	bool active = false; // state of release trigger
	int key = 0;
	int mode = 0;
	int count = 0;
	float speed = 1.0f;
	float moveScaleX = 1.0f;
	float moveScaleY = 1.0f;
	QString name;
	QString groupName;
	bool groupBase = false;
	Actions acRun;
	Actions acEnd;
	POINT cursor = {};
	WndInfo wndInfo;
	WndInput wndInput;
	HANDLE thRun = nullptr;
	HANDLE thEnd = nullptr;
	QiInterpreter* interpreter;
	QiVarMap varMap;
	QString makePath() const
	{
		if (groupBase) return Qi::macroDir + name + Qi::macroType;
		return Qi::macroDir + groupName + QString('/') + name + Qi::macroType;
	}
};
using Macros = QiVector<Macro>;
class MacroGroup
{
public:
	bool base;
	QString name;
	Macros macros;
	MacroGroup(bool base, const QString name)
	{
		this->base = base;
		this->name = name;
	}
	MacroGroup(const MacroGroup&) = delete;
	void operator=(const MacroGroup&) = delete;
	MacroGroup(MacroGroup&& right) noexcept { operator=(std::move(right)); }
	void operator=(MacroGroup&& right) noexcept { base = right.base; macros = std::move(right.macros); name = std::move(right.name); }
	void reName(const QString& newName)
	{
		name = newName;
		for (auto& i : macros) i.groupName = name;
	}
	QString makeName(const QString& macroName) const
	{
		int i = 0;
		QString alloc = macroName;
		bool not_unique = true;
		while (true)
		{
			for (const auto& macro : macros)
			{
				if (macro.name == alloc)
				{
					not_unique = false;
					i++;
					alloc = macroName + QString::number(i);
				}
			}
			if (not_unique) break;
			not_unique = true;
		}
		return alloc;
	}
	QString makePath(const QString& macroName) const
	{
		if (base) return Qi::macroDir + makeName(macroName) + Qi::macroType;
		return Qi::macroDir + name + QString("/") + makeName(macroName) + Qi::macroType;
	}
	QString makePath() const
	{
		return Qi::macroDir + name;
	}
};
class MacroGroups : public QiVector<MacroGroup>
{
	using Base = QiVector<MacroGroup>;
public: 
	using Base::Base;
	QString makeName(const QString& groupName = "组")
	{
		int i = 0;
		QString alloc = groupName;
		bool not_unique = true;
		while (true)
		{
			for (size_t i = 0; i < Base::size(); i++)
			{
				MacroGroup& group = Base::at(i);
				if (group.name == alloc)
				{
					not_unique = false;
					i++;
					alloc = groupName + QString::number(i);
				}
			}
			if (not_unique) break;
			not_unique = true;
		}
		return alloc;
	}
};
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
	WndInfo wndInfo;
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
	// eidt
	bool tabLock = false;
	bool tabHideTip = false;
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
	QWidget* edit = nullptr;
};

namespace Qi
{
	inline QiOnnxOcr* ocr = nullptr;
	inline QiScriptInterpreter interpreter;
	// for setStyle
	inline QApplication* application = nullptr;
	inline QiUi::QuickInputUi ui;
	// state
	inline bool state = false;
	inline bool run = false;
	// record
	inline bool recordState = false;
	inline bool recording = false;
	inline clock_t recordClock = 0;
	inline HWND recordWindow = 0;
	// macro
	inline Actions record;
	inline Actions clipboard;
	inline MacroGroups macroGroups;
	inline QList<Macro*> macroActive;
	// data
	inline FuncData fun;
	inline SettingsData set;
	inline Widget widget;
	inline QPopText* popText = nullptr;
	inline QWindowSelection* windowSelection = nullptr;
	// input
	inline bool keyState[key_size];
	inline bool keyBlock[key_size];
	inline int curBlock = 0;
	inline XBoxPad xboxpad;
	// dir
	inline const QString dir = QDir::fromNativeSeparators(QString::fromWCharArray(Process::runPath().c_str()));
	inline const QString folder = dir.mid(dir.lastIndexOf('/') + 1);
	inline const QString macroDir = dir + "/macro/";
	inline const QString macroType = ".json";
	inline const QString configFile = "QuickInput.json";
	// other
	inline SIZE screen = {};
	inline MSG msg;
	inline bool debug = false;
}

namespace QiRange
{
	constexpr int macro_count_max = 9999;
	constexpr int macro_mode_max = Macro::up;
	constexpr float macro_speed_min = 0.1f;
	constexpr float macro_speed_max = 10.0f;
	constexpr float macro_moveScale_min = 0.1f;
	constexpr float macro_moveScale_max = 10.0f;
	inline void Restricted(int& param, int max, int min = 0)
	{
		if (param > max) param = max;
		else if (param < min) param = min;
	}
	inline void Restricted(float& param, float max, float min = 0.0f)
	{
		if (param > max) param = max;
		else if (param < min) param = min;
	}
}