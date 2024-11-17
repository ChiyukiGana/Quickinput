#pragma once
#include "ui_SettingsUi.h"
#include "../static.h"

class SettingsUi : public QWidget
{
	Q_OBJECT;
	Ui::SettingsUiClass ui;
	SettingsData* sets = &Global::qi.set;

public:
	SettingsUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
	}

private:
	void WidInit()
	{
		ui.hkKey->Mode(2);
		ui.hkRec->Mode(0);
		ui.hkKey->VirtualKey(sets->key & 0xFFFF, sets->key >> 16);
		ui.hkRec->VirtualKey(sets->recKey);

		ui.chbDefOn->setChecked(sets->defOn);
		ui.chbShowTips->setChecked(sets->showTips);
		ui.chbAudFx->setChecked(sets->audFx);
		ui.chbMinMode->setChecked(sets->minMode);
		ui.chbStart->setChecked(Task::Find(L"QuickInput").result);
		ui.chbZoomBlock->setChecked(sets->zoomBlock);
	}

	void WidEvent()
	{
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

	void OnHkKey() { sets->key = ui.hkKey->virtualKey(); SaveJson(); }
	void OnHkRec() { sets->recKey = ui.hkRec->virtualKey(); SaveJson(); }
	void OnDefOn() { sets->defOn = ui.chbDefOn->isChecked(); SaveJson(); }
	void OnShowTips() { sets->showTips = ui.chbShowTips->isChecked(); SaveJson(); }
	void OnAudFx() { sets->audFx = ui.chbAudFx->isChecked(); SaveJson(); }
	void OnMinMode() { sets->minMode = ui.chbMinMode->isChecked(); SaveJson(); }
	void OnZoomBlock() { sets->zoomBlock = ui.chbZoomBlock->isChecked(); SaveJson(); }

	void OnStart()
	{
		if (Task::Find(L"QuickInput").result)
		{
			Task::TaskStruct ts = Task::Delete(L"QuickInput");
			if (!ts.result) MsgBox::Error(ts.log.c_str(), L"删除任务错误");
		}
		else
		{
			Task::TaskStruct ts = Task::Register(L"QuickInput");
			if (!ts.result) MsgBox::Error(ts.log.c_str(), L"创建任务错误");
		}
	}
};