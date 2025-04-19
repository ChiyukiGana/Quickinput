#pragma once
#include "UpdateUi.h"
#include "ui_AboutUi.h"
#include <src/inc_header.h>
#include <src/update.h>
class AboutUi : public QWidget
{
	Q_OBJECT;
	Ui::AboutUiClass ui;
	QiUpdate* update;
	QString version_res;
	std::string version, content;

public:
	AboutUi(QWidget* parent);
	QString Version() const;

private:
	bool eventFilter(QObject*, QEvent*);
	void customEvent(QEvent*);
};