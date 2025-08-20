#include "RecordUi.h"
RecordUi::RecordUi(WndInfo* wndInfo) : wndInfo(wndInfo)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	if ("init")
	{
		ui.start_button->setText(QiUi::Text::rcStart);
		ui.close_button->setText(QiUi::Text::rcClose);
		if ("clear shortcut")
		{
			ui.start_button->installEventFilter(this);
			ui.close_button->installEventFilter(this);
		}
		connect(ui.start_button, &QPushButton::clicked, this, [this] { if (Qi::recording) RecStop(); else RecStart(); });
		connect(ui.close_button, &QPushButton::clicked, this, [this] { RecClose(); });
		StyleGroup();
	}
}

void RecordUi::StyleGroup()
{
	setProperty("group", "frame");
	ui.content_widget->setProperty("group", "client");
	ui.start_button->setProperty("group", "record-button");
	ui.close_button->setProperty("group", "record-button");
}

Macro RecordUi::Start()
{
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
		macro.name = "窗口录制";
		POINT wpt = Window::pos(Qi::recordWindow);
		move(wpt.x, wpt.y);
		WndLock::Lock(Qi::recordWindow);
		exec();
		WndLock::UnLock();
	}
	else
	{
		macro.name = "录制";
		exec();
	}
	QiFn::QiHook(false);
	Qi::recordState = false;
	Qi::widget.record = nullptr;
	return std::move(macro);
}

void RecordUi::RecStart()
{
	if (Qi::set.recKey)
	{
		QString text("按下");
		text += QKeyEdit::keyName(Qi::set.recKey);
		text += "停止录制";
		Qi::popText->Show(text);
	}
	ui.start_button->setText(QiUi::Text::rcStop);
	Qi::record.clear();
	Qi::recordClock = 0;
	Qi::recording = true;
}
void RecordUi::RecStop()
{
	Qi::recording = false;
	if (Qi::record.size()) macro.acRun = std::move(Qi::record);
	Qi::popText->Hide();
	close();
}
void RecordUi::RecClose()
{
	Qi::recording = false;
	Qi::popText->Hide();
	close();
}

bool RecordUi::event(QEvent* e)
{
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
	{
		QKeyEvent* keyEvent = (QKeyEvent*)e;
		if ((keyEvent->key() == Qt::Key_Escape) || (keyEvent->key() == Qt::Key_Return) || keyEvent->key() == Qt::Key_Enter || (keyEvent->key() == Qt::Key_Space)) return true;
	}
	return QDialogFrameless::event(e);
}
bool RecordUi::eventFilter(QObject* obj, QEvent* e)
{
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease)) return true;
	return QDialogFrameless::eventFilter(obj, e);
}
void RecordUi::showEvent(QShowEvent*)
{
	SetForegroundWindow((HWND)QWidget::winId());
}
void RecordUi::customEvent(QEvent* e)
{
	if (e->type() == QiEvent::rec_start) RecStart();
	else if (e->type() == QiEvent::rec_stop) RecStop();
	else if (e->type() == QiEvent::rec_close) RecClose();
}