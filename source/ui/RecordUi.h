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

		StyleGroup();
		ui.bnStart->setText(qis.ui.text.rcStart);
		ui.bnClose->setText(qis.ui.text.rcClose);
		connect(ui.bnStart, SIGNAL(clicked()), this, SLOT(OnBnStart()));
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));

		qis.widget.record = this;
		qis.recordState = true;
		QiFn::QiHook(true);

		if (qis.set.recKey)
		{
			QString text("按下");
			text += QKeyEdit::keyName(qis.set.recKey);
			text += L"开始录制";
			qis.popText->Show(text, QColor(0x20, 0xFF, 0x20));
		}

		macro.mode = Macro::down;
		macro.count = 1;

		if (wi)
		{
			qis.recordWindow = wi->wnd;
			macro.wi = *wi;
			macro.wndState = true;
			macro.name = QiFn::AllocName(L"窗口录制");

			POINT wpt = Window::pos(qis.recordWindow);
			move(wpt.x, wpt.y);
			WndLock::Lock(qis.recordWindow);
			exec();
			WndLock::UnLock();
		}
		else
		{
			macro.name = QiFn::AllocName(L"录制");
			exec();
		}
		QiFn::QiHook(false);
		qis.recordState = false;
		qis.widget.record = nullptr;
	}
	void StyleGroup()
	{
		setProperty("group", QVariant(QString::fromUtf8("frame")));
		ui.clientWidget->setProperty("group", QVariant(QString::fromUtf8("client")));
		ui.bnStart->setProperty("group", QVariant(QString::fromUtf8("record-button")));
		ui.bnClose->setProperty("group", QVariant(QString::fromUtf8("record-button")));
	}

private:
	void RecStart()
	{
		if (qis.set.recKey)
		{
			QString text("按下");
			text += QKeyEdit::keyName(qis.set.recKey);
			text += "停止录制";
			qis.popText->Show(text);
		}

		ui.bnStart->setText(qis.ui.text.rcStop);

		qis.record.clear();
		qis.recordClock = 0;
		qis.recording = true;
	}
	void RecStop()
	{
		qis.recording = false;
		if (qis.record.size())
		{
			macro.acRun = qis.record;
			qis.macros.Add(macro);
			QiJson::SaveMacro(macro);
		}
		qis.popText->Hide();
		close();
	}
	void RecClose()
	{
		qis.recording = false;
		qis.popText->Hide();
		close();
	}
	QPoint msPos; bool mouseDown = false; void mousePressEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) msPos = e->pos(), mouseDown = true; e->accept(); }void mouseMoveEvent(QMouseEvent* e) { if (mouseDown) move(e->pos() + pos() - msPos); }void mouseReleaseEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) mouseDown = false; }
private:
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
	void OnBnStart() { if (qis.recording) RecStop(); else RecStart(); }
	void OnBnClose() { RecClose(); }
};