#pragma once
#include "ui_ClickUi.h"
#include <src/inc_header.h>

class ClickUi : public QWidget
{
	Q_OBJECT;
	Ui::ClickUiClass ui;
	std::once_flag lang_once;

public:
	ClickUi(QWidget* parent = nullptr);

private:
	void Init();
	void Event();
	void StyleGroup();
	void LoadLanguage();

	void TableRemove(int index);
	void TableUpdate(int index);
	void TableAppend(int index);
	void TableReload();

	void customEvent(QEvent* e);
};