#include "json.h"

namespace QiJson
{
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