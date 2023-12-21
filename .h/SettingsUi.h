#pragma once
#pragma execution_character_set("utf-8")
#include "ui_SettingsUi.h"
#include "QuickInputDef.h"

class SettingsUi : public QWidget
{
	Q_OBJECT

public:

	SettingsUi(QWidget* parent = 0, QuickInputStruct* qis = 0) : QWidget(parent)
	{
		this->qis = qis;

		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		ControlInit();
		ControlEvent();
	}

private:

	Ui::SettingsUiClass ui;

	QuickInputStruct* qis;

	void ControlInit()
	{
		ui.hkKey->Mode(2);
		ui.hkRec->Mode(0);
		ui.hkKey->VirtualKey(qis->set.key & 0xFFFF, qis->set.key >> 16);
		ui.hkRec->VirtualKey(qis->set.recKey);

		ui.chbShowTips->setChecked(qis->set.showTips);
		ui.chbAudFx->setChecked(qis->set.audFx);
		ui.chbMinMode->setChecked(qis->set.minMode);
		ui.chbStart->setChecked(Task::Find(L"QuickInput").result);
		ui.chbZoom->setChecked(qis->set.wndZoom);
	}

	void ControlEvent()
	{
		connect(ui.hkKey, SIGNAL(changed()), this, SLOT(OnHkKey()));
		connect(ui.hkRec, SIGNAL(changed()), this, SLOT(OnHkRec()));
		connect(ui.chbShowTips, SIGNAL(clicked()), this, SLOT(OnShowTips()));
		connect(ui.chbAudFx, SIGNAL(clicked()), this, SLOT(OnAud()));
		connect(ui.chbMinMode, SIGNAL(clicked()), this, SLOT(OnMinMode()));
		connect(ui.chbStart, SIGNAL(clicked()), this, SLOT(OnStart()));
		connect(ui.chbZoom, SIGNAL(clicked()), this, SLOT(OnZoom()));
	}

private slots:

	void OnHkKey()
	{
		qis->set.key = ui.hkKey->virtualKey();
		SaveJson(qis);
	}

	void OnHkRec()
	{
		qis->set.recKey = ui.hkRec->virtualKey();
		SaveJson(qis);
	}

	void OnAud()
	{
		qis->set.audFx = ui.chbAudFx->isChecked();
		SaveJson(qis);
	}

	void OnShowTips()
	{
		qis->set.showTips = ui.chbShowTips->isChecked();
		SaveJson(qis);
	}

	void OnMinMode()
	{
		qis->set.minMode = ui.chbMinMode->isChecked();
		SaveJson(qis);
	}

	void OnStart()
	{
		if (Task::Find(L"QuickInput").result)
		{
			Task::TaskStruct ts = Task::Delete(L"QuickInput");
			if (!ts.result)
			{
				MsgBox::Error(ts.log.c_str(), L"错误");
			}
		}
		else
		{
			Task::TaskStruct ts = Task::Register(L"QuickInput");
			if (!ts.result)
			{
				MsgBox::Error(ts.log.c_str(), L"错误");
			}
		}
	}

	void OnZoom()
	{
		qis->set.wndZoom = ui.chbZoom->isChecked();
		SaveJson(qis);
	}
};