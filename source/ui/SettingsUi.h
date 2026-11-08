#pragma once
#include "HelpUi.h"
#include <src/inc_header.h>
#include "ui_SettingsUi.h"

class SettingsUi : public QWidget
{
	Q_OBJECT;
	using This = SettingsUi;
	Ui::SettingsUiClass ui;
	std::once_flag lang_once;
	HelpUi help;
	SettingsData& sets = Qi::set;

public:
	SettingsUi(QWidget* parent = nullptr);

private:
	void Init();
	void Event();
	void StyleGroup();
	void LoadLanguage();

	bool event(QEvent*);
	void customEvent(QEvent*);
};