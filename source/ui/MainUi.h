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
	MainUi() : QMainWindow()
	{
		qis.widget.main = this;
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		WidInit();

		show();
		if (qis.set.minMode)
		{
			QApplication::sendEvent(this, new QEvent(QEvent::WindowDeactivate));
			hide();
		}
	}
	void SetStyleGroup()
	{
		setProperty("group", QVariant(QString::fromUtf8("frame")));
		ui.titleWidget->setProperty("group", QVariant(QString::fromUtf8("title")));
		ui.clientWidget->setProperty("group", QVariant(QString::fromUtf8("client")));

		ui.bnClose->setProperty("group", QVariant(QString::fromUtf8("title-close_button")));
		ui.bnHide->setProperty("group", QVariant(QString::fromUtf8("title-hide_button")));
		ui.bnMin->setProperty("group", QVariant(QString::fromUtf8("title-min_button")));

		ui.tabWidget->setProperty("group", QVariant(QString::fromUtf8("tab_widget")));
		ui.tabWidget->tabBar()->setProperty("group", QVariant(QString::fromUtf8("tab_widget_bar")));

		menu->setProperty("group", QVariant(QString::fromUtf8("context_menu")));
	}
	void ReStyle()
	{
		setStyleSheet("");
		setStyleSheet(qis.ui.themes[qis.set.theme].style);
		menu->setStyleSheet("");
		menu->setStyleSheet(qis.ui.themes[qis.set.theme].style);
		ui.macro->ReStyle();
		ui.trigger->ReStyle();
		ui.func->ReStyle();
		ui.settings->ReStyle();
	}
private:
	void WidInit()
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
		
		SetStyleGroup();
	}
	void showEvent(QShowEvent* et)
	{
		ReStyle();
		SetForegroundWindow((HWND)QWidget::winId());
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
				QiFn::QiHook(true);
				if (qis.set.defOn) QiFn::QiState(true);
			}
		}
		return QWidget::event(et);
	}
	bool nativeEvent(const QByteArray& type, void* pMsg, long* pResult)
	{
		if (((MSG*)pMsg)->message == WM_DISPLAYCHANGE)
		{
			qis.screen = System::screenSize();
			return true;
		}
		return false;
	}
	
	QPoint msPos;bool mouseDown = false;void mousePressEvent(QMouseEvent* et) { if (et->button() == Qt::LeftButton) msPos = et->pos(), mouseDown = true; et->accept(); }void mouseMoveEvent(QMouseEvent* et) { if (mouseDown) move(et->pos() + pos() - msPos); }void mouseReleaseEvent(QMouseEvent* et) { if (et->button() == Qt::LeftButton) mouseDown = false; }
private slots:
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