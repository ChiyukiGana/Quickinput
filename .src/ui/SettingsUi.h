#pragma once
#include <qevent.h>
#include "ui_SettingsUi.h"
#include "header.h"

class SettingsUi : public QWidget
{
	Q_OBJECT;
	Ui::SettingsUiClass ui;
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

	void ReStyle()
	{
		ui.clientWidget->setStyleSheet(qis.themes[qis.set.theme].style);
		ui.hkKey->setStyleSheet(qis.themes[qis.set.theme].style);
		ui.hkRec->setStyleSheet(qis.themes[qis.set.theme].style);
	}

private:
	void WidInit()
	{
		for (size_t i = 0; i < qis.themes.size(); i++) ui.cmbTheme->addItem(qis.themes[i].name);
		if (qis.set.theme >= qis.themes.size()) qis.set.theme = 0;
		ui.cmbTheme->setCurrentIndex(qis.set.theme);

		ui.hkKey->Mode(2);
		ui.hkRec->Mode(0);
		ui.hkKey->VirtualKey(sets->key & 0xFFFF, sets->key >> 16);
		ui.hkRec->VirtualKey(sets->recKey);

		ui.chbDefOn->setChecked(sets->defOn);
		ui.chbShowTips->setChecked(sets->showTips);
		ui.chbAudFx->setChecked(sets->audFx);
		ui.chbMinMode->setChecked(sets->minMode);
		ui.chbStart->setChecked(Task::Find(L"QuickInput"));
		ui.chbZoomBlock->setChecked(sets->zoomBlock);
	}

	void WidEvent()
	{
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

private slots:

	void OnCmbTheme(int item)
	{
		if (qis.set.theme != item)
		{
			qis.set.theme = item;
			QApplication::sendEvent(qis.widget.main, new QEvent((QEvent::Type)QiEvent::setTheme));
			QiFn::SaveJson();
		}
	}
	void OnHkKey() {
		DWORD vk = ui.hkKey->virtualKey();
		if (vk == VK_LBUTTON)
		{
			ui.hkKey->VirtualKey(VK_F8);
			vk = VK_F8;
		}
		qis.set.key = vk;
		QiFn::SaveJson();
	}
	void OnHkRec() {
		DWORD vk = ui.hkRec->virtualKey();
		if (vk == VK_LBUTTON)
		{
			ui.hkRec->VirtualKey(VK_F8);
			vk = VK_F8;
		}
		qis.set.recKey = vk;
		QiFn::SaveJson();
	}
	void OnDefOn() { sets->defOn = ui.chbDefOn->isChecked(); QiFn::SaveJson(); }
	void OnShowTips() { sets->showTips = ui.chbShowTips->isChecked(); QiFn::SaveJson(); }
	void OnAudFx() { sets->audFx = ui.chbAudFx->isChecked(); QiFn::SaveJson(); }
	void OnMinMode() { sets->minMode = ui.chbMinMode->isChecked(); QiFn::SaveJson(); }
	void OnZoomBlock() { sets->zoomBlock = ui.chbZoomBlock->isChecked(); QiFn::SaveJson(); }
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