#pragma once
#include <src/inc_header.h>
#include "ui_MacroUi.h"
class MacroUi : public QWidget
{
	Q_OBJECT;
	using This = MacroUi;
	Ui::MacroUiClass ui;
	MacroGroups* groups = &Qi::macroGroups;
	MacroGroup* currentGroup = &groups->front();
	QList<Macro*> currentMacros;
	bool updating = false;

public:
	MacroUi(QWidget* parent);

private:
	void Init();
	void Event();
	void StyleGroup();

	void ResetWidget();
	void DisableWidget();
	void TableUpdate();
	void RecStart(bool);

	bool isSold();
	bool isMult();

private:
	void showEvent(QShowEvent*);
	void customEvent(QEvent*);
	bool eventFilter(QObject*, QEvent*);
};