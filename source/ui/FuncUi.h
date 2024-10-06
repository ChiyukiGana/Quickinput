#pragma execution_character_set("utf-8")
#pragma once
#include <qstandarditemmodel.h>
#include <qabstractitemview.h>
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
	void SetStyleGroup()
	{
		ui.clientWidget->setProperty("group", QVariant(QString::fromUtf8("client")));
		ui.bnWndSelect->setProperty("group", QVariant(QString::fromUtf8("get_button")));
		ui.chbQkClick->setProperty("group", QVariant(QString::fromUtf8("check")));
		ui.chbClock->setProperty("group", QVariant(QString::fromUtf8("check")));
		ui.chbWndActive->setProperty("group", QVariant(QString::fromUtf8("check")));
		ui.cmbMode->setProperty("group", QVariant(QString::fromUtf8("combo")));
		ui.etQkDelay->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.etWndActive->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.hkQkClick->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.hkClock->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.cmbMode->view()->setProperty("group", QVariant(QString::fromUtf8("table")));
	}
	void ReStyle()
	{
		setStyleSheet("");
		setStyleSheet(qis.ui.themes[qis.set.theme].style);
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

		ui.cmbMode->setEditable(true);
		ui.cmbMode->lineEdit()->setReadOnly(true);
		ui.cmbMode->lineEdit()->setAlignment(Qt::AlignCenter);
		ui.cmbMode->addItem("按下");
		ui.cmbMode->addItem("切换");
		QStandardItemModel* model = (QStandardItemModel*)ui.cmbMode->view()->model();
		for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);

		ui.chbQkClick->setChecked(func->quickClick.state);
		ui.hkQkClick->VirtualKey(func->quickClick.key);
		ui.etQkDelay->setText(QString::number(func->quickClick.delay));
		ui.cmbMode->setCurrentIndex(func->quickClick.mode);

		ui.chbWndActive->setChecked(func->wndActive.state);
		ui.etWndActive->setText(QString::fromWCharArray(func->wndActive.wi.wndName.c_str()));

		ui.hkClock->Mode(0);
		ui.chbClock->setChecked(func->showClock.state);
		ui.hkClock->VirtualKey(func->showClock.key);

		{
			ui.chbQkClick->setShortcut(Qt::Key_unknown);
			ui.chbWndActive->setShortcut(Qt::Key_unknown);
			ui.bnWndSelect->setShortcut(Qt::Key_unknown);
			ui.chbClock->setShortcut(Qt::Key_unknown);
		}

		SetStyleGroup();
	}
	void WidEvent()
	{
		connect(ui.chbQkClick, SIGNAL(stateChanged(int)), this, SLOT(OnQcState(int)));
		connect(ui.hkQkClick, SIGNAL(changed()), this, SLOT(OnQcKey()));
		connect(ui.etQkDelay, SIGNAL(textEdited(const QString&)), this, SLOT(OnQcDelay(const QString&)));
		connect(ui.cmbMode, SIGNAL(activated(int)), this, SLOT(OnQcMode(int)));
		connect(ui.chbWndActive, SIGNAL(stateChanged(int)), this, SLOT(OnWcState(int)));
		connect(ui.bnWndSelect, SIGNAL(clicked()), this, SLOT(OnWcSelect()));
		connect(ui.chbClock, SIGNAL(stateChanged(int)), this, SLOT(OnClockState(int)));
		connect(ui.hkClock, SIGNAL(changed()), this, SLOT(OnClockKey()));
	}
private slots:
	void OnQcState(int state)
	{
		func->quickClick.state = state;
		QiJson::SaveJson();
	}
	void OnQcKey()
	{
		func->quickClick.key = ui.hkQkClick->virtualKey();
		QiJson::SaveJson();
	}
	void OnQcDelay(const QString& text)
	{
		func->quickClick.delay = text.toInt();
		QiJson::SaveJson();
	}
	void OnQcMode(int row)
	{
		func->quickClick.mode = row;
		QiJson::SaveJson();
	}
	void OnWcState(int state)
	{
		func->wndActive.state = state;
		QiJson::SaveJson();
	}
	void OnWcSelect()
	{
		qis.widget.dialogActive = true;
		qis.widget.main->hide();
		func->wndActive.wi = QiFn::WindowSelection();
		ui.etWndActive->setText(QString::fromWCharArray(func->wndActive.wi.wndName.c_str()));
		qis.widget.dialogActive = false;
		qis.widget.main->show();
		QiJson::SaveJson();
	}
	void OnClockState(int state)
	{
		func->showClock.state = state;
		QiJson::SaveJson();
	}
	void OnClockKey()
	{
		func->showClock.key = ui.hkClock->virtualKey();
		QiJson::SaveJson();
	}
};