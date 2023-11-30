#pragma once
#pragma execution_character_set("utf-8")
#include <qtimer.h>
#include "ui_MacroUi.h"
#include "RecordUi.h"
#include "EditUi.h"
#include "QuickInputDef.h"

class MacroUi : public QWidget
{
	Q_OBJECT

public:

	MacroUi(QWidget* parent = 0, QuickInputStruct* qis = 0) : QWidget(parent)
	{
		this->qis = qis;
		this->main = parent;
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

	Ui::MacroUiClass ui;
	QWidget* main;
	QuickInputStruct* qis;
	List<Script>* scripts;
	QTimer* timer;

	void FontResize()
	{
		ui.tbItem->setFont(UI::font1);
		ui.lbMacro->setFont(UI::font3);
		ui.etName->setFont(UI::font2);
		ui.bnRec->setFont(UI::font2);
		ui.bnAdd->setFont(UI::font2);
		ui.bnEdit->setFont(UI::font2);
		ui.bnDel->setFont(UI::font2);
	}

	void ControlInit()
	{
		timer = new QTimer(this);

		//Table
		{
			ui.tbItem->setColumnCount(1);
			QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"名称"));
			tbi->setFont(UI::font2);
			ui.tbItem->setHorizontalHeaderItem(0, tbi);
			ui.tbItem->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
		}
	}

	void ControlEvent()
	{
		connect(timer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
		connect(ui.tbItem, SIGNAL(cellClicked(int, int)), this, SLOT(OnTbClicked(int, int)));
		connect(ui.etName, SIGNAL(returnPressed()), this, SLOT(OnEtReturn()));
		connect(ui.bnRec, SIGNAL(clicked()), this, SLOT(OnBnRec()));
		connect(ui.bnAdd, SIGNAL(clicked()), this, SLOT(OnBnAdd()));
		connect(ui.bnEdit, SIGNAL(clicked()), this, SLOT(OnBnEdit()));
		connect(ui.bnDel, SIGNAL(clicked()), this, SLOT(OnBnDel()));
	}

	void ResetControl()
	{
		ui.etName->setText("");
		ui.tbItem->setCurrentItem(0);
	}

	void LockControl(bool state)
	{
		ui.etName->setDisabled(state);
		ui.bnEdit->setDisabled(state);
		ui.bnDel->setDisabled(state);
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
		}
	}

private slots:

	void showEvent(QShowEvent*)
	{
		ResetControl();
		LockControl(1);
		TbUpdate();
	}

	void OnTimeOut()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;
		timer->stop();
		ui.etName->setDisabled(0);
		ui.etName->setText(QString::fromWCharArray(qis->scripts[pos].name.c_str()));
	}

	void OnTbClicked(int row, int column)
	{
		LockControl(1);
		if (row < 0) return;

		ui.etName->setText(QString::fromWCharArray(qis->scripts[row].name.c_str()));

		LockControl(0);
	}

	void OnEtReturn()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		if (File::NameFilter((LPCWSTR)ui.etName->text().utf16()) == L"")
		{
			timer->setSingleShot(1);
			timer->start(1000);

			ui.etName->setDisabled(1);
			ui.etName->setText("名称不可用");
			return;
		}

		std::wstring path = L"json\\";
		path += (LPCWSTR)ui.etName->text().utf16();
		path += L".json";

		if (File::FileState(path.c_str()))
		{
			timer->setSingleShot(1);
			timer->start(1000);

			ui.etName->setDisabled(1);
			ui.etName->setText("已存在该名称");
			return;
		}

		path = L"json\\" + qis->scripts[pos].name + L".json";
		File::FileDelete(path.c_str());

		qis->scripts[pos].name = (LPCWSTR)ui.etName->text().utf16();
		ui.etName->setText(QString::fromWCharArray(qis->scripts[pos].name.c_str()));

		TbUpdate();
		SaveScript(qis->scripts[pos]);
	}

	void OnBnRec()
	{
		qis->scripts.Add();
		RecordUi rw(main, qis, &qis->scripts[qis->scripts.len() - 1].items);

		main->hide();
		qis->HookState(1);
		qis->state = 0;

		rw.exec();

		main->show();
		qis->HookState(0);

		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnAdd()
	{
		qis->scripts.Add();
		qis->scripts[qis->scripts.len() - 1].name = NameFilter(qis, L"宏");
		qis->scripts[qis->scripts.len() - 1].mode = 1;
		TbUpdate();
		ResetControl();
		LockControl(1);
		SaveScript(qis->scripts[qis->scripts.len() - 1]);
	}

	void OnBnEdit()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		EditUi edit(main, qis, &qis->scripts[pos].items);
		edit.setWindowTitle("编辑 - " + QString::fromWCharArray(qis->scripts[pos].name.c_str()));
		edit.exec();

		SaveScript(qis->scripts[pos]);
	}

	void OnBnDel()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		std::wstring name = L"json\\" + qis->scripts[pos].name + L".json";
		qis->scripts.Del(pos);

		File::FileDelete((name.c_str()));

		ui.tbItem->setCurrentItem(0);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
};