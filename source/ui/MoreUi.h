#pragma execution_character_set("utf-8")
#pragma once
#include <qtabbar.h>
#include <qevent.h>
#include "../src/minc.h"
#include "ui_MoreUi.h"

class MoreUi : public QDialog
{
	Q_OBJECT;
	Ui::MoreUiClass ui;
public:
	MoreUi() : QDialog()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
		StyleGroup();
	}
	void StyleGroup()
	{
		setProperty("group", QVariant(QString::fromUtf8("frame")));
		ui.titleWidget->setProperty("group", QVariant(QString::fromUtf8("title")));
		ui.clientWidget->setProperty("group", QVariant(QString::fromUtf8("client")));
		ui.bnClose->setProperty("group", QVariant(QString::fromUtf8("title-close_button")));
		ui.tabWidget->setProperty("group", QVariant(QString::fromUtf8("tab_widget")));
		ui.tabWidget->tabBar()->setProperty("group", QVariant(QString::fromUtf8("tab_widget_bar")));
		ui.toolBox->setProperty("group", QVariant(QString::fromUtf8("ltab_widget")));
	}
private:
	bool event(QEvent* e)
	{
		if (e->type() == QEvent::WindowActivate)
		{
			qis.widget.moreActive = true;
			if (qis.state) QiFn::QiState(false);
			QiFn::QiHook(false);
		}
		else if (e->type() == QEvent::WindowDeactivate)
		{
			qis.widget.moreActive = false;
			if (QiFn::SelfActive())
			{
				if (qis.set.defOn) QiFn::QiState(true);
				QiFn::QiHook(true);
			}
		}
		return QWidget::event(e);
	}
	void showEvent(QShowEvent*)
	{
		SetForegroundWindow((HWND)QWidget::winId());
	}
	QPoint msPos; bool mouseDown = false; void mousePressEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) msPos = e->pos(), mouseDown = true; e->accept(); }void mouseMoveEvent(QMouseEvent* e) { if (mouseDown) move(e->pos() + pos() - msPos); }void mouseReleaseEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) mouseDown = false; }
private Q_SLOTS:
	void OnBnClose()
	{
		hide();
	}
};