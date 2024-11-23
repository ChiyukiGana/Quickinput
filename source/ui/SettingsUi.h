#pragma once
#include <qstandarditemmodel.h>
#include <qabstractitemview.h>
#include "MoreUi.h"
#include "PopsUi.h"
#include <src/inc_header.h>
#include "ui_SettingsUi.h"

class SettingsUi : public QWidget
{
	Q_OBJECT;
	using This = SettingsUi;
	Ui::SettingsUiClass ui;
	MoreUi more;
	SettingsData* sets = &Qi::set;
public:
	SettingsUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		Init();
		Event();
		StyleGroup();
	}
	void StyleGroup()
	{
		ui.content_widget->setProperty("group", "client");
		ui.readme_button->setProperty("group", "settings-button");
		ui.popText_button->setProperty("group", "settings-button");
		ui.recordTrack_check->setProperty("group", "check");
		ui.enableDefault_check->setProperty("group", "check");
		ui.showState_check->setProperty("group", "check");
		ui.sound_check->setProperty("group", "check");
		ui.hideDefault_check->setProperty("group", "check");
		ui.start_check->setProperty("group", "check");
		ui.theme_combo->setProperty("group", "combo");
		ui.stateKey_keyedit->setProperty("group", "line_edit");
		ui.recordKey_keyedit->setProperty("group", "line_edit");
		ui.theme_combo->view()->setProperty("group", "table");
	}
private:
	void Init()
	{
		// state key
		ui.stateKey_keyedit->setMode(QKeyEdit::Mode::solid);
		ui.stateKey_keyedit->setMaxKeys(2);
		ui.stateKey_keyedit->setMouseEnable(true);
		ui.stateKey_keyedit->setWheelEnable(true);
		ui.stateKey_keyedit->setPadEnable(true);
		// record key
		ui.recordKey_keyedit->setMode(QKeyEdit::Mode::solid);
		ui.recordKey_keyedit->setMaxKeys(2);
		ui.recordKey_keyedit->setMouseEnable(true);
		ui.recordKey_keyedit->setWheelEnable(true);
		ui.recordKey_keyedit->setPadEnable(true);
		// theme
		ui.theme_combo->setEditable(true);
		ui.theme_combo->lineEdit()->setReadOnly(true); 
		ui.theme_combo->lineEdit()->setAlignment(Qt::AlignCenter);
		for (size_t i = 0; i < Qi::ui.themes.size(); i++) ui.theme_combo->addItem(Qi::ui.themes[i].name);
		QStandardItemModel* model = (QStandardItemModel*)ui.theme_combo->view()->model();
		for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);
		if (Qi::set.theme >= Qi::ui.themes.size()) Qi::set.theme = 0;
		ui.theme_combo->setCurrentIndex(Qi::set.theme);
		if ("key edit")
		{
			QList<QKeyEdit::Key> keys;
			keys.push_back(QKeyEdit::Key(sets->key & 0xFFFF));
			keys.push_back(QKeyEdit::Key(sets->key >> 16));
			ui.stateKey_keyedit->setKeys(keys);
			keys.clear();

			keys.push_back(QKeyEdit::Key(sets->recKey & 0xFFFF));
			keys.push_back(QKeyEdit::Key(sets->recKey >> 16));
			ui.recordKey_keyedit->setKeys(keys);
		}
		ui.recordTrack_check->setChecked(sets->recTrack);
		ui.enableDefault_check->setChecked(sets->defOn);
		ui.showState_check->setChecked(sets->showTips);
		ui.sound_check->setChecked(sets->audFx);
		ui.hideDefault_check->setChecked(sets->minMode);
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
			ui.start_check->installEventFilter(this);
		}
	}
	void Event()
	{
		connect(ui.readme_button, &QPushButton::clicked, this, [this] { more.show(); });
		connect(ui.popText_button, &QPushButton::clicked, this, [this]{
			PopsUi ps;
			Qi::widget.dialogActive = true;
			Qi::widget.main->hide();
			ps.exec();
			Qi::widget.main->show();
			Qi::widget.dialogActive = false;
			QiJson::SaveJson(); });
		connect(ui.theme_combo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
			if (Qi::set.theme != index)
			{
				Qi::set.theme = index;
				Qi::application->setStyleSheet(Qi::ui.themes[Qi::set.theme].style);
				QiJson::SaveJson();
			}});
		connect(ui.stateKey_keyedit, &QKeyEdit::changed, this, [this] {
			QList<QKeyEdit::Key> keys = ui.stateKey_keyedit->keys();
			DWORD vk = VK_F8;
			if (keys.size() == 1)
			{
				if (keys[0].keyCode == VK_LBUTTON) ui.stateKey_keyedit->setKey(QKeyEdit::Key(VK_F8));
				else vk = keys[0].keyCode;
			}
			else if (keys.size() == 2)
			{
				vk = keys[0].keyCode | (keys[1].keyCode << 16);
			}
			Qi::set.key = vk;
			QiJson::SaveJson(); });
		connect(ui.recordKey_keyedit, &QKeyEdit::changed, this, [this] {
			QList<QKeyEdit::Key> keys = ui.recordKey_keyedit->keys();
			DWORD vk = VK_F8;
			if (keys.size() == 1)
			{
				if (keys[0].keyCode == VK_LBUTTON) ui.recordKey_keyedit->setKey(QKeyEdit::Key(VK_F8));
				else vk = keys[0].keyCode;
			}
			else if (keys.size() == 2)
			{
				vk = keys[0].keyCode | (keys[1].keyCode << 16);
			}
			Qi::set.recKey = vk;
			QiJson::SaveJson(); });
		connect(ui.recordTrack_check, &QCheckBox::clicked, this, [this](bool state) { sets->recTrack = state; QiJson::SaveJson(); });
		connect(ui.enableDefault_check, &QCheckBox::clicked, this, [this](bool state) { sets->defOn = state; QiJson::SaveJson(); });
		connect(ui.showState_check, &QCheckBox::clicked, this, [this](bool state) { sets->showTips = state; QiJson::SaveJson(); });
		connect(ui.sound_check, &QCheckBox::clicked, this, [this](bool state) { sets->audFx = state; QiJson::SaveJson(); });
		connect(ui.hideDefault_check, &QCheckBox::clicked, this, [this](bool state) { sets->minMode = state; QiJson::SaveJson(); });
		connect(ui.start_check, &QCheckBox::clicked, this, [this] {
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
	bool event(QEvent* e)
	{
		if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
		{
			QKeyEvent* keyEvent = (QKeyEvent*)e;
			if ((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Space)) return true;
		}
		return QWidget::event(e);
	}
	bool eventFilter(QObject* obj, QEvent* e)
	{
		if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease)) return true;
		return QWidget::eventFilter(obj, e);
	}
	void closeEvent(QCloseEvent* e)
	{
		more.close();
	}
};