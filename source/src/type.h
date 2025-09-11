#pragma once
#include "inc_project.h"
#include "scriptinterpreter.h"
#include "ui.h"
#include "range.h"
#include "thread.h"

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
		type,
		group,
		macro
	};
};
struct QiEvent
{
	enum
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
	MsgViewEvent(int type) : QEvent((QEvent::Type)type) {}
	MsgViewEvent(int type, const MsgViewInfo& info) : QEvent((QEvent::Type)type), _info(info) {}
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
		msgView,
		count
	};
};

struct Actions : QiVector<Action>
{
	QJsonArray toJson() const;
	void fromJson(const QJsonArray& json);
};

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
	virtual QString name() const { return "base"; }
	virtual QJsonObject toJson() const
	{
		QJsonObject json;
		json.insert("dis", disable);
		json.insert("type", type);
		json.insert("mark", mark);
		return json;
	}
	virtual void fromJson(const QJsonObject& json)
	{
		disable = json.value("dis").toBool();
		type = json.value("type").toInt();
		mark = json.value("mark").toString();
	}
};
struct QiEnd : QiBase
{
	QiEnd() : QiBase(QiType::end) {}
	QString name() const override { return "结束"; }
	QJsonObject toJson() const override
	{
		return QiBase::toJson();
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
	}
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
		json.insert("ms", (int)min);
		json.insert("ex", (int)max);
		json.insert("v_min", (QString)v_min);
		json.insert("v_max", (QString)v_max);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		min = json.value("ms").toInt();
		max = json.value("ex").toInt();
		v_min = json.value("v_min").toString();
		v_max = json.value("v_max").toString();
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
	QiMouse() : QiBase(QiType::mouse) {}
	QString name() const override { return "鼠标"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("x", (int)x);
		json.insert("y", (int)y);
		json.insert("ex", (int)ex);
		json.insert("spd", (int)speed);
		json.insert("trk", (bool)track);
		json.insert("move", (bool)move);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		x = json.value("x").toInt();
		y = json.value("y").toInt();
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
	QiColor() : QiBase(QiType::color) {}
	QString name() const override { return "找色"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("next", next.toJson());
		json.insert("next2", next2.toJson());
		json.insert("move", (bool)move);
		json.insert("left", (int)rect.left);
		json.insert("top", (int)rect.top);
		json.insert("right", (int)rect.right);
		json.insert("bottom", (int)rect.bottom);
		json.insert("rgbe", (int)rgbe.toCOLORREF());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
		move = json.value("move").toBool();
		rect.left = json.value("left").toInt();
		rect.top = json.value("top").toInt();
		rect.right = json.value("right").toInt();
		rect.bottom = json.value("bottom").toInt();
		rgbe = json.value("rgbe").toInt();
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
		json.insert("next", next.toJson());
		json.insert("count", (int)min);
		json.insert("rand", (int)max);
		json.insert("v_min", (QString)v_min);
		json.insert("v_max", (QString)v_max);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		next.fromJson(json.value("next").toArray());
		min = json.value("count").toInt();
		max = json.value("rand").toInt();
		v_min = json.value("v_min").toString();
		v_max = json.value("v_max").toString();
	}
};
struct QiLoopEnd : QiBase
{
	QiLoopEnd() : QiBase(QiType::loopEnd) {}
	QString name() const override { return "结束循环"; }
	QJsonObject toJson() const override
	{
		return QiBase::toJson();
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
	}
};
struct QiKeyState : QiBase
{
	int vk = 0;
	QiKeyState() : QiBase(QiType::keyState) {}
	QString name() const override { return "按键状态"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("next", next.toJson());
		json.insert("next2", next2.toJson());
		json.insert("vk", (int)vk);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
		vk = json.value("vk").toInt();
	}
};
struct QiResetPos : QiBase
{
	QiResetPos() : QiBase(QiType::resetPos) {}
	QString name() const override { return "恢复位置"; }
	QJsonObject toJson() const override
	{
		return QiBase::toJson();
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
	}
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
	QString name() const override { return "找图"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("next", next.toJson());
		json.insert("next2", next2.toJson());
		json.insert("move", (bool)move);
		json.insert("left", (int)rect.left);
		json.insert("top", (int)rect.top);
		json.insert("right", (int)rect.right);
		json.insert("bottom", (int)rect.bottom);
		json.insert("sim", (int)sim);
		json.insert("width", (int)map.width());
		json.insert("height", (int)map.height());
		json.insert("data", (QString)toBase64());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
		move = json.value("move").toBool();
		rect.left = json.value("left").toInt();
		rect.top = json.value("top").toInt();
		rect.right = json.value("right").toInt();
		rect.bottom = json.value("bottom").toInt();
		sim = json.value("sim").toInt();
		int width = json.value("width").toInt();
		int height = json.value("height").toInt();
		QString data = json.value("data").toString();
		if (!data.isEmpty() && width && height) fromBase64(data, width, height);
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
		json.insert("sync", (bool)sync);
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
	QJsonObject toJson() const override
	{
		return QiBase::toJson();
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
	}
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
		json.insert("next", next.toJson());
		json.insert("next2", next2.toJson());
		json.insert("min", (int)min);
		json.insert("max", (int)max);
		json.insert("v_min", (QString)v_min);
		json.insert("v_max", (QString)v_max);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
		min = json.value("min").toInt();
		max = json.value("max").toInt();
		v_min = json.value("v_min").toString();
		v_max = json.value("v_max").toString();
	}
};
struct QiJump : QiBase
{
	int id = 0;
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
	int id = 0;
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
		json.insert("next", next.toJson());
		json.insert("next2", next2.toJson());
		json.insert("style", (int)style);
		json.insert("title", (QString)title);
		json.insert("text", (QString)text);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
		style = json.value("style").toInt();
		title = json.value("title").toString();
		text = json.value("text").toString();
	}
};
struct QiBlock : QiBase
{
	int id = 0;
	QiBlock() : QiBase(QiType::block) {}
	QString name() const override { return "块"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("next", next.toJson());
		json.insert("id", (int)id);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		next.fromJson(json.value("next").toArray());
		id = json.value("id").toInt();
	}
};
struct QiBlockExec : QiBase
{
	int id = 0;
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
		QJsonArray array;
		for (const auto& i : chars) array.append((int)i);
		json.insert("c", array);
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
		json.insert("next", next.toJson());
		json.insert("next2", next2.toJson());
		json.insert("t", (int)time);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
		time = json.value("t").toInt();
	}
};
struct QiOcr : QiBase
{
	static constexpr QiIntRange range_rect = { 0, 10000 };

