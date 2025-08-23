#pragma once
#include <src/inc_header.h>
#include "VarViewUi.h"
#include "MsgViewUi.h"
#include "ui_MainUi.h"
class MainUi : public QMainWindowFrameless
{
	Q_OBJECT;
	using This = MainUi;
	Ui::MainUiClass ui;

	QSystemTrayIcon* tray = nullptr;
	QMenu* menu = nullptr;
	QAction* ac_on = nullptr;
	QAction* ac_off = nullptr;
	QAction* ac_show = nullptr;
	QAction* ac_hide = nullptr;
	QAction* ac_exit = nullptr;

	VarViewUi varView;
	MsgViewUi msgView;

public:
	MainUi(int tab = 0);
	QString Version() const;

private:
	void Init();
	void Event();
	void StyleGroup();

	bool event(QEvent*);
	bool eventFilter(QObject*, QEvent*);
	void showEvent(QShowEvent*);
};