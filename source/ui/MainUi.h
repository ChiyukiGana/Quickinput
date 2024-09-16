#pragma once
#include <qsystemtrayicon.h>
#include <qmenu.h>
#include "MacroUi.h"
#include "TriggerUi.h"
#include "FuncUi.h"
#include "SettingsUi.h"
#include "AboutUi.h"
#include "../src/minc.h"
#include "ui_MainUi.h"

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
		qis.widget.main = this;
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		WidInit();
	}

	void ReStyle()
	{
		setStyleSheet("");
		setStyleSheet(qis.ui.themes[qis.set.theme].style);
		menu->setStyleSheet("");
		menu->setStyleSheet(qis.ui.themes[qis.set.theme].style);
		wm->ReStyle();
		wt->ReStyle();
		wf->ReStyle();
		ws->ReStyle();
	}

private:
	void MenuInit()
	{
		menu = new QMenu(this);
		QAction* tnon = new QAction(qis.ui.text.muOn, this);
		QAction* tnoff = new QAction(qis.ui.text.muOff, this);
		QAction* show = new QAction(qis.ui.text.muShow, this);
		QAction* hide = new QAction(qis.ui.text.muHide, this);
		QAction* exit = new QAction(qis.ui.text.muExit, this);
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

	void customEvent(QEvent* et)
	{
		if (et->type() == QiEvent::setTheme) ReStyle();
	}

	bool event(QEvent* et)
	{

		if (et->type() == QEvent::WindowActivate)
		{
			qis.widget.mainActive = true;
			if (qis.state) QiFn::QiState(false);
			QiFn::QiHook(false);
		}
		else if (et->type() == QEvent::WindowDeactivate)
		{
			qis.widget.mainActive = false;
			if (QiFn::SelfActive())
			{
				if (qis.set.defOn) QiFn::QiState(true);
				QiFn::QiHook(true);
			}
		}
		return QWidget::event(et);
	}
	void showEvent(QShowEvent* et) { SetForegroundWindow((HWND)QWidget::winId()); ReStyle(); }
	// Move
	void mousePressEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) msPos = et->pos(); }
	void mouseMoveEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - msPos); }

private slots:
	void OnTrayClick(QSystemTrayIcon::ActivationReason reason) { if (reason == QSystemTrayIcon::Trigger) setWindowState(Qt::WindowNoState), show(); }
	void OnMenuTnon() { if (QiFn::SelfActive()) QiFn::QiState(true), QiFn::QiHook(true); }
	void OnMenuTnoff() { QiFn::QiState(false); QiFn::QiHook(false); }
	void OnMenuShow() { setWindowState(Qt::WindowNoState), show(); }
	void OnMenuHide() { hide(); }
	void OnMenuExit() { exit(0); }

	void OnBnClose() { exit(0); }
	void OnBnMin() { setWindowState(Qt::WindowMinimized); }
	void OnBnHide() { hide(); }
};