#pragma once
#include <src/inc_header.h>
#include "ui_MacroUi.h"
class MacroUi : public QWidget
{
	Q_OBJECT;
	using This = MacroUi;
	Ui::MacroUiClass ui;
	std::once_flag lang_once;
	MacroGroups* groups = &Qi::macroGroups;
	MacroGroup* currentGroup = nullptr;
	MacroPointers currentMacros;
	Macro* currentEdit = nullptr;
	bool updating = false;

public:
	MacroUi(QWidget* parent = nullptr);

private:
	void Init();
	void Event();
	void StyleGroup();
	void LoadLanguage();

	void ResetWidget();
	void DisableWidget();
	void TableUpdate();
	void RecStart(bool);
	void CurrentChanged(int);
	bool SelectGroup();

	bool isSold();
	bool isMult();

private:
	void showEvent(QShowEvent*);
	void customEvent(QEvent*);
	bool eventFilter(QObject*, QEvent*);
};