#pragma once
#include <src/inc_header.h>
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
private:
	void StyleGroup()
	{
		ui.content_widget->setProperty("group", "client");
		ui.window_select_button->setProperty("group", "get_button");
		ui.click_check->setProperty("group", "check");
		ui.clock_check->setProperty("group", "check");
		ui.window_check->setProperty("group", "check");
		ui.click_mode_combo->setProperty("group", "combo");
		ui.click_delay_edit->setProperty("group", "line_edit");
		ui.window_name_edit->setProperty("group", "line_edit");
		ui.click_keyedit->setProperty("group", "line_edit");
		ui.clock_keyedit->setProperty("group", "line_edit");
		ui.click_mode_combo->view()->setProperty("group", "table");
	}
	void Init()
	{
		if ("click")
		{
			ui.click_check->setChecked(func->quickClick.state);
			ui.click_keyedit->setMode(QKeyEdit::Mode::solid);
			ui.click_keyedit->setMouseEnable(true);
			ui.click_keyedit->setWheelEnable(true);
			ui.click_keyedit->setKey(QKeyEdit::Key(func->quickClick.key));
			ui.click_delay_edit->setValidator(new QIntValidator(0, 99999, this));
			ui.click_delay_edit->setText(QString::number(func->quickClick.delay));
			ui.click_mode_combo->setEditable(true);
			ui.click_mode_combo->lineEdit()->setReadOnly(true);
			ui.click_mode_combo->lineEdit()->setAlignment(Qt::AlignCenter);
			ui.click_mode_combo->addItem("按下");
			ui.click_mode_combo->addItem("切换");
			ui.click_mode_combo->setCurrentIndex(func->quickClick.mode);
			QStandardItemModel* model = (QStandardItemModel*)ui.click_mode_combo->view()->model();
			for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);
		}
		if ("clock")
		{
			ui.clock_check->setChecked(func->showClock.state);
			ui.clock_keyedit->setMode(QKeyEdit::Mode::solid);
			ui.clock_keyedit->setMouseEnable(true);
			ui.clock_keyedit->setWheelEnable(true);
			ui.clock_keyedit->setPadEnable(true);
			ui.clock_keyedit->setKey(QKeyEdit::Key(func->showClock.key));
		}
		if ("window")
		{
			ui.window_check->setChecked(func->wndActive.state);
			ui.window_name_edit->setText(func->wndActive.wi.wndName);
		}
		if ("clear shortcut")
		{
			ui.window_select_button->installEventFilter(this);
			ui.click_check->installEventFilter(this);
			ui.window_check->installEventFilter(this);
			ui.window_select_button->installEventFilter(this);
			ui.clock_check->installEventFilter(this);
		}
	}
	void Event()
	{
		connect(ui.click_check,&QCheckBox::stateChanged, this, &This::OnClickState);
		connect(ui.click_keyedit, &QKeyEdit::changed, this, &This::OnClickKey);
		connect(ui.click_delay_edit, &QLineEdit::textEdited, this, &This::OnClickDelay);
		connect(ui.click_mode_combo, QOverload<int>::of(&QComboBox::activated), this, &This::OnClickMode);
		connect(ui.window_check, &QCheckBox::stateChanged, this, &This::OnWindowState);
		connect(ui.window_select_button, &QPushButton::clicked, this, &This::OnWindowSelect);
		connect(ui.clock_check, &QCheckBox::stateChanged, this, &This::OnClockState);
		connect(ui.clock_keyedit, &QKeyEdit::changed, this, &This::OnClockKey);
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
	void OnClickState(int state)
	{
		func->quickClick.state = state;
		QiJson::SaveJson();
	}
	void OnClickKey()
	{
		func->quickClick.key = ui.click_keyedit->key().keyCode;
		QiJson::SaveJson();
	}
	void OnClickDelay(const QString& text)
	{
		func->quickClick.delay = text.toInt();
		QiJson::SaveJson();
	}
	void OnClickMode(int row)
	{
		func->quickClick.mode = row;
		QiJson::SaveJson();
	}
	void OnWindowState(int state)
	{
		func->wndActive.state = state;
		QiJson::SaveJson();
	}
	void OnWindowSelect()
	{
		Qi::widget.dialogActive = true;
		Qi::widget.main->hide();
		func->wndActive.wi = QiFn::WindowSelection();
		ui.window_name_edit->setText(func->wndActive.wi.wndName);
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
		func->showClock.key = ui.clock_keyedit->key().keyCode;
		QiJson::SaveJson();
	}
};