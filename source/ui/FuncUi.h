#pragma once
#include <qvalidator.h>
#include "../src/minc.h"
#include "ui_FuncUi.h"

class FuncUi : public QWidget
{
	Q_OBJECT;
	Ui::FuncUiClass ui;
	FuncData* func = &qis.fun;

public:
	FuncUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
	}

	void ReStyle()
	{
		ui.clientWidget->setStyleSheet("");
		ui.clientWidget->setStyleSheet(qis.ui.themes[qis.set.theme].style);
		ui.hkQkClick->setStyleSheet("");
		ui.hkQkClick->setStyleSheet(qis.ui.themes[qis.set.theme].style);
		ui.hkClock->setStyleSheet("");
		ui.hkClock->setStyleSheet(qis.ui.themes[qis.set.theme].style);
	}

private:
	void WidInit()
	{
		ui.bnWndSelect->setShortcut(Qt::Key_unknown);

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
		connect(ui.bnWndSelect, SIGNAL(clicked()), this, SLOT(OnBnWndActive()));

		connect(ui.chbClock, SIGNAL(stateChanged(int)), this, SLOT(OnClock(int)));
		connect(ui.hkClock, SIGNAL(changed()), this, SLOT(OnShowClock()));
	}

private slots:
	// Click
	void OnQkClick(int state) { func->quickClick.state = state; QiJson::SaveJson(); }
	void OnQkClickKey() { func->quickClick.key = ui.hkQkClick->virtualKey(); QiJson::SaveJson(); }
	void OnQkDelay(const QString& text) { func->quickClick.delay = text.toInt(); QiJson::SaveJson(); }
	void OnCmbMode(int row) { func->quickClick.mode = row; QiJson::SaveJson(); }
	// WndControl
	void OnWndActive(int state) { func->wndActive.state = state; QiJson::SaveJson(); }
	void OnBnWndActive()
	{
		qis.widget.dialogActive = true;
		qis.widget.main->hide();
		func->wndActive.wi = QiFn::WindowSelection();
		ui.etWndActive->setText(QString::fromWCharArray(func->wndActive.wi.wndName.c_str()));
		qis.widget.dialogActive = false;
		qis.widget.main->show();
		QiJson::SaveJson();
	}
	// Clock
	void OnClock(int state) { func->showClock.state = state; QiJson::SaveJson(); }
	void OnShowClock() { func->showClock.key = ui.hkClock->virtualKey(); QiJson::SaveJson(); }
};