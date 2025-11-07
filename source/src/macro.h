#pragma once
#include "action.h"
#include "thread.h"
#include "scriptinterpreter.h"
#include "inc_project.h"

struct WndInput
{
	HWND wnd = nullptr; // parent
	bool child = false; // select child
	POINT pt = {}; // prev point
	WORD mk = 0; // prev key
	bool active() { return IsWindow(wnd); }
	HWND find(const POINT& pt, POINT& ptrel)
	{
		if (!active()) return nullptr;
		if (child)
		{
			QList<HWND> children = Window::FindChild(wnd);
			int minArea = INT_MAX;
			RECT minRect = {};
			HWND minWnd = nullptr;
			for (auto i = children.rbegin(); i != children.rend(); i++)
			{
				minRect = Window::childRect(wnd, *i);
				if (InRect(minRect, pt))
				{
					int area = RectArea(minRect);
					if (area < minArea)
					{
						minArea = area;
						minWnd = *i;
					}
				}
			}
			if (minWnd)
			{
				ptrel = InRectPos(minRect, pt);
				return minWnd;
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

class QiInterpreter;
struct Macro
{
	Macro() {};
	Macro(Macro&&) = default;
	Macro(const Macro&) = delete;
	Macro& operator=(Macro&&) = default;
	Macro& operator=(const Macro&) = delete;
	enum { sw, down, up };

	static constexpr QiIntRange range_count = { 0, 9999 };
	static constexpr QiIntRange range_mode = { sw, up };
	static constexpr QiIntRange range_timer = { 0, 60 * 60 * 24 };
	static constexpr QiFloatRange range_speed = { 0.1f, 10.0f };
	static constexpr QiFloatRange range_moveScale = { 0.1f, 10.0f };
	static constexpr QiFloatRange range_posScale = { -1.0f, 1.0f };

	bool state = false;
	bool keyBlock = false;
	bool curBlock = false;
	bool wndState = false;
	bool active = false; // release triggered flag
	bool timer = false;
	bool groupBase = false;
	short key1 = 0;
	short key2 = 0;
	int mode = 0;
	int count = 0;
	time_t timerStart = 0;
	time_t timerEnd = 0;
	float speed = 1.0f;
	float moveScaleX = 1.0f;
	float moveScaleY = 1.0f;
	float posScaleX = 0.0f;
	float posScaleY = 0.0f;
	POINT cursor = {};
	RECT range = { 0, 0, 10000, 10000 };
	QString script;
	QString name;
	QString groupName;
	Actions acRun;
	Actions acEnd;
	WndInfo wndInfo;
	WndInput wndInput;
	QiInterpreter* interpreter;
	QiScriptInterpreter script_interpreter;
	QiMacroThread thread;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
	QString makePath() const
	{
		if (groupBase) return Qi::macroDir + name + Qi::macroType;
		return Qi::macroDir + groupName + QString('/') + name + Qi::macroType;
	}
	Macro copy() const
	{
		Macro macro;
		macro.state = state;
		macro.keyBlock = keyBlock;
		macro.curBlock = curBlock;
		macro.wndState = wndState;
		macro.active = active;
		macro.timer = timer;
		macro.groupBase = groupBase;
		macro.key1 = key1;
		macro.key2 = key2;
		macro.mode = mode;
		macro.count = count;
		macro.timerStart = timerStart;
		macro.timerEnd = timerEnd;
		macro.speed = speed;
		macro.moveScaleX = moveScaleX;
		macro.moveScaleY = moveScaleY;
		macro.posScaleX = posScaleX;
		macro.posScaleY = posScaleY;
		macro.cursor = cursor;
		macro.range = range;
		macro.script = script;
		macro.name = name;
		macro.groupName = groupName;
		macro.acRun = acRun;
		macro.acEnd = acEnd;
		macro.wndInfo = wndInfo;
		macro.wndInput = wndInput;
		macro.interpreter = interpreter;
		macro.script_interpreter;
		macro.thread;
		return macro;
	}
};
using Macros = QiVector<Macro>;
using MacroPointers = QiVector<Macro*>;
struct MacroGroup
{
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
struct MacroGroups : public QiVector<MacroGroup>
{
	using Base = QiVector<MacroGroup>;
	using Base::Base;
	using Base::operator=;
	QString makeName(const QString& groupName = "组")
	{
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