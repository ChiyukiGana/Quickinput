#pragma execution_character_set("utf-8")
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
};