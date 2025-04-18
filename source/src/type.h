#pragma once
#include "inc_project.h"
#include "scriptinterpreter.h"
#include "ui.h"
#include "range.h"

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
		volume,
		soundPlay,
		count
	};
};
using Actions = QiVector<Action>;
struct QiBase
{
	int type;
	bool disable = false;
	bool debug_entry = false;
	bool debug_break = false;
	bool debug_exit = false;
	QString mark = QString();
	Actions next = Actions();
	Actions next2 = Actions();
	QiBase(int type = QiType::none) noexcept : type(type) {}
};
struct QiEnd : QiBase
{
	QiEnd() : QiBase(QiType::end) {}
};
struct QiDelay : QiBase
{
	static constexpr QiIntRange range_time = { 0, (~unsigned int(0)) >> 1 };

	int min = 0, max = 0;
	QiDelay() : QiBase(QiType::delay) {}
};
struct QiKey : QiBase
{
	enum { up, down, click };
	int vk = 0, state = down;
	QiKey() : QiBase(QiType::key) {}
};
struct QiMouse : QiBase
{
	static constexpr QiIntRange range_move = { -10000, 10000 };
	static constexpr QiIntRange range_pos = { 0, 10000 };
	static constexpr QiIntRange range_rand = { 0, 10000 };
	static constexpr QiIntRange range_speed = { 0, 99 };

	float speed = 0;
	int x = 0, y = 0, ex = 0;
	bool move = false, track = false;
	QiMouse() : QiBase(QiType::mouse) {}
};
struct QiCopyText : QiBase
{
	QString text;
	QiCopyText() : QiBase(QiType::copyText) {}
};
struct QiColor : QiBase
{
	static constexpr QiIntRange range_rect = { 0, 10000 };
	static constexpr QiIntRange range_rgb = { 0, 255 };

	Rgba rgbe;
	RECT rect = {};
	bool move = false;
	QiColor() : QiBase(QiType::color) {}
};
struct QiLoop : QiBase
{
	static constexpr QiIntRange range_count = { 0, (~unsigned int(0)) >> 1 };

	int min = 0, max = 0;
	QiLoop() : QiBase(QiType::loop) {}
};
struct QiLoopEnd : QiBase
{
	QiLoopEnd() : QiBase(QiType::loopEnd) {}
};
struct QiKeyState : QiBase
{
	int vk = 0;
	QiKeyState() : QiBase(QiType::keyState) {}
};
struct QiResetPos : QiBase
{
	QiResetPos() : QiBase(QiType::resetPos) {}
};
struct QiImage : QiBase
{
	static constexpr QiIntRange range_rect = { 0, 10000 };
	static constexpr QiIntRange range_sim = { 0, 99 };

	RgbMap map;
	int sim = 0;
	RECT rect = {};
	bool move = false;
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
struct QiPopText : QiBase
{
	static constexpr QiIntRange range_time = { 0, (~unsigned int(0)) >> 1 };

	QString text;
	int time = 0;
	bool sync = false;
	QiPopText() : QiBase(QiType::popText) {}
};
struct QiSavePos : QiBase
{
	QiSavePos() : QiBase(QiType::savePos) {}
};
struct QiTimer : QiBase
{
	static constexpr QiIntRange range_count = { 0, (~unsigned int(0)) >> 1 };

	int min = 0, max = 0;
	QiTimer() : QiBase(QiType::timer) {}
};
struct QiJump : QiBase
{
	int id = 0;
	QiJump() : QiBase(QiType::jump) {}
};
struct QiJumpPoint : QiBase
{
	int id = 0;
	QiJumpPoint() : QiBase(QiType::jumpPoint) {}
};
struct QiDialog : QiBase
{
	enum Style
	{
		none,
		warning,
		error
	};
	static constexpr QiIntRange range_style = { Style::none, Style::error };

	int style = Style::none;
	QString title;
	QString text;
	QiDialog() : QiBase(QiType::dialog) {}
};
struct QiBlock : QiBase
{
	int id = 0;
	QiBlock() : QiBase(QiType::block) {}
};
struct QiBlockExec : QiBase
{
	int id = 0;
	QiBlockExec() : QiBase(QiType::blockExec) {}
};
struct QiQuickInput : QiBase
{
	QiVector<unsigned char> chars;
	QiQuickInput() : QiBase(QiType::quickInput) {}
};
struct QiKeyBlock : QiBase
{
	int vk = 0;
	bool block = false;
	QiKeyBlock() : QiBase(QiType::keyBlock) {}
};
struct QiClock : QiBase
{
	static constexpr QiIntRange range_time = { 0, 60 * 60 * 24 };

	int time = 0;
	QiClock() : QiBase(QiType::clock) {}
};
struct QiOcr : QiBase
{
	static constexpr QiIntRange range_rect = { 0, 10000 };

	bool match = false;
	bool row = false;
	RECT rect = {};
	QString text;
	QString var;
	QiOcr() : QiBase(QiType::ocr) {}
};
struct QiVarOperator : QiBase
{
	QString script;
	QiVarOperator() : QiBase(QiType::varOperator) {}
};
struct QiVarCondition : QiBase
{
	QString script;
	QiVarCondition() : QiBase(QiType::varCondition) {}
};
struct QiMouseTrack : QiBase
{
	struct MovePart
	{
		int x;
		int y;
		clock_t t;
	};
	clock_t t = 0;
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
struct QiOpen : QiBase
{
	QString url;
	QiOpen() : QiBase(QiType::open) {}
};
struct QiTextPad : QiBase
{
	QString text;
	QiTextPad() : QiBase(QiType::textPad) {}
};
struct QiEditDialog : QiBase
{
	bool mult = false;
	QString title;
	QString text;
	QString var;
	QiEditDialog() : QiBase(QiType::editDialog) {}
};
struct QiVolume : QiBase
{
	static constexpr QiIntRange range_time = { 10, 10000 };
	static constexpr QiFloatRange range_volume = { 0.0f, 1.0f };

	bool max = false;
	int time = 10;
	float volume = 0.0f;
	QiVolume() : QiBase(QiType::volume) {}
};
struct QiSoundPlay : QiBase
{
	bool sync = false, stop = false;
	QString file;
	QiSoundPlay() : QiBase(QiType::soundPlay) {}
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
	QiVolume,
	QiSoundPlay
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
struct Macro
{
	enum { sw, down, up };

	static constexpr QiIntRange range_count = { 0, 9999 };
	static constexpr QiIntRange range_mode = { sw, up };
	static constexpr QiIntRange range_timer = { 0, 60 * 60 * 24 };
	static constexpr QiFloatRange range_speed = { 0.1f, 10.0f };
	static constexpr QiFloatRange range_moveScale = { 0.1f, 10.0f };
	static constexpr QiFloatRange range_posScale = { -1.0f, 1.0f };

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
	// other
	inline SIZE screen = {};
	inline MSG msg;
	inline bool debug = false;
}