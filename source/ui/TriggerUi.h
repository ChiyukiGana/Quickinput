#pragma once
#include <src/inc_header.h>
#include <qlistview.h>
#include "ui_TriggerUi.h"
class TriggerUi : public QWidget
{
	Q_OBJECT;
	using This = TriggerUi;
	enum
	{
		tableColumn_name,
		tableColumn_key,
		tableColumn_mode,
		tableColumn_state,
	};

	Ui::TriggerUiClass ui;
	std::once_flag lang_once;
	MacroGroups* groups = &Qi::macroGroups;
	MacroGroup* currentGroup = nullptr;
	Macro* currentMacro = nullptr;
	QTableWidget* currentTable = nullptr;
	int currentRow = -1;

public:
	TriggerUi(QWidget* parent = nullptr);

private:
	void Init();
	void Event();
	void StyleGroup();
	void LoadLanguage();

	bool GroupCurrented();
	bool TableCurrented();
	bool MacroCurrented();
	bool ItemCurrented();

	void TableState(int macroGroup = -1);
	void SetTableItem(QTableWidget*, int, const Macro&);
	void TableUpdate();
	void VarEditUpdate(const Macro*);

	bool event(QEvent*);
	void showEvent(QShowEvent*);
	void customEvent(QEvent*);
};