#pragma execution_character_set("utf-8")
#pragma once
#include <qevent.h>
#include "../src/minc.h"
#include "ui_RecordUi.h"

class RecordUi : public QDialog
{
	Q_OBJECT;
	Ui::RecordUiClass ui;
	Macro macro;
public:
	enum
	{
		_start = QEvent::User + 1,
		_stop,
		_close,
	};
public:
	RecordUi(WndInfo* wi) : QDialog()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setMouseTracking(true);

		if ("init")
		{
			ui.bnStart->setText(Qi::ui.text.rcStart);
			ui.bnClose->setText(Qi::ui.text.rcClose);
			if ("clear shortcut")
			{
				ui.bnStart->installEventFilter(this);
				ui.bnClose->installEventFilter(this);
			}
			connect(ui.bnStart, SIGNAL(clicked()), this, SLOT(OnBnStart()));
			connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
			StyleGroup();
		}

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

		if (wi)
		{
			Qi::recordWindow = wi->wnd;
			macro.wi = *wi;
			macro.wndState = true;
			macro.name = QiFn::AllocName(L"窗口录制");

			POINT wpt = Window::pos(Qi::recordWindow);
			move(wpt.x, wpt.y);
			WndLock::Lock(Qi::recordWindow);
			exec();
			WndLock::UnLock();
		}
		else
		{
			macro.name = QiFn::AllocName(L"录制");
			exec();
		}
		QiFn::QiHook(false);
		Qi::recordState = false;
		Qi::widget.record = nullptr;
	}
	void StyleGroup()
	{
		setProperty("group", "frame");
		ui.clientWidget->setProperty("group", "client");
		ui.bnStart->setProperty("group", "record-button");
		ui.bnClose->setProperty("group", "record-button");
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

		ui.bnStart->setText(Qi::ui.text.rcStop);

		Qi::record = Actions();
		Qi::recordClock = 0;
		Qi::recording = true;
	}
	void RecStop()
	{
		Qi::recording = false;
		if (Qi::record.size())
		{
			macro.acRun = std::move(Qi::record);
			Qi::macros.Add(macro);
			QiJson::SaveMacro(macro);
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
	QPoint msPos; bool mouseDown = false; void mousePressEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) msPos = e->pos(), mouseDown = true; e->accept(); }void mouseMoveEvent(QMouseEvent* e) { if (mouseDown) move(e->pos() + pos() - msPos); }void mouseReleaseEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) mouseDown = false; }
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
public Q_SLOTS:
	void OnBnStart() { if (Qi::recording) RecStop(); else RecStart(); }
	void OnBnClose() { RecClose(); }
};