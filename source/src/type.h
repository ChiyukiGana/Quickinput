#pragma once
#include "inc_project.h"
#include "scriptinterpreter.h"
#include "ui.h"
#include "range.h"
#include "thread.h"

struct RECTF { float left; float top; float right; float bottom; };

class QiInterpreter;
class MacroGroup;
class Action;

enum class InterpreterResult
{
	r_exit,
	r_continue,
	r_break,
	r_top
};
enum class DataRole
{
	id = Qt::UserRole,
	type,
	group,
	macro
};
enum class QiEvent
{
	wid_close = QEvent::User + 1,
	rec_start,
	rec_stop,
	rec_close,
	mac_edit_enter,
	mac_edit_exit,
	mac_edit_exit_d,
	mac_load,
	edt_debug_pause,
	edt_varop_stop,
	var_reload,
	key_reset
};

class QiIdList : public QiVector<int>
{
	using base_vector = QiVector<int>;
public:
	using base_vector::base_vector;
	bool exist(int id) const
	{
		for (int i : *this) if (i == id) return true;
		return false;
	}
	int unique(int id = 1) const
	{
		for (int i : *this) if (i >= id) id = i + 1;
		return id;
	}
};

struct MsgViewInfo
{
	enum
	{
		_msg,
		_war,
		_err
	};
	size_t level = _msg;
	time_t time = ::time(nullptr);
	QString text;
	QString macro;
};
using MsgViewInfos = QiVector<MsgViewInfo>;
class MsgViewEvent : public QEvent
{
	const MsgViewInfo _info;
public:
	enum Type
	{
		setText = QEvent::User + 1,
		newLine,
		clear,
		show,
		hide
	};
	MsgViewEvent(int type) : QEvent(static_cast<QEvent::Type>(type)) {}
	MsgViewEvent(int type, const MsgViewInfo& info) : QEvent(static_cast<QEvent::Type>(type)), _info(info) {}
	MsgViewInfo info() const { return _info; }
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
enum class QiType
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
	msgView,
	count
};

struct Actions : public QiVector<Action>
{
	using base_actions = QiVector<Action>;
	using base_actions::base_actions;
	using base_actions::operator=;
	QJsonArray toJson() const;
	void fromJson(const QJsonArray& json);
	Action* iter(std::function<bool(Action&)> callBack, QiType type = QiType::none);
	const Action* iter(std::function<bool(const Action&)> callBack, QiType type = QiType::none) const;
	Action* find(QiType type, int id);
	const Action* find(QiType type, int id) const;
	Actions loadType(QiType type) const;
	QiIdList loadId(QiType type) const;
};

using HistoryActions = QiHistoryVector<Action>;

struct QiBase
{
	QiType type;
	int id = -1;
	bool disable = false;
	bool debug_entry = false;
	bool debug_break = false;
	bool debug_exit = false;
	QString mark = QString();
	Actions next = Actions();
	Actions next2 = Actions();
	QiBase(QiType type = QiType::none) noexcept : type(type) {}
	virtual QString name() const { return "base"; }
	virtual QJsonObject toJson() const
	{
		QJsonObject json;
		json.insert("type", static_cast<int>(type));
		if (disable) json.insert("dis", disable);
		if (!mark.isEmpty()) json.insert("mark", mark);
		return json;
	}
	virtual void fromJson(const QJsonObject& json)
	{
		type = static_cast<QiType>(json.value("type").toInt());
		disable = json.value("dis").toBool();
		mark = json.value("mark").toString();
	}
};
struct QiEnd : QiBase
{
	QiEnd() : QiBase(QiType::end) {}
	QString name() const override { return "结束"; }
};
struct QiDelay : QiBase
{
	static constexpr QiIntRange range_time = { 0, (~unsigned int(0)) >> 1 };

