#pragma once
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qimage.h>
#include <qbuffer.h>
#include "tools/tools.h"
#include "typepack/typepack.h"
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
	typepack::array toPack() const;
	void fromPack(const typepack::array& pack);
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
	virtual typepack::object toPack() const
	{
		typepack::object pack;
		pack.set("t", static_cast<int>(type));
		if (disable) pack.set("d", disable);
		if (!mark.isEmpty()) pack.set("m", mark.toStdString());
		return pack;
	}
	virtual void fromPack(const typepack::object pack)
	{
		type = static_cast<QiType>(pack.get("t").toInt());
		disable = pack.get("d").toBool();
		mark = QString::fromStdString(pack.get("m").toString());
	}

	static RECT RectUnjson(const QJsonArray& json)
	{
		if (json.size() != 4) return {};
		return { static_cast<LONG>(json[0].toInt()), static_cast<LONG>(json[1].toInt()), static_cast<LONG>(json[2].toInt()), static_cast<LONG>(json[3].toInt()) };
	}
	static void RectUnjson(const QJsonArray& json, QString& left, QString& top, QString& right, QString& bottom)
	{
		if (json.size() != 4)
		{
			left.clear(), top.clear(), right.clear(), bottom.clear();
			return;
		}
		left = json[0].toString(), top = json[1].toString(), right = json[2].toString(), bottom = json[3].toString();
	}
	static QJsonArray RectJson(const RECT& rect)
	{
		QJsonArray json;
		json.append(static_cast<int>(rect.left)), json.append(static_cast<int>(rect.top)), json.append(static_cast<int>(rect.right)), json.append(static_cast<int>(rect.bottom));
		return json;
	}
	static QJsonArray RectJson(const QString& left, const QString& top, const QString& right, const QString& bottom)
	{
		QJsonArray json;
		json.append(left), json.append(top), json.append(right), json.append(bottom);
		return json;
	}
	static RECT RectUnpack(const typepack::array& pack)
	{
		if (pack.size() != 4) return {};
		return { static_cast<LONG>(pack[0].toInt()), static_cast<LONG>(pack[1].toInt()), static_cast<LONG>(pack[2].toInt()), static_cast<LONG>(pack[3].toInt()) };
	}
	static void RectUnpack(const typepack::array& pack, QString& left, QString& top, QString& right, QString& bottom)
	{
		if (pack.size() != 4)
		{
			left.clear(), top.clear(), right.clear(), bottom.clear();
			return;
		}
		left = QString::fromStdString(pack[0].toString()), top = QString::fromStdString(pack[1].toString()), right = QString::fromStdString(pack[2].toString()), bottom = QString::fromStdString(pack[3].toString());
	}
	static typepack::array RectPack(const RECT& rect)
	{
		typepack::array pack;
		pack.resize(4);
		pack[0] = static_cast<int>(rect.left), pack[1] = static_cast<int>(rect.top), pack[2] = static_cast<int>(rect.right), pack[3] = static_cast<int>(rect.bottom);
		return pack;
	}
	static typepack::array RectPack(const QString& left, const QString& top, const QString& right, const QString& bottom)
	{
		typepack::array pack;
		pack.resize(4);
		pack[0] = left.toStdString(), pack[1] = top.toStdString(), pack[2] = right.toStdString(), pack[3] = bottom.toStdString();
		return pack;
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
		v_min.isEmpty() ? json.insert("min", min) : json.insert("v_min", v_min);
		v_max.isEmpty() ? json.insert("max", max) : json.insert("v_max", v_max);
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
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		v_min.isEmpty() ? pack.set("mn", min) : pack.set("_mn", v_min.toStdString());
		v_max.isEmpty() ? pack.set("mx", max) : pack.set("_mx", v_max.toStdString());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		min = pack.get("mn").toInt(), v_min = QString::fromStdString(pack.get("_mn").toString());
		min = pack.get("mx").toInt(), v_min = QString::fromStdString(pack.get("_mx").toString());
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
		json.insert("vk", vk);
		json.insert("state", state);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		vk = json.value("vk").toInt();
		state = json.value("state").toInt();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("vk", vk);
		pack.set("st", state);
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		vk = pack.get("vk").toInt();
		state = pack.get("st").toInt();
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
		v_x.isEmpty() ? json.insert("y", y) : json.insert("v_x", v_x);
		v_y.isEmpty() ? json.insert("x", x) : json.insert("v_y", v_y);
		if (ex) json.insert("ex", ex);
		if (track) json.insert("trk", track), json.insert("spd", speed);
		if (move) json.insert("move", move);
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
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		v_x.isEmpty() ? pack.set("y", y) : pack.set("_x", v_x.toStdString());
		v_y.isEmpty() ? pack.set("x", x) : pack.set("_y", v_y.toStdString());
		if (ex) pack.set("ex", ex);
		if (track) pack.set("tr", track), pack.set("sp", speed);
		if (move) pack.set("mv", move);
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		x = pack.get("x").toInt(), v_x = QString::fromStdString(pack.get("_x").toString());
		y = pack.get("y").toInt(), v_y = QString::fromStdString(pack.get("_y").toString());
		ex = pack.get("ex").toInt();
		speed = pack.get("sp").toInt();
		track = pack.get("tr").toBool();
		move = pack.get("mv").toBool();
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
		json.insert("text", text);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		text = json.value("text").toString();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("tx", text.toStdString());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		text = QString::fromStdString(pack.get("tx").toString());
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
		json.insert("rect", QiBase::RectJson(rect));
		if (!v_left.isEmpty() || !v_top.isEmpty() || !v_right.isEmpty() || !v_bottom.isEmpty()) json.insert("v_rect", QiBase::RectJson(v_left, v_top, v_right, v_bottom));
		json.insert("rgbe", static_cast<int>(rgbe.toCOLORREF()));
		if (move) json.insert("move", (bool)move);
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		if (json.contains("left") || json.contains("top") || json.contains("right") || json.contains("bottom"))
		{
			rect.left = json.value("left").toInt(), v_left = json.value("v_l").toString();
			rect.top = json.value("top").toInt(), v_top = json.value("v_t").toString();
			rect.right = json.value("right").toInt(), v_right = json.value("v_r").toString();
			rect.bottom = json.value("bottom").toInt(), v_bottom = json.value("v_b").toString();
		}
		else
		{
			rect = QiBase::RectUnjson(json.value("rect").toArray());
			QiBase::RectUnjson(json.value("v_rect").toArray(), v_left, v_top, v_right, v_bottom);
		}
		rgbe = json.value("rgbe").toInt();
		move = json.value("move").toBool();
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("rt", QiBase::RectPack(rect));
		if (!v_left.isEmpty() || !v_top.isEmpty() || !v_right.isEmpty() || !v_bottom.isEmpty()) pack.set("_rt", QiBase::RectPack(v_left, v_top, v_right, v_bottom));
		pack.set("co", static_cast<int>(rgbe.toCOLORREF()));
		if (move) pack.set("mv", move);
		if (next) pack.set("n1", next.toPack());
		if (next2) pack.set("n2", next2.toPack());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		rect = QiBase::RectUnpack(pack.get("rt").toArray());
		QiBase::RectUnpack(pack.get("_rt").toArray(), v_left, v_top, v_right, v_bottom);
		rgbe = static_cast<COLORREF>(pack.get("co").toInt());
		next.fromPack(pack.get("n1").toArray());
		next2.fromPack(pack.get("n2").toArray());
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
		v_min.isEmpty() ? json.insert("min", min) : json.insert("v_min", v_min);
		v_max.isEmpty() ? json.insert("max", max) : json.insert("v_max", v_max);
		if (next) json.insert("next", next.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		if (json.contains("count") || json.contains("rand"))
		{
			min = json.value("count").toInt();
			max = json.value("rand").toInt();
		}
		else
		{
			min = json.value("min").toInt(), v_min = json.value("v_min").toString();
			max = json.value("max").toInt(), v_max = json.value("v_max").toString();
		}
		next.fromJson(json.value("next").toArray());
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		v_min.isEmpty() ? pack.set("mn", min) : pack.set("_mn", v_min.toStdString());
		v_max.isEmpty() ? pack.set("mx", max) : pack.set("_mx", v_max.toStdString());
		if (next) pack.set("n1", next.toPack());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		min = pack.get("mn").toInt(), v_min = QString::fromStdString(pack.get("_mn").toString());
		min = pack.get("mx").toInt(), v_min = QString::fromStdString(pack.get("_mx").toString());
		next.fromPack(pack.get("n1").toArray());
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
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("vk", vk);
		if (next) pack.set("n1", next.toPack());
		if (next2) pack.set("n2", next2.toPack());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		vk = pack.get("vk").toInt();
		next.fromPack(pack.get("n1").toArray());
		next2.fromPack(pack.get("n2").toArray());
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
		json.insert("rect", QiBase::RectJson(rect));
		if (!v_left.isEmpty() || !v_top.isEmpty() || !v_right.isEmpty() || !v_bottom.isEmpty()) json.insert("v_rect", QiBase::RectJson(v_left, v_top, v_right, v_bottom));
		json.insert("sim", sim);
		if (move) json.insert("move", move);
		if (mult) json.insert("mult", mult);
		if (map) json.insert("data", toBase64RgbMap(map));
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		if (json.contains("left") || json.contains("top") || json.contains("right") || json.contains("bottom"))
		{
			rect.left = json.value("left").toInt(), v_left = json.value("v_l").toString();
			rect.top = json.value("top").toInt(), v_top = json.value("v_t").toString();
			rect.right = json.value("right").toInt(), v_right = json.value("v_r").toString();
			rect.bottom = json.value("bottom").toInt(), v_bottom = json.value("v_b").toString();
		}
		else
		{
			rect = QiBase::RectUnjson(json.value("rect").toArray());
			QiBase::RectUnjson(json.value("v_rect").toArray(), v_left, v_top, v_right, v_bottom);
		}
		if (json.contains("width") && json.contains("height"))
		{
			int width = json.value("width").toInt();
			int height = json.value("height").toInt();
			QString b64 = json.value("data").toString();
			if (!b64.isEmpty() && width && height) map = fromBase64_v1(b64, width, height);
			else map = {};
		}
		else map = fromBase64RgbMap(json.value("data").toString());
		sim = json.value("sim").toInt();
		move = json.value("move").toBool();
		mult = json.value("mult").toBool();
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("rt", QiBase::RectPack(rect));
		if (!v_left.isEmpty() || !v_top.isEmpty() || !v_right.isEmpty() || !v_bottom.isEmpty()) pack.set("_rt", QiBase::RectPack(v_left, v_top, v_right, v_bottom));
		pack.set("sm", sim);
		if (move) pack.set("mv", move);
		if (mult) pack.set("mt", mult);
		if (map) pack.set("mp", toBinaryRgbMap(map));
		if (next) pack.set("n1", next.toPack());
		if (next2) pack.set("n2", next2.toPack());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		rect = QiBase::RectUnpack(pack.get("rt").toArray());
		QiBase::RectUnpack(pack.get("_rt").toArray(), v_left, v_top, v_right, v_bottom);
		sim = pack.get("sm").toInt();
		move = pack.get("mv").toBool();
		map = fromBinaryRgbMap(pack.get("mp").toBinary());
		next.fromPack(pack.get("n1").toArray());
		next2.fromPack(pack.get("n2").toArray());
	}

	static RgbMap fromBase64_v1(const QString& base64, size_t width, size_t height)
	{
		if (width && height && !base64.isEmpty())
		{
			QByteArray data = QByteArray::fromBase64(base64.toLatin1());
			if (data.isEmpty()) return {};
			if (width * height * sizeof(RgbMap::value_type) != data.size()) return {};
			RgbMap map(width, height);
			memcpy(map.data(), data.data(), data.size());
			return map;
		}
		return {};
	}

	static QImage toImage(const RgbMap& map)
	{
		if (map.empty()) return {};
		QImage img(map.width(), map.height(), QImage::Format_RGB888);
		for (auto y = 0; y < map.height(); y++) memcpy(img.scanLine(y), &map.at(0, y), map.width() * 3);
		return img;
	}
	static RgbMap toRgbMap(const QImage& img)
	{
		if (img.isNull()) return {};
		const QImage* src = &img;
		QImage cvt;
		if (img.format() != QImage::Format_RGB888) cvt = img.convertToFormat(QImage::Format_RGB888), src = &cvt;
		RgbMap map(src->width(), src->height());
		for (auto y = 0; y < src->height(); y++) memcpy(&map.at(0, y), src->constScanLine(y), src->width() * 3);
		return map;
	}

	static QString toBase64RgbMap(const RgbMap& map)
	{
		if (map.empty()) return {};
		QImage img = toImage(map);
		if (img.isNull()) return {};
		return toBase64Image(img);
	}
	static RgbMap fromBase64RgbMap(const QString& b64)
	{
		if (b64.isEmpty()) return {};
		QImage img = fromBase64Image(b64);
		if (img.isNull()) return {};
		return toRgbMap(img);
	}
	static QString toBase64Image(const QImage& img)
	{
		if (img.isNull()) return {};
		QByteArray data;
		QBuffer buffer(&data);
		img.save(&buffer, "PNG");
		return data.toBase64();
	}
	static QImage fromBase64Image(const QString& b64)
	{
		if (b64.isEmpty()) return{};
		QByteArray data = QByteArray::fromBase64(b64.toLatin1());
		if (data.isEmpty()) return {};
		QImage img;
		img.loadFromData(data, "PNG");
		return img;
	}

	static typepack::binary toBinaryRgbMap(const RgbMap& map)
	{
		if (map.empty()) return {};
		QImage img = toImage(map);
		if (img.isNull()) return {};
		return toBinaryImage(img);
	}
	static RgbMap fromBinaryRgbMap(const typepack::binary bin)
	{
		if (bin.empty()) return{};
		QImage img = fromBinaryImage(bin);
		if (img.isNull()) return {};
		return toRgbMap(img);
	}
	static typepack::binary toBinaryImage(const QImage& img)
	{
		if (img.isNull()) return {};
		QByteArray data;
		QBuffer buffer(&data);
		img.save(&buffer, "PNG");
		return typepack::binary(data.constData(), data.constData() + data.size());
	}
	static QImage fromBinaryImage(const typepack::binary bin)
	{
		if (bin.empty()) return{};
		QByteArray data(bin.data(), bin.size());
		QImage img;
		img.loadFromData(data, "PNG");
		return img;
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
		json.insert("text", text);
		json.insert("time", time);
		if (sync) json.insert("sync", sync);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		text = json.value("text").toString();
		time = json.value("time").toInt();
		sync = json.value("sync").toBool();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("tx", text.toStdString());
		pack.set("tm", time);
		if (sync) pack.set("sy", sync);
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		text = QString::fromStdString(pack.get("tx").toString());
		time = pack.get("tm").toInt();
		sync = pack.get("sy").toBool();
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
		v_min.isEmpty() ? json.insert("min", min) : json.insert("v_min", v_min);
		v_max.isEmpty() ? json.insert("max", max) : json.insert("v_max", v_max);
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		if (json.contains("count") || json.contains("rand"))
		{
			min = json.value("count").toInt();
			max = json.value("rand").toInt();
		}
		else
		{
			min = json.value("min").toInt(), v_min = json.value("v_min").toString();
			max = json.value("max").toInt(), v_max = json.value("v_max").toString();
		}
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		v_min.isEmpty() ? pack.set("mn", min) : pack.set("_mn", v_min.toStdString());
		v_max.isEmpty() ? pack.set("mx", max) : pack.set("_mx", v_max.toStdString());
		if (next) pack.set("n1", next.toPack());
		if (next2) pack.set("n2", next2.toPack());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		min = pack.get("mn").toInt(), v_min = QString::fromStdString(pack.get("_mn").toString());
		min = pack.get("mx").toInt(), v_min = QString::fromStdString(pack.get("_mx").toString());
		next.fromPack(pack.get("n1").toArray());
		next2.fromPack(pack.get("n2").toArray());
	}
};
struct QiJump : QiBase
{
	QiJump() : QiBase(QiType::jump, QiTypeNext::none) {}
	QString name() const override { return "跳转"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("id", id);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		id = json.value("id").toInt();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("id", id);
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		id = pack.get("id").toInt();
	}
};
struct QiJumpPoint : QiBase
{
	QiJumpPoint() : QiBase(QiType::jumpPoint, QiTypeNext::none) {}
	QString name() const override { return "锚点"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("id", id);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		id = json.value("id").toInt();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("id", id);
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		id = pack.get("id").toInt();
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
		if (!title.isEmpty()) json.insert("title", title);
		if (!text.isEmpty()) json.insert("text", text);
		if (style) json.insert("style", style);
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
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		if (!title.isEmpty()) pack.set("tt", title.toStdString());
		if (!text.isEmpty()) pack.set("tx", text.toStdString());
		if (style) pack.set("st", style);
		if (next) pack.set("n1", next.toPack());
		if (next2) pack.set("n2", next2.toPack());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		style = pack.get("st").toInt();
		title = QString::fromStdString(pack.get("tt").toString());
		text = QString::fromStdString(pack.get("tx").toString());
		next.fromPack(pack.get("n1").toArray());
		next2.fromPack(pack.get("n2").toArray());
	}
};
struct QiBlock : QiBase
{
	QiBlock() : QiBase(QiType::block, QiTypeNext::nextOnly) {}
	QString name() const override { return "块"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("id", id);
		if (next) json.insert("next", next.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		id = json.value("id").toInt();
		next.fromJson(json.value("next").toArray());
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("id", id);
		if (next) pack.set("n1", next.toPack());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		id = pack.get("id").toInt();
		next.fromPack(pack.get("n1").toArray());
	}
};
struct QiBlockExec : QiBase
{
	QiBlockExec() : QiBase(QiType::blockExec, QiTypeNext::none) {}
	QString name() const override { return "执行"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("id", id);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		id = json.value("id").toInt();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("id", id);
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		id = pack.get("id").toInt();
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
		if (json.contains("c")) for (const auto& c : json.value("c").toArray()) chars.append(c.toInt());
		else text = json.value("s").toString();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("ss", text.toStdString());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		text = QString::fromStdString(pack.get("ss").toString());
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
		json.insert("k", vk);
		json.insert("b", block);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		vk = json.value("k").toInt();
		block = json.value("b").toBool();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("vk", vk);
		pack.set("bk", block);
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		vk = pack.get("vk").toInt();
		block = pack.get("bk").toBool();
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
		json.insert("t", time);
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
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("tm", time);
		if (next) pack.set("n1", next.toPack());
		if (next2) pack.set("n2", next2.toPack());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		time = pack.get("tm").toInt();
		next.fromPack(pack.get("n1").toArray());
		next2.fromPack(pack.get("n2").toArray());
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
		json.insert("rect", QiBase::RectJson(rect));
		if (!v_left.isEmpty() || !v_top.isEmpty() || !v_right.isEmpty() || !v_bottom.isEmpty()) json.insert("v_rect", QiBase::RectJson(v_left, v_top, v_right, v_bottom));
		if (!text.isEmpty()) json.insert("text", text);
		if (!var.isEmpty()) json.insert("var", var);
		if (match) json.insert("match", match);
		if (row) json.insert("row", row);
		if (move) json.insert("move", move);
		if (mult) json.insert("mult", mult);
		if (next) json.insert("next", next.toJson());
		if (next2) json.insert("next2", next2.toJson());
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		if (json.contains("left") || json.contains("top") || json.contains("right") || json.contains("bottom"))
		{
			rect.left = json.value("left").toInt(), v_left = json.value("v_l").toString();
			rect.top = json.value("top").toInt(), v_top = json.value("v_t").toString();
			rect.right = json.value("right").toInt(), v_right = json.value("v_r").toString();
			rect.bottom = json.value("bottom").toInt(), v_bottom = json.value("v_b").toString();
		}
		else
		{
			rect = QiBase::RectUnjson(json.value("rect").toArray());
			QiBase::RectUnjson(json.value("v_rect").toArray(), v_left, v_top, v_right, v_bottom);
		}
		text = json.value("text").toString();
		var = json.value("var").toString();
		match = json.value("match").toBool();
		row = json.value("row").toBool();
		move = json.value("move").toBool();
		mult = json.value("mult").toBool();
		next.fromJson(json.value("next").toArray());
		next2.fromJson(json.value("next2").toArray());
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("rt", QiBase::RectPack(rect));
		if (!v_left.isEmpty() || !v_top.isEmpty() || !v_right.isEmpty() || !v_bottom.isEmpty()) pack.set("_rt", QiBase::RectPack(v_left, v_top, v_right, v_bottom));
		if (!text.isEmpty()) pack.set("tx", text.toStdString());
		if (!var.isEmpty()) pack.set("va", var.toStdString());
		if (match) pack.set("mc", match);
		if (row) pack.set("ro", row);
		if (move) pack.set("mv", move);
		if (mult) pack.set("mt", mult);
		if (next) pack.set("n1", next.toPack());
		if (next2) pack.set("n2", next2.toPack());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		rect = QiBase::RectUnpack(pack.get("rt").toArray());
		QiBase::RectUnpack(pack.get("_rt").toArray(), v_left, v_top, v_right, v_bottom);
		text = QString::fromStdString(pack.get("tx").toString());
		var = QString::fromStdString(pack.get("va").toString());
		match = pack.get("mc").toBool();
		row = pack.get("ro").toBool();
		move = pack.get("mv").toBool();
		mult = pack.get("mt").toBool();
		next.fromPack(pack.get("n1").toArray());
		next2.fromPack(pack.get("n2").toArray());
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
		json.insert("t", script);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		script = json.value("t").toString();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("sc", script.toStdString());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		script = QString::fromStdString(pack.get("sc").toString());
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
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("sc", script.toStdString());
		if (next) pack.set("n1", next.toPack());
		if (next2) pack.set("n2", next2.toPack());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		script = QString::fromStdString(pack.get("sc").toString());
		next.fromPack(pack.get("n1").toArray());
		next2.fromPack(pack.get("n2").toArray());
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
		if (!s.empty())
		{
			json.insert("size", static_cast<int>(s.size()));
			json.insert("data", toBase64());
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
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		if (!s.empty())
		{
			typepack::array arr;
			arr.resize(s.size());
			for (size_t i = 0; i < arr.size(); i++)
			{
				const MovePart& part = s[i];
				typepack::array p;
				p.resize(3);
				p[0] = static_cast<int>(part.x);
				p[1] = static_cast<int>(part.y);
				p[2] = static_cast<int>(part.t);
				arr[i] = p;
			}
			pack.set("ps", arr);
		}
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		typepack::array arr = pack.get("ps").toArray();
		s.clear();
		for (const auto& i : arr)
		{
			typepack::array p = i.toArray();
			if (p.size() < 3) continue;
			MovePart part;
			part.x = p[0].toInt();
			part.y = p[1].toInt();
			part.t = p[2].toInt();
			s.push_back(std::move(part));
		}
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
		json.insert("url", url);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		url = json.value("url").toString();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("ul", url.toStdString());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		url = QString::fromStdString(pack.get("ul").toString());
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
		json.insert("text", text);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		text = json.value("text").toString();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("tx", text.toStdString());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		text = QString::fromStdString(pack.get("tx").toString());
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
		json.insert("title", title);
		json.insert("text", text);
		json.insert("var", var);
		if (mult) json.insert("mult", mult);
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
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		if (!title.isEmpty()) pack.set("tt", title.toStdString());
		if (!text.isEmpty()) pack.set("tx", text.toStdString());
		if (!var.isEmpty()) pack.set("va", var.toStdString());
		if (mult) pack.set("mt", mult);
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		title = QString::fromStdString(pack.get("tt").toString());
		text = QString::fromStdString(pack.get("tx").toString());
		var = QString::fromStdString(pack.get("va").toString());
		mult = pack.get("mt").toBool();
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
		json.insert("volume", volume);
		json.insert("time", time);
		if (max) json.insert("max", max);
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
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("vl", volume);
		pack.set("tm", time);
		if (max) pack.set("mx", max);
		if (next) pack.set("n1", next.toPack());
		if (next2) pack.set("n2", next2.toPack());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		volume = pack.get("vl").toNumber();
		time = pack.get("tm").toInt();
		max = pack.get("mx").toBool();
		next.fromPack(pack.get("n1").toArray());
		next2.fromPack(pack.get("n2").toArray());
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
		json.insert("file", file);
		json.insert("state", state);
		if (sync) json.insert("sync", sync);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		file = json.value("file").toString();
		state = json.value("state").toInt();
		sync = json.value("sync").toBool();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("fl", file.toStdString());
		pack.set("ac", state);
		if (sync) pack.set("sy", sync);
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		file = QString::fromStdString(pack.get("fl").toString());
		state = pack.get("ac").toInt();
		sync = pack.get("sy").toBool();
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
		json.insert("opt", option);
		json.insert("level", level);
		json.insert("text", text);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		option = json.value("opt").toInt();
		level = json.value("level").toInt();
		text = json.value("text").toString();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		if (!text.isEmpty()) pack.set("tx", text.toStdString());
		pack.set("op", option);
		pack.set("lv", level);
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		text = QString::fromStdString(pack.get("tx").toString());
		option = pack.get("op").toInt();
		level = pack.get("lv").toInt();
	}
};
struct QiRangeSet : QiBase
{
	static constexpr QiIntRange range_rect = { 0, 10000 };

