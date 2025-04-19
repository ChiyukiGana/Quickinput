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
	MacroGroups* groups = &Qi::macroGroups;
	MacroGroup* currentGroup = &groups->front();
	Macro* currentMacro = nullptr;
	QTableWidget* currentTable = nullptr;
	int currentRow = -1;

public:
	TriggerUi(QWidget* parent);

private:
	void Init();
	void Event();
	void StyleGroup();

	bool GroupCurrented();
	bool TableCurrented();
	bool MacroCurrented();
	bool ItemCurrented();

	void TableState(int macroGroup = -1);
	void SetTableItem(QTableWidget*, int, const Macro&);
	void TableUpdate();

	bool event(QEvent*);
	bool eventFilter(QObject*, QEvent*);
	void showEvent(QShowEvent*);
};