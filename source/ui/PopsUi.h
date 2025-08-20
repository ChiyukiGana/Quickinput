#pragma once
#include <src/inc_header.h>
#include "ui_PopsUi.h"
class PopsUi : public QDialogFrameless
{
	Q_OBJECT;
	using This = PopsUi;
	Ui::PopsUiClass ui;
	QiVector<QLineEdit*> bind_text_edit;
	QiVector<QFileEdit*> bind_sound_edit;
	SettingsData* sets = &Qi::set;

public:
	PopsUi();

private:
	void Init();
	void Event();
	void StyleGroup();
	
	void SetColor(const QColor& color, QPushButton* button);
	void SelectColor(QiUi::PopTextInfo& p);
	void Update();

	bool event(QEvent* e);
	bool eventFilter(QObject* obj, QEvent* e);
	void showEvent(QShowEvent*);
};