#pragma once
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

	MacroUi* wm = 0;
	TriggerUi* wt = 0;
	FuncUi* wf = 0;
	SettingsUi* ws = 0;
	AboutUi* wa = 0;

	QPoint msPos;

public:
	MainUi() : QMainWindow()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		WidInit();
		ReStyle();
	}

	void ReStyle()
	{
		setStyleSheet("");
		setStyleSheet(Global::qi.styles[Global::qi.set.style].style);
		
		menu->setStyleSheet(Global::qi.styles[Global::qi.set.style].style);
		wm->ReStyle();
		wt->ReStyle();
		wf->ReStyle();
		ws->ReStyle();
		wa->ReStyle();
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
		tray->setToolTip(u8"Quick input");
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
		if (et->type() == QEvent::User)
		{
			ReStyle();
		}
		else if (et->type() == QEvent::WindowActivate)
		{
			if (Global::qi.state) QiState(false);
			HookState(false);
		}
		else if (et->type() == QEvent::WindowDeactivate)
		{
			if (!((MacroUi*)wm)->working && !((FuncUi*)wf)->working)
			{
				if (Global::qi.set.defOn) QiState(true);
				HookState(true);
			}
		}
		return QWidget::event(et);
	}
	void showEvent(QShowEvent* et) { SetForegroundWindow((HWND)QWidget::winId()); }
	// Move
	void mousePressEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) msPos = et->pos(); }
	void mouseMoveEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - msPos); }

private slots:
	void OnTrayClick(QSystemTrayIcon::ActivationReason reason) { if (reason == QSystemTrayIcon::Trigger) setWindowState(Qt::WindowNoState), show(); }
	void OnMenuTnon() { if (!((MacroUi*)wm)->working && !((FuncUi*)wf)->working) QiState(1), HookState(1); }
	void OnMenuTnoff() { QiState(0); HookState(0); }
	void OnMenuShow() { setWindowState(Qt::WindowNoState), show(); }
	void OnMenuHide() { hide(); }
	void OnMenuExit() { exit(0); }

	void OnBnClose() { exit(0); }
	void OnBnMin() { setWindowState(Qt::WindowMinimized); }
	void OnBnHide() { hide(); }
};