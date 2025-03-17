#pragma once
#include <src/inc_header.h>
#include "ui_RecordUi.h"
class RecordUi : public QDialog
{
	Q_OBJECT;
	using This = RecordUi;
	Ui::RecordUiClass ui;
	MacroGroup* group;
	Macro macro;
public:
	enum
	{
		_start = QEvent::User + 1,
		_stop,
		_close,
	};
public:
	RecordUi(MacroGroup* group, WndInfo* wndInfo) : QDialog()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		if ("init")
		{
			ui.start_button->setText(Qi::ui.text.rcStart);
			ui.close_button->setText(Qi::ui.text.rcClose);
			if ("clear shortcut")
			{
				ui.start_button->installEventFilter(this);
				ui.close_button->installEventFilter(this);
			}
			connect(ui.start_button, &QPushButton::clicked, this, [this]{ if (Qi::recording) RecStop(); else RecStart(); });
			connect(ui.close_button, &QPushButton::clicked, this, [this] { RecClose(); });
			StyleGroup();
		}
		this->group = group;
		macro.groupName = group->name;
		macro.groupBase = group->base;
		Qi::widget.record = this;
		Qi::recordState = true;
		QiFn::QiHook(true);
		if (Qi::set.recKey)
		{
			QString text("按下");
			text += QKeyEdit::keyName(Qi::set.recKey);
			text += L"开始录制";
			Qi::popText->Show(text, QColor(0x20, 0xFF, 0x20));
		}
		macro.mode = Macro::down;
		macro.count = 1;
		if (wndInfo)
		{
			Qi::recordWindow = wndInfo->wnd;
			macro.wndInfo = *wndInfo;
			macro.wndState = true;
			macro.name = group->makeName("窗口录制");
			POINT wpt = Window::pos(Qi::recordWindow);
			move(wpt.x, wpt.y);
			WndLock::Lock(Qi::recordWindow);
			exec();
			WndLock::UnLock();
		}
		else
		{
			macro.name = group->makeName("录制");
			exec();
		}
		QiFn::QiHook(false);
		Qi::recordState = false;
		Qi::widget.record = nullptr;
	}
	void StyleGroup()
	{
		setProperty("group", "frame");
		ui.content_widget->setProperty("group", "client");
		ui.start_button->setProperty("group", "record-button");
		ui.close_button->setProperty("group", "record-button");
	}
private:
	void RecStart()
	{
		if (Qi::set.recKey)
		{
			QString text("按下");
			text += QKeyEdit::keyName(Qi::set.recKey);
			text += "停止录制";
			Qi::popText->Show(text);
		}
		ui.start_button->setText(Qi::ui.text.rcStop);
		Qi::record.clear();
		Qi::recordClock = 0;
		Qi::recording = true;
	}
	void RecStop()
	{
		Qi::recording = false;
		if (Qi::record.size())
		{
			macro.acRun = std::move(Qi::record);
			QiJson::SaveMacro(group->macros.append(std::move(macro)));
		}
		Qi::popText->Hide();
		close();
	}
	void RecClose()
	{
		Qi::recording = false;
		Qi::popText->Hide();
		close();
	}
	// window move
	QPoint mouse_positon;
	bool mouse_down = false;
	void mousePressEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			mouse_positon = e->pos();
			mouse_down = true;
			e->accept();
		}
	}
	void mouseMoveEvent(QMouseEvent* e)
	{
		if (mouse_down)
		{
			if (Distance(mouse_positon.x(), mouse_positon.y(), e->pos().x(), e->pos().y()) < 100)
			{
				move(e->pos() + pos() - mouse_positon);
				e->accept();
			}
		}
	}
	void mouseReleaseEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			mouse_down = false;
			e->accept();
		}
	}
private:
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
	void showEvent(QShowEvent*)
	{
		SetForegroundWindow((HWND)QWidget::winId());
	}
	void customEvent(QEvent* e)
	{
		if (e->type() == _start) RecStart();
		else if (e->type() == _stop) RecStop();
		else if (e->type() == _close) RecClose();
	}
};