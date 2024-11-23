﻿#pragma once
#include "MacroUi.h"
#include "TriggerUi.h"
#include "FuncUi.h"
#include "SettingsUi.h"
#include "AboutUi.h"
#include <src/inc_header.h>
#include "ui_MainUi.h"
class MainUi : public QMainWindow
{
	Q_OBJECT;
	using This = MainUi;
	Ui::MainUiClass ui;
	QSystemTrayIcon* tray = nullptr;
	QMenu* menu = nullptr;
public:
	MainUi(int tab = 0)
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
		ui.title_widget->setProperty("group", "title");
		ui.content_widget->setProperty("group", "client");
		ui.title_close_button->setProperty("group", "title-close_button");
		ui.title_hide_button->setProperty("group", "title-hide_button");
		ui.title_min_button->setProperty("group", "title-min_button");
		ui.tabWidget->setProperty("group", "tab_widget");
		ui.tabWidget->tabBar()->setProperty("group", "tab_widget_bar");
		menu->setProperty("group", "context_menu");
	}
private:
	void Init()
	{
		if ("clear shortcut")
		{
			ui.title_min_button->installEventFilter(this);
			ui.title_hide_button->installEventFilter(this);
			ui.title_close_button->installEventFilter(this);
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
			QAction* ac_on = new QAction(Qi::ui.text.muOn, this);
			QAction* ac_off = new QAction(Qi::ui.text.muOff, this);
			QAction* ac_show = new QAction(Qi::ui.text.muShow, this);
			QAction* ac_hide = new QAction(Qi::ui.text.muHide, this);
			QAction* ac_exit = new QAction(Qi::ui.text.muExit, this);
			menu->addAction(ac_on);
			menu->addAction(ac_off);
			menu->addAction(ac_show);
			menu->addAction(ac_hide);
			menu->addAction(ac_exit);
			tray->setContextMenu(menu);
			tray->setToolTip("Quick input");
			connect(ac_on, &QAction::triggered, this, [] { if (QiFn::SelfActive()) QiFn::QiState(true), QiFn::QiHook(true); });
			connect(ac_off, &QAction::triggered, this, [] { QiFn::QiState(false); QiFn::QiHook(false); });
			connect(ac_show, &QAction::triggered, this, [this] { setWindowState(Qt::WindowNoState), show(); });
			connect(ac_hide, &QAction::triggered, this, &This::hide);
			connect(ac_exit, &QAction::triggered, this, [] { exit(0); });
		}
		if ("event")
		{
			connect(tray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason) { setWindowState(Qt::WindowNoState), show(); });
			connect(ui.title_close_button, &QPushButton::clicked, this, [] { exit(0); });
			connect(ui.title_min_button, &QPushButton::clicked, this, [this] { setWindowState(Qt::WindowMinimized); });
			connect(ui.title_hide_button, &QPushButton::clicked, this, &This::hide);
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
	// window move
	QPoint mouse_positon;
	bool mouse_down = false;
	void mousePressEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			mouse_positon = e->pos();
			mouse_down = true;
			e->accept();
		}
	}
	void mouseMoveEvent(QMouseEvent* e)
	{
		if (mouse_down)
		{
			if (Distance(mouse_positon.x(), mouse_positon.y(), e->pos().x(), e->pos().y()) < 100)
			{
				move(e->pos() + pos() - mouse_positon);
				e->accept();
			}
		}
	}
	void mouseReleaseEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			mouse_down = false;
			e->accept();
		}
	}
};