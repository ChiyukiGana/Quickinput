#include "RecordUi.h"
RecordUi::RecordUi(WndInfo* wndInfo) : wndInfo(wndInfo)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	if ("init")
	{
		ui.start_button->setText(QiUi::Text::rcStart());
		ui.close_button->setText(QiUi::Text::rcClose());
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
	setProperty(Prop::style_group, "frame");
	style_set_group(ui.content_widget, "client");
	style_set_group(ui.start_button, "record-button");
	style_set_group(ui.close_button, "record-button");
}

Macro& RecordUi::Start()
{
	Qi::widget.record = this;
	Qi::recordState = true;
	QiTr::QiHook(true);
	if (Qi::set.recKey)
	{
		QString text = lang_trans("按下") + " ";
		text += QKeyEdit::keyName(Qi::set.recKey);
		text += " ";
		text += lang_trans("开始录制");
		Qi::popText->Show(text, QColor(0x20, 0xFF, 0x20));
	}
	macro->mode = Macro::down;
	macro->count = 1;
	if (wndInfo)
	{
		Qi::recordWindow = wndInfo->wnd;
		macro->wndInfo = *wndInfo;
		macro->wndState = true;
		macro->name = lang_trans("窗口录制");
		POINT wpt = Window::pos(Qi::recordWindow);
		move(wpt.x, wpt.y);
		WndLock::Lock(Qi::recordWindow);
		exec();
		WndLock::UnLock();
	}
	else
	{
		macro->name = lang_trans("录制");
		exec();
	}
	QiTr::QiHook(false);
	Qi::recordState = false;
	Qi::widget.record = nullptr;
	return *macro;
}

void RecordUi::RecStart()
{
	if (Qi::set.recKey)
	{
		QString text = lang_trans("按下") + " ";
		text += QKeyEdit::keyName(Qi::set.recKey);
		text += " ";
		text += lang_trans("停止录制");
		Qi::popText->Show(text);
	}
	ui.start_button->setText(QiUi::Text::rcStop());
	Qi::record.clear();
	Qi::recordClock = 0;
	Qi::recording = true;
}
void RecordUi::RecStop()
{
	Qi::recording = false;
	if (Qi::record.size()) macro->acRun = std::move(Qi::record);
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
	if (e->type() == static_cast<int>(QiEvent::rec_start)) RecStart();
	else if (e->type() == static_cast<int>(QiEvent::rec_stop)) RecStop();
	else if (e->type() == static_cast<int>(QiEvent::rec_close)) RecClose();
}