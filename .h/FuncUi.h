#pragma once
#include <qvalidator.h>
#include "ui_FuncUi.h"
#include "../static.h"

class FuncUi : public QWidget
{
	Q_OBJECT;
	Ui::FuncUiClass ui;
	FuncData* func = &Global::qi.fun;

public:
	bool working = 0;

	FuncUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
	}

private:
	void WidInit()
	{
		ui.hkQkClick->Mode(0);
		ui.etQkDelay->setValidator(new QIntValidator(0, 99999, this));
		ui.cmbMode->addItem(u8"按下");
		ui.cmbMode->addItem(u8"切换");
		ui.chbQkClick->setChecked(func->quickClick.state);
		ui.hkQkClick->VirtualKey(func->quickClick.key);
		ui.etQkDelay->setText(QString::number(func->quickClick.delay));
		ui.cmbMode->setCurrentIndex(func->quickClick.mode);

		ui.chbWndActive->setChecked(func->wndActive.state);
		ui.etWndActive->setText(QString::fromWCharArray(func->wndActive.wi.wndName.c_str()));

		ui.hkClock->Mode(0);
		ui.chbClock->setChecked(func->showClock.state);
		ui.hkClock->VirtualKey(func->showClock.key);
	}
	void WidEvent()
	{
		connect(ui.chbQkClick, SIGNAL(stateChanged(int)), this, SLOT(OnQkClick(int)));
		connect(ui.hkQkClick, SIGNAL(changed()), this, SLOT(OnQkClickKey()));
		connect(ui.etQkDelay, SIGNAL(textEdited(const QString&)), this, SLOT(OnQkDelay(const QString&)));
		connect(ui.cmbMode, SIGNAL(activated(int)), this, SLOT(OnCmbMode(int)));

		connect(ui.chbWndActive, SIGNAL(stateChanged(int)), this, SLOT(OnWndActive(int)));
		connect(ui.bnWndActive, SIGNAL(clicked()), this, SLOT(OnBnWndActive()));

		connect(ui.chbClock, SIGNAL(stateChanged(int)), this, SLOT(OnClock(int)));
		connect(ui.hkClock, SIGNAL(changed()), this, SLOT(OnShowClock()));
	}

private slots:
	// Click
	void OnQkClick(int state) { func->quickClick.state = state; SaveJson(); }
	void OnQkClickKey() { func->quickClick.key = ui.hkQkClick->virtualKey(); SaveJson(); }
	void OnQkDelay(const QString& text) { func->quickClick.delay = text.toInt(); SaveJson(); }
	void OnCmbMode(int row) { func->quickClick.mode = row; SaveJson(); }
	// WndControl
	void OnWndActive(int state) { func->wndActive.state = state; SaveJson(); }
	void OnBnWndActive()
	{
		working = 1;
		Global::qi.main->hide();
		func->wndActive.wi = WindowSelection();
		ui.etWndActive->setText(QString::fromWCharArray(func->wndActive.wi.wndName.c_str()));
		working = 0;
		Global::qi.main->show();
		SaveJson();
	}
	// Clock
	void OnClock(int state) { func->showClock.state = state; SaveJson(); }
	void OnShowClock() { func->showClock.key = ui.hkClock->virtualKey(); SaveJson(); }
};