	int min = 0, max = 0;
	QString v_min, v_max;
	QiDelay() : QiBase(QiType::delay) {}
	QString name() const override { return "等待"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		v_min.isEmpty() ? json.insert("ms", (int)min) : json.insert("v_min", (QString)v_min);
		v_max.isEmpty() ? json.insert("ex", (int)max) : json.insert("v_max", (QString)v_max);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		min = json.value("ms").toInt(), v_min = json.value("v_min").toString();
		max = json.value("ex").toInt(), v_max = json.value("v_max").toString();
	}
};
struct QiKey : QiBase
{
	enum { up, down, click };
	int vk = 0, state = down;
	QiKey() : QiBase(QiType::key) {}
	QString name() const override { return "按键"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("vk", (int)vk);
		json.insert("state", (int)state);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		vk = json.value("vk").toInt();
		state = json.value("state").toInt();
	}
};
struct QiMouse : QiBase
{
	static constexpr QiIntRange range_move = { -10000, 10000 };
	static constexpr QiIntRange range_pos = { 0, 10000 };
	static constexpr QiIntRange range_rand = { 0, 10000 };
	static constexpr QiIntRange range_speed = { 0, 99 };

	int x = 0, y = 0, ex = 0, speed = 0;
	bool move = false, track = false;
	QString v_x, v_y;
	QiMouse() : QiBase(QiType::mouse) {}
	QString name() const override { return "鼠标"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		v_x.isEmpty() ? json.insert("y", (int)y) : json.insert("v_x", (QString)v_x);
		v_y.isEmpty() ? json.insert("x", (int)x) : json.insert("v_y", (QString)v_y);
		if (ex) json.insert("ex", (int)ex);
		if (track) json.insert("trk", (bool)track), json.insert("spd", (int)speed);
		if (move) json.insert("move", (bool)move);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		x = json.value("x").toInt(), v_x = json.value("v_x").toString();
		y = json.value("y").toInt(), v_y = json.value("v_y").toString();
		ex = json.value("ex").toInt();
		speed = json.value("spd").toInt();
		track = json.value("trk").toBool();
		move = json.value("move").toBool();
	}
};
struct QiCopyText : QiBase
{
	QString text;
	QiCopyText() : QiBase(QiType::copyText) {}
	QString name() const override { return "复制"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("text", (QString)text);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		text = json.value("text").toString();
	}
};
struct QiColor : QiBase
{
	static constexpr QiIntRange range_rect = { 0, 10000 };
	static constexpr QiIntRange range_rgb = { 0, 255 };

	Rgba rgbe;
	RECT rect = {};
	bool move = false;
	QString v_left, v_top, v_right, v_bottom;
	QiColor() : QiBase(QiType::color) {}
	QString name() const override { return "找色"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("rgbe", (int)rgbe.toCOLORREF());
		v_left.isEmpty() ? json.insert("left", (int)rect.left) : json.insert("v_l", (QString)v_left);
		v_top.isEmpty() ? json.insert("top", (int)rect.top) : json.insert("v_t", (QString)v_top);
		v_right.isEmpty() ? json.insert("right", (int)rect.right) : json.insert("v_r", (QString)v_right);
		v_bottom.isEmpty() ? json.insert("bottom", (int)rect.bottom) : json.insert("v_b", (QString)v_bottom);
		if (move) json.insert("move", (bool)move);
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		rgbe = json.value("rgbe").toInt();
		rect.left = json.value("left").toInt(), v_left = json.value("v_l").toString();
		rect.top = json.value("top").toInt(), v_top = json.value("v_t").toString();
		rect.right = json.value("right").toInt(), v_right = json.value("v_r").toString();
		rect.bottom = json.value("bottom").toInt(), v_bottom = json.value("v_b").toString();
		move = json.value("move").toBool();
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
};
struct QiLoop : QiBase
{
	static constexpr QiIntRange range_count = { 0, (~unsigned int(0)) >> 1 };

	int min = 0, max = 0;
	QString v_min, v_max;
	QiLoop() : QiBase(QiType::loop) {}
	QString name() const override { return "循环"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		v_min.isEmpty() ? json.insert("count", (int)min) : json.insert("v_min", (QString)v_min);
		v_max.isEmpty() ? json.insert("rand", (int)max) : json.insert("v_max", (QString)v_max);
		if (next) json.insert("next", next.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		min = json.value("count").toInt(), v_min = json.value("v_min").toString();
		max = json.value("rand").toInt(), v_max = json.value("v_max").toString();
		next.fromJson(json.value("next").toArray());
	}
};
struct QiLoopEnd : QiBase
{
	QiLoopEnd() : QiBase(QiType::loopEnd) {}
	QString name() const override { return "结束循环"; }
};
struct QiKeyState : QiBase
{
	int vk = 0;
	QiKeyState() : QiBase(QiType::keyState) {}
	QString name() const override { return "按键状态"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("vk", (int)vk);
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		vk = json.value("vk").toInt();
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
};
struct QiResetPos : QiBase
{
	QiResetPos() : QiBase(QiType::resetPos) {}
	QString name() const override { return "恢复位置"; }
};
struct QiImage : QiBase
{
	static constexpr QiIntRange range_rect = { 0, 10000 };
	static constexpr QiIntRange range_sim = { 0, 99 };

