#include "MainUi.h"
MainUi::MainUi(int tab)
{
	Qi::widget.main = this;
	Qi::widget.varView = &varView;
	Qi::widget.msgView = &msgView;
	ui.setupUi(this);
	if (!Qi::title.isEmpty()) setWindowTitle(Qi::title), ui.title_label->setText(Qi::title);

	QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
	setMaximumHeight(screenGeometry.height());
	if (Qi::set.mainSize.width() >= minimumWidth() && Qi::set.mainSize.height() >= minimumHeight())
	{
		Qi::set.mainSize.setWidth(std::clamp(Qi::set.mainSize.width(), minimumWidth(), screenGeometry.width()));
		Qi::set.mainSize.setHeight(std::clamp(Qi::set.mainSize.height(), minimumHeight(), screenGeometry.height()));
		QRect centeredRect(QPoint(0, 0), Qi::set.mainSize);
		centeredRect.moveCenter(screenGeometry.center());
		setGeometry(centeredRect);
	}

#ifdef Q_MACRO_HIDE
	ui.tabWidget->removeTab(0);
#endif
#ifdef Q_FUNC_HIDE
	ui.tabWidget->removeTab(2);
#endif
#ifdef Q_ABOUT_HIDE
	ui.tabWidget->removeTab(4);
#endif

	Init();
	Event();
	StyleGroup();
	ui.tabWidget->setCurrentIndex(tab);
	Qi::application->setStyleSheet(Qi::ui.themes[Qi::set.theme].style);
	show();
	if (Qi::set.minMode) hide();
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
		ac_on = new QAction(QiUi::Text::muOn(), this);
		ac_off = new QAction(QiUi::Text::muOff(), this);
		ac_show = new QAction(QiUi::Text::muShow(), this);
		ac_hide = new QAction(QiUi::Text::muHide(), this);
		ac_exit = new QAction(QiUi::Text::muExit(), this);
		menu->addAction(ac_on);
		menu->addAction(ac_off);
		menu->addAction(ac_show);
		menu->addAction(ac_hide);
		menu->addAction(ac_exit);
		tray->setContextMenu(menu);
		tray->setToolTip(windowTitle());
	}
}
void MainUi::Event()
{
	connect(tray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) { if (!Qi::widget.active() && reason == QSystemTrayIcon::ActivationReason::Trigger) setWindowState(Qt::WindowNoState), show(); });
	connect(ui.title_close_button, &QPushButton::clicked, this, [] { Qi::exit(0); });
	connect(ui.title_min_button, &QPushButton::clicked, this, [this] { setWindowState(Qt::WindowMinimized); });
	connect(ui.title_hide_button, &QPushButton::clicked, this, [this] { hide(); });
	connect(ac_on, &QAction::triggered, this, [] { if (!Qi::widget.active()) QiTr::QiState(true), QiTr::QiHook(true); });
	connect(ac_off, &QAction::triggered, this, [] { QiTr::QiState(false); QiTr::QiHook(false); });
	connect(ac_show, &QAction::triggered, this, [this] { setWindowState(Qt::WindowNoState), show(); });
	connect(ac_hide, &QAction::triggered, this, [this] { hide(); });
	connect(ac_exit, &QAction::triggered, this, [] { Qi::exit(0); });
}
void MainUi::StyleGroup()
{
	setProperty(Prop::style_group, "frame");
	style_set_group(ui.title_widget, "title");
	style_set_group(ui.content_widget, "client");
	style_set_group(ui.title_close_button, "title-close_button");
	style_set_group(ui.title_hide_button, "title-hide_button");
	style_set_group(ui.title_min_button, "title-min_button");
	style_set_group(ui.tabWidget, "tab_widget");
	style_set_group(ui.tabWidget->tabBar(), "tab_widget_bar");
	style_set_group(menu, "context_menu");
}
void MainUi::LoadLanguage()
{
	std::call_once(lang_once, [this] {
		int count = ui.tabWidget->count();
		for (size_t i = 0; i < count; i++) ui.tabWidget->widget(i)->setProperty(Prop::lang_save, ui.tabWidget->tabText(i));
		lang_save_tp(ui.title_close_button);
		lang_save_tp(ui.title_min_button);
		lang_save_tp(ui.title_hide_button);
	});
	int count = ui.tabWidget->count();
	for (size_t i = 0; i < count; i++) ui.tabWidget->setTabText(i, lang_trans(ui.tabWidget->widget(i)->property(Prop::lang_save).toString()));
	lang_load_tp(ui.title_close_button);
	lang_load_tp(ui.title_min_button);
	lang_load_tp(ui.title_hide_button);
	ac_on->setText(QiUi::Text::muOn());
	ac_off->setText(QiUi::Text::muOff());
	ac_show->setText(QiUi::Text::muShow());
	ac_hide->setText(QiUi::Text::muHide());
	ac_exit->setText(QiUi::Text::muExit());
}

bool MainUi::event(QEvent* e)
{
	if (e->type() == QEvent::WindowActivate)
	{
		Qi::widget.mainActive = true;
		if (Qi::widget.onload)
		{
			if (Qi::state) QiTr::QiState(false);
			QiTr::QiHook(false);
		}
	}
	else if (e->type() == QEvent::WindowDeactivate)
	{
		Qi::widget.mainActive = false;
		if (Qi::widget.onload && !Qi::widget.active())
		{
			QiTr::QiHook(true);
			if (Qi::set.defOn) QiTr::QiState(true);
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
void MainUi::resizeEvent(QResizeEvent* e)
{
	Qi::set.mainSize = e->size();
}
void MainUi::customEvent(QEvent* e)
{
	if (e->type() == static_cast<int>(QiEvent::main_show))
	{
		Qi::widget.main->setAttribute(Qt::WA_ShowWithoutActivating, true);
		show();
		Qi::widget.main->setAttribute(Qt::WA_ShowWithoutActivating, false);
	}
	else if (e->type() == static_cast<int>(QiEvent::main_hide))
	{
		hide();
	}
	else if (e->type() == static_cast<int>(QiEvent::lang_reload))
	{
		LoadLanguage();
	}
	else if (e->type() == static_cast<int>(QiEvent::lang_notify))
	{
		Qi::widget.langReload();
	}
}
void MainUi::closeEvent(QCloseEvent*)
{
	Qi::exit();
}