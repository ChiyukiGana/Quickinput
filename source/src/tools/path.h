#pragma once
#include "shlwapi.h"
#include "string.h"
#define EnvUser L"%UserProFile%"
#define EnvProgram L"%ProgramFiles%"

namespace QiTools
{
	class Path
	{
		std::wstring _dir;
		std::wstring _file;
		std::wstring _args;
	public:
		typedef enum Type
		{
			e_cmd,
			e_file,
			e_dir,
			e_args
		} Type;
		Path() { }
		Path(const wchar_t* path, const Type type = e_dir) { parse(path, type); }
		Path(std::wstring path, const Type type = e_dir) { parse(path, type); }
		Path(const Path& path) { parse(path); }
		// overload operator
	public:
		// copy
		std::wstring operator*() const { return s_cmd(); }
		Path operator=(const wchar_t* path) { parse(path, Type::e_dir); return *this; }
		Path operator=(const std::wstring& path) { parse(path, Type::e_dir); return *this; }
		Path operator=(const Path& path) { parse(path); return *this; }
		// append
		Path operator+(const wchar_t* path) { append(path, Type::e_dir); return *this; }
		Path operator+(const std::wstring& path) { append(path, Type::e_dir); return *this; }
		Path operator+(const Path& path) { append(path); return *this; }
		// remove
		Path operator-(const wchar_t* path) { remove(path, Type::e_dir); return *this; }
		Path operator-(const std::wstring& path) { remove(path, Type::e_dir); return *this; }
		Path operator-(const Path& path) { remove(path); return *this; }
		Path operator-(unsigned int step) { back(step); return *this; }
		// external opertor
	public:
		/* %ProgramFiles%  >  C:\Program Files */
		static std::wstring ExpandEnvironment(std::wstring envString = L"%ProgramFiles%")
		{
			std::wstring str(ExpandEnvironmentStringsW(envString.c_str(), 0, 0), '\0'); // alloc
			ExpandEnvironmentStringsW(envString.c_str(), &str[0], str.size()); // write
			return str;
		}
		/* slash of args will be change */
		static std::wstring toBackslash(std::wstring path)
		{
			for (size_t p = 0; p < path.length(); p++) if (path[p] == L'/') path[p] = L'\\';
			return path;
		}
		/* backslash of args will be change */
		static std::wstring toSlash(std::wstring path)
		{
			for (size_t p = 0; p < path.length(); p++) if (path[p] == L'\\') path[p] = L'/';
			return path;
		}
		/* Dir  to  Dir/File.exe */
		static std::wstring Append(std::wstring path, std::wstring path2, bool backSlash = true, bool mark = false)
		{
			path = RemoveDirMark(path);
			path2 = RemoveDirMark(path2);
			if (backSlash) path += L"\\";
			else path += L'/';
			if (mark) return AppendMark(path + path2);
			return path + path2;
		}
		/* backSpace: 'Dir/File.exe' to 'Dir/File.exe ' */
		static std::wstring AppendSpace(std::wstring path, bool backSpace = true, bool removeElse = true)
		{
			if (removeElse) path = RemoveSpace(path);
			if (backSpace) return path + L" "; // append right
			return std::wstring(1, L' ') + path; // append left
		}
		/* '  Dir/File.exe  '  to  'Dir/File.exe' */
		static std::wstring RemoveSpace(std::wstring path)
		{
			size_t c = 0;
			for (size_t p = 0; p < path.length(); p++) if (path[p] == L' ') c++; else break;
			path = path.substr(c); // '  Dir/File.exe  '  to  'Dir/File.exe  '
			c = 0;
			for (int p = path.length() - 1; p > -1; p--) if (path[p] == L' ') c++; else break;
			path = path.substr(0, path.length() - c);	// 'Dir/File.exe  '  to  'Dir/File.exe'
			return path;
		}
		static bool haveMark(std::wstring path)
		{
			path = RemoveSpace(path);
			if (!path.size()) return false;

			if (path[0] == L'\"' && path.find_first_of(L'\"', 1) != std::wstring::npos) return true; // have left mark and right mark
			else if (path[0] == L'\'' && path.find_first_of(L'\'', 1) != std::wstring::npos) return true; // have left mark and right mark
			return false; // no mark or no right mark
		}
		/* Dir/File.exe  to  "Dir/File.exe" */
		static std::wstring AppendMark(std::wstring path, std::wstring mark = L"\"")
		{
			path = RemoveSpace(path);
			if (!path.size()) return path;

			if (haveMark(path)) return path;
			return mark + path + mark;
		}
		/* "Dir/File.exe" args  to  Dir/File.exe args */
		static std::wstring RemoveMark(std::wstring path, bool keepArgs = false)
		{
			path = RemoveSpace(path);
			if (!path.size()) return path;

			size_t p = 0;
			if (path[0] == L'\"') p = path.find_first_of(L'\"', 1); // have left mark
			else if (path[0] == L'\'') p = path.find_first_of(L'\'', 1); // have left mark
			else return path; // no mark

			if (p == std::wstring::npos) return path.substr(1); // no right mark

			std::wstring str = path.substr(1, p - 1); // no args
			if (keepArgs && (p + 1 < path.length())) str += path.substr(p + 1); // have args

			return RemoveSpace(str);
		}
		/* "Dir/"  to  Dir */
		static std::wstring RemoveDirMark(std::wstring path, bool mark = false)
		{
			path = RemoveMark(path);
			if (!path.size()) return path;

			if (path[0] == L'/' || path[0] == L'\\') path = path.substr(1); // remove left mark
			if (path[path.length() - 1] == L'/' || path[path.length() - 1] == L'\\') path = path.substr(0, path.length() - 1); // remove right mark
			if (mark) return AppendMark(path);
			return path;
		}
		/* Dir/File.exe  to  Dir */
		static std::wstring GetDir(std::wstring path, bool mark = false)
		{
			path = RemoveMark(path, false);
			if (!path.size()) return path;

			size_t s = path.find_last_of(L'/');
			size_t bs = path.find_last_of(L'\\');
			if (s != std::wstring::npos)
				path = path.substr(0, s);
			else if (bs != std::wstring::npos)
				path = path.substr(0, bs);
			else path = L"";

			if (mark) return AppendMark(path, L"\"");
			return path;
		}
		/* Dir/File.exe  to  File.exe */
		static std::wstring RemoveDir(std::wstring path, bool mark = false)
		{
			path = RemoveMark(path, false);
			if (!path.size()) return path;

			size_t s = path.find_last_of(L'/');
			size_t bs = path.find_last_of(L'\\');
			if (s != std::wstring::npos)
				path = path.substr(s + 1);
			else if (bs != std::wstring::npos)
				path = path.substr(bs + 1);

			if (mark) return AppendMark(path, L"\"");
			return path;
		}
		/* Dir/File.exe  to  File.exe */
		static std::wstring GetFile(std::wstring path, bool mark = false)
		{
			path = RemoveMark(path, false);
			if (!path.size()) return path;

			size_t s = path.find_last_of(L'/');
			size_t bs = path.find_last_of(L'\\');
			if (s != std::wstring::npos)
				path = path.substr(s + 1);
			else if (bs != std::wstring::npos)
				path = path.substr(bs + 1);

			if (mark) return AppendMark(path, L"\"");
			return path;
		}
		/* Dir/File.exe  to  Dir */
		static std::wstring RemoveFile(std::wstring path, bool mark = false)
		{
			path = RemoveMark(path, false);
			if (!path.size()) return path;

			size_t s = path.find_last_of(L'/');
			size_t bs = path.find_last_of(L'\\');
			if (s != std::wstring::npos)
				path = path.substr(0, s);
			else if (bs != std::wstring::npos)
				path = path.substr(0, bs);
			else path = L"";

			if (mark) return AppendMark(path, L"\"");
			return path;
		}
		/* Dir/File.exe  to  .exe */
		static std::wstring GetExtension(std::wstring path, bool mark = false)
		{
			path = GetFile(path, false);
			if (!path.size()) return L"";

			size_t p = path.find_last_of(L'.');
			if (p != std::wstring::npos) path = path.substr(p);

			if (mark) return AppendMark(path, L"\"");
			return path;
		}
		/* Dir/File.exe  to  Dir/File */
		static std::wstring RemoveExtension(std::wstring path, bool mark = false)
		{
			path = RemoveMark(path, false);
			if (!path.size()) return L"";

			size_t s = path.find_last_of(L'/');
			size_t bs = path.find_last_of(L'\\');
			if (s != std::wstring::npos) bs = s;
			else if (bs != std::wstring::npos);
			else bs = 0;
			s = path.find_last_of(L'.');
			if (s != std::wstring::npos && s > bs) path = path.substr(0, s);

			if (mark) return AppendMark(path);
			return path;
		}
		/* Dir/File.exe args  to  args */
		static std::wstring GetArgs(std::wstring path)
		{
			path = RemoveSpace(path);
			if (!path.size()) return path;

			if (haveMark(path))
			{
				size_t p = 0;
				if (path[0] == L'\"') p = path.find_first_of(L'\"', 1); // have left mark
				else if (path[0] == L'\'') p = path.find_first_of(L'\'', 1); // have left mark
				else return L"";

				if (p == std::wstring::npos) return L""; // no right mark
				if (p + 1 < path.length()) return RemoveSpace(path.substr(p + 1)); // have args
				return L""; // no length
			}
			else
			{
				size_t p = path.find_first_of(L' ');
				if (p == std::wstring::npos) return L""; // no args
				if (p + 1 < path.length()) return RemoveSpace(path.substr(p + 1)); // have args
				return L""; // no length
			}
		}
		/* Dir/File.exe args  to  Dir/File.exe args */
		static std::wstring RemoveArgs(std::wstring path, bool mark = true)
		{
			path = RemoveSpace(path);
			if (!path.size()) return path;

			if (haveMark(path))
			{
				if (mark) return AppendMark(RemoveMark(path, false));
				else return RemoveMark(path, false);
			}
			else
			{
				size_t p = path.find_first_of(L' ');
				if (p == std::wstring::npos) return path; // no args
				return RemoveSpace(path.substr(0, p)); // have args
			}
		}
		static bool PathState(std::wstring path) { if (_waccess(path.c_str(), 0)) return false; return true; }
		/* "1/2/3"  to  "3" */
		static std::wstring Last(std::wstring path) {
			size_t p = path.find_last_of(L"\\");
			if (p != std::wstring::npos) return path.substr(p + 1);
			return path;
		}
		/* "1/2/3"  to  "1/2" */
		static std::wstring Prev(std::wstring path) {
			size_t p = path.find_last_of(L"\\");
			if (p != std::wstring::npos) return path.substr(0, p);
			return path;
		}
		// internal operator
	public:
		void parse(std::wstring path, const Type type = Type::e_dir)
		{
			switch (type)
			{
			case Type::e_args:
			{
				_args = RemoveSpace(path);						// 'args  '  to  'args'
			}
			break;
			case Type::e_dir:
			{
				_dir = toSlash(RemoveDirMark(path, false));		// "/Dir"  to  Dir
			}
			break;
			case Type::e_file:
			{
				_dir = toSlash(RemoveDirMark(GetDir(path)));	// "/Dir/File.exe"  to  Dir
				_file = GetFile(path);							// /Dir/File.exe  to  File.exe
			}
			break;
			case Type::e_cmd:
			{
				_dir = toSlash(RemoveDirMark(GetDir(path)));	// "/Dir/File.exe" args  to  Dir
				_file = GetFile(path);							// "/Dir/File.exe" args  to  File.exe
				_args = GetArgs(path);							// "/Dir/File.exe" args  to  args
			}
			}
		}
		void parse(const Path& path)
		{
			_dir = path._dir;
			_file = path._file;
			_args = path._args;
		}
		void append(std::wstring path, const Type type = Type::e_dir)
		{
			switch (type)
			{
			case Type::e_args:
			{
				_args += AppendSpace(path, false, true);								// args + p  =  args p
			}
			break;
			case Type::e_dir:
			{
				_dir += std::wstring(1, L'/') + toSlash(RemoveDirMark(path));			// Dir + /L2/  =  Dir/L2
			}
			break;
			case Type::e_file:
			{
				std::wstring str = toSlash(RemoveDirMark(GetDir(path)));
				if (str.size()) _dir += std::wstring(1, L'/') + str;					// Dir + /L2/File.exe  =  Dir/L2
				_file = GetFile(path);													// /L2/File.exe  =  File.exe
			}
			break;
			case Type::e_cmd:
			{
				_dir += std::wstring(1, L'/') + toSlash(RemoveDirMark(GetDir(path)));	// Dir + "/L2/File.exe" args  =  Dir/L2
				_file = GetFile(path);													// "/L2/File.exe" args  =  File.exe
				_args = GetArgs(path);													// "/L2/File.exe" args  =  args
			}
			}
		}
		void append(const Path& path)
		{
			_dir += std::wstring(1, L'/') + path._dir;
			_file = path._file;
			_args = path._args;
		}
		void remove(std::wstring path, const Type type = Type::e_dir)
		{
			switch (type)
			{
			case Type::e_args:
			{
				path = RemoveSpace(path);
				if (!path.size()) return;

				size_t p = _args.find(path);
				if (p != std::wstring::npos) _args.replace(p, path.size(), L"");
				else return;
			}
			break;
			case Type::e_dir:
			{
				path = RemoveDirMark(path);
				if (!path.size()) return;

				size_t p = _dir.find(path);
				if (p != std::wstring::npos)
				{
					if (p + path.size() < _dir.size() - 1 && _dir[p + path.size()] == L'/') _dir.replace(p, path.size() + 1, L"");
					else _dir.replace(p, path.size(), L"");
					_dir = RemoveDirMark(_dir);
				}
				else return;
			}
			break;
			case Type::e_file:
			{
				path = RemoveSpace(path);
				if (!path.size()) return;

				std::wstring str;
				if ((str = RemoveDirMark(GetDir(path))).size())
				{
					size_t p = _dir.find(str);
					if (p != std::wstring::npos)
					{
						if (p + str.size() < _dir.size() - 1 && _dir[p + str.size()] == L'/') _dir.replace(p, str.size() + 1, L"");
						else _dir.replace(p, str.size(), L"");
						_dir = RemoveDirMark(_dir);
					}
					else return;
				}

				if ((str = GetFile(path)).size())
				{
					if (str == _file) _file = L"";
				}
			}
			break;
			case Type::e_cmd:
			{
				path = RemoveSpace(path);
				if (!path.size()) return;

				std::wstring str;

				if ((str = RemoveDirMark(GetDir(path))).size())
				{
					size_t p = _dir.find(str);
					if (p != std::wstring::npos)
					{
						if (p + str.size() < _dir.size() - 1 && _dir[p + str.size()] == L'/') _dir.replace(p, str.size() + 1, L"");
						else _dir.replace(p, str.size(), L"");
						_dir = RemoveDirMark(_dir);
					}
					else return;
				}

				if ((str = GetFile(path)).size())
				{
					if (str == _file) _file = L"";
				}

				if ((str = GetArgs(path)).size())
				{
					size_t p = _args.find(str);
					if (p != std::wstring::npos) _args.replace(p, str.size(), L"");
					else return;
				}
			}
			}
		}
		void remove(const Path& path)
		{
			size_t p = _dir.find(path._dir);
			if (p != std::wstring::npos)
			{
				if (p + path._dir.size() < _dir.size() && _dir[p + path._dir.size() < _dir.size()] == L'/') _dir.replace(p, p + path._dir.size(), L""); // remove dir and /
				else _dir.replace(p, p + path._dir.size() - 1, L""); // remove dir
			}

			if (_file == path._file) _file = L"";

			p = _args.find(path._args);
			if (p != std::wstring::npos)
			{
				if (p + path._args.size() < _args.size() && _args[p + path._args.size() < _args.size()] == L'/') _args.replace(p, p + path._args.size(), L""); // remove dir and /
				_args.replace(p, p + path._args.size() - 1, L"");
			}
		}
		void back(unsigned int step = 1)
		{
			if (_file.size()) _file = L"", step--;

			while (step)
			{
				size_t s = _dir.find_last_of(L'/');
				if (s != std::wstring::npos) _dir = _dir.substr(0, s);
				else
				{
					_dir = L"";
					break;
				}
				step--;
			}
		}
		void dropDir() { _dir.resize(0); }
		void dropFile() { _file.resize(0); }
		void dropArgs() { _args.resize(0); }
		// output
	public:
		/* mark: "Dir" , backSlash: Dir\1.exe */
		std::wstring s_cmd(bool mark = true, bool backSlash = true) const
		{
			std::wstring path;

			if (mark) path += L"\"";							// "

			if (backSlash) path += toBackslash(_dir) + L'\\';	// "Dir\¡¡
			else path += _dir + L"/";							// "Dir/

			path += _file;										// "Dir/File.exe

			if (mark) path += L"\"";							// "Dir/File.exe"

			path += L" " + _args;								// "Dir/File.exe" args

			return path;
		}
		/* mark: "Dir" , backSlash: Dir\1.exe */
		std::wstring s_path(bool mark = false, bool backSlash = true) const
		{
			std::wstring path;

			if (mark) path += L"\"";							// "

			if (backSlash) path += toBackslash(_dir) + L'\\';	// "Dir\¡¡
			else path += _dir + L"/";							// "Dir/

			path += _file;										// "Dir/File.exe

			if (mark) path += L"\"";							// "Dir/File.exe"

			return path;
		}
		/* mark: "Dir" , backSlash: Dir\1.exe */
		std::wstring s_dir(bool mark = false, bool backSlash = true) const
		{
			std::wstring path;

			if (mark) path += L"\"";							// "

			if (backSlash) path += toBackslash(_dir);			// "Dir\Dir
			else path += _dir;									// "Dir/Dir

			if (mark) path += L"\"";							// "Dir/Dir"

			return path;
		}
		/* mark: "Dir" */
		std::wstring s_file(bool mark = false) const
		{
			std::wstring path;

			if (mark) path += L"\"";							// "

			path += _file;										// "File.exe

			if (mark) path += L"\"";							// "File.exe"

			return path;
		}
		std::wstring s_args() const
		{
			return _args;
		}
	};
}