	RgbMap map;
	int sim = 0;
	RECT rect = {};
	bool move = false;
	QString v_left, v_top, v_right, v_bottom;
	QiImage() : QiBase(QiType::image) {}
	QString name() const override { return "找图"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("sim", (int)sim);
		v_left.isEmpty() ? json.insert("left", (int)rect.left) : json.insert("v_l", (QString)v_left);
		v_top.isEmpty() ? json.insert("top", (int)rect.top) : json.insert("v_t", (QString)v_top);
		v_right.isEmpty() ? json.insert("right", (int)rect.right) : json.insert("v_r", (QString)v_right);
		v_bottom.isEmpty() ? json.insert("bottom", (int)rect.bottom) : json.insert("v_b", (QString)v_bottom);
		if (move) json.insert("move", (bool)move);
		if (map) json.insert("width", (int)map.width()), json.insert("height", (int)map.height()), json.insert("data", (QString)toBase64());
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		sim = json.value("sim").toInt();
		rect.left = json.value("left").toInt(), v_left = json.value("v_l").toString();
		rect.top = json.value("top").toInt(), v_top = json.value("v_t").toString();
		rect.right = json.value("right").toInt(), v_right = json.value("v_r").toString();
		rect.bottom = json.value("bottom").toInt(), v_bottom = json.value("v_b").toString();
		move = json.value("move").toBool();
		{
			int width = json.value("width").toInt();
			int height = json.value("height").toInt();
			QString data = json.value("data").toString();
			if (!data.isEmpty() && width && height) fromBase64(data, width, height);
		}
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
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
	QString name() const override { return "弹出"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("text", (QString)text);
		json.insert("time", (int)time);
		if (sync) json.insert("sync", (bool)sync);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		text = json.value("text").toString();
		time = json.value("time").toInt();
		sync = json.value("sync").toBool();
	}
};
struct QiSavePos : QiBase
{
	QiSavePos() : QiBase(QiType::savePos) {}
	QString name() const override { return "记录位置"; }
};
struct QiTimer : QiBase
{
	static constexpr QiIntRange range_time = { 0, (~unsigned int(0)) >> 1 };

	int min = 0, max = 0;
	QString v_min, v_max;
	QiTimer() : QiBase(QiType::timer) {}
	QString name() const override { return "定时"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		v_min.isEmpty() ? json.insert("count", (int)min) : json.insert("v_min", (QString)v_min);
		v_max.isEmpty() ? json.insert("rand", (int)max) : json.insert("v_max", (QString)v_max);
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		min = json.value("count").toInt(), v_min = json.value("v_min").toString();
		max = json.value("rand").toInt(), v_max = json.value("v_max").toString();
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
};
struct QiJump : QiBase
{
	QiJump() : QiBase(QiType::jump) {}
	QString name() const override { return "跳转"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("id", (int)id);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		id = json.value("id").toInt();
	}
};
struct QiJumpPoint : QiBase
{
	QiJumpPoint() : QiBase(QiType::jumpPoint) {}
	QString name() const override { return "锚点"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("id", (int)id);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		id = json.value("id").toInt();
	}
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
	QString name() const override { return "对话框"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("style", (int)style);
		json.insert("title", (QString)title);
		json.insert("text", (QString)text);
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		style = json.value("style").toInt();
		title = json.value("title").toString();
		text = json.value("text").toString();
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
};
struct QiBlock : QiBase
{
	QiBlock() : QiBase(QiType::block) {}
	QString name() const override { return "块"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("id", (int)id);
		if (next) json.insert("next", next.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		id = json.value("id").toInt();
		next.fromJson(json.value("next").toArray());
	}
};
struct QiBlockExec : QiBase
{
	QiBlockExec() : QiBase(QiType::blockExec) {}
	QString name() const override { return "执行"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("id", (int)id);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		id = json.value("id").toInt();
	}
};
struct QiQuickInput : QiBase
{
	QiVector<unsigned char> chars;
	QiQuickInput() : QiBase(QiType::quickInput) {}
	QString name() const override { return "输入字符"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		if (chars)
		{
			QJsonArray array;
			for (const auto& i : chars) array.append((int)i);
			json.insert("c", array);
		}
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		for (const auto& c : json.value("c").toArray()) chars.append(c.toInt());
	}
};
struct QiKeyBlock : QiBase
{
	int vk = 0;
	bool block = false;
	QiKeyBlock() : QiBase(QiType::keyBlock) {}
	QString name() const override { return "屏蔽按键"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("k", (int)vk);
		json.insert("b", (bool)block);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		vk = json.value("k").toInt();
		block = json.value("b").toBool();
	}
};
struct QiClock : QiBase
{
	static constexpr QiIntRange range_time = { 0, 60 * 60 * 24 };

