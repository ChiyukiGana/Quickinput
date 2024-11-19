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
	using This = MainUi;
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
		StyleGroup();
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
		if ("clear shortcut")
		{
			ui.bnMin->installEventFilter(this);
			ui.bnHide->installEventFilter(this);
			ui.bnClose->installEventFilter(this);
		}

		if ("tray")
		{
			tray = new QSystemTrayIcon(this);
			tray->setIcon(QIcon(":/icon.png"));
			tray->show();
		}

		if ("task bar icon")
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
			connect(tnon, &QAction::triggered, this, &This::OnMenuTnon);
			connect(tnoff, &QAction::triggered, this, &This::OnMenuTnoff);
			connect(show, &QAction::triggered, this, &This::OnMenuShow);
			connect(hide, &QAction::triggered, this, &This::OnMenuHide);
			connect(exit, &QAction::triggered, this, &This::OnMenuExit);
		}

		if ("event")
		{
			connect(tray, &QSystemTrayIcon::activated, this, &This::OnTrayClick);
			connect(ui.bnClose, &QPushButton::clicked, this, &This::OnBnClose);
			connect(ui.bnMin, &QPushButton::clicked, this, &This::OnBnMin);
			connect(ui.bnHide, &QPushButton::clicked, this, &This::OnBnHide);
		}
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
		else if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
		{
			QKeyEvent* keyEvent = (QKeyEvent*)e;
			if ((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Space)) return true;
		}
		return QMainWindow::event(e);
	}
	bool eventFilter(QObject* obj, QEvent* e)
	{
		if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease)) return true;
		return QMainWindow::eventFilter(obj, e);
	}
	void showEvent(QShowEvent* e)
	{
		SetForegroundWindow((HWND)QWidget::winId());
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