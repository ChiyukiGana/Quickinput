#pragma once
#include "inc_external.h"
// quickinput tools
#include <src/tools/vector.h>
#include <src/tools/base.h>
#include <src/tools/string.h>
#include <src/tools/path.h>
#include <src/tools/file.h>
#include <src/tools/window.h>
#include <src/tools/msgbox.h>
#include <src/tools/system.h>
#include <src/tools/xboxpad.h>
#include <src/tools/input.h>
#include <src/tools/ihook.h>
#include <src/tools/sound.h>
#include <src/tools/color.h>
#include <src/tools/image.h>
#include <src/tools/process.h>
#include <src/tools/thread.h>
#include <src/tools/threadqueue.h>
#include <src/tools/threadpool.h>
#include <src/tools/task.h>
#include <src/tools/time.h>
#include <src/tools/texteditbox.h>
#include <src/tools/csvtool.h>
using namespace QiTools;
// custom widget
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
	inline const QString dir = QDir::fromNativeSeparators(QString::fromWCharArray(Process::runPath().c_str()));
	inline const QString folder = dir.mid(dir.lastIndexOf('/') + 1);
	inline const QString macroDir = dir + "/macro/";
	inline const QString macroType = ".json";
	inline const QString configFile = "QuickInput.json";
}