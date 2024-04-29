#pragma once
#include "ui_TriggerUi.h"
#include "../static.h"

class TriggerUi : public QWidget
{
	Q_OBJECT;
	const int32 countMax = 9999;

	Ui::TriggerUiClass ui;
	Macros& macros = Global::qi.macros;

public:
	TriggerUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
		LockControl(1);
		TbUpdate();
		ReStyle();
	}

	void ReStyle()
	{
		ui.clientWidget->setStyleSheet(Global::qi.styles[Global::qi.set.style].style);
		ui.hkTr->setStyleSheet(Global::qi.styles[Global::qi.set.style].style);
	}

private:
	void WidInit()
	{
		ui.hkTr->Mode(2);
		ui.cmbMode->addItem(u8"切换");
		ui.cmbMode->addItem(u8"按下");
		ui.cmbMode->addItem(u8"松开");
		ui.etCount->setValidator(new QIntValidator(0, countMax, this));

		//Table
		{
			ui.tbActions->setColumnCount(4);
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"名称"));
				ui.tbActions->setHorizontalHeaderItem(0, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"按键"));
				ui.tbActions->setHorizontalHeaderItem(1, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"模式"));
				ui.tbActions->setHorizontalHeaderItem(2, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"状态"));
				ui.tbActions->setHorizontalHeaderItem(3, tbi);
			}

			{
				ui.tbActions->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
				ui.tbActions->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
				ui.tbActions->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
				ui.tbActions->setColumnWidth(1, 120);
				ui.tbActions->setColumnWidth(2, 80);
				ui.tbActions->setColumnWidth(3, 60);
			}
		}
	}
	void WidEvent()
	{
		connect(ui.tbActions, SIGNAL(cellClicked(int, int)), this, SLOT(OnTbClicked(int, int)));
		connect(ui.chbBlock, SIGNAL(clicked()), this, SLOT(OnChbBlock()));
		connect(ui.cmbMode, SIGNAL(activated(int)), this, SLOT(OnCmbMode(int)));
		connect(ui.hkTr, SIGNAL(changed()), this, SLOT(OnKeyChanged()));
		connect(ui.etCount, SIGNAL(textEdited(const QString&)), this, SLOT(OnEtCount(const QString&)));
	}

	void ResetControl()
	{
		ui.chbBlock->setChecked(0);
		ui.hkTr->VirtualKey(0);
		ui.etCount->setText("");
	}
	void LockControl(bool state)
	{
		ui.chbBlock->setDisabled(state);
		ui.cmbMode->setDisabled(state);
		ui.hkTr->setDisabled(state);
		if (state) ui.etCount->setDisabled(state);
	}

	void TbUpdate()
	{
		ui.tbActions->clearMask();
		ui.tbActions->setRowCount(macros.size());
		ui.tbActions->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.tbActions->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tbActions->verticalHeader()->setDefaultSectionSize(0);

		for (uint32 u = 0; u < macros.size(); u++) {
			ui.tbActions->setItem(u, 0, new QTableWidgetItem(QString::fromWCharArray(macros[u].name.c_str())));
			ui.tbActions->item(u, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//
			QString qs = u8"----";
			if ((macros[u].key & 0xFFFF) != 0)
			{
				qs = QString::fromWCharArray(Input::Name(macros[u].key & 0xFFFF));
				if ((macros[u].key >> 16) != 0)
				{
					qs += u8" + ";
					qs += QString::fromWCharArray(Input::Name(macros[u].key >> 16));
				}
			}
			ui.tbActions->setItem(u, 1, new QTableWidgetItem(qs));
			ui.tbActions->item(u, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//
			qs = "";
			switch (macros[u].mode)
			{
			case Macro::sw:
				qs = u8"切换";
				break;
			case Macro::down:
				qs = u8"按下(";
				if (macros[u].count) qs += QString::number(macros[u].count);
				else qs += u8"无限";
				qs += u8")";
				break;
			case Macro::up:
				qs = u8"松开(";
				if (macros[u].count) qs += QString::number(macros[u].count);
				else qs += u8"无限";
				qs += u8")";
				break;
			}
			ui.tbActions->setItem(u, 2, new QTableWidgetItem(qs));
			ui.tbActions->item(u, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//
			if (macros[u].state) qs = UI::trOn;
			else qs = UI::trOff;
			ui.tbActions->setItem(u, 3, new QTableWidgetItem(qs));
			ui.tbActions->item(u, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}

		ui.tbActions->setStyleSheet(u8"QHeaderView::section,QScrollBar{background:transparent}");
	}

	void showEvent(QShowEvent*)
	{
		ui.tbActions->setCurrentItem(0);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

private slots:
	void OnTbClicked(int row, int column)
	{
		ResetControl();
		LockControl(1);
		if (row < 0) return;

		if (column == 3) macros[row].state ? macros[row].state = 0 : macros[row].state = 1;
		ui.chbBlock->setChecked(macros[row].block);
		ui.cmbMode->setCurrentIndex(macros[row].mode);
		ui.hkTr->VirtualKey(macros[row].key & 0xFFFF, macros[row].key >> 16);
		if (macros[row].mode >= Macro::down)
		{
			ui.etCount->setText(QString::number(macros[row].count));
			ui.etCount->setDisabled(0);
		}

		LockControl(0);
		TbUpdate();
		SaveMacro(macros[row]);
	}
	void OnKeyChanged()
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;

		macros[pos].key = ui.hkTr->virtualKey();

		TbUpdate();
		SaveMacro(macros[pos]);
	}
	void OnChbBlock()
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;

		macros[pos].block = ui.chbBlock->isChecked();

		TbUpdate();
		SaveMacro(macros[pos]);
	}
	void OnCmbMode(int index)
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;

		switch (index)
		{
		case Macro::sw:
			ui.etCount->setText(u8"无限");
			ui.etCount->setDisabled(1);
			macros[pos].count = 0;
			macros[pos].mode = Macro::sw;
			break;
		case Macro::down:
			ui.etCount->setText("");
			ui.etCount->setDisabled(0);
			macros[pos].count = 1;
			macros[pos].mode = Macro::down;
			break;
		case Macro::up:
			ui.etCount->setText("");
			ui.etCount->setDisabled(0);
			macros[pos].count = 1;
			macros[pos].mode = Macro::up;
			break;
		}

		TbUpdate();
		SaveMacro(macros[pos]);
	}
	void OnEtCount(const QString& count)
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;

		int i = count.toInt();
		if (i > countMax) i = countMax;
		macros[pos].count = i;

		TbUpdate();
		SaveMacro(macros[pos]);
	}
};