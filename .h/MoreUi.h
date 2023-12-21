#pragma once
#include <qevent.h>
#include "QuickInputDef.h"
#include "ui_MoreUi.h"

class MoreUi : public QDialog
{
	Q_OBJECT

public:

	MoreUi(QWidget* parent = 0) : QDialog(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		setAttribute(Qt::WA_TranslucentBackground);
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
	}

private:

	Ui::MoreUiClass ui;
	QPoint msPos;

	void mousePressEvent(QMouseEvent* et)
	{
		if (et->buttons() & Qt::LeftButton) msPos = et->pos();
	}

	void mouseMoveEvent(QMouseEvent* et)
	{
		if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - msPos);
	}

private slots:

	void OnBnClose()
	{
		hide();
	}
};