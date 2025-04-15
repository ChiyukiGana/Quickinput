#pragma once
#include "inc_project.h"

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
		QString syEntry = "🔜";
		QString syPause = "⏸️";
		QString syExit = "🔙";
		QString syAny = "🟡";
		QString syOn = "✅";
		QString syOff = "⛔";
		QString syOk = "⭕";
		QString syYes = "✔️";
		QString syNot = "❌";
		QString syStop = "🛑";
		QString syShow = "🔼";
		QString syHide = "🔽";
		QString syOption = "⚙";
		QString syLink = "🔗";
		QString syEdit = "🔧";
		QString syUp = "⬆️";
		QString syDown = "⬇️";
		QString syTurn = "🔃";
		QString syLeft = "🔙";
		QString syTop = "🔝";
		QString syRight = "🔜";
		QString syMove = "🔛";
		QString syTime = "⏳";
		QString syText = "🅰️";
		QString syLoop = "♾️";
		QString syColor = "🌈";
		QString syImage = "🖼";
		QString syPoint = "🪂";
		QString syJump = "🛩";
		QString syBlock = "🪂";
		QString syBlockExec = "🛩";
		QString syEqual = "🟰";
		QString syVar = "💠";
		QString syTrack = "➰";
		QString sySpeaker = "🔈️";
		// menu
		QString muOn = QString("启用") + syOn;
		QString muOff = QString("禁用") + syOff;
		QString muShow = QString("显示") + syShow;
		QString muHide = QString("隐藏") + syHide;
		QString muExit = QString("退出") + syStop;
		// action
		QString acDown = QString("按下") + syDown;
		QString acUp = QString("松开") + syUp;
		QString acClick = QString("点击") + syTurn;
		QString acPos = QString("位置") + syLeft;
		QString acMove = QString("移动") + syMove;
		QString acWait = QString("等待") + syTime;
		QString acCopyText = QString("复制") + syText;
		QString acLoop = QString("循环") + syLoop;
		QString acColor = QString("找色") + syColor;
		QString acEnd = QString("结束") + syStop;
		QString acEndLoop = QString("结束循环") + syStop;
		QString acKeyState = QString("按键状态") + syStop;
		QString acResetPos = QString("恢复位置") + syTurn;
		QString acImage = QString("找图") + syImage;
		QString acPopText = QString("弹出") + syText;
		QString acSavePos = QString("记录位置") + syTurn;
		QString acTimer = QString("定时") + syLoop;
		QString acJump = QString("跳转") + syJump;
		QString acJumpPoint = QString("锚点") + syPoint;
		QString acDialog = QString("对话框") + syText;
		QString acBlock = QString("块") + syBlock;
		QString acBlockExec = QString("执行") + syBlockExec;
		QString acQuickInput = QString("输入字符") + syText;
		QString acKeyBlock = QString("屏蔽按键") + syStop;
		QString acClock = QString("时钟") + syTime;
		QString acOcr = QString("文字识别") + syText;
		QString acVarOperator = QString("变量运算") + syEqual;
		QString acVarCondition = QString("变量判断") + syVar;
		QString acMouseTrack = QString("鼠标轨迹") + syTrack;
		QString acOpen = QString("打开") + syLink;
		QString acTextPad = QString("文本") + syText;
		QString acEditDialog = QString("编辑框") + syText;
		QString acVolume = QString("音量检测") + sySpeaker;
		// state
		QString trOn = QString("启用") + syOn;
		QString trOff = QString("禁用") + syOff;
		// record
		QString rcStart = QString("开始") + syOk;
		QString rcStop = QString("停止") + syOk;
		QString rcClose = QString("取消") + syNot;
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

