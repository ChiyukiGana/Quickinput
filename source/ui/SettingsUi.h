#pragma execution_character_set("utf-8")
#pragma once
#include <qstandarditemmodel.h>
#include <qabstractitemview.h>
#include "MoreUi.h"
#include "PopsUi.h"
#include "../src/minc.h"
#include "ui_SettingsUi.h"

class SettingsUi : public QWidget
{
	Q_OBJECT;
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
		ui.clientWidget->setProperty("group", "client");
		ui.bnReadme->setProperty("group", "settings-button");
		ui.bnTboxs->setProperty("group", "settings-button");
		ui.chbRecTrack->setProperty("group", "check");
		ui.chbDefOn->setProperty("group", "check");
		ui.chbShowTips->setProperty("group", "check");
		ui.chbAudFx->setProperty("group", "check");
		ui.chbMinMode->setProperty("group", "check");
		ui.chbScaleBlock->setProperty("group", "check");
		ui.chbStart->setProperty("group", "check");
		ui.cmbTheme->setProperty("group", "combo");
		ui.hkKey->setProperty("group", "line_edit");
		ui.hkRec->setProperty("group", "line_edit");
		ui.cmbTheme->view()->setProperty("group", "table");
	}
private:
	void Init()
	{
		ui.hkKey->setMode(QKeyEdit::Mode::solid);
		ui.hkKey->setMaxKeys(2);
		ui.hkKey->setMouseEnable(true);
		ui.hkKey->setWheelEnable(true);
		ui.hkKey->setPadEnable(true);

		ui.hkRec->setMode(QKeyEdit::Mode::solid);
		ui.hkRec->setMaxKeys(2);
		ui.hkRec->setMouseEnable(true);
		ui.hkRec->setWheelEnable(true);
		ui.hkRec->setPadEnable(true);

		ui.cmbTheme->setEditable(true);
		ui.cmbTheme->lineEdit()->setReadOnly(true); 
		ui.cmbTheme->lineEdit()->setAlignment(Qt::AlignCenter);
		for (size_t i = 0; i < Qi::ui.themes.size(); i++) ui.cmbTheme->addItem(Qi::ui.themes[i].name);
		QStandardItemModel* model = (QStandardItemModel*)ui.cmbTheme->view()->model();
		for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);
		if (Qi::set.theme >= Qi::ui.themes.size()) Qi::set.theme = 0;
		ui.cmbTheme->setCurrentIndex(Qi::set.theme);

		if ("key edit")
		{
			QList<QKeyEdit::Key> keys;
			keys.push_back(QKeyEdit::Key(sets->key & 0xFFFF));
			keys.push_back(QKeyEdit::Key(sets->key >> 16));
			ui.hkKey->setKeys(keys);
			keys.clear();

			keys.push_back(QKeyEdit::Key(sets->recKey & 0xFFFF));
			keys.push_back(QKeyEdit::Key(sets->recKey >> 16));
			ui.hkRec->setKeys(keys);
		}

		ui.chbRecTrack->setChecked(sets->recTrack);
		ui.chbDefOn->setChecked(sets->defOn);
		ui.chbShowTips->setChecked(sets->showTips);
		ui.chbAudFx->setChecked(sets->audFx);
		ui.chbMinMode->setChecked(sets->minMode);
		ui.chbStart->setChecked(Task::Find(L"QuickInput"));
		ui.chbScaleBlock->setChecked(sets->scaleBlock);

		if ("clear shortcut")
		{
			ui.bnReadme->installEventFilter(this);
			ui.bnTboxs->installEventFilter(this);
			ui.chbRecTrack->installEventFilter(this);
			ui.chbDefOn->installEventFilter(this);
			ui.chbShowTips->installEventFilter(this);
			ui.chbAudFx->installEventFilter(this);
			ui.chbMinMode->installEventFilter(this);
			ui.chbScaleBlock->installEventFilter(this);
			ui.chbStart->installEventFilter(this);
		}
	}
	void Event()
	{
		connect(ui.bnReadme, SIGNAL(clicked()), this, SLOT(OnBnReadme()));
		connect(ui.bnTboxs, SIGNAL(clicked()), this, SLOT(OnBnTboxs()));
		connect(ui.cmbTheme, SIGNAL(activated(int)), this, SLOT(OnCmbTheme(int)));
		connect(ui.hkKey, SIGNAL(changed()), this, SLOT(OnHkKey()));
		connect(ui.hkRec, SIGNAL(changed()), this, SLOT(OnHkRec()));
		connect(ui.chbRecTrack, SIGNAL(clicked()), this, SLOT(OnRecTrack()));
		connect(ui.chbDefOn, SIGNAL(clicked()), this, SLOT(OnDefOn()));
		connect(ui.chbShowTips, SIGNAL(clicked()), this, SLOT(OnShowTips()));
		connect(ui.chbAudFx, SIGNAL(clicked()), this, SLOT(OnAudFx()));
		connect(ui.chbMinMode, SIGNAL(clicked()), this, SLOT(OnMinMode()));
		connect(ui.chbStart, SIGNAL(clicked()), this, SLOT(OnStart()));
		connect(ui.chbScaleBlock, SIGNAL(clicked()), this, SLOT(OnScaleBlock()));
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
private Q_SLOTS:
	void OnBnReadme() { more.show(); }
	void OnBnTboxs()
	{
		PopsUi ps;
		Qi::widget.dialogActive = true;
		Qi::widget.main->hide();
		ps.exec();
		Qi::widget.main->show();
		Qi::widget.dialogActive = false;
		QiJson::SaveJson();
	}
	void OnCmbTheme(int item)
	{
		if (Qi::set.theme != item)
		{
			Qi::set.theme = item;
			Qi::application->setStyleSheet(Qi::ui.themes[Qi::set.theme].style);
			QiJson::SaveJson();
		}
	}
	void OnHkKey() {
		QList<QKeyEdit::Key> keys = ui.hkKey->keys();
		DWORD vk = VK_F8;
		if (keys.size() == 1)
		{
			if (keys[0].keyCode == VK_LBUTTON) ui.hkKey->setKey(QKeyEdit::Key(VK_F8));
			else vk = keys[0].keyCode;
		}
		else if (keys.size() == 2)
		{
			vk = keys[0].keyCode | (keys[1].keyCode << 16);
		}
		Qi::set.key = vk;
		QiJson::SaveJson();
	}
	void OnHkRec() {
		QList<QKeyEdit::Key> keys = ui.hkRec->keys();
		DWORD vk = VK_F8;
		if (keys.size() == 1)
		{
			if (keys[0].keyCode == VK_LBUTTON) ui.hkRec->setKey(QKeyEdit::Key(VK_F8));
			else vk = keys[0].keyCode;
		}
		else if (keys.size() == 2)
		{
			vk = keys[0].keyCode | (keys[1].keyCode << 16);
		}
		Qi::set.recKey = vk;
		QiJson::SaveJson();
	}
	void OnRecTrack()
	{
		sets->recTrack = ui.chbRecTrack->isChecked(); QiJson::SaveJson();
	}
	void OnDefOn() { sets->defOn = ui.chbDefOn->isChecked(); QiJson::SaveJson(); }
	void OnShowTips() { sets->showTips = ui.chbShowTips->isChecked(); QiJson::SaveJson(); }
	void OnAudFx() { sets->audFx = ui.chbAudFx->isChecked(); QiJson::SaveJson(); }
	void OnMinMode() { sets->minMode = ui.chbMinMode->isChecked(); QiJson::SaveJson(); }
	void OnScaleBlock() { sets->scaleBlock = ui.chbScaleBlock->isChecked(); QiJson::SaveJson(); }
	void OnStart()
	{
		if (Task::Find(L"QuickInput"))
		{
			if (Task::Delete(L"QuickInput")) ui.chbStart->setChecked(false);
			else
			{
				ui.chbStart->setChecked(true);
				MsgBox::Error(L"需要以管理员权限运行", L"删除任务错误");
			}
		}
		else
		{
			if (Task::Register(L"QuickInput")) ui.chbStart->setChecked(true);
			else
			{
				ui.chbStart->setChecked(false);
				MsgBox::Error(L"需要以管理员权限运行", L"创建任务错误");
			}
		}
	}
};