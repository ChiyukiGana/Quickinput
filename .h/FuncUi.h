#pragma once
#pragma execution_character_set("utf-8")
#include <qvalidator.h>
#include "ui_FuncUi.h"
#include "QuickInputDef.h"

class FuncUi : public QWidget
{
	Q_OBJECT

public:

	FuncUi(QWidget* parent = 0, QuickInputStruct* qis = 0) : QWidget(parent)
	{
		this->qis = qis;

		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		ControlInit();
		ControlEvent();
	}

private:

	Ui::FuncUiClass ui;
	QuickInputStruct* qis;

	void ControlInit()
	{
		ui.chbQkClick->setChecked(qis->fun.quickClick.state);
		ui.chbClock->setChecked(qis->fun.showClock.state);
		ui.chbWndActive->setChecked(qis->fun.wndActive.state);

		ui.etQkDelay->setText(QString::number(qis->fun.quickClick.delay));
		ui.etQkDelay->setValidator(new QIntValidator(0, 100000, this));
		ui.etWndActive->setText(QString::fromWCharArray(qis->fun.wndActive.name.c_str()));

		ui.hkQkClick->Mode(0);
		ui.hkQkClick->VirtualKey(qis->fun.quickClick.key);
		ui.hkClock->Mode(0);
		ui.hkClock->VirtualKey(qis->fun.showClock.key);

		ui.cmbMode->addItem("按下");
		ui.cmbMode->addItem("切换");
		ui.cmbMode->setCurrentIndex(qis->fun.quickClick.mode);

	}

	void ControlEvent()
	{
		connect(ui.hkQkClick, SIGNAL(changed()), this, SLOT(OnQkClickKey()));
		connect(ui.etQkDelay, SIGNAL(textEdited(const QString&)), this, SLOT(OnQkDelay(const QString&)));
		connect(ui.hkClock, SIGNAL(changed()), this, SLOT(OnShowClock()));
		connect(ui.chbQkClick, SIGNAL(stateChanged(int)), this, SLOT(OnQkClick(int)));
		connect(ui.chbClock, SIGNAL(stateChanged(int)), this, SLOT(OnClock(int)));
		connect(ui.chbWndActive, SIGNAL(stateChanged(int)), this, SLOT(OnWndActive(int)));
		connect(ui.cmbMode, SIGNAL(activated(int)), this, SLOT(OnCmbMode(int)));
		connect(ui.bnWndActive, SIGNAL(clicked()), this, SLOT(OnBnWndActive()));
	}

private slots:

	void OnQkClickKey()
	{
		qis->fun.quickClick.key = ui.hkQkClick->virtualKey();
		SaveJson(qis);
	}

	void OnQkDelay(const QString& text)
	{
		qis->fun.quickClick.delay = text.toInt();
		SaveJson(qis);
	}

	void OnShowClock()
	{
		qis->fun.showClock.key = ui.hkClock->virtualKey();
		SaveJson(qis);
	}

	void OnQkClick(int state)
	{
		qis->fun.quickClick.state = state;
		SaveJson(qis);
	}

	void OnClock(int state)
	{
		qis->fun.showClock.state = state;
		SaveJson(qis);
	}

	void OnCmbMode(int row)
	{
		qis->fun.quickClick.mode = row;
		SaveJson(qis);
	}

	void OnWndActive(int state)
	{
		qis->fun.wndActive.state = state;
		SaveJson(qis);
	}

	void OnBnWndActive()
	{
		TipsWindow::Show(L"按回车开始，切换到对应窗口并再按回车", RGB(0x20, 0xFF, 0x20));
		while (!Input::state(VK_RETURN)) sleep(10);
		while (Input::state(VK_RETURN)) sleep(10);

		WCHAR name[MAX_PATH];
		while (!Input::state(VK_RETURN))
		{
			qis->fun.wndActive.wnd = GetForegroundWindow();
			GetWindowTextW(qis->fun.wndActive.wnd, name, MAX_PATH);
			TipsWindow::Show(name, RGB(0xCC, 0xEE, 0xFF));
			sleep(50);
		}
		TipsWindow::Popup(name, RGB(0x20, 0xFF, 0x20));
		qis->fun.wndActive.name = name;
		ui.etWndActive->setText(QString::fromWCharArray(qis->fun.wndActive.name.c_str()));

		SaveJson(qis);
	}
};