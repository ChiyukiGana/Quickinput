#pragma once
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include "tools/tools.h"
#include "range.h"

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
	range,
	count
};
enum class QiTypeNext
{
	none,
	nextOnly,
	nextAll
};
enum class QiTypeProperty
{
	disable,
	debug_entry,
	debug_break,
	debug_exit
};

struct QiIndex
{
	static constexpr size_t npos = ~0ull;
	size_t index = npos;
	bool next1 = false;
	bool next2 = false;
	std::unique_ptr<QiIndex> next;
};
struct QiIdList : public QiVector<int>
{
	using base_vector = QiVector<int>;
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

struct Action;
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
	QiIndex find_index(QiType type, int id) const;
	QiIndex find_index(QiTypeProperty property) const;
	Actions loadType(QiType type) const;
	QiIdList loadId(QiType type) const;
};
using HistoryActions = QiHistoryVector<Action>;

struct QiBase
{
	QiType type;
	QiTypeNext typeNext;
	int id = -1;
	bool disable = false;
	bool debug_entry = false;
	bool debug_break = false;
	bool debug_exit = false;
	QString mark = QString();
	Actions next = Actions();
	Actions next2 = Actions();
	QiBase(QiType type = QiType::none, QiTypeNext typeNext = QiTypeNext::none) noexcept : type(type), typeNext(typeNext) {}
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
	QiEnd() : QiBase(QiType::end, QiTypeNext::none) {}
	QString name() const override { return "结束"; }
};
struct QiDelay : QiBase
{
	static constexpr QiIntRange range_time = { 0, (~unsigned int(0)) >> 1 };

	int min = 0, max = 0;
	QString v_min, v_max;
	QiDelay() : QiBase(QiType::delay, QiTypeNext::none) {}
	QString name() const override { return "等待"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		v_min.isEmpty() ? json.insert("min", (int)min) : json.insert("v_min", (QString)v_min);
		v_max.isEmpty() ? json.insert("max", (int)max) : json.insert("v_max", (QString)v_max);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		min = json.value("min").toInt(), v_min = json.value("v_min").toString();
		max = json.value("max").toInt(), v_max = json.value("v_max").toString();
		if (!min) min = json.value("ms").toInt();
		if (!max) max = json.value("ex").toInt();
	}
};
struct QiKey : QiBase
{
	enum { up, down, click };
	int vk = 0, state = down;
	QiKey() : QiBase(QiType::key, QiTypeNext::none) {}
	QString name() const override
	{
		if (state == up) return "松开";
		if (state == down) return "按下";
		if (state == click) return "点击";
		return "按键";
	}
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
	QiMouse() : QiBase(QiType::mouse, QiTypeNext::none) {}
	QString name() const override
	{
		if (move) return "移动";
		return "位置";
	}
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
	QiCopyText() : QiBase(QiType::copyText, QiTypeNext::none) {}
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
	QiColor() : QiBase(QiType::color, QiTypeNext::nextAll) {}
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
	QiLoop() : QiBase(QiType::loop, QiTypeNext::nextOnly) {}
	QString name() const override { return "循环"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		v_min.isEmpty() ? json.insert("min", (int)min) : json.insert("v_min", (QString)v_min);
		v_max.isEmpty() ? json.insert("max", (int)max) : json.insert("v_max", (QString)v_max);
		if (next) json.insert("next", next.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		min = json.value("min").toInt(), v_min = json.value("v_min").toString();
		max = json.value("max").toInt(), v_max = json.value("v_max").toString();
		if (!min) min = json.value("count").toInt();
		if (!max) max = json.value("rand").toInt();
		next.fromJson(json.value("next").toArray());
	}
};
struct QiLoopEnd : QiBase
{
	QiLoopEnd() : QiBase(QiType::loopEnd, QiTypeNext::none) {}
	QString name() const override { return "结束循环"; }
};
struct QiKeyState : QiBase
{
	int vk = 0;
	QiKeyState() : QiBase(QiType::keyState, QiTypeNext::nextAll) {}
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
	QiResetPos() : QiBase(QiType::resetPos, QiTypeNext::none) {}
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
	bool mult = false;
	QString v_left, v_top, v_right, v_bottom;
	QiImage() : QiBase(QiType::image, QiTypeNext::nextAll) {}
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
		if (mult) json.insert("mult", (bool)mult);
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
		mult = json.value("mult").toBool();
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
			map.assign(width, height);
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
	QiPopText() : QiBase(QiType::popText, QiTypeNext::none) {}
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
	QiSavePos() : QiBase(QiType::savePos, QiTypeNext::none) {}
	QString name() const override { return "记录位置"; }
};
struct QiTimer : QiBase
{
	static constexpr QiIntRange range_time = { 0, (~unsigned int(0)) >> 1 };

