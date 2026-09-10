#include "PopsUi.h"
#include "SettingsUi.h"
SettingsUi::SettingsUi(QWidget* parent) : QWidget(parent)
{
	Qi::widget.settings = this;
	ui.setupUi(this);
	Qi::widget.help = &help;
	setWindowFlags(Qt::FramelessWindowHint);
	Init();
	Event();
	StyleGroup();
#ifndef Q_DRIVER
	ui.driver_label->setHidden(true);
	ui.driver_check->setHidden(true);
#endif
#ifndef Q_KEYEDIT_PAD_ENABLED
	ui.pad_label->setHidden(true);
	ui.pad_check->setHidden(true);
#endif
#ifdef QIOCR_INTERNAL
	ui.ocr_lang_label->setHidden(true);
	ui.ocr_lang_combo->setHidden(true);
#endif
#ifdef Q_ENCRYPT
	ui.macro_save_label->setHidden(true);
	ui.macro_save_combo->setHidden(true);
#endif
}

void SettingsUi::Init()
{
	// state key
	ui.stateKey_keyedit->setCombinationMode(false);
	ui.stateKey_keyedit->setDeviceEnabled(true, true, true);
	ui.stateKey_keyedit->setMaximumKeys(2);
	// record key
	ui.recordKey_keyedit->setCombinationMode(false);
	ui.recordKey_keyedit->setDeviceEnabled(true, true, true);
	ui.recordKey_keyedit->setMaximumKeys(1);

#ifdef Q_KEYEDIT_PAD_ENABLED
	ui.stateKey_keyedit->setPadEnabled(Qi::set.pad);
	ui.recordKey_keyedit->setPadEnabled(Qi::set.pad);
#endif

	if ("ocr thraed")
	{
		ui.ocr_thread_combo->setEditable(true);
		ui.ocr_thread_combo->lineEdit()->setReadOnly(true);
		ui.ocr_thread_combo->lineEdit()->setAlignment(Qt::AlignCenter);
		ui.ocr_thread_combo->addItem("最高速度");
		for (size_t i = 0; i < Qi::ocr_thread_max; i++) ui.ocr_thread_combo->addItem(QString::number(i + 1));
		QStandardItemModel* model = (QStandardItemModel*)ui.ocr_thread_combo->view()->model();
		for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);
		ui.ocr_thread_combo->setCurrentIndex(std::clamp(Qi::set.ocr_thread, 0, Qi::ocr_thread_max));
	}
	if ("ocr lang")
	{
		ui.ocr_lang_combo->setEditable(true);
		ui.ocr_lang_combo->lineEdit()->setReadOnly(true);
		ui.ocr_lang_combo->lineEdit()->setAlignment(Qt::AlignCenter);
		ui.ocr_lang_combo->addItem(lang_trans("默认"));
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
				if (File::FileExist(rec) && File::FileExist(keys))
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
	if ("macro save type")
	{
		ui.macro_save_combo->setEditable(true);
		ui.macro_save_combo->lineEdit()->setReadOnly(true);
		ui.macro_save_combo->lineEdit()->setAlignment(Qt::AlignCenter);
		ui.macro_save_combo->addItem("Json(可读)");
		ui.macro_save_combo->addItem("Qim(体积小)");
		if (Qi::set.save_type > Macro::StorageType::QIM) ui.macro_save_combo->setCurrentIndex(Macro::StorageType::JSON);
		else ui.macro_save_combo->setCurrentIndex(Qi::set.save_type);
	}
	if ("key edit")
	{
		QKeyEditKeys keys;
		if (sets.key1) keys.append(sets.key1);
		if (sets.key2) keys.append(sets.key2);
		ui.stateKey_keyedit->setKeys(keys);

		ui.recordKey_keyedit->setKey(sets.recKey);
	}
	if ("lang")
	{
		ui.lang_combo->setEditable(true);
		ui.lang_combo->lineEdit()->setReadOnly(true);
		ui.lang_combo->lineEdit()->setAlignment(Qt::AlignCenter);
		ui.lang_combo->addItem("中文(默认)");
		int index{};
		auto files = File::Find(Qi::langDir, QString("*") + Qi::langType);
		for (size_t i = 0; i < files.size(); i++)
		{
			auto& file = files[i];
			QString name = file.baseName();
			ui.lang_combo->addItem(name);
			if (index == 0 && name == Qi::set.lang) index = i + 1;
		}
		if (index)
		{
			Qi::lang = Language::load(Language::makePath(Qi::set.lang));
			if (!Qi::lang.map.empty())
			{
				ui.lang_combo->setCurrentIndex(index);
				Qi::widget.langNotify();
			}
		}
	}
#ifdef Q_DRIVER
	ui.driver_check->setChecked(sets.driver && Qi::driver.isInit());
#endif
	ui.recordTrack_check->setChecked(sets.recTrack);
	ui.enableDefault_check->setChecked(sets.defOn);
	ui.showState_check->setChecked(sets.showTips);
	ui.sound_check->setChecked(sets.audFx);
	ui.hideDefault_check->setChecked(sets.minMode);
	ui.pad_check->setChecked(sets.pad);
	ui.start_check->setChecked(Task::Find(L"QuickInput"));
	if ("clear shortcut")
	{
		ui.readme_button->installEventFilter(this);
		ui.popText_button->installEventFilter(this);
#ifdef Q_DRIVER
		ui.driver_check->installEventFilter(this);
#endif
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
	connect(ui.readme_button, &QPushButton::clicked, this, [this] { help.show(); });
	connect(ui.popText_button, &QPushButton::clicked, this, [this] {
		PopsUi ps;
		Qi::widget.dialogActive = true;
		Qi::widget.main->hide();
		ps.exec();
		Qi::widget.main->show();
		Qi::widget.dialogActive = false;
		QiJson::SaveJson(); });
	connect(ui.lang_combo, &QComboBox::currentIndexChanged, this, [this](int index) {
		if (index == 0)
		{
			Qi::set.lang.clear();
			Qi::lang.name.clear();
			Qi::lang.map.clear();
			Qi::widget.langReload();
		}
		else
		{
			QString lang = ui.lang_combo->currentText();
			auto language = Language::load(Language::makePath(lang));
			if (language.map.empty()) return;
			Qi::set.lang = lang;
			Qi::lang.name = lang;
			Qi::lang.map = std::move(language.map);
			Qi::widget.langReload();
		}
		QiJson::SaveJson();
		});
	connect(ui.ocr_lang_combo, &QComboBox::currentIndexChanged, this, [this](int index) {
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
	connect(ui.ocr_thread_combo, &QComboBox::currentIndexChanged, this, [this](int index) {
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
	connect(ui.theme_combo, &QComboBox::currentIndexChanged, this, [this](int index) {
		if (index >= 0 && index != Qi::set.theme)
		{
			Qi::set.theme = index;
			Qi::application->setStyleSheet(Qi::ui.themes[Qi::set.theme].style);
			QiJson::SaveJson();
		}});
	connect(ui.macro_save_combo, &QComboBox::currentIndexChanged, this, [this](int index) {
		if (index >= 0 && index != Qi::set.save_type)
		{
			Qi::set.save_type = index;
			QiJson::SaveJson();
		}
		});
	connect(ui.stateKey_keyedit, &QKeyEdit::changed, this, [this] {
		QKeyEditKeys keys = ui.stateKey_keyedit->keys();
		Qi::set.key1 = Qi::set.key2 = 0;
		if (!keys.isEmpty()) Qi::set.key1 = keys.at(0);
		if (keys.size() > 1) Qi::set.key2 = keys.at(1);
		if (keys.size() == 1 && keys.at(0) == VK_LBUTTON)
		{
			ui.stateKey_keyedit->setKey(VK_F8);
			Qi::set.key1 = VK_F8;
			Qi::set.key2 = 0;
		}
		QiJson::SaveJson();
		});
	connect(ui.recordKey_keyedit, &QKeyEdit::changed, this, [this] {
		QKeyEditKey key = ui.recordKey_keyedit->key();
		Qi::set.recKey = key;
		if (key == VK_LBUTTON)
		{
			ui.recordKey_keyedit->setKey(VK_F8);
			Qi::set.recKey = VK_F8;
		}
		QiJson::SaveJson();
		});
#ifdef Q_DRIVER
	connect(ui.driver_check, &QCheckBox::toggled, this, [this](bool state) 
		{
			if (state && !Qi::driver.isInit())
			{
				QiFn::InitDriver();
				if (Qi::driver.isInit()) sets.driver = true;
				else ui.driver_check->setChecked(sets.driver = false);
			}
			else sets.driver = false;
			QiJson::SaveJson();
		});
#endif
	connect(ui.recordTrack_check, &QCheckBox::toggled, this, [this](bool state) { sets.recTrack = state; QiJson::SaveJson(); });
	connect(ui.enableDefault_check, &QCheckBox::toggled, this, [this](bool state) { sets.defOn = state; QiJson::SaveJson(); });
	connect(ui.showState_check, &QCheckBox::toggled, this, [this](bool state) { sets.showTips = state; QiJson::SaveJson(); });
	connect(ui.sound_check, &QCheckBox::toggled, this, [this](bool state) { sets.audFx = state; QiJson::SaveJson(); });
	connect(ui.hideDefault_check, &QCheckBox::toggled, this, [this](bool state) { sets.minMode = state; QiJson::SaveJson(); });
	connect(ui.pad_check, &QCheckBox::toggled, this, [this](bool state) { sets.pad = state; Qi::widget.keyEditReload(); QiJson::SaveJson(); });
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
	style_set_group(ui.readme_button, "settings-button");
	style_set_group(ui.popText_button, "settings-button");
#ifdef Q_DRIVER
	style_set_group(ui.driver_check, "check");
#endif
	style_set_group(ui.recordTrack_check, "check");
	style_set_group(ui.enableDefault_check, "check");
	style_set_group(ui.showState_check, "check");
	style_set_group(ui.sound_check, "check");
	style_set_group(ui.hideDefault_check, "check");
	style_set_group(ui.pad_check, "check");
	style_set_group(ui.start_check, "check");
	style_set_group(ui.lang_combo, "combo");
	ui.lang_combo->setView(new QListView());
	style_set_group(ui.lang_combo->view(), "combo_body");
	style_set_group(ui.ocr_lang_combo, "combo");
	ui.ocr_lang_combo->setView(new QListView());
	style_set_group(ui.ocr_lang_combo->view(), "combo_body");
	style_set_group(ui.ocr_thread_combo, "combo");
	ui.ocr_thread_combo->setView(new QListView());
	style_set_group(ui.ocr_thread_combo->view(), "combo_body");
	style_set_group(ui.theme_combo, "combo");
	ui.theme_combo->setView(new QListView());
	ui.theme_combo->style_set_group(view(), "combo_body");
	style_set_group(ui.macro_save_combo, "combo");
	ui.macro_save_combo->setView(new QListView());
	style_set_group(ui.macro_save_combo->view(), "combo_body");
	style_set_group(ui.stateKey_keyedit, "line_edit");
	style_set_group(ui.recordKey_keyedit, "line_edit");
	ui.scrollArea_widget->setStyleSheet(QString("#") + ui.scrollArea_widget->objectName() + "{background-color:rgba(0,0,0,0)}");
	ui.scrollArea->setStyleSheet("QScrollArea,QScrollBar,QScrollBar::sub-line,QScrollBar::add-line{background-color:rgba(0,0,0,0);border:none}QScrollBar::handle{background-color:rgba(128,128,128,0.3);border:none}");
}
void SettingsUi::LoadLanguage()
{
	std::call_once(lang_once, [this] {
		lang_save_t(ui.readme_label);
		lang_save_t(ui.readme_button);

		lang_save_t(ui.popText_label);
		lang_save_t(ui.popText_button);

		lang_save_t(ui.theme_label);
		lang_save_cmb(ui.theme_combo);

		lang_save_t(ui.ocr_lang_label);
		lang_save_t(ui.ocr_thread_label);
		lang_save_icmb(ui.ocr_lang_combo, 0);
		lang_save_icmb(ui.ocr_thread_combo, 0);

		lang_save_t(ui.macro_save_label);
		lang_save_cmb(ui.macro_save_combo);

		lang_save_t(ui.stateKey_label);
		lang_save_t(ui.recordKey_label);
		lang_save_t(ui.recordTrack_label);
		lang_save_t(ui.enableDefault_label);
		lang_save_t(ui.showState_label);
		lang_save_t(ui.sound_label);
		lang_save_t(ui.hideDefault_label);
#ifdef Q_KEYEDIT_PAD_ENABLED
		lang_save_t(ui.pad_label);
#endif
		lang_save_t(ui.start_label);
	});
	lang_load_t(ui.readme_label);
	lang_load_t(ui.readme_button);

	lang_load_t(ui.popText_label);
	lang_load_t(ui.popText_button);

	lang_load_t(ui.theme_label);
	lang_load_cmb(ui.theme_combo);

	lang_load_t(ui.ocr_lang_label);
	lang_load_t(ui.ocr_thread_label);
	lang_load_icmb(ui.ocr_lang_combo, 0);
	lang_load_icmb(ui.ocr_thread_combo, 0);

	lang_load_t(ui.macro_save_label);
	lang_load_cmb(ui.macro_save_combo);

	lang_load_t(ui.stateKey_label);
	lang_load_ke(ui.stateKey_keyedit);
	lang_load_t(ui.recordKey_label);
	lang_load_ke(ui.recordKey_keyedit);
	lang_load_t(ui.recordTrack_label);
	lang_load_t(ui.enableDefault_label);
	lang_load_t(ui.showState_label);
	lang_load_t(ui.sound_label);
	lang_load_t(ui.hideDefault_label);
#ifdef Q_KEYEDIT_PAD_ENABLED
	lang_load_t(ui.pad_label);
#endif
	lang_load_t(ui.start_label);
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
void SettingsUi::customEvent(QEvent* e)
{
	if (e->type() == static_cast<int>(QiEvent::lang_reload))
	{
		LoadLanguage();
	}
#ifdef Q_KEYEDIT_PAD_ENABLED
	else if (e->type() == static_cast<int>(QiEvent::key_reset))
	{
		ui.stateKey_keyedit->setPadEnabled(Qi::set.pad);
		ui.recordKey_keyedit->setPadEnabled(Qi::set.pad);
	}
#endif
}