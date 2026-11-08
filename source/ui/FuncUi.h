#pragma once
#include <qlistview.h>
#include <src/inc_header.h>
#include "ui_FuncUi.h"
class FuncUi : public QWidget
{
	Q_OBJECT;
	using This = FuncUi;
	Ui::FuncUiClass ui;
	std::once_flag lang_once;
	FuncData* func = &Qi::fun;

public:
	FuncUi(QWidget* parent = nullptr);

private:
	void Init();
	void Event();
	void StyleGroup();
	void LoadLanguage();


	bool event(QEvent*);
	void customEvent(QEvent*);
};