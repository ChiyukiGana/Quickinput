#include "MsgViewUi.h"
MsgViewUi::MsgViewUi()
{
	ui.setupUi(this);
	ui.text->setTabStopDistance(QFontMetrics(QFont("Microsoft YaHei")).horizontalAdvance("    "));
	connect(ui.title_close_button, &QPushButton::clicked, this, [this] { hide(); });
	StyleGroup();
}

void MsgViewUi::StyleGroup()
{
	setProperty("group", "frame");
	ui.title_widget->setProperty("group", "title");
	ui.content_widget->setProperty("group", "client");
	ui.title_close_button->setProperty("group", "title-close_button");
	ui.text->setProperty("group", "text_edit");
}

void MsgViewUi::customEvent(QEvent* e)
{
	MsgViewEvent* event = reinterpret_cast<MsgViewEvent*>(e);
	if (e->type() == MsgViewEvent::Type::setText)
	{
		ui.text->setPlainText(event->text());
	}
	else if (e->type() == MsgViewEvent::Type::newLine)
	{
		ui.text->appendPlainText(event->text());
	}
	else if (e->type() == MsgViewEvent::Type::clear)
	{
		ui.text->clear();
	}
	else if (e->type() == MsgViewEvent::Type::show)
	{
		show();
	}
	else if (e->type() == MsgViewEvent::Type::hide)
	{
		hide();
	}
}