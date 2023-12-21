#pragma once
#pragma execution_character_set("utf-8")
#include <qfiledialog.h>
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
		setWindowFlags(Qt::FramelessWindowHint);

		ControlInit();
		ControlEvent();
		LockControl(1);
		TbUpdate();
	}

private:

	Ui::MacroUiClass ui;
	QWidget* main;
	QuickInputStruct* qis;
	List<Script>* scripts;
	QTimer* timer;

	void ControlInit()
	{
		timer = new QTimer(this);

		//Table
		{
			ui.tbItem->setColumnCount(1);
			QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"名称"));
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
		connect(ui.bnExp, SIGNAL(clicked()), this, SLOT(OnBnExp()));
		connect(ui.bnImp, SIGNAL(clicked()), this, SLOT(OnBnImp()));
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
		ui.bnExp->setDisabled(state);
		ui.bnDel->setDisabled(state);
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

		SaveScript(qis->scripts[pos]);
		LoadJson(qis);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnRec()
	{
		qis->scripts.Add();
		RecordUi rw(main, qis, &qis->scripts[qis->scripts.len() - 1].items);
		main->hide();
		qis->HookState(1);
		qis->state = 0;
		qis->rec = &rw;

		std::wstring text = L"按下";
		QKeyEdit ke;
		ke.Mode(2);
		ke.VirtualKey(qis->set.recKey & 0xFFFF, qis->set.recKey >> 16);
		text += ke.Name().toStdWString();
		text += L"开始/停止录制";
		TipsWindow::Show(text, RGB(0x20, 0xFF, 0x20));

		rw.exec();

		main->show();
		qis->HookState(0);

		LoadJson(qis);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnAdd()
	{
		qis->scripts.Add();
		qis->scripts[qis->scripts.len() - 1].name = NameFilter(qis, L"宏");
		qis->scripts[qis->scripts.len() - 1].mode = 0;

		SaveScript(qis->scripts[qis->scripts.len() - 1]);
		LoadJson(qis);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnEdit()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		EditUi edit(qis, &qis->scripts[pos].items);
		edit.setWindowTitle("编辑 - " + QString::fromWCharArray(qis->scripts[pos].name.c_str()));
		edit.exec();

		SaveScript(qis->scripts[pos]);
		LoadJson(qis);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnExp()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		QString path = QDir::toNativeSeparators(QFileDialog::getSaveFileName(this, u8"导出到", QString::fromWCharArray((qis->scripts[pos].name + L".json").c_str()), u8"Quick input macro (*.json)"));
		if (path == "") return;

		std::wstring srcFile = L"json\\" + qis->scripts[pos].name + L".json";
		std::wstring newFile = path.toStdWString();

		CopyFileW(srcFile.c_str(), newFile.c_str(), 0);

		LoadJson(qis);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnImp()
	{
		QString path = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, u8"导入", QString(), u8"Quick input macro (*.json)"));
		if (path == "") return;

		std::wstring srcFile = path.toStdWString();
		std::wstring newFile = L"json\\" + File::FileName(srcFile);

		if (File::FileState(newFile))
		{
			if (MsgBox::Warning(L"文件已存在，是否覆盖？", L"Warning", MB_YESNO | MB_ICONWARNING | MB_TOPMOST) == IDYES)
			{
				CopyFileW(srcFile.c_str(), newFile.c_str(), 0);
			}
		}
		else CopyFileW(srcFile.c_str(), newFile.c_str(), 0);

		LoadJson(qis);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnDel()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		std::wstring name = L"json\\" + qis->scripts[pos].name + L".json";
		qis->scripts.Del(pos);

		File::FileDelete(name.c_str());
		ui.tbItem->setCurrentItem(0);

		LoadJson(qis);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
};