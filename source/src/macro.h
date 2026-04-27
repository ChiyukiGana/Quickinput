#pragma once
#include "action.h"
#include "thread.h"
#include "scriptinterpreter.h"
#include "inc_project.h"

struct WndMatch
{
	HWND wnd = nullptr;
	DWORD pid = 0;
	QString name;
	QString clas;
	QString proc;
	void update(HWND hwnd = nullptr)
	{
		IsWindow(hwnd) ? wnd = hwnd : wnd = GetForegroundWindow();
		name = Window::text(wnd);
		clas = Window::className(wnd);
		GetWindowThreadProcessId(wnd, &pid);
		proc = QString::fromStdWString(Process::path(pid));
	}
	bool match(const QString& name_pattern, const QString& clas_pattern = {}, const QString& proc_pattern = {})
	{
		return String::match(name.toStdWString(), name_pattern.toStdWString()) && String::match(clas.toStdWString(), clas_pattern.toStdWString()) && String::match(proc.toStdWString(), proc_pattern.toStdWString());
	}
	static HWND find(const QString& name_pattern, const QString& clas_pattern = {}, const QString& proc_pattern = {}, const HWND exclude = nullptr)
	{
		struct Param
		{
			Param(
				WndMatch& window_match,
				const QString& name_pattern,
				const QString& clas_pattern,
				const QString& proc_pattern,
				const HWND& exclude) :
				window_match(window_match),
				name_pattern(name_pattern),
				clas_pattern(clas_pattern),
				proc_pattern(proc_pattern),
				exclude(exclude) {
			}
			WndMatch& window_match;
			const QString& name_pattern;
			const QString& clas_pattern;
			const QString& proc_pattern;
			const HWND& exclude;
		};
		WndMatch wm;
		Param p(wm, name_pattern, clas_pattern, proc_pattern, exclude);
		if (FALSE == EnumWindows([](HWND wnd, LPARAM lp) {
			auto p = reinterpret_cast<Param*>(lp);
			if (wnd != p->exclude)
			{
				p->window_match.update(wnd);
				return p->window_match.match(p->name_pattern, p->clas_pattern, p->proc_pattern) ? FALSE : TRUE;
			}
			return TRUE;
			}, reinterpret_cast<LPARAM>(&p))) return p.window_match.wnd;
		return nullptr;
	}
	static QiVector<HWND> finds(const QString& name_pattern, const QString& clas_pattern = {}, const QString& proc_pattern = {}, const HWND exclude = nullptr)
	{
		struct Param
		{
			Param(
				WndMatch& window_match,
				const QString& name_pattern,
				const QString& clas_pattern,
				const QString& proc_pattern,
				const HWND& exclude,
				QiVector<HWND>& wnds) :
				window_match(window_match),
				name_pattern(name_pattern),
				clas_pattern(clas_pattern),
				proc_pattern(proc_pattern),
				exclude(exclude),
				wnds(wnds) {
			}
			WndMatch& window_match;
			const QString& name_pattern;
			const QString& clas_pattern;
			const QString& proc_pattern;
			const HWND& exclude;
			QiVector<HWND>& wnds;
		};
		WndMatch wm;
		QiVector<HWND> wnds;
		Param p(wm, name_pattern, clas_pattern, proc_pattern, exclude, wnds);
		EnumWindows([](HWND wnd, LPARAM lp) {
			auto p = reinterpret_cast<Param*>(lp);
			if (wnd != p->exclude)
			{
				p->window_match.update(wnd);
				if (p->window_match.match(p->name_pattern, p->clas_pattern, p->proc_pattern)) p->wnds.append(wnd);
			}
			return TRUE;
			}, reinterpret_cast<LPARAM>(&p));
		return wnds;
	}
};
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
	QString name;
	QString clas;
	QString proc;
	bool update_fromName()
	{
		if (!IsWindow(wnd))
		{
			return wnd = WndMatch::find(name, clas, proc);
		}
		return true;
	}
	bool update_fromHwnd()
	{
		if (IsWindow(wnd))
		{
			name = Window::text(wnd);
			clas = Window::className(wnd);
			DWORD pid;
			GetWindowThreadProcessId(wnd, &pid);
			proc = QString::fromStdWString(Process::path(pid));
			return true;
		}
		return false;
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
	enum TriggerMode { sw, down, up };
	enum StorageType { CURRENT = -1, JSON, QIM };

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
	bool matchState = false;
	bool timer = false;
	bool groupBase = false;
	short key1 = 0;
	short key2 = 0;
	int storageType = 0;
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
	WndInfo wndInfo; // window input data info
	WndInfo wndMatch; // window match for trigger
	WndInput wndInput; // window input status info, use for macro running
	QiInterpreter* interpreter;
	QiScriptInterpreter script_interpreter;
	QiMacroThread thread;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
	typepack::object toPack() const;
	void fromPack(const typepack::object& pack);
	QString fileType() const { return storageType == StorageType::QIM ? Qi::macroQimType : Qi::macroType; }
	QString makeFile() const
	{
		return name + fileType();
	}
	QString makeDir() const
	{
		if (groupBase) return Qi::macroDir;
		return Qi::macroDir + groupName + QString('/');
	}
	QString makePath() const
	{
		return makeDir() + makeFile();
	}
	bool createPath() const
	{
		if (!QDir(Qi::macroDir).exists() && !QDir(Qi::macroDir).mkdir(Qi::macroDir))
		{
			MsgBox::Error(L"创建宏目录失败");
			return false;
		}
		const QString dir = makeDir();
		if (!QDir(dir).exists() && !QDir(dir).mkdir(dir))
		{
			MsgBox::Error(L"创建分组目录失败");
			return false;
		}
		return true;
	}
	bool exist() const { return QFile::exists(makePath()); }
	bool remove() const { return QFile::moveToTrash(makePath()); }
#ifdef Q_ENCRYPT
	bool save(StorageType type = StorageType::CURRENT) const;
#else
	bool save(StorageType type = StorageType::CURRENT)
	{
		if (!createPath()) return false;
		remove();

		if (type != StorageType::CURRENT) storageType = static_cast<int>(type);

		if (storageType == Macro::StorageType::QIM)
		{
			typepack::binary data = toPack().toBinary();
			return File::FileSave(makePath().toStdWString(), data.data(), data.size());
		}
		QJsonDocument json(toJson());
		return File::SaveText(makePath(), json.toJson(QJsonDocument::JsonFormat::Compact));
	}
#endif
	bool saveTo(QString path, StorageType type = StorageType::CURRENT) const
	{
		if (type == StorageType::CURRENT) type = static_cast<StorageType>(storageType);

		if (type == Macro::StorageType::QIM)
		{
			typepack::binary data = toPack().toBinary();
			return File::FileSave(path.toStdWString(), data.data(), data.size());
		}
		QJsonDocument json(toJson());
		return File::SaveText(path, json.toJson(QJsonDocument::JsonFormat::Compact));
	}
	static void loadAll();
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
	QString makePath(const QString& macroName, Macro::StorageType type = Macro::StorageType::JSON) const
	{
		const QString file_type = type == Macro::StorageType::QIM ? Qi::macroQimType : Qi::macroType;
		if (base) return Qi::macroDir + makeName(macroName) + file_type;
		return Qi::macroDir + name + QString("/") + makeName(macroName) + file_type;
	}
	QString makePath() const
	{
		if (base) return Qi::macroDir;
		return Qi::macroDir + name + QString("/");
	}
	bool createPath() const
	{
		QString path = Qi::macroDir;
		if (!QDir(path).exists() && !QDir(path).mkdir(path))
		{
			MsgBox::Error(L"创建宏目录失败");
			return false;
		}
		if (!base)
		{
			path += name + QString("/");
			if (!QDir(path).exists() && !QDir(path).mkdir(path))
			{
				MsgBox::Error(L"创建分组目录失败");
				return false;
			}
		}
		return true;
	}
	bool remove() const { return QFile::moveToTrash(makePath(), nullptr); }
	bool moveIn(MacroGroup& other_group, Macro& other_macro)
	{
		if (&other_group == this) return true;

		const QString new_name = makeName(other_macro.name);
		const QString old_path = other_macro.makePath();
		const QString new_path = makePath(new_name, static_cast<Macro::StorageType>(other_macro.storageType));

		if (!QFile::rename(old_path, new_path)) return false;
		other_macro.name = new_name;
		other_macro.groupBase = base;
		other_macro.groupName = name;
		macros.append(std::move(other_macro));
		other_group.macros.remove_of_find(&other_macro);
		return true;
	}
	void moveIn(MacroGroup& other_group, const QiVectorIndex& other_index)
	{
		if (&other_group == this) return;
		QiVectorIndex moved;
		for (const auto& i : other_index)
		{
			Macros& other_macros = other_group.macros;
			if (i >= other_macros.size()) continue;
			Macro& other_macro = other_macros[i];

			const QString new_name = makeName(other_macro.name);
			const QString old_path = other_macro.makePath();
			const QString new_path = makePath(new_name, static_cast<Macro::StorageType>(other_macro.storageType));

			if (!QFile::rename(old_path, new_path)) continue;
			other_macro.name = new_name;
			other_macro.groupBase = base;
			other_macro.groupName = name;
			moved.push_back(i);
			macros.append(std::move(other_macro));
		}
		other_group.macros.remove(moved);
		return;
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