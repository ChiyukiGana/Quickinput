#pragma execution_character_set("utf-8")
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
	QSystemTrayIcon* tray = nullptr;
	QMenu* menu = nullptr;
public:
	MainUi(int tab = 0) : QMainWindow()
	{
		Qi::widget.main = this;
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		Init();
		ui.tabWidget->setCurrentIndex(tab);
		Qi::application->setStyleSheet(Qi::ui.themes[Qi::set.theme].style);

		show();
		if (Qi::set.minMode)
		{
			QApplication::sendEvent(this, new QEvent(QEvent::WindowDeactivate));
			hide();
		}
	}
	void StyleGroup()
	{
		setProperty("group", "frame");
		ui.titleWidget->setProperty("group", "title");
		ui.clientWidget->setProperty("group", "client");

		ui.bnClose->setProperty("group", "title-close_button");
		ui.bnHide->setProperty("group", "title-hide_button");
		ui.bnMin->setProperty("group", "title-min_button");

		ui.tabWidget->setProperty("group", "tab_widget");
		ui.tabWidget->tabBar()->setProperty("group", "tab_widget_bar");

		menu->setProperty("group", "context_menu");
	}

private:
	void Init()
	{
		{
			ui.bnMin->setShortcut(Qt::Key_unknown);
			ui.bnHide->setShortcut(Qt::Key_unknown);
			ui.bnClose->setShortcut(Qt::Key_unknown);
		}

		{
			tray = new QSystemTrayIcon(this);
			tray->setIcon(QIcon(":/icon.png"));
			tray->show();
		}

		{
			menu = new QMenu(this);
			QAction* tnon = new QAction(Qi::ui.text.muOn, this);
			QAction* tnoff = new QAction(Qi::ui.text.muOff, this);
			QAction* show = new QAction(Qi::ui.text.muShow, this);
			QAction* hide = new QAction(Qi::ui.text.muHide, this);
			QAction* exit = new QAction(Qi::ui.text.muExit, this);
			menu->addAction(tnon);
			menu->addAction(tnoff);
			menu->addAction(show);
			menu->addAction(hide);
			menu->addAction(exit);
			tray->setContextMenu(menu);
			tray->setToolTip("Quick input");
			connect(tnon, SIGNAL(triggered()), this, SLOT(OnMenuTnon()));
			connect(tnoff, SIGNAL(triggered()), this, SLOT(OnMenuTnoff()));
			connect(show, SIGNAL(triggered()), this, SLOT(OnMenuShow()));
			connect(hide, SIGNAL(triggered()), this, SLOT(OnMenuHide()));
			connect(exit, SIGNAL(triggered()), this, SLOT(OnMenuExit()));
		}

		{
			connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(OnTrayClick(QSystemTrayIcon::ActivationReason)));
			connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
			connect(ui.bnMin, SIGNAL(clicked()), this, SLOT(OnBnMin()));
			connect(ui.bnHide, SIGNAL(clicked()), this, SLOT(OnBnHide()));
		}
		
		StyleGroup();
	}
	void showEvent(QShowEvent* e)
	{
		SetForegroundWindow((HWND)QWidget::winId());
	}
	bool event(QEvent* e)
	{

		if (e->type() == QEvent::WindowActivate)
		{
			Qi::widget.mainActive = true;
			if (Qi::state) QiFn::QiState(false);
			QiFn::QiHook(false);
		}
		else if (e->type() == QEvent::WindowDeactivate)
		{
			Qi::widget.mainActive = false;
			if (QiFn::SelfActive())
			{
				QiFn::QiHook(true);
				if (Qi::set.defOn) QiFn::QiState(true);
			}
		}
		return QWidget::event(e);
	}
	bool nativeEvent(const QByteArray& type, void* pMsg, long* pResult)
	{
		if (((MSG*)pMsg)->message == WM_DISPLAYCHANGE)
		{
			Qi::screen = System::screenSize();
			return true;
		}
		return false;
	}
	
	QPoint msPos;bool mouseDown = false;void mousePressEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) msPos = e->pos(), mouseDown = true; e->accept(); }void mouseMoveEvent(QMouseEvent* e) { if (mouseDown) move(e->pos() + pos() - msPos); }void mouseReleaseEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) mouseDown = false; }
private Q_SLOTS:
	void OnTrayClick(QSystemTrayIcon::ActivationReason reason)
	{
		if (reason == QSystemTrayIcon::Trigger)
		{
			setWindowState(Qt::WindowNoState);
			show();
		}
	}
	void OnMenuTnon() { if (QiFn::SelfActive()) QiFn::QiState(true), QiFn::QiHook(true); }
	void OnMenuTnoff() { QiFn::QiState(false); QiFn::QiHook(false); }
	void OnMenuShow() { setWindowState(Qt::WindowNoState), show(); }
	void OnMenuHide() { hide(); }
	void OnMenuExit() { exit(0); }
	void OnBnClose() { exit(0); }
	void OnBnMin() { setWindowState(Qt::WindowMinimized); }
	void OnBnHide() { hide(); }
};