	bool match = false, row = false, move = false;
	RECT rect = {};
	QString text, var;
	QiOcr() : QiBase(QiType::ocr) {}
	QString name() const override { return "文字识别"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("next", next.toJson());
		json.insert("next2", next2.toJson());
		json.insert("left", (int)rect.left);
		json.insert("top", (int)rect.top);
		json.insert("right", (int)rect.right);
		json.insert("bottom", (int)rect.bottom);
		json.insert("text", (QString)text);
		json.insert("var", (QString)var);
		json.insert("match", (bool)match);
		json.insert("row", (bool)row);
		json.insert("move", (bool)move);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
		rect.left = json.value("left").toInt();
		rect.top = json.value("top").toInt();
		rect.right = json.value("right").toInt();
		rect.bottom = json.value("bottom").toInt();
		text = json.value("text").toString();
		var = json.value("var").toString();
		match = json.value("match").toBool();
		row = json.value("row").toBool();
		move = json.value("move").toBool();
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
		json.insert("next", next.toJson());
		json.insert("next2", next2.toJson());
		json.insert("t", (QString)script);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
		script = json.value("t").toString();
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
		json.insert("size", (int)s.size());
		json.insert("data", (QString)toBase64());
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
		json.insert("mult", (bool)mult);
		json.insert("title", (QString)title);
		json.insert("text", (QString)text);
		json.insert("var", (QString)var);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		mult = json.value("mult").toBool();
		title = json.value("title").toString();
		text = json.value("text").toString();
		var = json.value("var").toString();
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
		json.insert("next", next.toJson());
		json.insert("next2", next2.toJson());
		json.insert("volume", (double)volume);
		json.insert("time", (int)time);
		json.insert("max", (bool)max);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
		volume = json.value("volume").toDouble();
		time = json.value("time").toInt();
		max = json.value("max").toBool();
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
		json.insert("sync", (bool)sync);
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
	void fromJson(const QJsonObject& json);
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
	int key = 0;
	int recKey = 0;
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
	bool active() const
	{
		return !(mainActive || dialogActive || moreActive);
	}
	void recordStart() const
	{
		QApplication::postEvent(record, new QEvent((QEvent::Type)QiEvent::rec_start));
	}
	void recordStop() const
	{
		QApplication::postEvent(record, new QEvent((QEvent::Type)QiEvent::rec_stop));
	}
	void recordClose() const
	{
		QApplication::postEvent(record, new QEvent((QEvent::Type)QiEvent::rec_close));
	}
	void macroLoad() const
	{
		QApplication::postEvent(macro, new QEvent((QEvent::Type)QiEvent::mac_load));
	}
	void macroEdit() const
	{
		QApplication::postEvent(macro, new QEvent((QEvent::Type)QiEvent::mac_edit_enter));
	}
	void macroEdited(bool save = true) const
	{
		QApplication::postEvent(macro, new QEvent((QEvent::Type)(save ? QiEvent::mac_edit_exit : QiEvent::mac_edit_exit_d)));
	}
	void editClose() const
	{
		QApplication::postEvent(edit, new QEvent((QEvent::Type)QiEvent::wid_close));
	}
	void editDebugPause() const
	{
		QApplication::postEvent(edit, new QEvent((QEvent::Type)QiEvent::edt_debug_pause));
	}
	void editVaropStop() const
	{
		QApplication::postEvent(edit, new QEvent((QEvent::Type)QiEvent::edt_varop_stop));
	}
	void varViewReload() const
	{
		if (!varView->isHidden()) QApplication::postEvent(varView, new QEvent((QEvent::Type)QiEvent::var_reload));
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
		QApplication::postEvent(macro, new QEvent((QEvent::Type)QiEvent::key_reset));
		QApplication::postEvent(trigger, new QEvent((QEvent::Type)QiEvent::key_reset));
		QApplication::postEvent(func, new QEvent((QEvent::Type)QiEvent::key_reset));
		QApplication::postEvent(settings, new QEvent((QEvent::Type)QiEvent::key_reset));
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