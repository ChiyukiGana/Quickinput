#pragma once

#pragma execution_character_set("utf-8")
#include "ui_TriggerUi.h"
#include "QuickInputDef.h"

class TriggerUi : public QWidget
{
	Q_OBJECT

public:

	TriggerUi(QWidget* parent = 0, QuickInputStruct* qis = 0) : QWidget(parent)
	{
		this->qis = qis;
		this->scripts = &qis->scripts;

		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		ControlInit();
		ControlEvent();
		LockControl(1);
		TbUpdate();
	}

private:

	const int32 countMax = 9999;

	Ui::TriggerUiClass ui;
	QuickInputStruct* qis;
	List<Script>* scripts;

	void ControlInit()
	{
		ui.hkTr->Mode(2);
		ui.cmbMode->addItem("切换");
		ui.cmbMode->addItem("按下");
		ui.cmbMode->addItem("松开");
		ui.etCount->setValidator(new QIntValidator(0, countMax, this));

		//Table
		{
			ui.tbItem->setColumnCount(4);
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"名称"));
				ui.tbItem->setHorizontalHeaderItem(0, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"按键"));
				ui.tbItem->setHorizontalHeaderItem(1, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"模式"));
				ui.tbItem->setHorizontalHeaderItem(2, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"状态"));
				ui.tbItem->setHorizontalHeaderItem(3, tbi);
			}

			{
				ui.tbItem->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
				ui.tbItem->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
				ui.tbItem->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
				ui.tbItem->setColumnWidth(1, 120);
				ui.tbItem->setColumnWidth(2, 80);
				ui.tbItem->setColumnWidth(3, 60);
			}
		}
	}

	void ControlEvent()
	{
		connect(ui.tbItem, SIGNAL(cellClicked(int, int)), this, SLOT(OnTbClicked(int, int)));
		connect(ui.chbState, SIGNAL(clicked()), this, SLOT(OnChbState()));
		connect(ui.chbBlock, SIGNAL(clicked()), this, SLOT(OnChbBlock()));
		connect(ui.cmbMode, SIGNAL(activated(int)), this, SLOT(OnCmbMode(int)));
		connect(ui.hkTr, SIGNAL(changed()), this, SLOT(OnKeyChanged()));
		connect(ui.etCount, SIGNAL(textEdited(const QString&)), this, SLOT(OnEtCount(const QString&)));
	}

	void ResetControl()
	{
		ui.chbState->setChecked(0);
		ui.chbBlock->setChecked(0);
		ui.hkTr->VirtualKey(0);
		ui.etCount->setText("");
	}

	void TbUpdate()
	{
		ui.tbItem->clearMask();
		ui.tbItem->setRowCount(scripts[0].len());
		ui.tbItem->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.tbItem->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tbItem->verticalHeader()->setDefaultSectionSize(0);

		for (uint32 u = 0; u < qis->scripts.len(); u++) {
			ui.tbItem->setItem(u, 0, new QTableWidgetItem(QString::fromWCharArray(scripts[0][u].name.c_str())));
			ui.tbItem->item(u, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//
			QString qs = u8"----";
			if ((scripts[0][u].key & 0xFFFF) != 0)
			{
				qs = QString::fromWCharArray(Input::Name(scripts[0][u].key & 0xFFFF));
				if ((scripts[0][u].key >> 16) != 0)
				{
					qs += u8" + ";
					qs += QString::fromWCharArray(Input::Name(scripts[0][u].key >> 16));
				}
			}
			ui.tbItem->setItem(u, 1, new QTableWidgetItem(qs));
			ui.tbItem->item(u, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//
			qs = "";
			switch (scripts[0][u].mode)
			{
			case Script::sw:
				qs = u8"切换";
				break;
			case Script::down:
				qs = u8"按下(";
				if (scripts[0][u].a) qs += QString::number(scripts[0][u].a);
				else qs += u8"无限";
				qs += u8")";
				break;
			case Script::up:
				qs = u8"松开(";
				if (scripts[0][u].a) qs += QString::number(scripts[0][u].a);
				else qs += u8"无限";
				qs += u8")";
				break;
			}
			ui.tbItem->setItem(u, 2, new QTableWidgetItem(qs));
			ui.tbItem->item(u, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//
			if (scripts[0][u].state) qs = UI::trOn;
			else qs = UI::trOff;
			ui.tbItem->setItem(u, 3, new QTableWidgetItem(qs));
			ui.tbItem->item(u, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
	}

	void LockControl(bool state)
	{
		ui.chbState->setDisabled(state);
		ui.chbBlock->setDisabled(state);
		ui.cmbMode->setDisabled(state);
		ui.hkTr->setDisabled(state);
		if (state) ui.etCount->setDisabled(state);
	}

private slots:

	void showEvent(QShowEvent*)
	{
		ui.tbItem->setCurrentItem(0);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnTbClicked(int row, int column)
	{
		ResetControl();
		LockControl(1);
		if (row < 0) return;

		ui.chbState->setChecked(scripts[0][row].state);
		ui.chbBlock->setChecked(scripts[0][row].block);
		ui.cmbMode->setCurrentIndex(scripts[0][row].mode);
		ui.hkTr->VirtualKey(scripts[0][row].key & 0xFFFF, scripts[0][row].key >> 16);
		if (scripts[0][row].mode >= Script::down)
		{
			ui.etCount->setText(QString::number(scripts[0][row].a));
			ui.etCount->setDisabled(0);
		}
		LockControl(0);
	}

	void OnKeyChanged()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		scripts[0][pos].key = ui.hkTr->virtualKey();

		TbUpdate();
		SaveScript(scripts[0][pos]);
	}

	void OnChbState()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		scripts[0][pos].state = ui.chbState->isChecked();

		TbUpdate();
		SaveScript(scripts[0][pos]);
	}

	void OnChbBlock()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		scripts[0][pos].block = ui.chbBlock->isChecked();

		TbUpdate();
		SaveScript(scripts[0][pos]);
	}

	void OnCmbMode(int index)
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		switch (index)
		{
		case Script::sw:
			ui.etCount->setText(u8"无限");
			ui.etCount->setDisabled(1);
			scripts[0][pos].a = 0;
			scripts[0][pos].mode = Script::sw;
			break;
		case Script::down:
			ui.etCount->setText("");
			ui.etCount->setDisabled(0);
			scripts[0][pos].a = 1;
			scripts[0][pos].mode = Script::down;
			break;
		case Script::up:
			ui.etCount->setText("");
			ui.etCount->setDisabled(0);
			scripts[0][pos].a = 1;
			scripts[0][pos].mode = Script::up;
			break;
		}

		TbUpdate();
		SaveScript(scripts[0][pos]);
	}

	void OnEtCount(const QString& count)
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		int i = count.toInt();
		if (i > countMax) i = countMax;
		scripts[0][pos].a = i;

		TbUpdate();
		SaveScript(scripts[0][pos]);
	}
};