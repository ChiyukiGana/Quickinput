﻿#pragma once
#include <src/inc_header.h>
#include "ui_MoreUi.h"
class MoreUi : public QDialog
{
	Q_OBJECT;
	DecWindowMove;
	using This = MoreUi;
	Ui::MoreUiClass ui;

public:
	MoreUi();

private:
	void StyleGroup();

	bool event(QEvent*);
	void showEvent(QShowEvent*);
};