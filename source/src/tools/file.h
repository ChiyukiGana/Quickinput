#pragma once
#include <windows.h>
#include <qstring.h>
#include <qdir.h>
#include <qdiriterator.h>
#include <qfile.h>
namespace QiTools
{
	class File
	{
	public:
		static bool UsableName(const QString file)
		{
			for (QChar c : file) if (c == L'\\' || c == L'/' || c == L':' || c == L'*' || c == L'?' || c == L'\"' || c == L'<' || c == L'>' || c == L'|' || c == L'.') return false;
			return true;
		}
		static QString RemoveExtension(QString file)
		{
			return file.mid(0, file.lastIndexOf('.'));
		}
		static QFileInfoList Find(const QString dir, const QString& filter)
		{
			QFileInfoList fileInfos;
			QStringList filters(filter);
			QDir d(dir);
			d.setNameFilters(filters);
			QDirIterator iter(d, QDirIterator::NoIteratorFlags);
			while (iter.hasNext())
			{
				iter.next();
				fileInfos.append(iter.fileInfo());
			}
			return fileInfos;
		}
		static QString Unique(const QString dir, const QString& name, const QString& extension, QString left = " (", QString right = ")", int begin = 1)
		{
			for (int i = begin;; i++)
			{
				QString path = dir + "/" + name + left + QString::number(i) + right + extension;
				if (!QFile::exists(path)) return path;
			}
			return QString();
		}
		static bool LoadText(QString path, QByteArray& text)
		{
			QFile file(path);
			if (file.open(QFile::ReadOnly | QFile::Text))
			{
				text = file.readAll();
				file.close();
				return true;
			}
			return false;
		}
		static bool SaveText(QString path, const QByteArray& text)
		{
			QFile file(path);
			if (file.open(QFile::WriteOnly | QFile::Text))
			{
				file.write(text);
				file.close();
				return true;
			}
			return false;
		}
	};
}