class QiInterpreter;
class MacroGroup;
class Action;

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
		id = Qt::UserRole,
		group,
		macro
	};
};
struct RecordEvent
{
	enum
	{
		start = QEvent::User + 1,
		stop,
		close,
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
struct VarViewEvent
{
	enum
	{
		close = QEvent::User + 1,
		reload
	};
};
struct MacroEvent
{
	enum
	{
		edit = QEvent::User,
		edited,
		load
	};
};
////////////////// Window
struct WndInput
{
	HWND wnd = nullptr; // parent
	QList<HWND> children;
	bool child = false;
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
				int minArea = (int)((~(0x8000000)) - 1);
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
		varCondition,
		mouseTrack,
		open,
		textPad,
		editDialog,
		volume
	};
};
using Actions = QiVector<Action>;
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
	void fromBase64(const QString& base64, size_t width, size_t height)
	{
		if (width && height && base64.size())
		{
			QByteArray data = QByteArray::fromBase64(base64.toUtf8());
			map.create(width, height);
			memcpy_s(map.data(), map.bytes(), data.data(), data.size());
		}
	}
	QString toBase64() const
	{
		QByteArray data((const char*)map.data(), map.bytes());
		return data.toBase64();
	}
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
class QiOcr : public QiBase
{
public:
	bool match = false;
	bool row = false;
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
class QiMouseTrack : public QiBase
{
public:
	struct MovePart
	{
		int x;
		int y;
		clock_t t;
	};
	clock_t t;
	std::vector<MovePart> s;
	QiMouseTrack() : QiBase(QiType::mouseTrack), t(clock()) {}
	void append(int x, int y)
	{
		MovePart t;
		t.x = x;
		t.y = y;
		t.t = clock() - this->t;
		s.push_back(t);
	}
	void append(const POINT& pt)
	{
		append(pt.x, pt.y);
	}
	void reset()
	{
		t = clock();
		s.clear();
	}
	void fromBase64(const QString& base64, size_t size)
	{
		if (size && base64.size())
		{
			QByteArray data = QByteArray::fromBase64(base64.toUtf8());
			s = std::vector<MovePart>((MovePart*)data.data(), (MovePart*)(data.data()) + size);
		}
	}
	QString toBase64() const
	{
		QByteArray data((const char*)s.data(), s.size() * sizeof(MovePart));
		return data.toBase64();
	}
};
class QiOpen : public QiBase
{
public:
	QString url;
	QiOpen() : QiBase(QiType::open) {}
};
class QiTextPad : public QiBase
{
public:
	QString text;
	QiTextPad() : QiBase(QiType::textPad) {}
};
class QiEditDialog : public QiBase
{
public:
	bool mult = false;
	QString title;
	QString text;
	QString var;
	QiEditDialog() : QiBase(QiType::editDialog) {}
};
class QiVolume : public QiBase
{
public:
	bool max = false;
	int time = 10;
	float volume = 0.0f;
	QiVolume() : QiBase(QiType::volume) {}
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
	QiVarCondition,
	QiMouseTrack,
	QiOpen,
	QiTextPad,
	QiEditDialog,
	QiVolume
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
class Macro
{
public:
	enum { sw, down, up };
	bool state = false; // enable | disable
	bool keyBlock = false; // block this trigger key
	bool curBlock = false; // block cursor move
	bool wndState = false; // window mode enable | disable
	bool active = false; // state of release trigger
	bool timer = false;
	int key = 0;
	int mode = 0;
	int count = 0;
	time_t timerStart = 0;
	time_t timerEnd = 0;
	float speed = 1.0f;
	float moveScaleX = 1.0f;
	float moveScaleY = 1.0f;
	float posScaleX = 0.0f;
	float posScaleY = 0.0f;
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
		if (base) return Qi::macroDir;
		return Qi::macroDir + name + QString("/");
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
struct FuncData
{
	struct QuickClick
	{
		bool state = 0;
		int mode = 0;
		int key = 0;
		int delay = 10;
		HANDLE thread = 0;
	} quickClick;
	struct ShowClock
	{
		bool state = false;
		int key = 0;
	} showClock;
	struct WndActive
	{
		bool state = false;
		WndInfo wndInfo;
		HANDLE thread = 0;
	} wndActive;
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
struct FoldData
{
	std::map<QString, bool> group;
};
struct Widget
{
	bool dialogActive = false;
	bool mainActive = false;
	bool moreActive = false;
	QWidget* main = nullptr;
	QWidget* macro = nullptr;
	QWidget* record = nullptr;
	QWidget* edit = nullptr;
	QWidget* varView = nullptr;
	bool active() const
	{
		return !(mainActive || dialogActive || moreActive);
	}
	void recordStart() const
	{
		QApplication::postEvent(record, new QEvent((QEvent::Type)RecordEvent::start));
	}
	void recordStop() const
	{
		QApplication::postEvent(record, new QEvent((QEvent::Type)RecordEvent::stop));
	}
	void recordClose() const
	{
		QApplication::postEvent(record, new QEvent((QEvent::Type)RecordEvent::close));
	}
	void macroLoad() const
	{
		QApplication::postEvent(macro, new QEvent((QEvent::Type)MacroEvent::load));
	}
	void macroEdit() const
	{
		QApplication::postEvent(macro, new QEvent((QEvent::Type)MacroEvent::edit));
	}
	void macroEdited() const
	{
		QApplication::postEvent(macro, new QEvent((QEvent::Type)MacroEvent::edited));
	}
	void editClose() const
	{
		QApplication::postEvent(edit, new QEvent((QEvent::Type)EditEvent::close));
	}
	void editDebugPause() const
	{
		QApplication::postEvent(edit, new QEvent((QEvent::Type)EditEvent::debug_pause));
	}
	void varViewReload() const
	{
		if (!varView->isHidden()) QApplication::postEvent(varView, new QEvent((QEvent::Type)VarViewEvent::reload));
	}
};

namespace Qi
{
	inline QString version;
	inline QiOcrInterface* ocr = nullptr;
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
	inline FoldData fold;
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
	constexpr time_t macro_timer_min = 0;
	constexpr time_t macro_timer_max = 60 * 60 * 24;
	constexpr float macro_speed_min = 0.1f;
	constexpr float macro_speed_max = 10.0f;
	constexpr float macro_moveScale_min = 0.1f;
	constexpr float macro_moveScale_max = 10.0f;
	constexpr float macro_posScale_min = -1.0f;
	constexpr float macro_posScale_max = 1.0f;
	inline int Restricted(int param, int max, int min = 0)
	{
		if (param > max) param = max;
		else if (param < min) param = min;
		return param;
	}
	inline time_t Restricted(time_t param, time_t max, time_t min = 0)
	{
		if (param > max) param = max;
		else if (param < min) param = min;
		return param;
	}
	inline float Restricted(float param, float max, float min = 0.0f)
	{
		if (param > max) param = max;
		else if (param < min) param = min;
		return param;
	}
}