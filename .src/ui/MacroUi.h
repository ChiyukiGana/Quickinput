#pragma once
#include <qfiledialog.h>
#include <qtimer.h>
#include "RecordUi.h"
#include "EditUi.h"
#include "ui_MacroUi.h"
#include "header.h"

class MacroUi : public QWidget
{
	Q_OBJECT;
	Ui::MacroUiClass ui;
	Macros* macros = &qis.macros;
	QTimer* timer = 0;

public:
	bool working = 0;

	MacroUi() {}

	MacroUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
		ReStyle();
	}

	void ReStyle()
	{
		ui.clientWidget->setStyleSheet(qis.themes[qis.set.theme].style);
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
		connect(ui.bnLoad, SIGNAL(clicked()), this, SLOT(OnBnLoad()));
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
		ui.tbActions->setRowCount(macros->size());
		ui.tbActions->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.tbActions->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tbActions->verticalHeader()->setDefaultSectionSize(0);

		for (size_t i = 0; i < macros->size(); i++) {
			ui.tbActions->setItem(i, 0, new QTableWidgetItem(QString::fromWCharArray(macros->at(i).name.c_str())));
			ui.tbActions->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}

		ui.tbActions->setStyleSheet(u8"QHeaderView::section,QScrollBar{background:transparent}");
	}

	void RecStart(bool wnd)
	{
		working = true;
		qis.widget.main->hide();
		WndInfo wi;
		if (wnd)
		{
			wi = QiFn::WindowSelection();
			if (!wi.wnd)
			{
				MsgPop::Popup(L"窗口已失效", RGB(255, 64, 64), 2000);
				return;
			}
		}
		
		if (wi.wnd) RecordUi rw(&wi);
		else RecordUi rw(nullptr);

		working = false;
		qis.widget.main->show();
	}

	void showEvent(QShowEvent*)
	{
		ResetControl();
		LockControl(true);
		TbUpdate();
	}

private slots:
	void OnTimeOut()
	{
		int p = ui.tbActions->currentRow();
		if (p < 0) return;
		timer->stop();
		ui.etName->setDisabled(0);
		ui.etName->setText(QString::fromWCharArray(macros->at(p).name.c_str()));
	}

	void OnTbClicked(int row, int column)
	{
		LockControl(1);
		if (row < 0) return;

		ui.etName->setText(QString::fromWCharArray(macros->at(row).name.c_str()));

		LockControl(0);
	}

	void OnEtReturn()
	{
		int p = ui.tbActions->currentRow();
		if (p < 0) return;

		std::wstring name = (LPCWSTR)ui.etName->text().utf16();

		if (!File::FileNameUsable(name))
		{
			timer->setSingleShot(1);
			timer->start(1000);

			ui.etName->setDisabled(1);
			ui.etName->setText(u8"名称不可用");
			return;
		}

		std::wstring newPath(MacroPath);
		newPath += name;
		newPath += MacroType;

		if (File::FileState(newPath.c_str()))
		{
			timer->setSingleShot(1);
			timer->start(1000);

			ui.etName->setDisabled(1);
			ui.etName->setText(u8"已存在该名称");
			return;
		}

		std::wstring oldPath(MacroPath);
		oldPath += macros->at(p).name;
		oldPath += MacroType;

		macros->at(p).name = name;

		File::Rename(oldPath.c_str(), newPath.c_str());

		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnRec()
	{
		RecStart(false);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
	void OnBnWndRec()
	{
		RecStart(true);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
	void OnBnAdd()
	{
		Macro macro;
		macro.name = QiFn::NameFilter(L"宏");
		macro.mode = Macro::down;
		macro.count = 1;
		qis.macros.Add(macro);
		QiFn::SaveMacro(macro);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
	void OnBnEdit()
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		EditParam ep;
		ep.macro = &macros->at(p);
		ep.actions = &macros->at(p).acRun;
		ep.child = false;
		EditUi edit(ep);
		edit.setWindowTitle(u8"编辑 - " + QString::fromWCharArray(macros->at(p).name.c_str()));

		working = true;
		qis.widget.main->hide();
		edit.exec();
		qis.widget.main->show();
		working = false;

		QiFn::SaveMacro(macros->at(p));
		ResetControl();
		LockControl(1);
	}
	void OnBnExp()
	{
		int p = ui.tbActions->currentRow();
		if (p < 0) return;

		QString path = QDir::toNativeSeparators(QFileDialog::getSaveFileName(this, u8"导出到", QString::fromWCharArray((macros->at(p).name + MacroType).c_str()), u8"Quick input macro (*.json)"));
		if (!path.length()) return;

		std::wstring srcFile(MacroPath);
		srcFile += macros->at(p).name;
		srcFile += MacroType;
		std::wstring newFile = (LPCWSTR)path.utf16();

		CopyFileW(srcFile.c_str(), newFile.c_str(), 0);

		ResetControl();
		LockControl(1);
	}
	void OnBnImp()
	{
		QString path = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, u8"导入", QString(), u8"Quick input macro (*.json)"));
		if (path == "") return;

		std::wstring srcFile = (LPCWSTR)path.utf16();
		std::wstring newFile(MacroPath);
		newFile += Path::Last(srcFile);

		File::FolderCreate(MacroPath);
		if (File::FileState(newFile))
		{
			if (MsgBox::Warning(L"文件已存在，是否覆盖？", L"Warning", MB_YESNO | MB_ICONWARNING | MB_TOPMOST) == IDYES) CopyFileW(srcFile.c_str(), newFile.c_str(), 0);
		}
		else CopyFileW(srcFile.c_str(), newFile.c_str(), 0);

		QiFn::LoadMacro();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
	void OnBnLoad()
	{
		QiFn::LoadMacro();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
	void OnBnDel()
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;

		std::wstring path(MacroPath);
		path += macros->at(p).name;
		path += MacroType;
		macros->Del(p);

		File::FileDelete(path.c_str());
		ui.tbActions->setCurrentItem(0);

		TbUpdate();
		ResetControl();
		LockControl(1);
	}
};