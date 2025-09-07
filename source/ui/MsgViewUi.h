#pragma once
#include <src/inc_header.h>
#include "ui_MsgViewUi.h"

class MsgViewUi : public QDialogFrameless
{
	Q_OBJECT;
	using This = MsgViewUi;
	enum
	{
		_col_time,
		_col_level,
		_col_macro,
		_col_text
	};

	static constexpr size_t max_row = 1024;

	Ui::MsgViewUiClass ui;
	MsgViewInfos infos;
	bool filter_msg = true;
	bool filter_war = true;
	bool filter_err = true;

public:
	MsgViewUi();
	void StyleGroup();

private:
	void append(const MsgViewInfo& info, bool update = false);
	void update();
	void customEvent(QEvent*);
};