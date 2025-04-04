#pragma once
#include <src/inc_header.h>
#include "ui_UpdateUi.h"
class UpdateUi : public QDialog
{
	Q_OBJECT;
	Ui::UpdateUiClass ui;

public:
	UpdateUi(const QString& version, const QString& content)
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
private:
	void StyleGroup()
	{
		setProperty("group", "frame");
		ui.title_widget->setProperty("group", "title");
		ui.content_widget->setProperty("group", "client");
	}
private:
	bool event(QEvent* e)
	{
		if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
		{
			QKeyEvent* keyEvent = (QKeyEvent*)e;
			if ((keyEvent->key() == Qt::Key_Escape) || (keyEvent->key() == Qt::Key_Return) || keyEvent->key() == Qt::Key_Enter || (keyEvent->key() == Qt::Key_Space)) return true;
		}
		return QWidget::event(e);
	}
	// window move
	QPoint mouse_positon;
	bool mouse_down = false;
	void mousePressEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			mouse_positon = e->pos();
			mouse_down = true;
			e->accept();
		}
	}
	void mouseMoveEvent(QMouseEvent* e)
	{
		if (mouse_down)
		{
			if (Distance(mouse_positon.x(), mouse_positon.y(), e->pos().x(), e->pos().y()) < 100)
			{
				move(e->pos() + pos() - mouse_positon);
				e->accept();
			}
		}
	}
	void mouseReleaseEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			mouse_down = false;
			e->accept();
		}
	}
};