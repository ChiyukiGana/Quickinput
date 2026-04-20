#include "macro.h"
#include "type.h"

#ifdef Q_ENCRYPT
#include <encrypt/encrypt.cpp>
bool LoadEMacro(Macro& macro, const QString path, const QString name);
#endif

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
	json.insert("wndName", (QString)wndInfo.name);
	json.insert("wndClass", (QString)wndInfo.clas);
	json.insert("wndProc", (QString)wndInfo.proc);
	json.insert("wndMatch", (bool)matchState);
	json.insert("wndMatchName", (QString)wndMatch.name);
	json.insert("wndMatchClass", (QString)wndMatch.clas);
	json.insert("wndMatchProc", (QString)wndMatch.proc);
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
	wndInfo.name = json.value("wndName").toString();
	wndInfo.clas = json.value("wndClass").toString();
	wndInfo.proc = json.value("wndProc").toString();

	matchState = json.value("wndMatch").toBool();
	wndMatch.name = json.value("wndMatchName").toString();
	wndMatch.clas = json.value("wndMatchClass").toString();
	wndMatch.proc = json.value("wndMatchProc").toString();

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

typepack::object Macro::toPack() const
{
	typepack::object pack;
	pack.set("ver", Qi::version.toStdString());
	pack.set("name", name.toStdString());
	pack.set("wndState", (bool)wndState);
	pack.set("wndChild", (bool)wndInfo.child);
	pack.set("wndName", wndInfo.name.toStdString());
	pack.set("wndClass", wndInfo.clas.toStdString());
	pack.set("wndProc", wndInfo.proc.toStdString());
	pack.set("wndMatch", (bool)matchState);
	pack.set("wndMatchName", wndMatch.name.toStdString());
	pack.set("wndMatchClass", wndMatch.clas.toStdString());
	pack.set("wndMatchProc", wndMatch.proc.toStdString());
	pack.set("script", script.toStdString());
	pack.set("state", (bool)state);
	pack.set("keyBlock", (bool)keyBlock);
	pack.set("curBlock", (bool)curBlock);
	pack.set("key", (int)(static_cast<int>(key1) | (static_cast<int>(key2) << 16)));
	pack.set("key1", (int)key1);
	pack.set("key2", (int)key2);
	pack.set("mode", (int)mode);
	pack.set("count", (int)count);
	pack.set("timer", (bool)timer);
	pack.set("timerStart", (int)timerStart);
	pack.set("timerEnd", (int)timerEnd);
	pack.set("speed", (double)speed);
	pack.set("moveScaleX", (double)moveScaleX);
	pack.set("moveScaleY", (double)moveScaleY);
	pack.set("posScaleX", (double)posScaleX);
	pack.set("posScaleY", (double)posScaleY);
	pack.set("actions", acRun.toPack());
	pack.set("actionsEnding", acEnd.toPack());
	return pack;
}
void Macro::fromPack(const typepack::object& pack)
{
	if (name.isEmpty()) name = QString::fromStdString(pack.get("name").toString());

	wndState = pack.get("wndState").toBool();
	wndInfo.child = pack.get("wndChild").toBool();
	wndInfo.name = QString::fromStdString(pack.get("wndName").toString());
	wndInfo.clas = QString::fromStdString(pack.get("wndClass").toString());
	wndInfo.proc = QString::fromStdString(pack.get("wndProc").toString());

	matchState = pack.get("wndMatch").toBool();
	wndMatch.name = QString::fromStdString(pack.get("wndMatchName").toString());
	wndMatch.clas = QString::fromStdString(pack.get("wndMatchClass").toString());
	wndMatch.proc = QString::fromStdString(pack.get("wndMatchProc").toString());

	script = QString::fromStdString(pack.get("script").toString());

	state = pack.get("state").toBool();
	keyBlock = pack.get("keyBlock").toBool();
	curBlock = pack.get("curBlock").toBool();

	key1 = pack.get("key1").toInt();
	key2 = pack.get("key2").toInt();

	mode = QiRange::Restricted(static_cast<int>(pack.get("mode").toInt()), Macro::range_mode);
	count = QiRange::Restricted(static_cast<int>(pack.get("count").toInt()), Macro::range_count);

	timer = pack.get("timer").toBool();

	timerStart = QiRange::Restricted((int)pack.get("timerStart").toInt(), Macro::range_timer);
	timerEnd = QiRange::Restricted((int)pack.get("timerEnd").toInt(), Macro::range_timer);

	speed = pack.get("speed").toNumber();
	if (speed == 0) speed = 1.0f;
	else speed = QiRange::Restricted(speed, Macro::range_speed);

	moveScaleX = pack.get("moveScaleX").toNumber();
	if (moveScaleX == 0) moveScaleX = 1.0f;
	else moveScaleX = QiRange::Restricted(moveScaleX, Macro::range_moveScale);

	moveScaleY = pack.get("moveScaleY").toNumber();
	if (moveScaleY == 0) moveScaleY = 1.0f;
	else moveScaleY = QiRange::Restricted(moveScaleY, Macro::range_moveScale);

	posScaleX = QiRange::Restricted(pack.get("posScaleX").toNumber(), Macro::range_posScale);
	posScaleY = QiRange::Restricted(pack.get("posScaleY").toNumber(), Macro::range_posScale);

	acRun.fromPack(pack.get("actions").toArray());
	acEnd.fromPack(pack.get("actionsEnding").toArray());
}

