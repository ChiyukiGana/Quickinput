#pragma once
#include <qevent.h>
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
		ReStyle();
	}

	void ReStyle()
	{
		ui.clientWidget->setStyleSheet(Global::qi.styles[Global::qi.set.style].style);
		ui.hkKey->setStyleSheet(Global::qi.styles[Global::qi.set.style].style);
		ui.hkRec->setStyleSheet(Global::qi.styles[Global::qi.set.style].style);
	}

private:
	void WidInit()
	{
		for (size_t i = 0; i < Global::qi.styles.size(); i++)
		{
			ui.cmbTheme->addItem(Global::qi.styles[i].name);
		}
		if (Global::qi.set.style >= Global::qi.styles.size()) Global::qi.set.style = 0;
		ui.cmbTheme->setCurrentIndex(Global::qi.set.style);

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
		if (Global::qi.set.style != item)
		{
			Global::qi.set.style = item;
			QApplication::sendEvent(Global::qi.main, new QEvent(QEvent::Type::User));
			SaveJson();
		}
	}
	void OnHkKey() {
		sets->key = ui.hkKey->virtualKey();
		if (ui.hkKey->virtualKey() == VK_LBUTTON)
		{
			ui.hkKey->VirtualKey(VK_F8);
		}
		SaveJson();
	}
	void OnHkRec() {
		sets->recKey = ui.hkRec->virtualKey();
		if (ui.hkRec->virtualKey() == VK_LBUTTON)
		{
			ui.hkRec->VirtualKey(VK_F8);
		}
		SaveJson();
	}
	void OnDefOn() { sets->defOn = ui.chbDefOn->isChecked(); SaveJson(); }
	void OnShowTips() { sets->showTips = ui.chbShowTips->isChecked(); SaveJson(); }
	void OnAudFx() { sets->audFx = ui.chbAudFx->isChecked(); SaveJson(); }
	void OnMinMode() { sets->minMode = ui.chbMinMode->isChecked(); SaveJson(); }
	void OnZoomBlock() { sets->zoomBlock = ui.chbZoomBlock->isChecked(); SaveJson(); }
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