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
		setParent(parent);
		setWindowFlags(Qt::FramelessWindowHint);
		move(0, 40);

		ControlInit();
		ControlEvent();
		LockControl(1);
		FontResize();
		TbUpdate();
	}

private:

	Ui::TriggerUiClass ui;
	QuickInputStruct* qis;
	List<Script>* scripts;

	void FontResize()
	{
		ui.tbItem->setFont(UI::font1);
		ui.lbTrigger->setFont(UI::font3);
		ui.lbState->setFont(UI::font2);
		ui.lbMode->setFont(UI::font2);
		ui.lbTr->setFont(UI::font2);
		ui.lbCount->setFont(UI::font2);
		ui.cmbMode->setFont(UI::font2);
		ui.etCount->setFont(UI::font1);
		ui.hkTr->setFont(UI::font1);
	}

	void ControlInit()
	{
		ui.hkTr->Mode(2);
		ui.cmbMode->addItem("长按");
		ui.cmbMode->addItem("点击");
		ui.cmbMode->addItem("次数");
		ui.etCount->setValidator(new QIntValidator(0, 1000, this));

		//Table
		{
			ui.tbItem->setColumnCount(4);
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"名称"));
				tbi->setFont(UI::font2);
				ui.tbItem->setHorizontalHeaderItem(0, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"按键"));
				tbi->setFont(UI::font2);
				ui.tbItem->setHorizontalHeaderItem(1, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"模式"));
				tbi->setFont(UI::font2);
				ui.tbItem->setHorizontalHeaderItem(2, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"状态"));
				tbi->setFont(UI::font2);
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
		connect(ui.cmbMode, SIGNAL(activated(int)), this, SLOT(OnCmbMode(int)));
		connect(ui.hkTr, SIGNAL(changed()), this, SLOT(OnKeyChanged()));
		connect(ui.etCount, SIGNAL(textEdited(const QString&)), this, SLOT(OnEtCount(const QString&)));
	}

	void TbUpdate()
	{
		ui.tbItem->clearMask();
		ui.tbItem->setRowCount(scripts[0].len());
		ui.tbItem->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
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
			switch (scripts[0][u].mode & 0xFFFF)
			{
			case 0:
				qs = u8"长按";
				break;
			case 1:
				qs = u8"点击";
				break;
			case 2:
				qs = u8"次数(";
				qs += QString::number(scripts[0][u].mode >> 16);
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

	void ResetControl()
	{
		ui.chbState->setChecked(0);
		ui.hkTr->VirtualKey(0);
		ui.etCount->setText("");
	}

	void LockControl(bool state)
	{
		ui.chbState->setDisabled(state);
		ui.cmbMode->setDisabled(state);
		ui.hkTr->setDisabled(state);
		if (state) ui.etCount->setDisabled(state);
	}

private slots:

	void showEvent(QShowEvent*)
	{
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnTbClicked(int row, int column)
	{
		ResetControl();
		LockControl(1);
		if (row < 0) return;

		int mode = scripts[0][row].mode & 0xFFFF;

		ui.chbState->setChecked(scripts[0][row].state);
		ui.cmbMode->setCurrentIndex(mode);
		ui.hkTr->VirtualKey(scripts[0][row].key & 0xFFFF, scripts[0][row].key >> 16);
		if (mode == 2)
		{
			ui.etCount->setText(QString::number(scripts[0][row].mode >> 16));
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

		if (ui.chbState->isChecked()) 	scripts[0][pos].state = 1;
		else scripts[0][pos].state = 0;

		TbUpdate();
		SaveScript(scripts[0][pos]);
	}

	void OnCmbMode(int index)
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		int mode = index;

		if (index == 2)
		{
			ui.etCount->setDisabled(0);
			mode |= 1 << 16;
		}
		else
		{
			ui.etCount->setText("");
			ui.etCount->setDisabled(1);
		}

		scripts[0][pos].mode = mode;

		TbUpdate();
		SaveScript(scripts[0][pos]);
	}

	void OnEtCount(const QString& count)
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		scripts[0][pos].mode = (count.toInt() << 16) | 2;

		TbUpdate();
		SaveScript(scripts[0][pos]);
	}
};