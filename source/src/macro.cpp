#include "macro.h"
QJsonObject Macro::toJson() const
{
	QJsonObject json;
	json.insert("document_charset", "UTF8");
	json.insert("app", "QuickInput");
	json.insert("type", "QuickInputMacro");
	json.insert("ver", (QString)Qi::version);
	json.insert("name", (QString)name);
	json.insert("wndState", (bool)wndState);
	json.insert("wndChild", (bool)wndInfo.child);
	json.insert("wndName", (QString)wndInfo.wndName);
	json.insert("wndClass", (QString)wndInfo.wndClass);
	json.insert("script", (QString)script);
	json.insert("state", (bool)state);
	json.insert("keyBlock", (bool)keyBlock);
	json.insert("curBlock", (bool)curBlock);
	json.insert("key", (int)(static_cast<int>(key1) | (static_cast<int>(key2) << 16)));
	json.insert("key1", (int)key1);
	json.insert("key2", (int)key2);
	json.insert("mode", (int)mode);
	json.insert("count", (int)count);
	json.insert("timer", (bool)timer);
	json.insert("timerStart", (int)timerStart);
	json.insert("timerEnd", (int)timerEnd);
	json.insert("speed", (double)speed);
	json.insert("moveScaleX", (double)moveScaleX);
	json.insert("moveScaleY", (double)moveScaleY);
	json.insert("posScaleX", (double)posScaleX);
	json.insert("posScaleY", (double)posScaleY);
	json.insert("actions", acRun.toJson());
	json.insert("actionsEnding", acEnd.toJson());
	return json;
}
void Macro::fromJson(const QJsonObject& json)
{
	if (name.isEmpty()) name = json.value("name").toString();

	wndState = json.value("wndState").toBool();
	wndInfo.child = json.value("wndChild").toBool();
	wndInfo.wndName = json.value("wndName").toString();
	wndInfo.wndClass = json.value("wndClass").toString();
	script = json.value("script").toString();

	state = json.value("state").toBool();
	keyBlock = json.value("keyBlock").toBool();
	curBlock = json.value("curBlock").toBool();

	if (json.contains("key1"))
	{
		key1 = json.value("key1").toInt();
		key2 = json.value("key2").toInt();
	}
	else
	{
		int key = json.value("key").toInt();
		key1 = key & 0xFFFF;
		key2 = key >> 16;
	}

	mode = QiRange::Restricted(json.value("mode").toInt(), Macro::range_mode);
	count = QiRange::Restricted(json.value("count").toInt(), Macro::range_count);

	timer = json.value("timer").toBool();

	timerStart = QiRange::Restricted((int)json.value("timerStart").toInt(), Macro::range_timer);
	timerEnd = QiRange::Restricted((int)json.value("timerEnd").toInt(), Macro::range_timer);

	speed = json.value("speed").toDouble();
	if (speed == 0) speed = 1.0f;
	else speed = QiRange::Restricted(speed, Macro::range_speed);

	moveScaleX = json.value("moveScaleX").toDouble();
	if (moveScaleX == 0) moveScaleX = 1.0f;
	else moveScaleX = QiRange::Restricted(moveScaleX, Macro::range_moveScale);

	moveScaleY = json.value("moveScaleY").toDouble();
	if (moveScaleY == 0) moveScaleY = 1.0f;
	else moveScaleY = QiRange::Restricted(moveScaleY, Macro::range_moveScale);

	posScaleX = QiRange::Restricted(json.value("posScaleX").toDouble(), Macro::range_posScale);
	posScaleY = QiRange::Restricted(json.value("posScaleY").toDouble(), Macro::range_posScale);

	acRun.fromJson(json.value("actions").toArray());
	acEnd.fromJson(json.value("actionsEnding").toArray());
}