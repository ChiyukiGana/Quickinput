#pragma once
#include <src/inc_header.h>
#include "ui_MoreUi.h"
class MoreUi : public QDialog
{
	Q_OBJECT;
	using This = MoreUi;
	Ui::MoreUiClass ui;
public:
	MoreUi()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		connect(ui.title_close_button, &QPushButton::clicked, this, [this] { hide(); });
		StyleGroup();
	}
private:
	void StyleGroup()
	{
		setProperty("group", "frame");
		ui.title_widget->setProperty("group", "title");
		ui.content_widget->setProperty("group", "client");
		ui.title_close_button->setProperty("group", "title-close_button");
		ui.toolBox->setProperty("group", "ltab_widget");
	}
private:
	bool event(QEvent* e)
	{
		if (e->type() == QEvent::WindowActivate)
		{
			Qi::widget.moreActive = true;
			if (Qi::state) QiFn::QiState(false);
			QiFn::QiHook(false);
		}
		else if (e->type() == QEvent::WindowDeactivate)
		{
			Qi::widget.moreActive = false;
			if (Qi::widget.active())
			{
				if (Qi::set.defOn) QiFn::QiState(true);
				QiFn::QiHook(true);
			}
		}
		if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
		{
			QKeyEvent* keyEvent = (QKeyEvent*)e;
			if ((keyEvent->key() == Qt::Key_Escape) || (keyEvent->key() == Qt::Key_Return) || keyEvent->key() == Qt::Key_Enter || (keyEvent->key() == Qt::Key_Space)) return true;
		}
		return QWidget::event(e);
	}
	void showEvent(QShowEvent*)
	{
		SetForegroundWindow((HWND)QWidget::winId());
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