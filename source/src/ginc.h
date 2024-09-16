#pragma once

// Change to your Qt directory
#ifdef _DEBUG
#pragma comment(lib, "C:/Qt/32/lib/Qt5WinExtrasd.lib")
#else
#pragma comment(lib, "C:/Qt/32s/lib/Qt5WinExtras.lib")
#endif

#include <QApplication>
#include <QWidget>
#include <QFont>
#include "cg/cg.h"