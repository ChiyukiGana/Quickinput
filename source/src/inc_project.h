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

#ifdef DEBUG
#pragma comment(lib,"msvcrtd.lib")
#else
#pragma comment(lib,"msvcrt.lib")
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
	inline const QString dir = QDir::fromNativeSeparators(QString::fromWCharArray(Path::RemoveFile(Process::exePath()).c_str()));
	inline const QString folder = dir.mid(dir.lastIndexOf('/') + 1);
	inline const QString macroDir = dir + "/macro/";
	inline const QString macroType = ".json";
	inline const QString emacroType = ".emacro";
	inline const QString configFile = "QuickInput.json";
	inline QString version;
	inline HICON ico = LoadIconW(GetModuleHandleW(nullptr), L"ICOAPP");
}