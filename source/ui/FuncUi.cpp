#include "FuncUi.h"
FuncUi::FuncUi(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	Init();
	Event();
	StyleGroup();
}

void FuncUi::Init()
{
	if ("click")
	{
		ui.click_check->setChecked(func->quickClick.state);
		ui.click_keyedit->setCombinationMode(false);
		ui.click_keyedit->setDeviceEnabled(true, true, true);
#ifdef Q_KEYEDIT_PAD_ENABLED
		ui.click_keyedit->setPadEnabled(Qi::set.pad);
#endif
		ui.click_keyedit->setKey(func->quickClick.key);
		ui.click_delay_edit->setValidator(new QIntValidator(0, 99999, this));
		ui.click_delay_edit->setText(QString::number(func->quickClick.delay));
		ui.click_mode_combo->setEditable(true);
		ui.click_mode_combo->lineEdit()->setReadOnly(true);
		ui.click_mode_combo->lineEdit()->setAlignment(Qt::AlignCenter);
		ui.click_mode_combo->addItem("按下");
		ui.click_mode_combo->addItem("切换");
		ui.click_mode_combo->setCurrentIndex(func->quickClick.mode);
		QStandardItemModel* model = (QStandardItemModel*)ui.click_mode_combo->view()->model();
		for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);
	}
	if ("clock")
	{
		ui.clock_check->setChecked(func->showClock.state);
		ui.clock_keyedit->setCombinationMode(false);
		ui.clock_keyedit->setDeviceEnabled(true, true, true);
#ifdef Q_KEYEDIT_PAD_ENABLED
		ui.clock_keyedit->setPadEnabled(Qi::set.pad);
#endif
		ui.clock_keyedit->setKey(func->showClock.key);
	}
	if ("window")
	{
		ui.window_check->setChecked(func->wndActive.state);
		ui.window_name_edit->setText(func->wndActive.wndInfo.wndName);
	}
	if ("clear shortcut")
	{
		ui.window_select_button->installEventFilter(this);
		ui.click_check->installEventFilter(this);
		ui.window_check->installEventFilter(this);
		ui.window_select_button->installEventFilter(this);
		ui.clock_check->installEventFilter(this);
		ui.varView_button->installEventFilter(this);
		ui.msgView_button->installEventFilter(this);
	}
}
void FuncUi::Event()
{
	connect(ui.click_check, &QCheckBox::toggled, this, [this](bool state) { func->quickClick.state = state; QiJson::SaveJson(); });
	connect(ui.click_keyedit, &QKeyEdit::changed, this, [this] { func->quickClick.key = ui.click_keyedit->key(); QiJson::SaveJson(); });
	connect(ui.click_delay_edit, &QLineEdit::textEdited, this, [this](const QString& text) { func->quickClick.delay = text.toInt(); QiJson::SaveJson(); });
	connect(ui.click_mode_combo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) { func->quickClick.mode = index; QiJson::SaveJson(); });
	connect(ui.window_check, &QCheckBox::toggled, this, [this](bool state) { func->wndActive.state = state; QiJson::SaveJson(); });
	connect(ui.window_select_button, &QPushButton::clicked, this, [this] {
		Qi::widget.dialogActive = true;
		Qi::widget.main->hide();
		func->wndActive.wndInfo = QiFn::WindowSelection();
		ui.window_name_edit->setText(func->wndActive.wndInfo.wndName);
		Qi::widget.dialogActive = false;
		Qi::widget.main->show();
		QiJson::SaveJson();
		});
	connect(ui.clock_check, &QCheckBox::toggled, this, [this](bool state) { func->showClock.state = state; QiJson::SaveJson(); });
	connect(ui.clock_keyedit, &QKeyEdit::changed, this, [this] { func->showClock.key = ui.clock_keyedit->key(); QiJson::SaveJson(); });
	connect(ui.varView_button, &QPushButton::clicked, this, [this] { Qi::widget.varView->show(); });
	connect(ui.msgView_button, &QPushButton::clicked, this, [this] { Qi::widget.msgView->show(); });
}
void FuncUi::StyleGroup()
{
	ui.window_select_button->setProperty("group", "get_button");
	ui.click_check->setProperty("group", "check");
	ui.clock_check->setProperty("group", "check");
	ui.window_check->setProperty("group", "check");
	ui.click_mode_combo->setView(new QListView());
	ui.click_mode_combo->setProperty("group", "combo");
	ui.click_mode_combo->view()->setProperty("group", "combo_body");
	ui.click_delay_edit->setProperty("group", "line_edit");
	ui.window_name_edit->setProperty("group", "line_edit");
	ui.click_keyedit->setProperty("group", "line_edit");
	ui.clock_keyedit->setProperty("group", "line_edit");
	ui.varView_button->setProperty("group", "get_button");
	ui.msgView_button->setProperty("group", "get_button");
}

bool FuncUi::event(QEvent* e)
{
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
	{
		QKeyEvent* keyEvent = (QKeyEvent*)e;
		if ((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Space)) return true;
	}
	return QWidget::event(e);
}
bool FuncUi::eventFilter(QObject* obj, QEvent* e)
{
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease)) return true;
	return QWidget::eventFilter(obj, e);
}
void FuncUi::customEvent(QEvent* e)
{
#ifdef Q_KEYEDIT_PAD_ENABLED
	if (e->type() == static_cast<int>(QiEvent::key_reset))
	{
		ui.click_keyedit->setPadEnabled(Qi::set.pad);
		ui.clock_keyedit->setPadEnabled(Qi::set.pad);
	}
#endif
}