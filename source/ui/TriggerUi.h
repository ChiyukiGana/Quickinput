﻿#pragma execution_character_set("utf-8")
#pragma once
#include <qstandarditemmodel.h>
#include <qabstractitemview.h>
#include "../src/minc.h"
#include "ui_TriggerUi.h"

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
		TableUpdate();
		ReStyle();
	}
	void ReStyle()
	{
		ui.clientWidget->setStyleSheet("");
		ui.clientWidget->setStyleSheet(qis.ui.themes[qis.set.theme].style);
		ui.hkTr->setStyleSheet("");
		ui.hkTr->setStyleSheet(qis.ui.themes[qis.set.theme].style);
	}
private:
	void WidInit()
	{
		ui.hkTr->Mode(2);
		ui.cmbMode->addItem("切换");
		ui.cmbMode->addItem("按下");
		ui.cmbMode->addItem("松开");
		QStandardItemModel* model = (QStandardItemModel*)ui.cmbMode->view()->model();
		for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);
		if (qis.set.theme >= qis.ui.themes.size()) qis.set.theme = 0;

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

		{
			ui.chbBlock->setShortcut(Qt::Key_unknown);
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
	void TableUpdate()
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
			QString qs = "----";
			if ((macros->at(i).key & 0xFFFF) != 0)
			{
				qs = QString::fromWCharArray(Input::Name(macros->at(i).key & 0xFFFF));
				if ((macros->at(i).key >> 16) != 0)
				{
					qs += " + ";
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
				qs = "切换";
				break;
			case Macro::down:
				qs = "按下(";
				if (macros->at(i).count) qs += QString::number(macros->at(i).count);
				else qs += "无限";
				qs += ")";
				break;
			case Macro::up:
				qs = "松开(";
				if (macros->at(i).count) qs += QString::number(macros->at(i).count);
				else qs += "无限";
				qs += ")";
				break;
			}
			ui.tbActions->setItem(i, 2, new QTableWidgetItem(qs));
			ui.tbActions->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//
			if (macros->at(i).state) qs = qis.ui.text.trOn;
			else qs = qis.ui.text.trOff;
			ui.tbActions->setItem(i, 3, new QTableWidgetItem(qs));
			ui.tbActions->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}

		ui.tbActions->setStyleSheet("QHeaderView::section,QScrollBar{background:transparent}");
	}
	void showEvent(QShowEvent*)
	{
		ui.tbActions->setCurrentItem(0);
		TableUpdate();
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
		TableUpdate();
		QiJson::SaveMacro(macros->at(row));
	}
	void OnKeyChanged()
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		macros->at(p).key = ui.hkTr->virtualKey();
		TableUpdate();
		QiJson::SaveMacro(macros->at(p));
	}
	void OnChbBlock()
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		macros->at(p).block = ui.chbBlock->isChecked();
		TableUpdate();
		QiJson::SaveMacro(macros->at(p));
	}
	void OnCmbMode(int index)
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		switch (index)
		{
		case Macro::sw:
			ui.etCount->setText("无限");
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

		TableUpdate();
		QiJson::SaveMacro(macros->at(p));
	}
	void OnEtCount(const QString& count)
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		int n = count.toInt();
		if (n > countMax) n = countMax;
		macros->at(p).count = n;
		TableUpdate();
		QiJson::SaveMacro(macros->at(p));
	}
};