#include "FuncUi.h"
FuncUi::FuncUi(QWidget* parent) : QWidget(parent)
{
	Qi::widget.func = this;
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
		ui.click_delay_spin->setValue(func->quickClick.delay);
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
		ui.window_name_edit->setText(func->wndActive.wndInfo.name);
		ui.window_class_edit->setText(func->wndActive.wndInfo.clas);
		ui.window_proc_edit->setText(func->wndActive.wndInfo.proc);
	}
	if ("clear shortcut")
	{
		ui.window_select_button->installEventFilter(this);
		ui.click_check->installEventFilter(this);
		ui.window_check->installEventFilter(this);
		ui.clock_check->installEventFilter(this);
		ui.varView_button->installEventFilter(this);
		ui.msgView_button->installEventFilter(this);
	}
}
void FuncUi::Event()
{
	connect(ui.click_check, &QCheckBox::toggled, this, [this](bool state) { func->quickClick.state = state; QiJson::SaveJson(); });
	connect(ui.click_keyedit, &QKeyEdit::changed, this, [this] { func->quickClick.key = ui.click_keyedit->key(); QiJson::SaveJson(); });
	connect(ui.click_delay_spin, &QSpinBox::valueChanged, this, [this](int value) { func->quickClick.delay = value; QiJson::SaveJson(); });
	connect(ui.click_mode_combo, &QComboBox::currentIndexChanged, this, [this](int index) { func->quickClick.mode = index; QiJson::SaveJson(); });
	connect(ui.window_check, &QCheckBox::toggled, this, [this](bool state) { func->wndActive.state = state; QiJson::SaveJson(); });
	connect(ui.window_name_edit, &QLineEdit::textEdited, this, [this](const QString& text) {
		func->wndActive.wndInfo.name = text;
		QiJson::SaveJson();
		});
	connect(ui.window_class_edit, &QLineEdit::textEdited, this, [this](const QString& text) {
		func->wndActive.wndInfo.clas = text;
		QiJson::SaveJson();
		});
	connect(ui.window_proc_edit, &QLineEdit::textEdited, this, [this](const QString& text) {
		func->wndActive.wndInfo.proc = text;
		QiJson::SaveJson();
		});
	connect(ui.window_select_button, &QPushButton::clicked, this, [this] {
		Qi::widget.dialogActive = true;
		Qi::widget.main->hide();
		func->wndActive.wndInfo = QiFn::WindowSelector();
		func->wndActive.wndInfo.update_fromHwnd();

		ui.window_name_edit->blockSignals(true);
		ui.window_class_edit->blockSignals(true);
		ui.window_proc_edit->blockSignals(true);

		ui.window_name_edit->setText(func->wndActive.wndInfo.name);
		ui.window_class_edit->setText(func->wndActive.wndInfo.clas);
		ui.window_proc_edit->setText(func->wndActive.wndInfo.proc);

		ui.window_name_edit->blockSignals(false);
		ui.window_class_edit->blockSignals(false);
		ui.window_proc_edit->blockSignals(false);

		Qi::widget.dialogActive = false;
		Qi::widget.main->show();
		QiJson::SaveJson();
		});
	connect(ui.clock_check, &QCheckBox::toggled, this, [this](bool state) { func->showClock.state = state; QiJson::SaveJson(); });
	connect(ui.clock_keyedit, &QKeyEdit::changed, this, [this] { func->showClock.key = ui.clock_keyedit->key(); QiJson::SaveJson(); });
	connect(ui.exePath_button, &QPushButton::clicked, this, [] { ShellExecuteW(nullptr, L"open", L"explorer.exe", Process::runPath().c_str(), nullptr, SW_SHOW); });
	connect(ui.varView_button, &QPushButton::clicked, this, [this] { Qi::widget.varView->show(); });
	connect(ui.msgView_button, &QPushButton::clicked, this, [this] { Qi::widget.msgView->show(); });
}
void FuncUi::StyleGroup()
{
	style_set_group(ui.window_select_button, "get_button");
	style_set_group(ui.click_check, "check");
	style_set_group(ui.clock_check, "check");
	style_set_group(ui.window_check, "check");
	ui.click_mode_combo->setView(new QListView());
	style_set_group(ui.click_mode_combo, "combo");
	style_set_group(ui.click_mode_combo->view(), "combo_body");
	style_set_group(ui.click_delay_spin, "line_edit");
	style_set_group(ui.window_name_edit, "line_edit");
	style_set_group(ui.window_class_edit, "line_edit");
	style_set_group(ui.window_proc_edit, "line_edit");
	style_set_group(ui.click_keyedit, "line_edit");
	style_set_group(ui.clock_keyedit, "line_edit");
	style_set_group(ui.exePath_button, "get_button");
	style_set_group(ui.varView_button, "get_button");
	style_set_group(ui.msgView_button, "get_button");
}
void FuncUi::LoadLanguage()
{
#define save(Obj) Obj->setProperty(Prop::lang_save, Obj->text());
#define load(Obj) Obj->setText(lang_trans(Obj->property(Prop::lang_save).toString()));
	std::call_once(lang_once, [this] {
		save(ui.click_label);
		save(ui.click_key_label);
		lang_save_cmb(ui.click_mode_combo);
		save(ui.click_mode_label);
		save(ui.click_delay_label);

		save(ui.window_label);
		save(ui.window_name_label);
		lang_save_pht(ui.window_name_edit);
		lang_save_pht(ui.window_class_edit);
		lang_save_pht(ui.window_proc_edit);

		save(ui.clock_label);
		save(ui.clock_key_label);

		save(ui.exePath_label);
		save(ui.exePath_button);
		save(ui.msgView_label);
		save(ui.msgView_button);
		save(ui.varView_label);
		save(ui.varView_button);
	});
	load(ui.click_label);
	load(ui.click_key_label);
	lang_load_ke(ui.click_keyedit);
	lang_load_cmb(ui.click_mode_combo);
	load(ui.click_mode_label);
	load(ui.click_delay_label);

	load(ui.window_label);
	load(ui.window_name_label);
	lang_load_pht(ui.window_name_edit);
	lang_load_pht(ui.window_class_edit);
	lang_load_pht(ui.window_proc_edit);

	load(ui.clock_label);
	load(ui.clock_key_label);
	lang_load_ke(ui.clock_keyedit);

	load(ui.exePath_label);
	load(ui.exePath_button);
	load(ui.msgView_label);
	load(ui.msgView_button);
	load(ui.varView_label);
	load(ui.varView_button);
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
	if (e->type() == static_cast<int>(QiEvent::lang_reload))
	{
			LoadLanguage();
	}
#ifdef Q_KEYEDIT_PAD_ENABLED
	else if (e->type() == static_cast<int>(QiEvent::key_reset))
	{
		ui.click_keyedit->setPadEnabled(Qi::set.pad);
		ui.clock_keyedit->setPadEnabled(Qi::set.pad);
	}
#endif
}