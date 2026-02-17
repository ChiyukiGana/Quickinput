#include "json.h"

#ifdef Q_ENCRYPT
#include <encrypt/encrypt.cpp>
#endif

namespace QiJson
{
#ifndef Q_ENCRYPT
	void SaveMacro(const Macro& macro)
	{
		QJsonDocument json(macro.toJson());

		QString path = Qi::macroDir;
		if (!QDir(path).exists() && !QDir(path).mkdir(path))
		{
			MsgBox::Error(L"创建宏目录失败");
			return;
		}
		if (!macro.groupBase)
		{
			path += macro.groupName + QString("/");
			if (!QDir(path).exists() && !QDir(path).mkdir(path))
			{
				MsgBox::Error(L"创建分组目录失败");
				return;
			}
		}
		path += macro.name + Qi::macroType;

		if (!File::SaveText(path, json.toJson(QJsonDocument::JsonFormat::Compact))) MsgBox::Error((const wchar_t*)macro.name.utf16(), L"保存宏失败");
	}
#endif
#if !defined(Q_ENCRYPT) || defined(Q_ENCRYPT_CVT)
	bool LoadMacro(Macro& macro, const QString path, const QString name)
	{
		QByteArray text;
		if (File::LoadText(path, text))
		{
			QJsonDocument json(QJsonDocument::fromJson(text));
			if (json.isObject())
			{
				macro.name = name;
				macro.fromJson(json.object());
				return true;
			}
		}
		MsgBox::Error((const wchar_t*)name.utf16(), L"加载宏失败");
		return false;
	}
#endif
	void LoadMacro()
	{
		Qi::macroGroups.clear();
		Qi::macroGroups.append(MacroGroup(true, "默认分组"));

		QDir dir(Qi::macroDir);
		dir.setFilter(QDir::Filter::Dirs);
		QFileInfoList dirInfos = dir.entryInfoList();
		if (dirInfos.isEmpty()) return;

		QList<QString> dirs;
		dirs.append(Qi::macroDir);
		for (auto& i : dirInfos) if (i.fileName() != "." && i.fileName() != "..") dirs.append(i.fileName());

		bool base = true;
		for (auto& i : dirs)
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
				group = &Qi::macroGroups.append(MacroGroup(false, i));
				group->name = i;
				path += i;
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
			for (const QFileInfo& file : File::Find(path, QString("*") + Qi::macroType))
			{
				if (file.fileName().compare(Qi::configFile, Qt::CaseInsensitive) == 0) continue;
				Macro macro;
				macro.groupName = group->name;
				macro.groupBase = group->base;
				if (LoadMacro(macro, file.filePath(), file.baseName()))
				{
#if defined(Q_ENCRYPT) && defined(Q_ENCRYPT_CVT)
					SaveMacro(macro);
					DeleteFileW(reinterpret_cast<const wchar_t*>(file.filePath().utf16()));
#endif
					group->macros.append(std::move(macro));
				}
			}
#endif
		}

		if (Qi::macroGroups)
		{
			std::sort(Qi::macroGroups.begin() + 1, Qi::macroGroups.end(), [&index = Qi::group.sort](const MacroGroup& a, const MacroGroup& b) -> bool {
				auto it_a = std::find(index.begin(), index.end(), a.name);
				auto it_b = std::find(index.begin(), index.end(), b.name);

				if (it_a != index.end() && it_b != index.end()) {
					return std::distance(index.begin(), it_a) < std::distance(index.begin(), it_b);
				}
				else if (it_a != index.end()) {
					return true;
				}
				else if (it_b != index.end()) {
					return false;
				}
				else {
					return false;
				}
				});
		}
	}

	void SaveJson()
	{
		QJsonDocument json(Qi::toJson());
		if (!File::SaveText(Qi::configFile, json.toJson(QJsonDocument::JsonFormat::Compact))) MsgBox::Error(L"保存配置失败");
	}
	void LoadJson()
	{
		QByteArray text;
		if (File::LoadText(Qi::configFile, text))
		{
			QJsonDocument json(QJsonDocument::fromJson(text));
			if (json.isObject())
			{
				Qi::fromJson(json.object());
				return;
			}
		}
		Qi::fromJson(QJsonObject());
	}
}