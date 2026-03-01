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
		static bool IsAttributes(const std::wstring& path, DWORD include, DWORD exclude = 0) { DWORD a = GetFileAttributesW(path.c_str()); return a != 0 && a != INVALID_FILE_ATTRIBUTES && (a & include) == include && (a & exclude) == 0; }
		static bool FileIsHide(const std::wstring& path) { return IsAttributes(path, FILE_ATTRIBUTE_HIDDEN, FILE_ATTRIBUTE_DIRECTORY); }
		static bool FolderIsHide(const std::wstring& path) { return IsAttributes(path, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_DIRECTORY); }
		static bool PathIsHide(const std::wstring& path) { return IsAttributes(path, FILE_ATTRIBUTE_HIDDEN); }
		static bool FileIsSystem(const std::wstring& path) { return IsAttributes(path, FILE_ATTRIBUTE_SYSTEM, FILE_ATTRIBUTE_DIRECTORY); }
		static bool FolderIsSystem(const std::wstring& path) { return IsAttributes(path, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY); }
		static bool PathIsSystem(const std::wstring& path) { return IsAttributes(path, FILE_ATTRIBUTE_SYSTEM); }
		static bool FileState(const std::wstring& path) { return IsAttributes(path, 0, FILE_ATTRIBUTE_DIRECTORY); }
		static bool FolderState(const std::wstring& path) { return IsAttributes(path, FILE_ATTRIBUTE_DIRECTORY); }
		static bool PathState(const std::wstring& path) { return IsAttributes(path, 0); }

		static DWORD FileSize(const std::wstring& path)
		{
			HANDLE handle = CreateFileW(path.c_str(), FILE_READ_EA, FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0);
			if (handle != INVALID_HANDLE_VALUE) {
				DWORD size = GetFileSize(handle, 0);
				CloseHandle(handle);
				return size;
			}
			return 0;
		}
		static bool FileLocked(const std::wstring& filePath) {
			HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (!hFile || hFile == INVALID_HANDLE_VALUE)
			{
				DWORD error = GetLastError();
				if (error == ERROR_SHARING_VIOLATION || error == ERROR_LOCK_VIOLATION) return true;
				return false;
			}
			CloseHandle(hFile);
			return false;
		}
		static bool UsableName(const QString& file)
		{
			for (QChar c : file) if (c == L'\\' || c == L'/' || c == L':' || c == L'*' || c == L'?' || c == L'\"' || c == L'<' || c == L'>' || c == L'|' || c == L'.') return false;
			return true;
		}
		static QString RemoveExtension(QString file)
		{
			return file.mid(0, file.lastIndexOf('.'));
		}
		static QFileInfoList Find(const QString& dir, const QString& filter)
		{
			QFileInfoList fileInfos;
			QStringList filters(filter);
			QDir d(dir);
			d.setNameFilters(filters);
			d.setFilter(QDir::Files | QDir::Hidden | QDir::System | QDir::NoSymLinks);
			QDirIterator iter(d, QDirIterator::NoIteratorFlags);
			while (iter.hasNext())
			{
				iter.next();
				fileInfos.append(iter.fileInfo());
			}
			return fileInfos;
		}
		static QString Unique(const QString& dir, const QString& name, const QString& extension, QString left = " (", QString right = ")", int begin = 1)
		{
			bool first = true;
			for (int i = begin;; first ? first = false : i++)
			{
				QString path;
				if (dir.isEmpty())
				{
					if (first) path = name + extension;
					else path = name + left + QString::number(i) + right + extension;
				}
				else
				{
					if (first) path = dir + "\\" + name + extension;
					else path = dir + "\\" + name + left + QString::number(i) + right + extension;
				}
				if (!QFile::exists(path)) return path;
			}
			return QString();
		}
		static bool FileSave(const std::wstring& path, const void* data, size_t bytes)
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
		static bool FileRead(const std::wstring& path, void* data, size_t bytes)
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
		static bool LoadText(const QString& path, QByteArray& text)
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
		static bool SaveText(const QString& path, const QByteArray& text)
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