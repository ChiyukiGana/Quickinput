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
		static bool FileIsHide(std::wstring path)
		{
			DWORD file = GetFileAttributesW(path.c_str());
			if ((file != INVALID_FILE_ATTRIBUTES) && (file != FILE_ATTRIBUTE_DIRECTORY) && (file == FILE_ATTRIBUTE_HIDDEN)) return true;
			return false;
		}
		static bool FolderIsHide(std::wstring path)
		{
			DWORD file = GetFileAttributesW(path.c_str());
			if ((file != INVALID_FILE_ATTRIBUTES) && (file == FILE_ATTRIBUTE_DIRECTORY) && (file == FILE_ATTRIBUTE_HIDDEN)) return true;
			return false;
		}
		static bool PathIsHide(std::wstring path)
		{
			DWORD file = GetFileAttributesW(path.c_str());
			if ((file != INVALID_FILE_ATTRIBUTES) && (file == FILE_ATTRIBUTE_HIDDEN)) return true;
			return false;
		}
		static bool FileIsSystem(std::wstring path)
		{
			DWORD file = GetFileAttributesW(path.c_str());
			if ((file != INVALID_FILE_ATTRIBUTES) && (file != FILE_ATTRIBUTE_DIRECTORY) && (file == FILE_ATTRIBUTE_SYSTEM)) return true;
			return false;
		}
		static bool FolderIsSystem(std::wstring path)
		{
			DWORD file = GetFileAttributesW(path.c_str());
			if ((file != INVALID_FILE_ATTRIBUTES) && (file == FILE_ATTRIBUTE_DIRECTORY) && (file == FILE_ATTRIBUTE_SYSTEM)) return true;
			return false;
		}
		static bool PathIsSystem(std::wstring path)
		{
			DWORD file = GetFileAttributesW(path.c_str());
			if ((file != INVALID_FILE_ATTRIBUTES) && (file == FILE_ATTRIBUTE_SYSTEM)) return true;
			return false;
		}
		static bool FileState(std::wstring path)
		{
			DWORD file = GetFileAttributesW(path.c_str());
			if ((file != INVALID_FILE_ATTRIBUTES) && (file != FILE_ATTRIBUTE_DIRECTORY)) return true;
			return false;
		}
		static bool FolderState(std::wstring path)
		{
			DWORD file = GetFileAttributesW(path.c_str());
			if ((file != INVALID_FILE_ATTRIBUTES) && (file == FILE_ATTRIBUTE_DIRECTORY)) return true;
			return false;
		}
		static bool PathState(std::wstring path)
		{
			DWORD file = GetFileAttributesW(path.c_str());
			if (file != INVALID_FILE_ATTRIBUTES) return true;
			return false;
		}
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
		static bool FileSave(std::wstring path, const void* data, size_t bytes)
		{
			DeleteFileW(path.c_str());
			HANDLE hFile = CreateFileW(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile)
			{
				DWORD bytesWrited;
				BOOL b = WriteFile(hFile, data, bytes, &bytesWrited, 0);
				CloseHandle(hFile);
				return b;
			}
			return false;
		}
		static bool FileRead(std::wstring path, void* data, size_t bytes)
		{
			HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile)
			{
				DWORD bytesReaded;
				BOOL b = ReadFile(hFile, data, bytes, &bytesReaded, 0);
				CloseHandle(hFile);
				return b;
			}
			return false;
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