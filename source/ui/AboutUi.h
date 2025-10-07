#pragma once
#include "UpdateUi.h"
#include "ui_AboutUi.h"
#include <src/inc_header.h>
#ifdef Q_UPDATE
#include <src/update.h>
#endif
class AboutUi : public QWidget
{
	Q_OBJECT;
	Ui::AboutUiClass ui;
#ifdef Q_UPDATE
	QiUpdate* update;
#endif
	QString version_res;
	std::string version, content;

public:
	AboutUi(QWidget* parent = nullptr);
	QString Version() const;

private:
	bool eventFilter(QObject*, QEvent*);
	void customEvent(QEvent*);
};