#pragma once
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
	}

private slots:
};