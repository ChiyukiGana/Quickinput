#pragma once
#include <qlistview.h>
#include <src/inc_header.h>
#include "ui_FuncUi.h"
class FuncUi : public QWidget
{
	Q_OBJECT;
	using This = FuncUi;
	Ui::FuncUiClass ui;
	FuncData* func = &Qi::fun;

public:
	FuncUi(QWidget* paren);

private:
	void Init();
	void Event();
	void StyleGroup();


	bool event(QEvent*);
	bool eventFilter(QObject*, QEvent*);
};