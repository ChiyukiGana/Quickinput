#pragma once
#include <qevent.h>
#include "ui_RecordUi.h"
#include "header.h"

class RecordUi : public QDialog
{
	Q_OBJECT;
	Ui::RecordUiClass ui;
	Macro macro;

public:
	RecordUi(WndInfo* wi) : QDialog()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setMouseTracking(true);

		ui.bnStart->setText(qis.ui.rcStart);
		ui.bnClose->setText(qis.ui.rcClose);

		WidEvent();
		ReStyle();

		qis.widget.record = this;
		qis.recordState = true;
		QiFn::QiHook(true);

		if (qis.set.recKey)
		{
			std::wstring text(L"按下");
			text += Input::Name(qis.set.recKey);
			text += L"开始录制";
			MsgPop::Show(text, RGB(0x20, 0xFF, 0x20));
		}

		macro.mode = Macro::down;
		macro.count = 1;

		if (wi)
		{
			qis.recordWindow = wi->wnd;
			macro.wi = *wi;
			macro.wndState = true;
			macro.name = QiFn::NameFilter(L"窗口录制");
			POINT wpt = Window::pos(qis.recordWindow);
			move(wpt.x, wpt.y);
			WndLock::Lock(qis.recordWindow);
			exec();
			WndLock::UnLock();
		}
		else
		{
			macro.name = QiFn::NameFilter(L"录制");
			exec();
		}
		QiFn::QiHook(false);
		qis.recordState = false;
		qis.widget.record = nullptr;
	}

	void ReStyle()
	{
		setStyleSheet("");
		setStyleSheet(qis.themes[qis.set.theme].style);
	}

private:
	void WidEvent()
	{
		connect(ui.bnStart, SIGNAL(clicked()), this, SLOT(OnBnStart()));
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
	}

	void mouseMoveEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - QPoint(5, 15)); }

	void RecStart()
	{
		if (qis.set.recKey)
		{
			std::wstring text(L"按下");
			text += Input::Name(qis.set.recKey);
			text += L"停止录制";
			MsgPop::Show(text);
		}

		ui.bnStart->setText(qis.ui.rcStop);

		qis.record.clear();
		qis.recordClock = 0;
		qis.recording = true;
	}
	void RecStop()
	{
		qis.recording = false;
		if (qis.record.size())
		{
			macro.actions = qis.record;
			qis.macros.Add(macro);
			QiFn::SaveMacro(macro);
		}
		MsgPop::Hide();
		close();
	}
	void RecClose()
	{
		qis.recording = false;
		MsgPop::Hide();
		close();
	}

private:
	void customEvent(QEvent* et)
	{
		if (et->type() == QiEvent::recStart) RecStart();
		else if (et->type() == QiEvent::recStop) RecStop();
		else if (et->type() == QiEvent::recClose) RecClose();
	}

public slots:
	void OnBnStart() { if (qis.recording) RecStop(); else RecStart(); }
	void OnBnClose() { RecClose(); }
};