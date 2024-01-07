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

	QWidget* wm = 0;
	QWidget* wt = 0;
	QWidget* wf = 0;
	QWidget* ws = 0;
	QWidget* wa = 0;

	QString styleOn = R"(QPushButton{background-color:#CEF;border:none;font-family:"Microsoft YaHei";font-size:18px;})";
	QString styleOff = R"(QPushButton{background-color:#ADE;border:none;font-family:"Microsoft YaHei";font-size:18px;}QPushButton:hover{background-color: #C0E2F2;})";

	QPoint msPos;

public:
	MainUi() : QMainWindow()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		setAttribute(Qt::WA_TranslucentBackground);
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

			wm = new MacroUi(this);
			wt = new TriggerUi(this);
			wf = new FuncUi(this);
			ws = new SettingsUi(this);
			wa = new AboutUi(this);
			wm->move(1, 71);
			wt->move(1, 71);
			wf->move(1, 71);
			ws->move(1, 71);
			wa->move(1, 71);
		}

		{
			connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(OnTrayClick(QSystemTrayIcon::ActivationReason)));
			connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
			connect(ui.bnMin, SIGNAL(clicked()), this, SLOT(OnBnMin()));
			connect(ui.bnHide, SIGNAL(clicked()), this, SLOT(OnBnHide()));

			connect(ui.bnScript, SIGNAL(clicked()), this, SLOT(OnBnScript()));
			connect(ui.bnTrigger, SIGNAL(clicked()), this, SLOT(OnBnTrigger()));
			connect(ui.bnFunc, SIGNAL(clicked()), this, SLOT(OnBnFunc()));
			connect(ui.bnSettings, SIGNAL(clicked()), this, SLOT(OnBnSettings()));
			connect(ui.bnAbout, SIGNAL(clicked()), this, SLOT(OnBnAbout()));
		}
		MenuInit();
		ShowWnd(wm);
	}

	void ShowWnd(QWidget* wnd)
	{
		if (!wnd) return;
		if (wm) wm->hide();
		if (wt) wt->hide();
		if (wf) wf->hide();
		if (ws) ws->hide();
		if (wa) wa->hide();

		ui.bnScript->setStyleSheet(styleOff);
		ui.bnTrigger->setStyleSheet(styleOff);
		ui.bnFunc->setStyleSheet(styleOff);
		ui.bnSettings->setStyleSheet(styleOff);
		ui.bnAbout->setStyleSheet(styleOff);

		if (wnd == wm) ui.bnScript->setStyleSheet(styleOn);
		else if (wnd == wt) ui.bnTrigger->setStyleSheet(styleOn);
		else if (wnd == wf) ui.bnFunc->setStyleSheet(styleOn);
		else if (wnd == ws) ui.bnSettings->setStyleSheet(styleOn);
		else if (wnd == wa) ui.bnAbout->setStyleSheet(styleOn);

		wnd->show();
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
	void closeEvent(QCloseEvent*)
	{
		wm->close();
		wt->close();
		wf->close();
		ws->close();
		wa->close();
	}
	// Move
	void mousePressEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) msPos = et->pos(); }
	void mouseMoveEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - msPos); }

private slots:
	void OnTrayClick(QSystemTrayIcon::ActivationReason reason) { if (reason == QSystemTrayIcon::Trigger) setWindowState(Qt::WindowNoState), show(), Window::Top((HWND)QWidget::winId()); }
	void OnMenuTnon() { if (!((MacroUi*)wm)->working && !((FuncUi*)wf)->working) QiState(1), HookState(1); }
	void OnMenuTnoff() { QiState(0); HookState(0); }
	void OnMenuShow() { setWindowState(Qt::WindowNoState), show(), Window::Top((HWND)QWidget::winId()); }
	void OnMenuHide() { hide(); }
	void OnMenuExit() { close(); }

	void OnBnClose() { close(); }
	void OnBnMin() { setWindowState(Qt::WindowMinimized); }
	void OnBnHide() { hide(); }
	void OnBnScript() { ShowWnd(wm); }
	void OnBnTrigger() { ShowWnd(wt); }
	void OnBnFunc() { ShowWnd(wf); }
	void OnBnSettings() { ShowWnd(ws); }
	void OnBnAbout() { ShowWnd(wa); }
};