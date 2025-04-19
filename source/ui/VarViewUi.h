#pragma once
#include <src/inc_header.h>
#include "ui_VarViewUi.h"
class VarViewUi : public QDialog
{
	Q_OBJECT;
	DecWindowMove;
	using This = VarViewUi;
	enum
	{
		tableColumn_name,
		tableColumn_type,
		tableColumn_value
	};
	Ui::VarViewUiClass ui;
	bool updating = false;
	bool reload = false;

public:
	VarViewUi();

private:
	void StyleGroup();
	void TableUpdate(QTableWidget*, const QiVarMap);
	void TableUpdate();

	bool event(QEvent*);
	void showEvent(QShowEvent*);
	void customEvent(QEvent*);
};