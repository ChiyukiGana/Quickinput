#include "HelpUi.h"
HelpUi::HelpUi()
{
	ui.setupUi(this);
	connect(ui.title_close_button, &QPushButton::clicked, this, [this] { hide(); });
	StyleGroup();
}

void HelpUi::StyleGroup()
{
	setProperty(Prop::style_group, "frame");
	style_set_group(ui.title_widget, "title");
	style_set_group(ui.content_widget, "client");
	style_set_group(ui.title_close_button, "title-close_button");
	style_set_group(ui.toolBox, "ltab_widget");
}

bool HelpUi::event(QEvent* e)
{
	if (e->type() == QEvent::WindowActivate)
	{
		Qi::widget.helpActive = true;
		if (Qi::state) QiTr::QiState(false);
		QiTr::QiHook(false);
	}
	else if (e->type() == QEvent::WindowDeactivate)
	{
		Qi::widget.helpActive = false;
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
void HelpUi::showEvent(QShowEvent*)
{
	SetForegroundWindow((HWND)QWidget::winId());
}