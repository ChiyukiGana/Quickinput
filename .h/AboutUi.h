#pragma once
#include "MoreUi.h"
#include "ui_AboutUi.h"

class AboutUi : public QWidget
{
	Q_OBJECT;
	Ui::AboutUiClass ui;
	MoreUi more;

public:
	AboutUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		ui.lbText->installEventFilter(this);
	}

private slots:
	void closeEvent(QCloseEvent*) { more.close(); }

	bool eventFilter(QObject* obj, QEvent* et)
	{
		if (obj == ui.lbText && et->type() == QEvent::MouseButtonRelease) more.show();
		return 0;
	}
};