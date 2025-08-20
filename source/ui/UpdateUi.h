#pragma once
#include <src/inc_header.h>
#include "ui_UpdateUi.h"
class UpdateUi : public QDialogFrameless
{
	Q_OBJECT;
	Ui::UpdateUiClass ui;

public:
	UpdateUi(const QString& version, const QString& content);

private:
	void StyleGroup();

	bool event(QEvent* e);
};