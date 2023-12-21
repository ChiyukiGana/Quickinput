#pragma once
#include "MoreUi.h"
#include "ui_AboutUi.h"

class AboutUi : public QWidget
{
	Q_OBJECT

public:

	AboutUi(QWidget* parent = 0) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		ui.lbText->installEventFilter(this);
	}

private:

	Ui::AboutUiClass ui;
	MoreUi more;

private slots:

	void closeEvent(QCloseEvent*) { more.close(); }

	bool eventFilter(QObject* obj, QEvent* et)
	{
		if (obj == ui.lbText && et->type() == QEvent::MouseButtonRelease)
		{
			more.show();
			return 1;
		}
		return 0;
	}
};