﻿#pragma once
// qt
#include <QtWinExtras/qwinfunctions.h>
#include <qsystemtrayicon.h>
#include <qdesktopservices.h>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qdialog.h>
#include <qwidget.h>
#include <qfiledialog.h>
#include <qcolordialog.h>
#include <qbuttongroup.h>
#include <qmenu.h>
#include <qvalidator.h>
#include <qstandarditemmodel.h>
#include <qabstractitemview.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qfont.h>
#include <qevent.h>
#include <qtimer.h>
#include <qmimedata.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qurl.h>
#include <qfile.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <qthread.h>
// std
#include <variant>
#include <functional>
#include <fstream>

// quickinput
constexpr int key_info = 214;

#pragma optimize("",off)
static const char* GET_QUICKINPUT_MACKER_STRING()
{
	static const char QUICKINPUT_MACKER_STRING[] = "QUICKINPUT_(C)CHIYUKIGANA";
	return QUICKINPUT_MACKER_STRING;
}
#pragma optimize("",on)