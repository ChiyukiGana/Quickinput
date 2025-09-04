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
// ocr
#include <src/ocr/QiOcrInterface.h>

// quickinput
namespace Qi
{
	constexpr int key_info = 214;
	constexpr int key_size = XBoxPad::end;
	constexpr int msg_exit = (WM_USER + 0xFF);
	inline bool run = false;
	inline bool debug = false;
	inline const QString dir = QDir::fromNativeSeparators(QString::fromWCharArray(Path::RemoveFile(Process::exePath()).c_str()));
	inline const QString folder = dir.mid(dir.lastIndexOf('/') + 1);
	inline const QString macroDir = dir + "/macro/";
	inline const QString macroType = ".json";
	inline const QString configFile = "QuickInput.json";
	inline QString version;

	bool IsActive();
	void PrecSleep(clock_t ms);
	bool PeekExitMsg();
	bool PeekSleep(clock_t ms);
}