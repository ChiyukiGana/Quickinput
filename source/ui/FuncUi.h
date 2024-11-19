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
	using This = FuncUi;
	Ui::FuncUiClass ui;
	FuncData* func = &Qi::fun;
public:
	FuncUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		Init();
		Event();
		StyleGroup();
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
		ui.etWndActive->setText(func->wndActive.wi.wndName);

		ui.chbClock->setChecked(func->showClock.state);
		ui.hkClock->setKey(QKeyEdit::Key(func->showClock.key));

		if ("clear shortcut")
		{
			ui.bnWndSelect->installEventFilter(this);
			ui.chbQkClick->installEventFilter(this);
			ui.chbWndActive->installEventFilter(this);
			ui.bnWndSelect->installEventFilter(this);
			ui.chbClock->installEventFilter(this);
		}
	}
	void Event()
	{
		connect(ui.chbQkClick,&QCheckBox::stateChanged, this, &This::OnQcState);
		connect(ui.hkQkClick, &QKeyEdit::changed, this, &This::OnQcKey);
		connect(ui.etQkDelay, &QLineEdit::textEdited, this, &This::OnQcDelay);
		connect(ui.cmbMode, QOverload<int>::of(&QComboBox::activated), this, &This::OnQcMode);
		connect(ui.chbWndActive, &QCheckBox::stateChanged, this, &This::OnWcState);
		connect(ui.bnWndSelect, &QPushButton::clicked, this, &This::OnWcSelect);
		connect(ui.chbClock, &QCheckBox::stateChanged, this, &This::OnClockState);
		connect(ui.hkClock, &QKeyEdit::changed, this, &This::OnClockKey);
	}

	bool event(QEvent* e)
	{
		if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
		{
			QKeyEvent* keyEvent = (QKeyEvent*)e;
			if ((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Space)) return true;
		}
		return QWidget::event(e);
	}
	bool eventFilter(QObject* obj, QEvent* e)
	{
		if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease)) return true;
		return QWidget::eventFilter(obj, e);
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
		ui.etWndActive->setText(func->wndActive.wi.wndName);
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