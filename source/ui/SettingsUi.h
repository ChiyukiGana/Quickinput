#pragma once
#include "MoreUi.h"
#include <src/inc_header.h>
#include "ui_SettingsUi.h"

class SettingsUi : public QWidget
{
	Q_OBJECT;
	using This = SettingsUi;
	Ui::SettingsUiClass ui;
	MoreUi more;
	SettingsData* sets = &Qi::set;

public:
	SettingsUi(QWidget* parent = nullptr);

private:
	void Init();
	void Event();
	void StyleGroup();

	bool event(QEvent*);
	bool eventFilter(QObject*, QEvent*);
	void closeEvent(QCloseEvent*);
	void customEvent(QEvent*);
};