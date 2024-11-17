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
	FuncData* func = &Qi::fun;
public:
	FuncUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		Init();
		Event();
	}
	void StyleGroup()
	{
		ui.clientWidget->setProperty("group", "client");
		ui.bnWndSelect->setProperty("group", "get_button");
		ui.chbQkClick->setProperty("group", "check");
		ui.chbClock->setProperty("group", "check");
		ui.chbWndActive->setProperty("group", "check");
		ui.cmbMode->setProperty("group", "combo");
		ui.etQkDelay->setProperty("group", "line_edit");
		ui.etWndActive->setProperty("group", "line_edit");
		ui.hkQkClick->setProperty("group", "line_edit");
		ui.hkClock->setProperty("group", "line_edit");
		ui.cmbMode->view()->setProperty("group", "table");
	}

private:
	void Init()
	{
		ui.hkQkClick->setMode(QKeyEdit::Mode::solid);
		ui.hkQkClick->setMouseEnable(true);
		ui.hkQkClick->setWheelEnable(true);
		ui.hkQkClick->setKey(QKeyEdit::Key(VK_MENU));

		ui.hkClock->setMode(QKeyEdit::Mode::solid);
		ui.hkClock->setMouseEnable(true);
		ui.hkClock->setWheelEnable(true);
		ui.hkClock->setPadEnable(true);
		ui.hkClock->setKey(QKeyEdit::Key(VK_LBUTTON));

		ui.etQkDelay->setValidator(new QIntValidator(0, 99999, this));

		ui.cmbMode->setEditable(true);
		ui.cmbMode->lineEdit()->setReadOnly(true);
		ui.cmbMode->lineEdit()->setAlignment(Qt::AlignCenter);
		ui.cmbMode->addItem("按下");
		ui.cmbMode->addItem("切换");
		QStandardItemModel* model = (QStandardItemModel*)ui.cmbMode->view()->model();
		for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);

		ui.chbQkClick->setChecked(func->quickClick.state);
		ui.hkQkClick->setKey(QKeyEdit::Key(func->quickClick.key));
		ui.etQkDelay->setText(QString::number(func->quickClick.delay));
		ui.cmbMode->setCurrentIndex(func->quickClick.mode);

		ui.chbWndActive->setChecked(func->wndActive.state);
		ui.etWndActive->setText(QString::fromWCharArray(func->wndActive.wi.wndName.c_str()));

		ui.chbClock->setChecked(func->showClock.state);
		ui.hkClock->setKey(QKeyEdit::Key(func->showClock.key));

		{
			ui.bnWndSelect->setShortcut(Qt::Key_unknown);
			ui.chbQkClick->setShortcut(Qt::Key_unknown);
			ui.chbWndActive->setShortcut(Qt::Key_unknown);
			ui.bnWndSelect->setShortcut(Qt::Key_unknown);
			ui.chbClock->setShortcut(Qt::Key_unknown);
		}

		StyleGroup();
	}
	void Event()
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
private Q_SLOTS:
	void OnQcState(int state)
	{
		func->quickClick.state = state;
		QiJson::SaveJson();
	}
	void OnQcKey()
	{
		func->quickClick.key = ui.hkQkClick->key().keyCode;
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
		Qi::widget.dialogActive = true;
		Qi::widget.main->hide();
		func->wndActive.wi = QiFn::WindowSelection();
		ui.etWndActive->setText(QString::fromWCharArray(func->wndActive.wi.wndName.c_str()));
		Qi::widget.dialogActive = false;
		Qi::widget.main->show();
		QiJson::SaveJson();
	}
	void OnClockState(int state)
	{
		func->showClock.state = state;
		QiJson::SaveJson();
	}
	void OnClockKey()
	{
		func->showClock.key = ui.hkClock->key().keyCode;
		QiJson::SaveJson();
	}
};