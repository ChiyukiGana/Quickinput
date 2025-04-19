#pragma once
#include <src/inc_header.h>
#include "ui_RecordUi.h"
class RecordUi : public QDialog
{
	Q_OBJECT;
	DecWindowMove;
	using This = RecordUi;
	Ui::RecordUiClass ui;

	MacroGroup* group;
	Macro macro;

public:
	RecordUi(MacroGroup* group, WndInfo* wndInfo);

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