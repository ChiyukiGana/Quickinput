#include "lang.h"
#include "inc_project.h"
QString Language::makePath(const QString& name) { return Qi::langDir + name + Qi::langType; }
Language Language::load(const QString& file)
{
	QByteArray data;
	if (!File::LoadText(file, data)) return {};
	auto pairmap = Format::pairmap(data.constData(), data.size(), '-', '\\');
	if (pairmap.empty()) return {};
	Language lang;
	lang.name = QFileInfo(file).baseName();
	lang.map = std::move(pairmap);
	return lang;
}