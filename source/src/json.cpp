﻿#include "json.h"
namespace QiJson
{
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

		if (!File::SaveText(path, json.toJson())) MsgBox::Error((const wchar_t*)macro.name.utf16(), L"保存宏失败");
	}
	void SaveJson()
	{
		QJsonDocument json(Qi::toJson());
		if (!File::SaveText(Qi::configFile, json.toJson())) MsgBox::Error(L"保存配置失败");
	}

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
			for (const QFileInfo& file : File::Find(path, QString("*") + Qi::macroType))
			{
				Macro macro;
				macro.groupName = group->name;
				macro.groupBase = group->base;
				if (LoadMacro(macro, file.filePath(), file.baseName())) group->macros.append(std::move(macro));
			}
		}
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
				LoadMacro();
				return;
			}
		}
		Qi::fromJson(QJsonObject());
		LoadMacro();
	}
}