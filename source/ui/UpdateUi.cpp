#include "UpdateUi.h"
DefWindowMove(UpdateUi);
UpdateUi::UpdateUi(const QString& version, const QString& content)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	StyleGroup();
	ui.version_label->setText(version);
	ui.content_label->setText(content);
	connect(ui.title_close_button, &QPushButton::clicked, this, [this](bool) { close(); });
	move(Qi::widget.main->pos());
	exec();
}

void UpdateUi::StyleGroup()
{
	setProperty("group", "frame");
	ui.title_widget->setProperty("group", "title");
	ui.content_widget->setProperty("group", "client");
}

bool UpdateUi::event(QEvent* e)
{
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
	{
		QKeyEvent* keyEvent = (QKeyEvent*)e;
		if ((keyEvent->key() == Qt::Key_Escape) || (keyEvent->key() == Qt::Key_Return) || keyEvent->key() == Qt::Key_Enter || (keyEvent->key() == Qt::Key_Space)) return true;
	}
	return QWidget::event(e);
}