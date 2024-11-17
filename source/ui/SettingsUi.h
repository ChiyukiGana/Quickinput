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
	SettingsData* sets = &qis.set;
public:
	SettingsUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
		ReStyle();
	}
	void SetStyleGroup()
	{
		ui.clientWidget->setProperty("group", QVariant(QString::fromUtf8("client")));
		ui.bnReadme->setProperty("group", QVariant(QString::fromUtf8("settings-button")));
		ui.bnTboxs->setProperty("group", QVariant(QString::fromUtf8("settings-button")));
		ui.chbDefOn->setProperty("group", QVariant(QString::fromUtf8("check")));
		ui.chbShowTips->setProperty("group", QVariant(QString::fromUtf8("check")));
		ui.chbAudFx->setProperty("group", QVariant(QString::fromUtf8("check")));
		ui.chbMinMode->setProperty("group", QVariant(QString::fromUtf8("check")));
		ui.chbZoomBlock->setProperty("group", QVariant(QString::fromUtf8("check")));
		ui.chbStart->setProperty("group", QVariant(QString::fromUtf8("check")));
		ui.cmbTheme->setProperty("group", QVariant(QString::fromUtf8("combo")));
		ui.hkKey->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.hkRec->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.cmbTheme->view()->setProperty("group", QVariant(QString::fromUtf8("table")));
	}
	void ReStyle()
	{
		setStyleSheet("");
		setStyleSheet(qis.ui.themes[qis.set.theme].style);
		more.setStyleSheet("");
		more.setStyleSheet(qis.ui.themes[qis.set.theme].style);
		ui.hkKey->setStyleSheet("");
		ui.hkKey->setStyleSheet(qis.ui.themes[qis.set.theme].style);
		ui.hkRec->setStyleSheet("");
		ui.hkRec->setStyleSheet(qis.ui.themes[qis.set.theme].style);
	}
private:
	void WidInit()
	{
		ui.hkKey->setMode(QKeyEdit::Mode::multiple);
		ui.hkKey->setMultipleMax(2);
		ui.hkKey->setMouseEnable(true);
		ui.hkKey->setWheelEnable(true);
		ui.hkKey->setPadEnable(true);

		ui.hkRec->setMode(QKeyEdit::Mode::multiple);
		ui.hkRec->setMultipleMax(2);
		ui.hkRec->setMouseEnable(true);
		ui.hkRec->setWheelEnable(true);
		ui.hkRec->setPadEnable(true);

		ui.cmbTheme->setEditable(true);
		ui.cmbTheme->lineEdit()->setReadOnly(true); 
		ui.cmbTheme->lineEdit()->setAlignment(Qt::AlignCenter);
		for (size_t i = 0; i < qis.ui.themes.size(); i++) ui.cmbTheme->addItem(qis.ui.themes[i].name);
		QStandardItemModel* model = (QStandardItemModel*)ui.cmbTheme->view()->model();
		for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);
		if (qis.set.theme >= qis.ui.themes.size()) qis.set.theme = 0;
		ui.cmbTheme->setCurrentIndex(qis.set.theme);

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

		ui.chbDefOn->setChecked(sets->defOn);
		ui.chbShowTips->setChecked(sets->showTips);
		ui.chbAudFx->setChecked(sets->audFx);
		ui.chbMinMode->setChecked(sets->minMode);
		ui.chbStart->setChecked(Task::Find(L"QuickInput"));
		ui.chbZoomBlock->setChecked(sets->zoomBlock);

		if ("clear shortcut")
		{
			ui.bnReadme->setShortcut(Qt::Key_unknown);
			ui.bnTboxs->setShortcut(Qt::Key_unknown);
			ui.chbDefOn->setShortcut(Qt::Key_unknown);
			ui.chbShowTips->setShortcut(Qt::Key_unknown);
			ui.chbAudFx->setShortcut(Qt::Key_unknown);
			ui.chbMinMode->setShortcut(Qt::Key_unknown);
			ui.chbZoomBlock->setShortcut(Qt::Key_unknown);
			ui.chbStart->setShortcut(Qt::Key_unknown);
		}

		SetStyleGroup();
	}
	void WidEvent()
	{
		connect(ui.bnReadme, SIGNAL(clicked()), this, SLOT(OnBnReadme()));
		connect(ui.bnTboxs, SIGNAL(clicked()), this, SLOT(OnBnTboxs()));
		connect(ui.cmbTheme, SIGNAL(activated(int)), this, SLOT(OnCmbTheme(int)));
		connect(ui.hkKey, SIGNAL(changed()), this, SLOT(OnHkKey()));
		connect(ui.hkRec, SIGNAL(changed()), this, SLOT(OnHkRec()));
		connect(ui.chbDefOn, SIGNAL(clicked()), this, SLOT(OnDefOn()));
		connect(ui.chbShowTips, SIGNAL(clicked()), this, SLOT(OnShowTips()));
		connect(ui.chbAudFx, SIGNAL(clicked()), this, SLOT(OnAudFx()));
		connect(ui.chbMinMode, SIGNAL(clicked()), this, SLOT(OnMinMode()));
		connect(ui.chbStart, SIGNAL(clicked()), this, SLOT(OnStart()));
		connect(ui.chbZoomBlock, SIGNAL(clicked()), this, SLOT(OnZoomBlock()));
	}
	void closeEvent(QCloseEvent* et)
	{
		more.close();
	}
private Q_SLOTS:
	void OnBnReadme() { more.show(); }
	void OnBnTboxs()
	{
		PopsUi ps;
		qis.widget.dialogActive = true;
		qis.widget.main->hide();
		ps.exec();
		qis.widget.main->show();
		qis.widget.dialogActive = false;
		QiJson::SaveJson();
	}
	void OnCmbTheme(int item)
	{
		if (qis.set.theme != item)
		{
			qis.set.theme = item;
			QApplication::sendEvent(qis.widget.main, new QEvent((QEvent::Type)QiEvent::setTheme));
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
		qis.set.key = vk;
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
		qis.set.recKey = vk;
		QiJson::SaveJson();
	}
	void OnDefOn() { sets->defOn = ui.chbDefOn->isChecked(); QiJson::SaveJson(); }
	void OnShowTips() { sets->showTips = ui.chbShowTips->isChecked(); QiJson::SaveJson(); }
	void OnAudFx() { sets->audFx = ui.chbAudFx->isChecked(); QiJson::SaveJson(); }
	void OnMinMode() { sets->minMode = ui.chbMinMode->isChecked(); QiJson::SaveJson(); }
	void OnZoomBlock() { sets->zoomBlock = ui.chbZoomBlock->isChecked(); QiJson::SaveJson(); }
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