#pragma once
#include "inc_external.h"
#include <src/tools/tools.h>
// custom widget
#include <QFrameless.h>
#include <QKeyEdit.h>
#include <QPopText.h>
#include <QWindowSelection.h>
#include <QColorSelection.h>
#include <QPointSelection.h>
#include <QRectSelection.h>
#include <QDistanceSelection.h>
#include <QPointView.h>
#include <QRectView.h>
#include <QTextDialog.h>
// modules
#include <src/ocr/QiOcrInterface.h>
#ifdef Q_RAWINPUT
#include <rawin/QiRawInput.h>
#endif

// for curl release
#ifdef DEBUG
#pragma comment(lib,"msvcrtd.lib")
#endif

// quickinput
namespace Qi
{
	constexpr int key_info = 214;
#ifdef Q_KEYEDIT_PAD_ENABLED
	constexpr int key_size = XBoxPad::end;
#else
	constexpr int key_size = 255;
#endif
	constexpr int ocr_thread_max = 8;
	inline bool run = false;
	inline bool debug = false;
	inline const QString dir = QDir::fromNativeSeparators(QString::fromStdWString(Path::RemoveFile(Process::exePath())));
	inline const QString folder = dir.mid(dir.lastIndexOf('/') + 1);
	inline const QString macroDir = dir + "/macro/";
	inline const QString macroType = ".json";
	inline const QString macroQimType = ".qim";
	inline const QString emacroType = ".emacro";
	inline const QString savedVarFile = "QuickInput.var";
	inline const QString configFile = "QuickInput.json";
	inline QString version;
	inline HICON ico = LoadIconW(GetModuleHandleW(nullptr), L"ICOAPP");
	void init();
	[[noreturn]] void exit(int code = 0);
}