#pragma once
#include <qfiledialog.h>
#include <qtimer.h>
#include "RecordUi.h"
#include "EditUi.h"
#include "ui_MacroUi.h"
#include "../static.h"

class MacroUi : public QWidget
{
	Q_OBJECT;
	Ui::MacroUiClass ui;
	Macros& macros = Global::qi.macros;
	QTimer* timer;

public:
	bool working = 0;

	MacroUi() {}

	MacroUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
	}

private:
	void WidInit()
	{
		timer = new QTimer(this);

		// Table
		{
			
			ui.tbActions->setColumnCount(1);
			QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"名称"));
			ui.tbActions->setHorizontalHeaderItem(0, tbi);
			ui.tbActions->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
		}
		LockControl(1);
		TbUpdate();
	}
	void WidEvent()
	{
		connect(timer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
		connect(ui.tbActions, SIGNAL(cellClicked(int, int)), this, SLOT(OnTbClicked(int, int)));
		connect(ui.etName, SIGNAL(returnPressed()), this, SLOT(OnEtReturn()));
		connect(ui.bnRec, SIGNAL(clicked()), this, SLOT(OnBnRec()));
		connect(ui.bnWndRec, SIGNAL(clicked()), this, SLOT(OnBnWndRec()));
		connect(ui.bnAdd, SIGNAL(clicked()), this, SLOT(OnBnAdd()));
		connect(ui.bnEdit, SIGNAL(clicked()), this, SLOT(OnBnEdit()));
		connect(ui.bnExp, SIGNAL(clicked()), this, SLOT(OnBnExp()));
		connect(ui.bnImp, SIGNAL(clicked()), this, SLOT(OnBnImp()));
		connect(ui.bnDel, SIGNAL(clicked()), this, SLOT(OnBnDel()));
	}

	void ResetControl()
	{
		ui.etName->setText("");
		ui.tbActions->setCurrentItem(0);
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
		ui.tbActions->clearMask();
		ui.tbActions->setRowCount(macros.size());
		ui.tbActions->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.tbActions->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tbActions->verticalHeader()->setDefaultSectionSize(0);

		for (uint32 u = 0; u < macros.size(); u++) {
			ui.tbActions->setItem(u, 0, new QTableWidgetItem(QString::fromWCharArray(macros[u].name.c_str())));
			ui.tbActions->item(u, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
	}

	void RecStart(bool wnd)
	{
		RecordUi rw;
		Global::qi.rec = &rw;
		Global::qi.state = 0, Global::qi.run = 0;

		HookState(true);
		working = true;
		Global::qi.main->hide();
		if (wnd)
		{
			WndInfo wi = WindowSelection();
			if (wi.wnd) rw.Start(&wi);
		}
		else rw.Start(0);
		HookState(false);
		working = false;
		Global::qi.main->show();
	}

	void showEvent(QShowEvent*)
	{
		ResetControl();
		LockControl(1);
		TbUpdate();
	}

private slots:
	void OnTimeOut()
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;
		timer->stop();
		ui.etName->setDisabled(0);
		ui.etName->setText(QString::fromWCharArray(macros[pos].name.c_str()));
	}

	void OnTbClicked(int row, int column)
	{
		LockControl(1);
		if (row < 0) return;

		ui.etName->setText(QString::fromWCharArray(macros[row].name.c_str()));

		LockControl(0);
	}

	void OnEtReturn()
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;

		std::wstring name = (LPCWSTR)ui.etName->text().utf16();

		if (!File::NameFilter(name).length())
		{
			timer->setSingleShot(1);
			timer->start(1000);

			ui.etName->setDisabled(1);
			ui.etName->setText("名称不可用");
			return;
		}

		std::wstring newPath(Global::qi.path);
		newPath += name;
		newPath += L".json";

		if (File::FileState(newPath.c_str()))
		{
			timer->setSingleShot(1);
			timer->start(1000);

			ui.etName->setDisabled(1);
			ui.etName->setText("已存在该名称");
			return;
		}

		std::wstring oldPath(Global::qi.path);
		oldPath += macros[pos].name;
		oldPath += L".json";

		macros[pos].name = name;

		File::Rename(oldPath.c_str(), newPath.c_str());

		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnRec()
	{
		RecStart(false);

		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
	void OnBnWndRec()
	{
		RecStart(true);

		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
	void OnBnAdd()
	{
		macros.AddNull();
		macros.Get().name = NameFilter(L"宏");
		macros.Get().mode = Macro::down;
		macros.Get().count = 1;

		SaveMacro(macros.Get());
		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
	void OnBnEdit()
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;

		EditParam ep;
		ep.macro = &macros[pos];
		ep.actions = &macros[pos].actions;
		ep.firstLayer = true;
		ep.prevState = false;
		EditUi edit(ep);
		edit.setWindowTitle(u8"编辑 - " + QString::fromWCharArray(macros[pos].name.c_str()));

		working = 1;
		edit.exec();
		working = 0;

		SaveMacro(macros[pos]);
		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
	void OnBnExp()
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;

		QString path = QDir::toNativeSeparators(QFileDialog::getSaveFileName(this, u8"导出到", QString::fromWCharArray((macros[pos].name + L".json").c_str()), u8"Quick input macro (*.json)"));
		if (!path.length()) return;

		std::wstring srcFile(Global::qi.path);
		srcFile += macros[pos].name;
		srcFile += L".json";
		std::wstring newFile = (LPCWSTR)path.utf16();

		CopyFileW(srcFile.c_str(), newFile.c_str(), 0);

		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
	void OnBnImp()
	{
		QString path = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, u8"导入", QString(), u8"Quick input macro (*.json)"));
		if (path == "") return;

		std::wstring srcFile = (LPCWSTR)path.utf16();
		std::wstring newFile(Global::qi.path);
		newFile += Path::Last(srcFile);

		File::FolderCreate(Global::qi.path);
		if (File::FileState(newFile))
		{
			if (MsgBox::Warning(L"文件已存在，是否覆盖？", L"Warning", MB_YESNO | MB_ICONWARNING | MB_TOPMOST) == IDYES) CopyFileW(srcFile.c_str(), newFile.c_str(), 0);
		}
		else CopyFileW(srcFile.c_str(), newFile.c_str(), 0);

		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
	void OnBnDel()
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;

		std::wstring path(Global::qi.path);
		path += macros[pos].name;
		path += L".json";
		macros.Del(pos);

		File::FileDelete(path.c_str());
		ui.tbActions->setCurrentItem(0);

		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
};