	int time = 0;
	QiClock() : QiBase(QiType::clock) {}
	QString name() const override { return "时钟"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("t", (int)time);
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		time = json.value("t").toInt();
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
};
struct QiOcr : QiBase
{
	static constexpr QiIntRange range_rect = { 0, 10000 };

	bool match = false, row = false, move = false;
	RECT rect = {};
	QString text, var;
	QString v_left, v_top, v_right, v_bottom;
	QiOcr() : QiBase(QiType::ocr) {}
	QString name() const override { return "文字识别"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		v_left.isEmpty() ? json.insert("left", (int)rect.left) : json.insert("v_l", (QString)v_left);
		v_top.isEmpty() ? json.insert("top", (int)rect.top) : json.insert("v_t", (QString)v_top);
		v_right.isEmpty() ? json.insert("right", (int)rect.right) : json.insert("v_r", (QString)v_right);
		v_bottom.isEmpty() ? json.insert("bottom", (int)rect.bottom) : json.insert("v_b", (QString)v_bottom);
		if (!text.isEmpty()) json.insert("text", (QString)text);
		if (!var.isEmpty()) json.insert("var", (QString)var);
		if (match) json.insert("match", (bool)match);
		if (row) json.insert("row", (bool)row);
		if (move) json.insert("move", (bool)move);
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		rect.left = json.value("left").toInt(), v_left = json.value("v_l").toString();
		rect.top = json.value("top").toInt(), v_top = json.value("v_t").toString();
		rect.right = json.value("right").toInt(), v_right = json.value("v_r").toString();
		rect.bottom = json.value("bottom").toInt(), v_bottom = json.value("v_b").toString();
		text = json.value("text").toString();
		var = json.value("var").toString();
		match = json.value("match").toBool();
		row = json.value("row").toBool();
		move = json.value("move").toBool();
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
};
struct QiVarOperator : QiBase
{
	QString script;
	QiVarOperator() : QiBase(QiType::varOperator) {}
	QString name() const override { return "变量运算"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("t", (QString)script);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		script = json.value("t").toString();
	}
};
struct QiVarCondition : QiBase
{
	QString script;
	QiVarCondition() : QiBase(QiType::varCondition) {}
	QString name() const override { return "变量判断"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("t", (QString)script);
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		script = json.value("t").toString();
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
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
	QString name() const override { return "鼠标轨迹"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		if (s.size())
		{
			json.insert("size", (int)s.size());
			json.insert("data", (QString)toBase64());
		}
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		int size = json.value("size").toInt();
		QString data = json.value("data").toString();
		if (!data.isEmpty() && size) fromBase64(data, size);
	}
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
	QString name() const override { return "打开"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("url", (QString)url);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		url = json.value("url").toString();
	}
};
struct QiTextPad : QiBase
{
	QString text;
	QiTextPad() : QiBase(QiType::textPad) {}
	QString name() const override { return "文本"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("text", (QString)text);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		text = json.value("text").toString();
	}
};
struct QiEditDialog : QiBase
{
	bool mult = false;
	QString title;
	QString text;
	QString var;
	QiEditDialog() : QiBase(QiType::editDialog) {}
	QString name() const override { return "编辑框"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("title", (QString)title);
		json.insert("text", (QString)text);
		json.insert("var", (QString)var);
		if (mult) json.insert("mult", (bool)mult);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		title = json.value("title").toString();
		text = json.value("text").toString();
		var = json.value("var").toString();
		mult = json.value("mult").toBool();
	}
};
struct QiVolume : QiBase
{
	static constexpr QiIntRange range_time = { 10, 10000 };
	static constexpr QiFloatRange range_volume = { 0.0f, 1.0f };