#if !defined(Q_ENCRYPT) || defined(Q_ENCRYPT_CVT)
bool LoadJsonMacro(Macro& macro, const QString& path, const QString& name)
{
	QByteArray text;
	if (!File::LoadText(path, text))
	{
		MsgBox::Error(reinterpret_cast<const wchar_t*>(name.utf16()), L"加载宏失败");
		return false;
	}
	QJsonDocument json(QJsonDocument::fromJson(text));
	if (!json.isObject())
	{
		MsgBox::Error(reinterpret_cast<const wchar_t*>(name.utf16()), L"宏数据错误");
		return false;
	}
	macro.storageType = Macro::StorageType::JSON;
	macro.name = name;
	macro.fromJson(json.object());
	return true;
}
bool LoadQimMacro(Macro& macro, const QString& path, const QString& name)
{
	auto data = File::FileReadAll(path.toStdWString());
	if (data.empty())
	{
		MsgBox::Error(reinterpret_cast<const wchar_t*>(name.utf16()), L"加载宏失败");
		return false;
	}
	typepack::object pack = typepack::object::fromBinary(data);
	if (pack.empty())
	{
		MsgBox::Error(reinterpret_cast<const wchar_t*>(name.utf16()), L"宏数据错误");
		return false;
	}
	macro.storageType = Macro::StorageType::QIM;
	macro.name = name;
	macro.fromPack(pack);
	return true;
}
#endif

void Macro::loadAll()
{
	Qi::macroGroups.clear();
	Qi::macroGroups.append(MacroGroup(true, "默认分组"));

	QList<QString> dirs;
	{
		QDir dir(Qi::macroDir);
		dir.setFilter(QDir::Filter::Dirs);
		QFileInfoList dirInfos = dir.entryInfoList();
		if (dirInfos.isEmpty()) return;

		dirs.append(Qi::macroDir);
		for (auto& i : dirInfos) if (i.fileName() != "." && i.fileName() != "..") dirs.append(i.fileName());
	}

	bool base = true;
	for (auto& dir : dirs)
	{
		QString path = Qi::macroDir;
		MacroGroup* group;
		if (base)
		{
			group = &Qi::macroGroups.front();
			base = false;
		}
		else
		{
			group = &Qi::macroGroups.append(MacroGroup(false, dir));
			group->name = dir;
			path += dir + "/";
		}

#ifdef Q_ENCRYPT
		for (const QFileInfo& file : File::Find(path, QString("*") + Qi::emacroType))
		{
			if (file.fileName().compare(Qi::configFile, Qt::CaseInsensitive) == 0) continue;
			Macro macro;
			macro.groupName = group->name;
			macro.groupBase = group->base;
			if (LoadEMacro(macro, file.filePath(), file.baseName())) group->macros.append(std::move(macro));
		}
#endif
#if !defined(Q_ENCRYPT) || defined(Q_ENCRYPT_CVT)
		for (size_t i = 0; i < Macro::StorageType::QIM + 1; i++)
		{
			const QString type = i == Macro::StorageType::QIM ? Qi::macroQimType : Qi::macroType;
			const QString filter = QString("*") + type;
			for (const QFileInfo& file : File::Find(path, filter))
			{
				if (file.fileName().compare(Qi::configFile, Qt::CaseInsensitive) == 0) continue;

				Macro macro;
				macro.groupName = group->name;
				macro.groupBase = group->base;

				bool load = false;
				if (i == Macro::StorageType::QIM) load = LoadQimMacro(macro, file.filePath(), file.baseName());
				else load = LoadJsonMacro(macro, file.filePath(), file.baseName());

				if (load)
				{
					const QString old_path = macro.makePath();
					macro.name = group->makeName(macro.name);
					const QString new_path = macro.makePath();
					if (old_path != new_path && MoveFileW(reinterpret_cast<const wchar_t*>(old_path.utf16()), reinterpret_cast<const wchar_t*>(new_path.utf16())) == FALSE) MsgBox::Error(file.fileName().toStdWString(), L"重命名文件失败");
#if defined(Q_ENCRYPT) && defined(Q_ENCRYPT_CVT)
					macro.remove();
					macro.save();
#endif
					group->macros.append(std::move(macro));
				}
			}
		}
#endif
	}

	if (Qi::macroGroups)
	{
		std::sort(Qi::macroGroups.begin() + 1, Qi::macroGroups.end(), [&index = Qi::group.sort](const MacroGroup& a, const MacroGroup& b) -> bool {
			auto it_a = std::find(index.begin(), index.end(), a.name);
			auto it_b = std::find(index.begin(), index.end(), b.name);

			if (it_a != index.end() && it_b != index.end()) return std::distance(index.begin(), it_a) < std::distance(index.begin(), it_b);
			else if (it_a != index.end()) return true;
			else if (it_b != index.end()) return false;
			else return false;
			});
	}
}