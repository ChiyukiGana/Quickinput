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
	using This = MacroUi;
	Ui::MacroUiClass ui;
	Macros* macros = &Qi::macros;
	QTimer* timer = nullptr;
public:
	MacroUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		Init();
		Event();
		StyleGroup();
	}
	void StyleGroup()
	{
		ui.clientWidget->setProperty("group", "client");
		ui.bnRec->setProperty("group", "macro-button");
		ui.bnWndRec->setProperty("group", "macro-button");
		ui.bnAdd->setProperty("group", "macro-button");
		ui.bnEdit->setProperty("group", "macro-button");
		ui.bnExp->setProperty("group", "macro-button");
		ui.bnImp->setProperty("group", "macro-button");
		ui.bnLoad->setProperty("group", "macro-button");
		ui.bnDel->setProperty("group", "macro-button");

		ui.etName->setProperty("group", "line_edit");

		ui.tbActions->setProperty("group", "table");
		ui.tbActions->horizontalHeader()->setProperty("group", "table_header");
		ui.tbActions->verticalHeader()->setProperty("group", "table_header");
		for (size_t i = 0; i < ui.tbActions->children().size(); i++)
		{
			if (!String::Compare(ui.tbActions->children().at(i)->metaObject()->className(), "QTableCornerButton"))
			{
				QWidget* corner = (QWidget*)ui.tbActions->children().at(i);
				QHBoxLayout* box = new QHBoxLayout(corner);
				box->setMargin(0);
				QWidget* widget = new QWidget(corner);
				box->addWidget(widget);
				widget->setProperty("group", "table_header");
				break;
			}
		}
	}

private:
	void Init()
	{
		if ("clear shortcut")
		{
			ui.bnRec->installEventFilter(this);
			ui.bnWndRec->installEventFilter(this);
			ui.bnAdd->installEventFilter(this);
			ui.bnEdit->installEventFilter(this);
			ui.bnExp->installEventFilter(this);
			ui.bnImp->installEventFilter(this);
			ui.bnLoad->installEventFilter(this);
			ui.bnDel->installEventFilter(this);
		}

		timer = new QTimer(this);
		ui.tbActions->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
		LockControl(true);
		TableUpdate();
	}
	void Event()
	{
		connect(timer, &QTimer::timeout, this, &This::OnTimeOut);
		connect(ui.tbActions, &QTableWidget::cellClicked, this, &This::OnTbClicked);
		connect(ui.etName, &QLineEdit::returnPressed, this, &This::OnEtReturn);
		connect(ui.bnRec, &QPushButton::clicked, this, &This::OnBnRec);
		connect(ui.bnWndRec, &QPushButton::clicked, this, &This::OnBnWndRec);
		connect(ui.bnAdd, &QPushButton::clicked, this, &This::OnBnAdd);
		connect(ui.bnEdit, &QPushButton::clicked, this, &This::OnBnEdit);
		connect(ui.bnExp, &QPushButton::clicked, this, &This::OnBnExp);
		connect(ui.bnImp, &QPushButton::clicked, this, &This::OnBnImp);
		connect(ui.bnLoad, &QPushButton::clicked, this, &This::OnBnLoad);
		connect(ui.bnDel, &QPushButton::clicked, this, &This::OnBnDel);
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
			ui.tbActions->setItem(i, 0, new QTableWidgetItem(macros->at(i).name));
			ui.tbActions->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
	}
	void RecStart(bool wnd)
	{
		Qi::widget.dialogActive = true;
		Qi::widget.main->hide();
		WndInfo wi;
		if (wnd)
		{
			wi = QiFn::WindowSelection();
			if (!wi.wnd)
			{
				
				Qi::popText->Popup(2000, "窗口已失效", RGB(255, 64, 64));
				return;
			}
		}
		
		if (wi.wnd) RecordUi rw(&wi);
		else RecordUi rw(nullptr);

		Qi::widget.dialogActive = false;
		Qi::widget.main->show();
	}

	bool event(QEvent* e)
	{
		if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
		{
			QKeyEvent* keyEvent = (QKeyEvent*)e;
			if ((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Space)) return true;
		}
		return QWidget::event(e);
	}
	bool eventFilter(QObject* obj, QEvent* e)
	{
		if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease)) return true;
		return QWidget::eventFilter(obj, e);
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
		ui.etName->setText(macros->at(p).name);
	}
	void OnTbClicked(int row, int column)
	{
		LockControl(1);
		if (row < 0) return;

		ui.etName->setText(macros->at(row).name);

		LockControl(false);
	}
	void OnEtReturn()
	{
		int p = ui.tbActions->currentRow();
		if (p < 0) return;

		QString mname = ui.etName->text();
		std::wstring name = (LPCWSTR)ui.etName->text().utf16();

		if (!File::FileNameUsable(name))
		{
			timer->setSingleShot(1);
			timer->start(1000);

			ui.etName->setDisabled(1);
			ui.etName->setText("名称不可用");
			return;
		}

		QString mnewPath = macroPath + mname + macroType;
		std::wstring newPath = QStringToW(mnewPath);

		if (File::FileState(newPath))
		{
			timer->setSingleShot(true);
			timer->start(1000);

			ui.etName->setDisabled(true);
			ui.etName->setText("已存在该名称");
			return;
		}

		QString moldPath = macroPath + macros->at(p).name + macroType;
		std::wstring oldPath = QStringToW(moldPath);

		macros->at(p).name = mname;

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
		macro.name = WToQString(QiFn::AllocName(L"宏"));

		Qi::macros.Add(macro);
		QiJson::SaveMacro(macro);
		TableUpdate();
		ResetControl();
		LockControl(true);
	}
	void OnBnEdit()
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		EditUi edit(&macros->at(p), &macros->at(p).acRun);

		Qi::widget.dialogActive = true;
		Qi::widget.main->hide();
		edit.exec();
		Qi::widget.main->show();
		Qi::widget.dialogActive = false;

		QiJson::SaveMacro(macros->at(p));
		ResetControl();
		LockControl(true);
	}
	void OnBnExp()
	{
		int p = ui.tbActions->currentRow();
		if (p < 0) return;
		Qi::widget.dialogActive = true;
		QString path = QDir::toNativeSeparators(QFileDialog::getSaveFileName(this, "导出", macros->at(p).name + macroType, "Quick input macro (*.json)"));
		Qi::widget.dialogActive = false;
		if (path.size())
		{
			CopyFileW((const wchar_t*)(macroPath + macros->at(p).name + macroType).utf16(), (const wchar_t*)path.utf16(), 0);
			ResetControl();
			LockControl(true);
		}
	}
	void OnBnImp()
	{
		Qi::widget.dialogActive = true;
		QString path = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "导入", QString(), "Quick input macro (*.json)"));
		Qi::widget.dialogActive = false;
		if (path.size())
		{
			std::wstring file = (wchar_t*)path.utf16();
			File::FolderCreate(QStringToW(macroPath));
			CopyFileW(file.c_str(), File::FileUnique(std::wstring(QStringToW(macroPath)) + Path::GetFile(file)).c_str(), 0);
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
		File::FileDelete(QStringToW(macroPath + macros->at(p).name + macroType));
		ui.tbActions->setCurrentItem(0);
		QiJson::LoadMacro();
		TableUpdate();
		ResetControl();
		LockControl(true);
	}
};