	bool max = false;
	int time = 10;
	float volume = 0.0f;
	QiVolume() : QiBase(QiType::volume) {}
	QString name() const override { return "音量检测"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("volume", (double)volume);
		json.insert("time", (int)time);
		if (max) json.insert("max", (bool)max);
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		volume = json.value("volume").toDouble();
		time = json.value("time").toInt();
		max = json.value("max").toBool();
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
};
struct QiSoundPlay : QiBase
{
	enum { play, pause, resume, stop };
	bool sync = false;
	int state = play;
	QString file;
	QiSoundPlay() : QiBase(QiType::soundPlay) {}
	QString name() const override { return "播放音频"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("file", (QString)file);
		json.insert("state", (int)state);
		if (sync) json.insert("sync", (bool)sync);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		file = json.value("file").toString();
		state = json.value("state").toInt();
		sync = json.value("sync").toBool();
	}
};
struct QiMsgView : QiBase
{
	enum { set, add, clear, show, hide };
	int option;
	int level;
	QString text;
	QiMsgView() : QiBase(QiType::msgView) {}
	QString name() const override { return "消息窗口"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("opt", (int)option);
		json.insert("level", level);
		json.insert("text", (QString)text);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		option = json.value("opt").toInt();
		level = json.value("level").toInt();
		text = json.value("text").toString();
	}
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
	QiSoundPlay,
	QiMsgView
> ;
class Action : public ActionVariant
{
public:
	using ActionVariant::ActionVariant;
	Action() : ActionVariant(QiBase()) {}
	QiType type() const
	{
		return static_cast<QiType>(index());
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
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
	Action* iter(std::function<bool(Action&)> callBack, QiType type = QiType::none);
	const Action* iter(std::function<bool(const Action&)> callBack, QiType type = QiType::none) const;
	Action* find(QiType type, int id);
	const Action* find(QiType type, int id) const;
	Actions loadType(QiType type) const;
	QiIdList loadId(QiType type) const;
};

////////////////// Macro
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

