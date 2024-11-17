#pragma once
#include <qevent.h>
#include "ui_MoreUi.h"
#include "header.h"

class MoreUi : public QDialog
{
	Q_OBJECT;
	Ui::MoreUiClass ui;
	QPoint msPos;

public:
	MoreUi() : QDialog()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
		ReStyle();
	}

	void ReStyle()
	{
		setStyleSheet(qis.themes[qis.set.theme].style);
		ui.lbHowUseText->setStyleSheet(qis.themes[qis.set.theme].style);
		ui.lbLogText->setStyleSheet(qis.themes[qis.set.theme].style);
	}

private:
	void mousePressEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) msPos = et->pos(); }
	void mouseMoveEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - msPos); }

private slots:
	void OnBnClose() { hide(); }
};