#pragma once
#include "RecordUi.h"
#include "EditUi.h"
#include <src/inc_header.h>
#include "ui_MacroUi.h"
class MacroUi : public QWidget
{
	Q_OBJECT;
	using This = MacroUi;
	enum Event
	{
		e_edit = QEvent::User,
		e_load
	};
	Ui::MacroUiClass ui;
	Macros* macros = &Qi::macros;
	QTimer* timer = nullptr;
public:
	MacroUi(QWidget* parent) : QWidget(parent)
	{
		timer = new QTimer(this);
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		Init();
		Event();
		StyleGroup();
		DisableWidget();
		TableUpdate();
	}
	void StyleGroup()
	{
		ui.content_widget->setProperty("group", "client");
		ui.name_edit->setProperty("group", "line_edit");
		ui.record_button->setProperty("group", "macro-button");
		ui.record_window_button->setProperty("group", "macro-button");
		ui.add_button->setProperty("group", "macro-button");
		ui.edit_button->setProperty("group", "macro-button");
		ui.export_button->setProperty("group", "macro-button");
		ui.import_button->setProperty("group", "macro-button");
		ui.reload_button->setProperty("group", "macro-button");
		ui.delete_button->setProperty("group", "macro-button");
		ui.macro_table->setProperty("group", "table");
		ui.macro_table->horizontalHeader()->setProperty("group", "table_header");
		ui.macro_table->verticalHeader()->setProperty("group", "table_header");
		for (size_t i = 0; i < ui.macro_table->children().size(); i++)
		{
			if (QString(ui.macro_table->children().at(i)->metaObject()->className()) == QString("QTableCornerButton"))
			{
				QWidget* corner = (QWidget*)ui.macro_table->children().at(i);
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
			ui.record_button->installEventFilter(this);
			ui.record_window_button->installEventFilter(this);
			ui.add_button->installEventFilter(this);
			ui.edit_button->installEventFilter(this);
			ui.export_button->installEventFilter(this);
			ui.import_button->installEventFilter(this);
			ui.reload_button->installEventFilter(this);
			ui.delete_button->installEventFilter(this);
		}
		ui.macro_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
	}
	void Event()
	{
		connect(timer, &QTimer::timeout, this, [this] {
			int p = ui.macro_table->currentRow();
			if (p < 0) return;
			timer->stop();
			ui.name_edit->setDisabled(0);
			ui.name_edit->setText(macros->at(p).name);
			});
		connect(ui.macro_table, &QTableWidget::itemSelectionChanged, this, [this] {
			DisableWidget();
			QList<QTableWidgetItem*> items = ui.macro_table->selectedItems();
			if (items.empty()) return;
			if (items.size() == ui.macro_table->columnCount())
			{
				ui.name_edit->setText(macros->at(items.first()->row()).name);
				ui.name_edit->setEnabled(true);
				ui.edit_button->setEnabled(true);
				ui.export_button->setEnabled(true);
				ui.delete_button->setEnabled(true);
			}
			else
			{
				ui.delete_button->setEnabled(true);
			}
			});
		connect(ui.name_edit, &QLineEdit::returnPressed, this, [this] {
			QList<QTableWidgetItem*> items = ui.macro_table->selectedItems();
			if (items.empty() || (items.size() != 1)) return;
			QString name = ui.name_edit->text();
			// not usable
			if (!File::UsableName(name))
			{
				timer->setSingleShot(true);
				timer->start(1000);
				ui.name_edit->setDisabled(true);
				ui.name_edit->setText("名称不可用");
				return;
			}
			// exists
			QString newPath = Qi::macroDir + name + Qi::macroType;
			if (QFile::exists(newPath))
			{
				timer->setSingleShot(true);
				timer->start(1000);
				ui.name_edit->setDisabled(true);
				ui.name_edit->setText("已存在该名称");
				return;
			}
			QString oldPath = Qi::macroDir + macros->at(items.first()->row()).name + Qi::macroType;
			macros->at(items.first()->row()).name = name;
			QFile::rename(oldPath, newPath);
			TableUpdate();
			ResetWidget();
			DisableWidget();
			});
		connect(ui.record_button, &QPushButton::clicked, this, [this] {
			RecStart(false);
			TableUpdate();
			ResetWidget();
			DisableWidget();
			});
		connect(ui.record_window_button, &QPushButton::clicked, this, [this] {
			RecStart(true);
			TableUpdate();
			ResetWidget();
			DisableWidget();
			});
		connect(ui.add_button, &QPushButton::clicked, this, [this] {
			Macro macro;
			macro.mode = Macro::down;
			macro.count = 1;
			macro.name = QiFn::AllocName("宏");
			QiJson::SaveMacro(Qi::macros.append(std::move(macro)));
			TableUpdate();
			ResetWidget();
			DisableWidget();
			});
		connect(ui.edit_button, &QPushButton::clicked, this, [this] {
			QList<QTableWidgetItem*> items = ui.macro_table->selectedItems();
			if (items.empty() || (items.size() != 1)) return;
			Qi::popText->Show("正在加载宏"); QApplication::postEvent(this, new QEvent((QEvent::Type)e_edit));
			});
		connect(ui.export_button, &QPushButton::clicked, this, [this] {
			QList<QTableWidgetItem*> items = ui.macro_table->selectedItems();
			if (items.empty() || (items.size() != 1)) return;
			Qi::widget.dialogActive = true;
			QString des = QFileDialog::getSaveFileName(this, "导出", macros->at(items.first()->row()).name + Qi::macroType, "Quickinput macro (*.json)");
			Qi::widget.dialogActive = false;
			if (des.size())
			{
				if (!QFile::copy(Qi::macroDir + macros->at(items.first()->row()).name + Qi::macroType, des)) MsgBox::Error(L"导出宏失败"), exit(0);
				ResetWidget();
				DisableWidget();
			}
			});
		connect(ui.import_button, &QPushButton::clicked, this, [this] {
			Qi::widget.dialogActive = true;
			QString src = QFileDialog::getOpenFileName(this, "导入", QString(), "Quickinput macro (*.json)");
			Qi::widget.dialogActive = false;
			if (src.size())
			{
				QFileInfo info(src);
				if (!QDir(Qi::macroDir).exists() && !QDir(Qi::macroDir).mkdir(Qi::macroDir)) MsgBox::Error(L"创建宏目录失败");
				if (!QFile::copy(src, Qi::macroDir + File::Unique(Qi::macroDir, File::RemoveExtension(info.baseName()), Qi::macroType))) MsgBox::Error(L"导入宏失败"), exit(0);
				QiJson::LoadMacro();
				TableUpdate();
				ResetWidget();
				DisableWidget();
			}
			});
		connect(ui.reload_button, &QPushButton::clicked, this, [this] { Qi::popText->Show("正在加载宏"); QApplication::postEvent(this, new QEvent((QEvent::Type)e_load)); });
		connect(ui.delete_button, &QPushButton::clicked, this, [this] {
			QList<QTableWidgetItem*> items = ui.macro_table->selectedItems();
			if (items.empty()) return;
			for (QTableWidgetItem*& item : items) if (!QFile::remove(Qi::macroDir + macros->at(item->row()).name + Qi::macroType)) MsgBox::Error(L"删除宏失败");
			QiJson::LoadMacro();
			TableUpdate();
			ResetWidget();
			DisableWidget();
			});
	}
	void ResetWidget()
	{
		ui.name_edit->setText("");
		ui.macro_table->setCurrentItem(0);
	}
	void DisableWidget()
	{
		ui.name_edit->setDisabled(true);
		ui.edit_button->setDisabled(true);
		ui.export_button->setDisabled(true);
		ui.delete_button->setDisabled(true);
	}
	void TableUpdate()
	{
		ui.macro_table->clearMask();
		ui.macro_table->setRowCount(macros->size());
		ui.macro_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.macro_table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.macro_table->verticalHeader()->setDefaultSectionSize(0);
		for (size_t i = 0; i < macros->size(); i++)
		{
			ui.macro_table->setItem(i, 0, new QTableWidgetItem(macros->at(i).name));
			ui.macro_table->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
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
				
				Qi::popText->Popup(2000, "窗口已失效");
				return;
			}
		}
		if (wi.wnd) RecordUi rw(&wi);
		else RecordUi rw(nullptr);
		Qi::widget.dialogActive = false;
		Qi::widget.main->show();
	}
private:
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
		ResetWidget();
		DisableWidget();
		TableUpdate();
	}
	void customEvent(QEvent* e)
	{
		if (e->type() == e_edit)
		{
			int p = ui.macro_table->currentRow(); if (p < 0) return;
			EditUi edit(&macros->at(p), &macros->at(p).acRun);
			Qi::widget.dialogActive = true;
			Qi::widget.main->hide();
			edit.exec();
			Qi::widget.main->show();
			Qi::widget.dialogActive = false;
			QiJson::SaveMacro(macros->at(p));
			Qi::popText->Hide();
			ResetWidget();
			DisableWidget();
		}
		else if (e->type() == e_load)
		{
			QiJson::LoadMacro();
			TableUpdate();
			ResetWidget();
			DisableWidget();
			Qi::popText->Hide();
		}
	}
};