	HWND w = nullptr;
	RECT rect = {};
	QString title;
	QString clas;
	QString proc;
	QString var;
	QiRangeSet() : QiBase(QiType::range, QiTypeNext::none) {}
	QString name() const override { return "操作区域"; }
	QJsonObject toJson() const override
	{
		QJsonObject json = QiBase::toJson();
		json.insert("rect", QiBase::RectJson(rect));
		if (!title.isEmpty()) json.insert("name", title);
		if (!clas.isEmpty()) json.insert("class", clas);
		if (!proc.isEmpty()) json.insert("proc", proc);
		if (!var.isEmpty()) json.insert("var", var);
		return json;
	}
	void fromJson(const QJsonObject& json) override
	{
		QiBase::fromJson(json);
		if (json.contains("left") || json.contains("top") || json.contains("right") || json.contains("bottom"))
		{
			rect.left = json.value("left").toInt();
			rect.top = json.value("top").toInt();
			rect.right = json.value("right").toInt();
			rect.bottom = json.value("bottom").toInt();
		}
		else rect = QiBase::RectUnjson(json.value("rect").toArray());
		title = json.value("name").toString();
		clas = json.value("class").toString();
		proc = json.value("proc").toString();
		var = json.value("var").toString();
	}
	typepack::object toPack() const override
	{
		typepack::object pack = QiBase::toPack();
		pack.set("rt", QiBase::RectPack(rect));
		pack.set("tt", title.toStdString());
		pack.set("cs", clas.toStdString());
		pack.set("pc", proc.toStdString());
		pack.set("va", var.toStdString());
		return pack;
	}
	void fromPack(const typepack::object pack) override
	{
		QiBase::fromPack(pack);
		rect = QiBase::RectUnpack(pack.get("rt").toArray());
		title = QString::fromStdString(pack.get("tt").toString());
		clas = QString::fromStdString(pack.get("cs").toString());
		proc = QString::fromStdString(pack.get("pc").toString());
		var = QString::fromStdString(pack.get("va").toString());
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
	typepack::object toPack() const;
	void fromPack(const typepack::object& pack);
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