	int min = 0, max = 0;
	QString v_min, v_max;
	QiTimer() : QiBase(QiType::timer, QiTypeNext::nextAll) {}
	QString name() const override { return "定时"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		v_min.isEmpty() ? json.insert("min", (int)min) : json.insert("v_min", (QString)v_min);
		v_max.isEmpty() ? json.insert("max", (int)max) : json.insert("v_max", (QString)v_max);
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		min = json.value("min").toInt(), v_min = json.value("v_min").toString();
		max = json.value("max").toInt(), v_max = json.value("v_max").toString();
		if (!min) min = json.value("count").toInt();
		if (!max) max = json.value("rand").toInt();
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
};
struct QiJump : QiBase
{
	QiJump() : QiBase(QiType::jump, QiTypeNext::none) {}
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
	QiJumpPoint() : QiBase(QiType::jumpPoint, QiTypeNext::none) {}
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
	QiDialog() : QiBase(QiType::dialog, QiTypeNext::nextAll) {}
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
	QiBlock() : QiBase(QiType::block, QiTypeNext::nextOnly) {}
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
	QiBlockExec() : QiBase(QiType::blockExec, QiTypeNext::none) {}
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
	QString text;
	QiVector<unsigned char> chars;
	QiQuickInput() : QiBase(QiType::quickInput, QiTypeNext::none) {}
	QString name() const override { return "输入字符"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("s", text);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		text = json.value("s").toString();
		for (const auto& c : json.value("c").toArray()) chars.append(c.toInt());
	}
	static QiVector<unsigned char> toKey(const std::string& str)
	{
		QiVector<unsigned char> keys;
		for (auto i : str)
		{
			unsigned char c = i;
			unsigned char converted = 0;
			if (InRange(char(c), '0', '9', char(0)) || InRange(char(c), 'A', 'Z', char(0))) converted = c;
			else if (InRange(char(c), 'a', 'z', char(0)))  converted = toupper(c);
			else
			{
				switch (c)
				{
				case '`': converted = VK_OEM_3; break;
				case '~': converted = VK_OEM_3; break;
				case '-': converted = VK_OEM_MINUS; break;
				case '_': converted = VK_OEM_MINUS; break;
				case '=': converted = VK_OEM_PLUS; break;
				case '+': converted = VK_OEM_PLUS; break;
				case '[': converted = VK_OEM_4; break;
				case '{': converted = VK_OEM_4; break;
				case ']': converted = VK_OEM_6; break;
				case '}': converted = VK_OEM_6; break;
				case ';': converted = VK_OEM_1; break;
				case ':': converted = VK_OEM_1; break;
				case '\'': converted = VK_OEM_7; break;
				case '\"': converted = VK_OEM_7; break;
				case ',': converted = VK_OEM_COMMA; break;
				case '<': converted = VK_OEM_COMMA; break;
				case '.': converted = VK_OEM_PERIOD; break;
				case '>': converted = VK_OEM_PERIOD; break;
				case '/': converted = VK_OEM_2; break;
				case '?': converted = VK_OEM_2; break;
				case '\\': converted = VK_OEM_5; break;
				case '|': converted = VK_OEM_5; break;
				}
			}
			if (converted) keys.append(converted);
		}
		return keys;
	}
	static QiVector<unsigned char> toKey(const QString& str)
	{
		QiVector<unsigned char> keys;
		for (auto i : str)
		{
			unsigned char c = i.toLatin1();
			unsigned char converted = 0;
			if (InRange(char(c), '0', '9', char(0)) || InRange(char(c), 'A', 'Z', char(0))) converted = c;
			else if (InRange(char(c), 'a', 'z', char(0)))  converted = toupper(c);
			else
			{
				switch (c)
				{
				case '`': converted = VK_OEM_3; break;
				case '~': converted = VK_OEM_3; break;
				case '-': converted = VK_OEM_MINUS; break;
				case '_': converted = VK_OEM_MINUS; break;
				case '=': converted = VK_OEM_PLUS; break;
				case '+': converted = VK_OEM_PLUS; break;
				case '[': converted = VK_OEM_4; break;
				case '{': converted = VK_OEM_4; break;
				case ']': converted = VK_OEM_6; break;
				case '}': converted = VK_OEM_6; break;
				case ';': converted = VK_OEM_1; break;
				case ':': converted = VK_OEM_1; break;
				case '\'': converted = VK_OEM_7; break;
				case '\"': converted = VK_OEM_7; break;
				case ',': converted = VK_OEM_COMMA; break;
				case '<': converted = VK_OEM_COMMA; break;
				case '.': converted = VK_OEM_PERIOD; break;
				case '>': converted = VK_OEM_PERIOD; break;
				case '/': converted = VK_OEM_2; break;
				case '?': converted = VK_OEM_2; break;
				case '\\': converted = VK_OEM_5; break;
				case '|': converted = VK_OEM_5; break;
				}
			}
			if (converted) keys.append(converted);
		}
		return keys;
	}
	static QString toString(const QiVector<unsigned char>& keys)
	{
		QString str;
		for (auto& i : keys)
		{
			unsigned char c = 0;
			if (InRange(char(i), '0', '9', char()) || InRange(char(i), 'A', 'Z', char())) c = i;
			else
			{
				switch (i)
				{
				case VK_OEM_3: c = '`'; break;
				case VK_OEM_MINUS: c = '-'; break;
				case VK_OEM_PLUS: c = '='; break;
				case VK_OEM_4: c = '['; break;
				case VK_OEM_6: c = ']'; break;
				case VK_OEM_1:c = ';'; break;
				case VK_OEM_7: c = '\''; break;
				case VK_OEM_COMMA: c = ','; break;
				case VK_OEM_PERIOD: c = '.'; break;
				case VK_OEM_2: c = '/'; break;
				case VK_OEM_5: c = '\\'; break;
				}
			}
			if (c) str += QChar(c);
		}
		return str;
	}
};
struct QiKeyBlock : QiBase
{
	enum { all = -1, move = 0 };
	int vk = 0;
	bool block = false;
	QiKeyBlock() : QiBase(QiType::keyBlock, QiTypeNext::none) {}
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
	QiClock() : QiBase(QiType::clock, QiTypeNext::nextAll) {}
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

