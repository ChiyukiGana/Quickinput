#pragma once
#include "ui_TriggerUi.h"
#include "header.h"

class TriggerUi : public QWidget
{
	Q_OBJECT;
	const int32 countMax = 9999;

	Ui::TriggerUiClass ui;
	Macros* macros = &qis.macros;

public:
	TriggerUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
		LockControl(true);
		TbUpdate();
		ReStyle();
	}

	void ReStyle()
	{
		ui.clientWidget->setStyleSheet("");
		ui.clientWidget->setStyleSheet(qis.themes[qis.set.theme].style);
		ui.hkTr->setStyleSheet("");
		ui.hkTr->setStyleSheet(qis.themes[qis.set.theme].style);
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
			ui.tbActions->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			ui.tbActions->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			ui.tbActions->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
			ui.tbActions->setColumnWidth(1, 120);
			ui.tbActions->setColumnWidth(2, 80);
			ui.tbActions->setColumnWidth(3, 60);
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
		ui.tbActions->setRowCount(macros->size());
		ui.tbActions->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.tbActions->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tbActions->verticalHeader()->setDefaultSectionSize(0);

		for (size_t i = 0; i < macros->size(); i++) {
			ui.tbActions->setItem(i, 0, new QTableWidgetItem(QString::fromWCharArray(macros->at(i).name.c_str())));
			ui.tbActions->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//
			QString qs = u8"----";
			if ((macros->at(i).key & 0xFFFF) != 0)
			{
				qs = QString::fromWCharArray(Input::Name(macros->at(i).key & 0xFFFF));
				if ((macros->at(i).key >> 16) != 0)
				{
					qs += u8" + ";
					qs += QString::fromWCharArray(Input::Name(macros->at(i).key >> 16));
				}
			}
			ui.tbActions->setItem(i, 1, new QTableWidgetItem(qs));
			ui.tbActions->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//
			qs = "";
			switch (macros->at(i).mode)
			{
			case Macro::sw:
				qs = u8"切换";
				break;
			case Macro::down:
				qs = u8"按下(";
				if (macros->at(i).count) qs += QString::number(macros->at(i).count);
				else qs += u8"无限";
				qs += u8")";
				break;
			case Macro::up:
				qs = u8"松开(";
				if (macros->at(i).count) qs += QString::number(macros->at(i).count);
				else qs += u8"无限";
				qs += u8")";
				break;
			}
			ui.tbActions->setItem(i, 2, new QTableWidgetItem(qs));
			ui.tbActions->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//
			if (macros->at(i).state) qs = qis.ui.trOn;
			else qs = qis.ui.trOff;
			ui.tbActions->setItem(i, 3, new QTableWidgetItem(qs));
			ui.tbActions->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
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
		LockControl(true);
		if (row < 0) return;

		if (column == 3) macros->at(row).state = !macros->at(row).state;
		ui.chbBlock->setChecked(macros->at(row).block);
		ui.cmbMode->setCurrentIndex(macros->at(row).mode);
		ui.hkTr->VirtualKey(macros->at(row).key & 0xFFFF, macros->at(row).key >> 16);
		if (macros->at(row).mode >= Macro::down)
		{
			ui.etCount->setText(QString::number(macros->at(row).count));
			ui.etCount->setDisabled(0);
		}

		LockControl(0);
		TbUpdate();
		QiFn::SaveMacro(macros->at(row));
	}
	void OnKeyChanged()
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		macros->at(p).key = ui.hkTr->virtualKey();
		TbUpdate();
		QiFn::SaveMacro(macros->at(p));
	}
	void OnChbBlock()
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		macros->at(p).block = ui.chbBlock->isChecked();
		TbUpdate();
		QiFn::SaveMacro(macros->at(p));
	}
	void OnCmbMode(int index)
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		switch (index)
		{
		case Macro::sw:
			ui.etCount->setText(u8"无限");
			ui.etCount->setDisabled(1);
			macros->at(p).count = 0;
			macros->at(p).mode = Macro::sw;
			break;
		case Macro::down:
			ui.etCount->setText("");
			ui.etCount->setDisabled(0);
			macros->at(p).count = 1;
			macros->at(p).mode = Macro::down;
			break;
		case Macro::up:
			ui.etCount->setText("");
			ui.etCount->setDisabled(0);
			macros->at(p).count = 1;
			macros->at(p).mode = Macro::up;
			break;
		}

		TbUpdate();
		QiFn::SaveMacro(macros->at(p));
	}
	void OnEtCount(const QString& count)
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		int n = count.toInt();
		if (n > countMax) n = countMax;
		macros->at(p).count = n;
		TbUpdate();
		QiFn::SaveMacro(macros->at(p));
	}
};