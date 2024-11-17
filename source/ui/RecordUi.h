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
	RecordUi(WndInfo* wi) : QDialog()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setMouseTracking(true);

		SetStyleGroup();
		ui.bnStart->setText(qis.ui.text.rcStart);
		ui.bnClose->setText(qis.ui.text.rcClose);
		WidEvent();

		qis.widget.record = this;
		qis.recordState = true;
		QiFn::QiHook(true);

		if (qis.set.recKey)
		{
			std::wstring text(L"按下");
			text += Input::Name(qis.set.recKey);
			text += L"开始录制";
			PopBox::Show(text, RGB(0x20, 0xFF, 0x20));
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
	void SetStyleGroup()
	{
		setProperty("group", QVariant(QString::fromUtf8("frame")));
		ui.clientWidget->setProperty("group", QVariant(QString::fromUtf8("client")));
		ui.bnStart->setProperty("group", QVariant(QString::fromUtf8("record-button")));
		ui.bnClose->setProperty("group", QVariant(QString::fromUtf8("record-button")));
	}
	void ReStyle()
	{
		setStyleSheet("");
		setStyleSheet(qis.ui.themes[qis.set.theme].style);
	}
private:
	void WidEvent()
	{
		connect(ui.bnStart, SIGNAL(clicked()), this, SLOT(OnBnStart()));
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
	}
	void RecStart()
	{
		if (qis.set.recKey)
		{
			std::wstring text(L"按下");
			text += Input::Name(qis.set.recKey);
			text += L"停止录制";
			PopBox::Show(text);
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
		PopBox::Hide();
		close();
	}
	void RecClose()
	{
		qis.recording = false;
		PopBox::Hide();
		close();
	}
	QPoint msPos; bool mouseDown = false; void mousePressEvent(QMouseEvent* et) { if (et->button() == Qt::LeftButton) msPos = et->pos(), mouseDown = true; et->accept(); }void mouseMoveEvent(QMouseEvent* et) { if (mouseDown) move(et->pos() + pos() - msPos); }void mouseReleaseEvent(QMouseEvent* et) { if (et->button() == Qt::LeftButton) mouseDown = false; }
private:
	void showEvent(QShowEvent*)
	{
		ReStyle();
		SetForegroundWindow((HWND)QWidget::winId());
	}
	void customEvent(QEvent* et)
	{
		if (et->type() == QiEvent::recStart) RecStart();
		else if (et->type() == QiEvent::recStop) RecStop();
		else if (et->type() == QiEvent::recClose) RecClose();
	}
public Q_SLOTS:
	void OnBnStart() { if (qis.recording) RecStop(); else RecStart(); }
	void OnBnClose() { RecClose(); }
};