	bool match = false, row = false, move = false, mult = false;
	RECT rect = {};
	QString text, var;
	QString v_left, v_top, v_right, v_bottom;
	QiOcr() : QiBase(QiType::ocr, QiTypeNext::nextAll) {}
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
		if (mult) json.insert("mult", (bool)mult);
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
		mult = json.value("mult").toBool();
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
};
struct QiVarOperator : QiBase
{
	QString script;
	QiVarOperator() : QiBase(QiType::varOperator, QiTypeNext::none) {}
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
	QiVarCondition() : QiBase(QiType::varCondition, QiTypeNext::nextAll) {}
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
	QiMouseTrack() : QiBase(QiType::mouseTrack, QiTypeNext::none), t(clock()) {}
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
		MovePart pt;
		pt.x = x;
		pt.y = y;
		pt.t = clock() - this->t;
		s.push_back(pt);
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
	QiOpen() : QiBase(QiType::open, QiTypeNext::none) {}
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
	QiTextPad() : QiBase(QiType::textPad, QiTypeNext::none) {}
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
	QiEditDialog() : QiBase(QiType::editDialog, QiTypeNext::none) {}
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
	QiVolume() : QiBase(QiType::volume, QiTypeNext::nextAll) {}
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
	QiSoundPlay() : QiBase(QiType::soundPlay, QiTypeNext::none) {}
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
	QiMsgView() : QiBase(QiType::msgView, QiTypeNext::none) {}
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
struct QiRangeSet : QiBase
{
	static constexpr QiIntRange range_rect = { 0, 10000 };

	HWND w = nullptr;
	RECT rect = {};
	QString wnd;
	QString var;
	QiRangeSet() : QiBase(QiType::range, QiTypeNext::none) {}
	QString name() const override { return "操作区域"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("left", (int)rect.left);
		json.insert("top", (int)rect.top);
		json.insert("right", (int)rect.right);
		json.insert("bottom", (int)rect.bottom);
		if (!wnd.isEmpty()) json.insert("wnd", wnd);
		if (!var.isEmpty()) json.insert("var", var);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		rect.left = json.value("left").toInt();
		rect.top = json.value("top").toInt();
		rect.right = json.value("right").toInt();
		rect.bottom = json.value("bottom").toInt();
		wnd = json.value("wnd").toString();
		var = json.value("var").toString();
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
	QiMsgView,
	QiRangeSet
> ;
struct Action : public ActionVariant
{
	using ActionVariant::ActionVariant;
	Action() : ActionVariant(QiBase()) {}
	QiType type() const { return static_cast<QiType>(index()); }
	QiTypeNext typeNext() const { return base().typeNext; }
	QiBase& base() { QiBase* base; std::visit([&base](auto&& var) { base = &var; }, *this); return *base; }
	const QiBase& base() const { const QiBase* base; std::visit([&base](auto&& var) { base = &var; }, *this); return *base; }
	Actions& next() { return base().next; }
	const Actions& next() const { return base().next; }
	Actions& next2() { return base().next2; }
	const Actions& next2() const { return base().next2; }
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
	Action* iter(std::function<bool(Action&)> callBack, QiType type = QiType::none);
	const Action* iter(std::function<bool(const Action&)> callBack, QiType type = QiType::none) const;
	Action* find(QiType type, int id);
	const Action* find(QiType type, int id) const;
	Actions loadType(QiType type) const;
	QiIdList loadId(QiType type) const;
	template<class QiTypes>
	QiTypes& to() { return std::get<QiTypes>(*this); }
	template<class QiTypes>
	const QiTypes& to() const { return std::get<QiTypes>(*this); }
};