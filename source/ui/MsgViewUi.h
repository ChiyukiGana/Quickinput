#pragma once
#include <src/inc_header.h>
#include "ui_MsgViewUi.h"

class MsgViewUi : public QDialogFrameless
{
	Q_OBJECT;
	using This = MsgViewUi;
	Ui::MsgViewUiClass ui;

public:
	MsgViewUi();
	void StyleGroup();

private:
	void customEvent(QEvent*);
};