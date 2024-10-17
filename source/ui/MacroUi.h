#pragma execution_character_set("utf-8")
#pragma once
#include <qfiledialog.h>
#include <qtimer.h>
#include "RecordUi.h"
#include "EditUi.h"
#include "../src/minc.h"
#include "ui_MacroUi.h"

class MacroUi : public QWidget
{
	Q_OBJECT;
	Ui::MacroUiClass ui;
	Macros* macros = &qis.macros;
	QTimer* timer = nullptr;
public:
	MacroUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		Init();
		Event();
	}
	void StyleGroup()
	{
		ui.clientWidget->setProperty("group", QVariant(QString::fromUtf8("client")));
		ui.bnRec->setProperty("group", QVariant(QString::fromUtf8("macro-button")));
		ui.bnWndRec->setProperty("group", QVariant(QString::fromUtf8("macro-button")));
		ui.bnAdd->setProperty("group", QVariant(QString::fromUtf8("macro-button")));
		ui.bnEdit->setProperty("group", QVariant(QString::fromUtf8("macro-button")));
		ui.bnExp->setProperty("group", QVariant(QString::fromUtf8("macro-button")));
		ui.bnImp->setProperty("group", QVariant(QString::fromUtf8("macro-button")));
		ui.bnLoad->setProperty("group", QVariant(QString::fromUtf8("macro-button")));
		ui.bnDel->setProperty("group", QVariant(QString::fromUtf8("macro-button")));

		ui.etName->setProperty("group", QVariant(QString::fromUtf8("line_edit")));

		ui.tbActions->setProperty("group", QVariant(QString::fromUtf8("table")));
		ui.tbActions->horizontalHeader()->setProperty("group", QVariant(QString::fromUtf8("table_header")));
		ui.tbActions->verticalHeader()->setProperty("group", QVariant(QString::fromUtf8("table_header")));
		for (size_t i = 0; i < ui.tbActions->children().size(); i++)
		{
			if (!String::Compare(ui.tbActions->children().at(i)->metaObject()->className(), "QTableCornerButton"))
			{
				QWidget* corner = (QWidget*)ui.tbActions->children().at(i);
				QHBoxLayout* box = new QHBoxLayout(corner);
				box->setMargin(0);
				QWidget* widget = new QWidget(corner);
				box->addWidget(widget);
				widget->setProperty("group", QVariant(QString::fromUtf8("table_header")));
				break;
			}
		}
	}

private:
	void Init()
	{
		StyleGroup();
		{
			ui.bnRec->setShortcut(Qt::Key_unknown);
			ui.bnWndRec->setShortcut(Qt::Key_unknown);
			ui.bnAdd->setShortcut(Qt::Key_unknown);
			ui.bnEdit->setShortcut(Qt::Key_unknown);
			ui.bnExp->setShortcut(Qt::Key_unknown);
			ui.bnImp->setShortcut(Qt::Key_unknown);
			ui.bnLoad->setShortcut(Qt::Key_unknown);
			ui.bnDel->setShortcut(Qt::Key_unknown);
		}

		timer = new QTimer(this);
		ui.tbActions->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
		LockControl(true);
		TableUpdate();
	}
	void Event()
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
		}

		ui.tbActions->setStyleSheet("QHeaderView::section,QScrollBar{background:transparent}");
	}
	void RecStart(bool wnd)
	{
		qis.widget.dialogActive = true;
		qis.widget.main->hide();
		WndInfo wi;
		if (wnd)
		{
			wi = QiFn::WindowSelection();
			if (!wi.wnd)
			{
				
				qis.popText->Popup("窗口已失效", RGB(255, 64, 64), 2000);
				return;
			}
		}
		
		if (wi.wnd) RecordUi rw(&wi);
		else RecordUi rw(nullptr);

		qis.widget.dialogActive = false;
		qis.widget.main->show();
	}
	void showEvent(QShowEvent*)
	{
		ResetControl();
		LockControl(true);
		TableUpdate();
	}
private Q_SLOTS:
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

		LockControl(false);
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
			ui.etName->setText("名称不可用");
			return;
		}

		std::wstring newPath = macroPath + name + macroType;

		if (File::FileState(newPath))
		{
			timer->setSingleShot(true);
			timer->start(1000);

			ui.etName->setDisabled(true);
			ui.etName->setText("已存在该名称");
			return;
		}

		std::wstring oldPath = macroPath + macros->at(p).name + macroType;

		macros->at(p).name = name;

		File::Rename(oldPath.c_str(), newPath.c_str());

		TableUpdate();
		ResetControl();
		LockControl(true);
	}
	void OnBnRec()
	{
		RecStart(false);
		TableUpdate();
		ResetControl();
		LockControl(true);
	}
	void OnBnWndRec()
	{
		RecStart(true);
		TableUpdate();
		ResetControl();
		LockControl(true);
	}
	void OnBnAdd()
	{
		Macro macro;
		macro.mode = Macro::down;
		macro.count = 1;
		macro.name = QiFn::AllocName(L"宏");

		qis.macros.Add(macro);
		QiJson::SaveMacro(macro);
		TableUpdate();
		ResetControl();
		LockControl(true);
	}
	void OnBnEdit()
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		EditParam ep;
		ep.macro = &macros->at(p);
		ep.actions = &macros->at(p).acRun;
		ep.child = false;
		EditUi edit(ep);
		edit.setWindowTitle("编辑 - " + QString::fromWCharArray(macros->at(p).name.c_str()));

		qis.widget.dialogActive = true;
		qis.widget.main->hide();
		edit.exec();
		qis.widget.main->show();
		qis.widget.dialogActive = false;

		QiJson::SaveMacro(macros->at(p));
		ResetControl();
		LockControl(true);
	}
	void OnBnExp()
	{
		int p = ui.tbActions->currentRow();
		if (p < 0) return;
		qis.widget.dialogActive = true;
		QString path = QDir::toNativeSeparators(QFileDialog::getSaveFileName(this, "导出", QString::fromWCharArray((macros->at(p).name + macroType).c_str()), "Quick input macro (*.json)"));
		qis.widget.dialogActive = false;
		if (path.size())
		{
			CopyFileW((macroPath + macros->at(p).name + macroType).c_str(), (wchar_t*)path.utf16(), 0);
			ResetControl();
			LockControl(true);
		}
	}
	void OnBnImp()
	{
		qis.widget.dialogActive = true;
		QString path = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "导入", QString(), "Quick input macro (*.json)"));
		qis.widget.dialogActive = false;
		if (path.size())
		{
			std::wstring file = (wchar_t*)path.utf16();
			File::FolderCreate(macroPath);
			CopyFileW(file.c_str(), File::FileUnique(macroPath + Path::GetFile(file)).c_str(), 0);
			QiJson::LoadMacro();
			TableUpdate();
			ResetControl();
			LockControl(true);
		}
	}
	void OnBnLoad()
	{
		QiJson::LoadMacro();
		TableUpdate();
		ResetControl();
		LockControl(true);
	}
	void OnBnDel()
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		File::FileDelete(macroPath + macros->at(p).name + macroType);
		ui.tbActions->setCurrentItem(0);
		QiJson::LoadMacro();
		TableUpdate();
		ResetControl();
		LockControl(true);
	}
};