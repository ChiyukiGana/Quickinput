#include "MoreUi.h"
MoreUi::MoreUi()
{
	ui.setupUi(this);
	connect(ui.title_close_button, &QPushButton::clicked, this, [this] { hide(); });
	StyleGroup();
}

void MoreUi::StyleGroup()
{
	setProperty("group", "frame");
	ui.title_widget->setProperty("group", "title");
	ui.content_widget->setProperty("group", "client");
	ui.title_close_button->setProperty("group", "title-close_button");
	ui.toolBox->setProperty("group", "ltab_widget");
}

bool MoreUi::event(QEvent* e)
{
	if (e->type() == QEvent::WindowActivate)
	{
		Qi::widget.moreActive = true;
		if (Qi::state) QiTr::QiState(false);
		QiTr::QiHook(false);
	}
	else if (e->type() == QEvent::WindowDeactivate)
	{
		Qi::widget.moreActive = false;
		if (!Qi::widget.active())
		{
			if (Qi::set.defOn) QiTr::QiState(true);
			QiTr::QiHook(true);
		}
	}
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
	{
		QKeyEvent* keyEvent = (QKeyEvent*)e;
		if ((keyEvent->key() == Qt::Key_Escape) || (keyEvent->key() == Qt::Key_Return) || keyEvent->key() == Qt::Key_Enter || (keyEvent->key() == Qt::Key_Space)) return true;
	}
	return QDialogFrameless::event(e);
}
void MoreUi::showEvent(QShowEvent*)
{
	SetForegroundWindow((HWND)QWidget::winId());
}