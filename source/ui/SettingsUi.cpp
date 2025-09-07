#include "PopsUi.h"
#include "SettingsUi.h"
SettingsUi::SettingsUi(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	Init();
	Event();
	StyleGroup();
}

void SettingsUi::Init()
{
	// state key
	ui.stateKey_keyedit->setCombinationMode(false);
	ui.stateKey_keyedit->setDeviceEnabled(true, true, true, Qi::set.pad);
	ui.stateKey_keyedit->setMaximumKeys(2);
	// record key
	ui.recordKey_keyedit->setCombinationMode(false);
	ui.recordKey_keyedit->setDeviceEnabled(true, true, true, Qi::set.pad);
	ui.recordKey_keyedit->setMaximumKeys(2);
	if ("ocr thraed")
	{
		ui.ocr_thread_combo->setEditable(true);
		ui.ocr_thread_combo->lineEdit()->setReadOnly(true);
		ui.ocr_thread_combo->lineEdit()->setAlignment(Qt::AlignCenter);
		ui.ocr_thread_combo->addItem("最高速度");
		for (size_t i = 0; i < 16; i++) ui.ocr_thread_combo->addItem(QString::number(i + 1));
		QStandardItemModel* model = (QStandardItemModel*)ui.ocr_thread_combo->view()->model();
		for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);
		ui.ocr_thread_combo->setCurrentIndex(std::clamp(Qi::set.ocr_thread, 0, 16));
	}
	if ("ocr lang")
	{
		ui.ocr_lang_combo->setEditable(true);
		ui.ocr_lang_combo->lineEdit()->setReadOnly(true);
		ui.ocr_lang_combo->lineEdit()->setAlignment(Qt::AlignCenter);
		ui.ocr_lang_combo->addItem("默认");
		QDir dir("OCR");
		dir.setFilter(QDir::Filter::Dirs);
		QFileInfoList dirInfos = dir.entryInfoList();
		if (!dirInfos.isEmpty())
		{
			for (auto& i : dirInfos) if (i.fileName() != "." && i.fileName() != "..")
			{
				std::wstring lang = i.fileName().toStdWString() + L"\\";
				std::wstring rec = std::wstring(L"OCR\\") + lang + L"ppocr.onnx";
				std::wstring keys = std::wstring(L"OCR\\") + lang + L"ppocr.keys";
				if (File::PathState(rec) && File::PathState(keys))
				{
					ui.ocr_lang_combo->addItem(i.fileName());
					if (i.fileName() == Qi::set.ocr_current) ui.ocr_lang_combo->setCurrentIndex(ui.ocr_lang_combo->count() - 1);
				}
			}
		}
		QStandardItemModel* model = (QStandardItemModel*)ui.ocr_thread_combo->view()->model();
		for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);
	}
	if ("theme")
	{
		ui.theme_combo->setEditable(true);
		ui.theme_combo->lineEdit()->setReadOnly(true);
		ui.theme_combo->lineEdit()->setAlignment(Qt::AlignCenter);
		for (size_t i = 0; i < Qi::ui.themes.size(); i++) ui.theme_combo->addItem(Qi::ui.themes[i].name);
		QStandardItemModel* model = (QStandardItemModel*)ui.theme_combo->view()->model();
		for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);
		if (Qi::set.theme >= Qi::ui.themes.size()) Qi::set.theme = 0;
		ui.theme_combo->setCurrentIndex(Qi::set.theme);
	}
	if ("key edit")
	{
		QKeyEditKeys keys;
		if (sets->key & 0xFFFF) keys.append(sets->key & 0xFFFF);
		if (sets->key >> 16) keys.append(sets->key >> 16);
		ui.stateKey_keyedit->setKeys(keys);

		keys.clear();

		if (sets->recKey & 0xFFFF) keys.append(sets->recKey & 0xFFFF);
		if (sets->recKey >> 16) keys.append(sets->recKey >> 16);
		ui.recordKey_keyedit->setKeys(keys);
	}
	ui.recordTrack_check->setChecked(sets->recTrack);
	ui.enableDefault_check->setChecked(sets->defOn);
	ui.showState_check->setChecked(sets->showTips);
	ui.sound_check->setChecked(sets->audFx);
	ui.hideDefault_check->setChecked(sets->minMode);
	ui.pad_check->setChecked(sets->pad);
	ui.start_check->setChecked(Task::Find(L"QuickInput"));
	if ("clear shortcut")
	{
		ui.readme_button->installEventFilter(this);
		ui.popText_button->installEventFilter(this);
		ui.recordTrack_check->installEventFilter(this);
		ui.enableDefault_check->installEventFilter(this);
		ui.showState_check->installEventFilter(this);
		ui.sound_check->installEventFilter(this);
		ui.hideDefault_check->installEventFilter(this);
		ui.pad_check->installEventFilter(this);
		ui.start_check->installEventFilter(this);
	}
}
void SettingsUi::Event()
{
	connect(ui.readme_button, &QPushButton::clicked, this, [this] { more.show(); });
	connect(ui.popText_button, &QPushButton::clicked, this, [this] {
		PopsUi ps;
		Qi::widget.dialogActive = true;
		Qi::widget.main->hide();
		ps.exec();
		Qi::widget.main->show();
		Qi::widget.dialogActive = false;
		QiJson::SaveJson(); });
	connect(ui.ocr_lang_combo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
		if (index >= 0)
		{
			QString lang;
			if (index != 0) lang = ui.ocr_lang_combo->currentText();
			Qi::popText->Show("正在切换");
			QTimer::singleShot(32, [lang] {
				Qi::set.ocr_current = lang;
				QiFn::InitOcr();
				QiJson::SaveJson();
				Qi::popText->Hide();
				});
		}
		});
	connect(ui.ocr_thread_combo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
		if (index >= 0 && index != Qi::set.ocr_thread)
		{
			Qi::popText->Show("正在切换");
			QTimer::singleShot(32, [index] {
				Qi::set.ocr_thread = index;
				QiFn::InitOcr();
				QiJson::SaveJson();
				Qi::popText->Hide();
				});
		}
		});
	connect(ui.theme_combo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
		if (index >= 0 && Qi::set.theme != index)
		{
			Qi::set.theme = index;
			Qi::application->setStyleSheet(Qi::ui.themes[Qi::set.theme].style);
			QiJson::SaveJson();
		}});
		connect(ui.stateKey_keyedit, &QKeyEdit::changed, this, [this] {
			QKeyEditKeys keys = ui.stateKey_keyedit->keys();
			DWORD vk = VK_F8;
			if (keys.size() == 1)
			{
				vk = keys.first();
				if (vk = keys.first() == VK_LBUTTON) ui.stateKey_keyedit->setKey(VK_F8);
				else vk = keys.first();
			}
			else if (keys.size() > 1)
			{
				vk = keys.first() | (keys.last() << 16);
			}
			Qi::set.key = vk;
			QiJson::SaveJson(); });
		connect(ui.recordKey_keyedit, &QKeyEdit::changed, this, [this] {
			QKeyEditKeys keys = ui.recordKey_keyedit->keys();
			DWORD vk = VK_F8;
			if (keys.size() == 1)
			{
				vk = keys.first();
				if (vk = keys.first() == VK_LBUTTON) ui.recordKey_keyedit->setKey(VK_F8);
				else vk = keys.first();
			}
			else if (keys.size() > 1)
			{
				vk = keys.first() | (keys.last() << 16);
			}
			Qi::set.recKey = vk;
			QiJson::SaveJson(); });
		connect(ui.recordTrack_check, &QCheckBox::toggled, this, [this](bool state) { sets->recTrack = state; QiJson::SaveJson(); });
		connect(ui.enableDefault_check, &QCheckBox::toggled, this, [this](bool state) { sets->defOn = state; QiJson::SaveJson(); });
		connect(ui.showState_check, &QCheckBox::toggled, this, [this](bool state) { sets->showTips = state; QiJson::SaveJson(); });
		connect(ui.sound_check, &QCheckBox::toggled, this, [this](bool state) { sets->audFx = state; QiJson::SaveJson(); });
		connect(ui.hideDefault_check, &QCheckBox::toggled, this, [this](bool state) { sets->minMode = state; QiJson::SaveJson(); });
		connect(ui.pad_check, &QCheckBox::toggled, this, [this](bool state) { sets->pad = state; Qi::widget.keyEditReload(); QiJson::SaveJson(); });
		connect(ui.start_check, &QCheckBox::toggled, this, [this] {
			if (Task::Find(L"QuickInput"))
			{
				if (Task::Delete(L"QuickInput")) ui.start_check->setChecked(false);
				else
				{
					ui.start_check->setChecked(true);
					MsgBox::Error(L"需要以管理员权限运行", L"删除任务错误");
				}
			}
			else
			{
				if (Task::Register(L"QuickInput")) ui.start_check->setChecked(true);
				else
				{
					ui.start_check->setChecked(false);
					MsgBox::Error(L"需要以管理员权限运行", L"创建任务错误");
				}
			}});
}
void SettingsUi::StyleGroup()
{
	ui.readme_button->setProperty("group", "settings-button");
	ui.popText_button->setProperty("group", "settings-button");
	ui.recordTrack_check->setProperty("group", "check");
	ui.enableDefault_check->setProperty("group", "check");
	ui.showState_check->setProperty("group", "check");
	ui.sound_check->setProperty("group", "check");
	ui.hideDefault_check->setProperty("group", "check");
	ui.pad_check->setProperty("group", "check");
	ui.start_check->setProperty("group", "check");
	ui.ocr_lang_combo->setProperty("group", "combo");
	ui.ocr_lang_combo->setView(new QListView());
	ui.ocr_lang_combo->view()->setProperty("group", "combo_body");
	ui.ocr_thread_combo->setProperty("group", "combo");
	ui.ocr_thread_combo->setView(new QListView());
	ui.ocr_thread_combo->view()->setProperty("group", "combo_body");
	ui.theme_combo->setProperty("group", "combo");
	ui.theme_combo->setView(new QListView());
	ui.theme_combo->view()->setProperty("group", "combo_body");
	ui.stateKey_keyedit->setProperty("group", "line_edit");
	ui.recordKey_keyedit->setProperty("group", "line_edit");
}

bool SettingsUi::event(QEvent* e)
{
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
	{
		QKeyEvent* keyEvent = (QKeyEvent*)e;
		if ((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Space)) return true;
	}
	return QWidget::event(e);
}
bool SettingsUi::eventFilter(QObject* obj, QEvent* e)
{
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease)) return true;
	return QWidget::eventFilter(obj, e);
}
void SettingsUi::closeEvent(QCloseEvent* e)
{
	more.close();
}
void SettingsUi::customEvent(QEvent* e)
{
	if (e->type() == QiEvent::key_reset)
	{
		ui.stateKey_keyedit->setPadEnabled(Qi::set.pad);
		ui.recordKey_keyedit->setPadEnabled(Qi::set.pad);
	}
}