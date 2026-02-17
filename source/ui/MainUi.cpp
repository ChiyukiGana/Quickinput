#include "MainUi.h"
MainUi::MainUi(int tab)
{
	Qi::widget.main = this;
	Qi::widget.varView = &varView;
	Qi::widget.msgView = &msgView;
	ui.setupUi(this);
	Qi::widget.macro = ui.tab_macro;
	Qi::widget.trigger = ui.tab_trigger;
	Qi::widget.func = ui.tab_func;
	Qi::widget.settings = ui.tab_settings;
	if (!Qi::title.isEmpty()) setWindowTitle(Qi::title), ui.title_label->setText(Qi::title);

#ifdef Q_MACRO_HIDE
	ui.tabWidget->removeTab(0);
#endif

	Init();
	Event();
	StyleGroup();
	ui.tabWidget->setCurrentIndex(tab);
	Qi::application->setStyleSheet(Qi::ui.themes[Qi::set.theme].style);
	show();
	if (Qi::set.minMode) hide();
}
QString MainUi::Version() const
{
	return ui.tab_about->Version();
}

void MainUi::Init()
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
		ac_on = new QAction(QiUi::Text::muOn, this);
		ac_off = new QAction(QiUi::Text::muOff, this);
		ac_show = new QAction(QiUi::Text::muShow, this);
		ac_hide = new QAction(QiUi::Text::muHide, this);
		ac_exit = new QAction(QiUi::Text::muExit, this);
		menu->addAction(ac_on);
		menu->addAction(ac_off);
		menu->addAction(ac_show);
		menu->addAction(ac_hide);
		menu->addAction(ac_exit);
		tray->setContextMenu(menu);
		tray->setToolTip("Quickinput");
	}
}
void MainUi::Event()
{
	connect(tray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) { if (!Qi::widget.active() && reason == QSystemTrayIcon::ActivationReason::Trigger) setWindowState(Qt::WindowNoState), show(); });
	connect(ui.title_close_button, &QPushButton::clicked, this, [] { ExitProcess(0); });
	connect(ui.title_min_button, &QPushButton::clicked, this, [this] { setWindowState(Qt::WindowMinimized); });
	connect(ui.title_hide_button, &QPushButton::clicked, this, [this] { hide(); });
	connect(ac_on, &QAction::triggered, this, [] { if (!Qi::widget.active()) QiFn::QiState(true), QiFn::QiHook(true); });
	connect(ac_off, &QAction::triggered, this, [] { QiFn::QiState(false); QiFn::QiHook(false); });
	connect(ac_show, &QAction::triggered, this, [this] { setWindowState(Qt::WindowNoState), show(); });
	connect(ac_hide, &QAction::triggered, this, [this] { hide(); });
	connect(ac_exit, &QAction::triggered, this, [] { ExitProcess(0); });
}
void MainUi::StyleGroup()
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

bool MainUi::event(QEvent* e)
{
	if (e->type() == QEvent::WindowActivate)
	{
		Qi::widget.mainActive = true;
		if (Qi::widget.onload)
		{
			if (Qi::state) QiFn::QiState(false);
			QiFn::QiHook(false);
		}
	}
	else if (e->type() == QEvent::WindowDeactivate)
	{
		Qi::widget.mainActive = false;
		if (Qi::widget.onload && !Qi::widget.active())
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
	return QMainWindowFrameless::event(e);
}
bool MainUi::eventFilter(QObject* obj, QEvent* e)
{
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease)) return true;
	return QMainWindowFrameless::eventFilter(obj, e);
}
void MainUi::showEvent(QShowEvent*)
{
	SetForegroundWindow((HWND)QWidget::winId());
}