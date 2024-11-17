#pragma once

#include <windows.h>
#include "file.h"

namespace CG {
	class Ini {
		std::wstring path;
		std::wstring app;
	public:
		Ini() {}

		// absolutePath
		Ini(std::wstring path, std::wstring app) { this->path = path; this->app = app; }

		// absolutePath
		void Path(std::wstring path) { this->path = path; }

		// appName
		void App(LPCWSTR app) { this->app = app; }

		// Empty appDate
		bool Empty() {
			if (!path.length()) return 0;
			if (!app.length()) return 0;
			return WritePrivateProfileStringW(app.c_str(), 0, 0, path.c_str());
		}

		// Delete iniFile
		bool Delete() {
			if (!path.length()) return 0;
			if (!app.length()) return 0;
			File::FileDelete(path.c_str());
		}

		// if keyName = 0 will be clear appData, if want to clear appData please call the Ini::Empty()
		bool Write(LPCWSTR keyName, LPCWSTR keyValue) {
			if (!path.length()) return 0;
			if (!app.length()) return 0;
			File::FileCreate(path.c_str());
			return WritePrivateProfileStringW(app.c_str(), keyName, keyValue, path.c_str());
		}

		// if keyName = 0 will be clear appData, if want to clear appData please call the Ini::Empty()
		bool WriteNum(LPCWSTR keyName, int keyValue) {
			if (!path.length()) return 0;
			if (!app.length()) return 0;
			File::FileCreate(path.c_str());
			return WritePrivateProfileStringW(app.c_str(), keyName, String::toWString(keyValue).c_str(), path.c_str());
		}

		LPCWSTR Read(LPCWSTR keyName, LPCWSTR defaultValue = L"") {
			if (!path.length()) return 0;
			if (!app.length()) return 0;
			WCHAR value[1024];
			GetPrivateProfileStringW(app.c_str(), keyName, defaultValue, value, 1024, path.c_str());
			return value;
		}

		int ReadNum(LPCWSTR keyName, int defaultValue = 0) {
			if (!path.length()) return 0;
			if (!app.length()) return 0;
			return GetPrivateProfileIntW(app.c_str(), keyName, defaultValue, path.c_str());
		}
	};
}