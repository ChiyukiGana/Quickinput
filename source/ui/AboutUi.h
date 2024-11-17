#pragma execution_character_set("utf-8")
#pragma once
#include <qurl.h>
#include <qevent.h>
#include <qdesktopservices.h>
#include "ui_AboutUi.h"

class AboutUi : public QWidget
{
	Q_OBJECT;
	Ui::AboutUiClass ui;
public:
	AboutUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		StyleGroup();
		ui.lb_url->installEventFilter(this);
	}
	void StyleGroup()
	{
		ui.clientWidget->setProperty("group", QVariant(QString::fromUtf8("client")));
	}
private:
	bool eventFilter(QObject* obj, QEvent* e)
	{
		if (e->type() == QEvent::MouseButtonRelease)
		{
			QDesktopServices::openUrl(QUrl("http://qinput.cyk.moe"));
			e->accept();
		}
		return QWidget::eventFilter(obj, e);
	}
};