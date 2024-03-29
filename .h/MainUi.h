﻿#pragma once
#include <qsystemtrayicon.h>
#include <qmenu.h>
#include "MacroUi.h"
#include "TriggerUi.h"
#include "FuncUi.h"
#include "SettingsUi.h"
#include "AboutUi.h"
#include "ui_MainUi.h"
#include "../static.h"

class MainUi : public QMainWindow
{
	Q_OBJECT;
	Ui::MainUiClass ui;
	QSystemTrayIcon* tray = 0;
	QMenu* menu = 0;

	QWidget* wm = 0;
	QWidget* wt = 0;
	QWidget* wf = 0;
	QWidget* ws = 0;
	QWidget* wa = 0;

	QPoint msPos;

public:
	MainUi() : QMainWindow()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		WidInit();
	}

private:
	void MenuInit()
	{
		menu = new QMenu(this);
		QAction* tnon = new QAction(UI::muOn, this);
		QAction* tnoff = new QAction(UI::muOff, this);
		QAction* show = new QAction(UI::muShow, this);
		QAction* hide = new QAction(UI::muHide, this);
		QAction* exit = new QAction(UI::muExit, this);
		menu->addAction(tnon);
		menu->addAction(tnoff);
		menu->addAction(show);
		menu->addAction(hide);
		menu->addAction(exit);
		tray->setContextMenu(menu);
		connect(tnon, SIGNAL(triggered()), this, SLOT(OnMenuTnon()));
		connect(tnoff, SIGNAL(triggered()), this, SLOT(OnMenuTnoff()));
		connect(show, SIGNAL(triggered()), this, SLOT(OnMenuShow()));
		connect(hide, SIGNAL(triggered()), this, SLOT(OnMenuHide()));
		connect(exit, SIGNAL(triggered()), this, SLOT(OnMenuExit()));
	}
	void WidInit()
	{
		{
			tray = new QSystemTrayIcon(this);
			tray->setIcon(QIcon(":/icon.png"));
			tray->show();

			wm = new MacroUi(ui.tabMacro);
			wt = new TriggerUi(ui.tabTrigger);
			wf = new FuncUi(ui.tabFunc);
			ws = new SettingsUi(ui.tabSettings);
			wa = new AboutUi(ui.tabAbout);
		}

		{
			connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(OnTrayClick(QSystemTrayIcon::ActivationReason)));
			connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
			connect(ui.bnMin, SIGNAL(clicked()), this, SLOT(OnBnMin()));
			connect(ui.bnHide, SIGNAL(clicked()), this, SLOT(OnBnHide()));
		}
		MenuInit();
	}

	bool event(QEvent* et)
	{
		if (et->type() == QEvent::WindowActivate)
		{
			if (Global::qi.state) QiState(0);
			HookState(0);
		}
		else if (et->type() == QEvent::WindowDeactivate)
		{
			if (!((MacroUi*)wm)->working && !((FuncUi*)wf)->working)
			{
				if (Global::qi.set.defOn) QiState(1);
				HookState(1);
			}
		}
		return QWidget::event(et);
	}
	void showEvent(QShowEvent* et) { if (et->type() == Qt::WindowNoState) { Window::Top((HWND)QWidget::winId()); } }
	// Move
	void mousePressEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) msPos = et->pos(); }
	void mouseMoveEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - msPos); }

private slots:
	void OnTrayClick(QSystemTrayIcon::ActivationReason reason) { if (reason == QSystemTrayIcon::Trigger) setWindowState(Qt::WindowNoState), show(), Window::Top((HWND)winId()); }
	void OnMenuTnon() { if (!((MacroUi*)wm)->working && !((FuncUi*)wf)->working) QiState(1), HookState(1); }
	void OnMenuTnoff() { QiState(0); HookState(0); }
	void OnMenuShow() { setWindowState(Qt::WindowNoState), show(), Window::Top((HWND)winId()); }
	void OnMenuHide() { hide(); }
	void OnMenuExit() { exit(0); }

	void OnBnClose() { exit(0); }
	void OnBnMin() { setWindowState(Qt::WindowMinimized); }
	void OnBnHide() { hide(); }
};