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
		SetStyleGroup();
		ReStyle();
	}
	void SetStyleGroup()
	{
		setProperty("group", QVariant(QString::fromUtf8("frame")));
		ui.titleWidget->setProperty("group", QVariant(QString::fromUtf8("title")));
		ui.clientWidget->setProperty("group", QVariant(QString::fromUtf8("client")));
		ui.bnClose->setProperty("group", QVariant(QString::fromUtf8("title-close_button")));
		ui.tabWidget->setProperty("group", QVariant(QString::fromUtf8("tab_widget")));
		ui.tabWidget->tabBar()->setProperty("group", QVariant(QString::fromUtf8("tab_widget_bar")));
		ui.toolBox->setProperty("group", QVariant(QString::fromUtf8("ltab_widget")));
	}
	void ReStyle()
	{
		setStyleSheet("");
		setStyleSheet(qis.ui.themes[qis.set.theme].style);
	}
private:
	bool event(QEvent* et)
	{
		if (et->type() == QEvent::WindowActivate)
		{
			qis.widget.moreActive = true;
			if (qis.state) QiFn::QiState(false);
			QiFn::QiHook(false);
		}
		else if (et->type() == QEvent::WindowDeactivate)
		{
			qis.widget.moreActive = false;
			if (QiFn::SelfActive())
			{
				if (qis.set.defOn) QiFn::QiState(true);
				QiFn::QiHook(true);
			}
		}
		return QWidget::event(et);
	}
	void showEvent(QShowEvent*)
	{
		ReStyle();
		SetForegroundWindow((HWND)QWidget::winId());
	}
	QPoint msPos; bool mouseDown = false; void mousePressEvent(QMouseEvent* et) { if (et->button() == Qt::LeftButton) msPos = et->pos(), mouseDown = true; et->accept(); }void mouseMoveEvent(QMouseEvent* et) { if (mouseDown) move(et->pos() + pos() - msPos); }void mouseReleaseEvent(QMouseEvent* et) { if (et->button() == Qt::LeftButton) mouseDown = false; }
private slots:
	void OnBnClose()
	{
		hide();
	}
};