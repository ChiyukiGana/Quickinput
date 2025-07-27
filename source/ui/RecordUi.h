#pragma once
#include <src/inc_header.h>
#include "ui_RecordUi.h"
class RecordUi : public QDialog
{
	Q_OBJECT;
	DecWindowMove;
	using This = RecordUi;
	Ui::RecordUiClass ui;
	WndInfo* wndInfo;
	Macro macro;

public:
	RecordUi(WndInfo* wndInfo);
	Macro Start();

private:
	void StyleGroup();
	void RecStart();
	void RecStop();
	void RecClose();

	bool event(QEvent*);
	bool eventFilter(QObject*, QEvent*);
	void showEvent(QShowEvent*);
	void customEvent(QEvent*);
};