	bool state = false; // enable | disable
	bool keyBlock = false; // block this trigger key
	bool curBlock = false; // block cursor move
	bool wndState = false; // window mode enable | disable
	bool active = false; // state of release trigger
	bool timer = false;
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
	QString script;
	QString name;
	QString groupName;
	bool groupBase = false;
	Actions acRun;
	Actions acEnd;
	POINT cursor = {};
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
		macro.script = script;
		macro.name = name;
		macro.groupName = groupName;
		macro.groupBase = groupBase;
		macro.acRun = acRun;
		macro.acEnd = acEnd;
		macro.cursor = cursor;
		macro.wndInfo = wndInfo;
		macro.wndInput = wndInput;
		macro.interpreter = interpreter;
		macro.script_interpreter;
		macro.thread;
		return macro;
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
		QiQuickClickThread thread;
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
		QiWindowBindThread thread;
	} wndActive;
};
struct SettingsData
{
	QString ocr_current;
	int ocr_thread = 0;
	int theme = 0;
	short key1 = 0;
	short key2 = 0;
	short recKey = 0;
	bool recTrack = false;
	bool defOn = false;
	bool showTips = false;
	bool audFx = false;
	bool minMode = false;
	bool pad = false;
	// eidt
	bool tabLock = false;
	bool tabHideTip = false;
	bool markPoint = false;
	QSize editSize;
};
struct GroupData
{
	std::map<QString, bool> fold;
	QiVector<QString> sort;
};
struct Widget
{
	bool dialogActive = false;
	bool mainActive = false;
	bool moreActive = false;
	QWidget* main = nullptr;
	QWidget* macro = nullptr;
	QWidget* trigger = nullptr;
	QWidget* func = nullptr;
	QWidget* settings = nullptr;
	QWidget* record = nullptr;
	QWidget* edit = nullptr;
	QWidget* varView = nullptr;
	QWidget* msgView = nullptr;
	Macro editMacro;
	bool active() const
	{
		return !(mainActive || dialogActive || moreActive);
	}
	void recordStart() const
	{
		QApplication::postEvent(record, new QEvent(static_cast<QEvent::Type>(QiEvent::rec_start)));
	}
	void recordStop() const
	{
		QApplication::postEvent(record, new QEvent(static_cast<QEvent::Type>(QiEvent::rec_stop)));
	}
	void recordClose() const
	{
		QApplication::postEvent(record, new QEvent(static_cast<QEvent::Type>(QiEvent::rec_close)));
	}
	void macroLoad() const
	{
		QApplication::postEvent(macro, new QEvent(static_cast<QEvent::Type>(QiEvent::mac_load)));
	}
	void macroEdit() const
	{
		QApplication::postEvent(macro, new QEvent(static_cast<QEvent::Type>(QiEvent::mac_edit_enter)));
	}
	void macroEdited(bool save = true) const
	{
		QApplication::postEvent(macro, new QEvent(static_cast<QEvent::Type>((save ? QiEvent::mac_edit_exit : QiEvent::mac_edit_exit_d))));
	}
	void editClose() const
	{
		QApplication::postEvent(edit, new QEvent(static_cast<QEvent::Type>(QiEvent::wid_close)));
	}
	void editDebugPause() const
	{
		QApplication::postEvent(edit, new QEvent(static_cast<QEvent::Type>(QiEvent::edt_debug_pause)));
	}
	void editVaropStop() const
	{
		QApplication::postEvent(edit, new QEvent(static_cast<QEvent::Type>(QiEvent::edt_varop_stop)));
	}
	void varViewReload() const
	{
		if (!varView->isHidden()) QApplication::postEvent(varView, new QEvent(static_cast<QEvent::Type>(QiEvent::var_reload)));
	}
	void msgViewSet(const QString& text) const
	{
		MsgViewInfo info; info.text = text;
		QApplication::postEvent(msgView, new MsgViewEvent(MsgViewEvent::Type::setText, info));
	}
	void msgViewAdd(const MsgViewInfo& info) const
	{
		QApplication::postEvent(msgView, new MsgViewEvent(MsgViewEvent::Type::newLine, info));
	}
	void msgViewClear() const
	{
		QApplication::postEvent(msgView, new MsgViewEvent(MsgViewEvent::Type::clear));
	}
	void msgViewShow() const
	{
		QApplication::postEvent(msgView, new MsgViewEvent(MsgViewEvent::Type::show));
	}
	void msgViewHide() const
	{
		QApplication::postEvent(msgView, new MsgViewEvent(MsgViewEvent::Type::hide));
	}
	void keyEditReload() const
	{
		QApplication::postEvent(macro, new QEvent(static_cast<QEvent::Type>(QiEvent::key_reset)));
		QApplication::postEvent(trigger, new QEvent(static_cast<QEvent::Type>(QiEvent::key_reset)));
		QApplication::postEvent(func, new QEvent(static_cast<QEvent::Type>(QiEvent::key_reset)));
		QApplication::postEvent(settings, new QEvent(static_cast<QEvent::Type>(QiEvent::key_reset)));
	}
};

namespace Qi
{
	inline size_t ocr_ver = 0;
	inline QiOcrModule ocr;
	// for setStyle
	inline QApplication* application = nullptr;
	inline QiUi::QuickInputUi ui;
	// state
	inline bool state = false;
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
	inline GroupData group;
	inline Widget widget;
	inline QPopText* popText = nullptr;
	inline QWindowSelection* windowSelection = nullptr;
	// input
	inline bool keyState[key_size];
	inline bool keyBlock[key_size];
	inline int curBlock = 0;
	inline XBoxPad xboxpad;
	// other
	inline QiDisplayUpdateThread display_update;
	inline SIZE screen = {};
	inline MSG msg;

	QJsonObject toJson();
	void fromJson(const QJsonObject& json);
}