#pragma once
#include <QEditDialog.h>
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
	MacroGroups* groups = &Qi::macroGroups;
	MacroGroup* currentGroup = &groups->front();
	QList<Macro*> currentMacros;
	bool updating = false;
public:
	MacroUi(QWidget* parent) : QWidget(parent)
	{
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
		ui.record_button->setProperty("group", "macro-button");
		ui.record_window_button->setProperty("group", "macro-button");
		ui.add_button->setProperty("group", "macro-button");
		ui.edit_button->setProperty("group", "macro-button");
		ui.export_button->setProperty("group", "macro-button");
		ui.import_button->setProperty("group", "macro-button");
		ui.reload_button->setProperty("group", "macro-button");
		ui.add_group_button->setProperty("group", "macro-button");
		ui.delete_group_button->setProperty("group", "macro-button");
		ui.delete_button->setProperty("group", "macro-button");
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
			ui.add_group_button->installEventFilter(this);
			ui.delete_group_button->installEventFilter(this);
			ui.delete_button->installEventFilter(this);
		}
		ui.macroGroup_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
		ui.macroGroup_table->horizontalHeader()->setHidden(true);
		ui.macroGroup_table->verticalHeader()->setHidden(true);
		ui.macroGroup_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.macroGroup_table->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
		ui.macroGroup_table->setShowGrid(false);
		ui.macroGroup_table->setDropIndicatorShown(false);
		ui.macroGroup_table->setAutoScroll(false);
		ui.macroGroup_table->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked);
		ui.macroGroup_table->setStyleSheet("QHeaderView::section,QScrollBar{background:transparent}");
	}
	void Event()
	{
		std::function currentChanged = [this](int table_index) {
			currentMacros.clear();
			currentGroup = &groups->front();
			if (table_index < 0) return;
			currentGroup = &groups->at(table_index);
			for (auto& i : ui.macroGroup_table->currentIndex(table_index)) currentMacros.append(&currentGroup->macros[i]);

			DisableWidget();
			if (isSold())
			{
				ui.edit_button->setEnabled(true);
				ui.export_button->setEnabled(true);
				ui.delete_button->setEnabled(true);
			}
			else if (isMult())
			{
				ui.delete_button->setEnabled(true);
			}
			if (!currentGroup->base) ui.delete_group_button->setEnabled(true);
			};
		connect(ui.macroGroup_table, &QMacroTable::currentChanged, this, currentChanged);
		connect(ui.macroGroup_table, &QMacroTable::itemClicked, this, currentChanged);
		connect(ui.macroGroup_table, &QMacroTable::headerClicked, this, currentChanged);
		connect(ui.macroGroup_table, &QMacroTable::headerDoubleClicked, this, [this](int index, int column) {
			if (index <= 0) return;
			QTableWidget* table = ui.macroGroup_table->table(index);
			if (!table) return;
			currentGroup = &groups->at(index);
			QEditDialog edit;
			edit.edit()->setProperty("group", "line_edit");
			QPoint pos = table->mapToGlobal(QPoint(0, 0));
			edit.setGeometry(pos.x(), pos.y(), table->width(), table->horizontalHeader()->height());
			edit.raise();

			QString oldPath = currentGroup->makePath();
			Qi::widget.dialogActive = true;
			QString name = edit.Start(currentGroup->name);
			Qi::widget.dialogActive = false;
			if (name != currentGroup->name && !name.isEmpty())
			{
				currentGroup->name = groups->makeName(name);
				QFile::rename(oldPath, currentGroup->makePath());
				TableUpdate();
			}
			});
		connect(ui.macroGroup_table, &QMacroTable::itemChanged, this, [this](int index, int row, int column) {
			if (updating || index < 0 || row < 0) return;
			QTableWidget* table = ui.macroGroup_table->table(index);
			if (!table) return;
			MacroGroup& group = groups->at(index);
			Macro& macro = groups->at(index).macros[row];

			QTableWidgetItem* item = table->item(row, column);
			QString name = item->text();
			if (name == macro.name) return;
			if (!File::UsableName(name))
			{
				item->setText(macro.name);
				return;
			}
			QString oldPath = macro.makePath();
			macro.name = group.makeName(table->item(row, column)->text());
			QFile::rename(oldPath, macro.makePath());

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
			if (!currentGroup) return;
			Macro macro;
			macro.mode = Macro::down;
			macro.count = 1;
			macro.name = currentGroup->makeName("宏");
			macro.groupName = currentGroup->name;
			macro.groupBase = currentGroup->base;
			QiJson::SaveMacro(currentGroup->macros.append(std::move(macro)));
			TableUpdate();
			ResetWidget();
			DisableWidget();
			});
		connect(ui.edit_button, &QPushButton::clicked, this, [this] {
			if (isSold()) Qi::popText->Show("正在加载宏"); QApplication::postEvent(this, new QEvent((QEvent::Type)e_edit));
			});
		connect(ui.export_button, &QPushButton::clicked, this, [this] {
			if (!isSold()) return;
			Qi::widget.dialogActive = true;
			Macro& macro = *currentMacros.first();
			QString des = QFileDialog::getSaveFileName(this, "导出", macro.name + Qi::macroType, QString("Quickinput macro (*") + Qi::macroType + QString(")"));
			if (des.size())
			{
				if (QFile::remove(des));
				if (!QFile::copy(macro.makePath(), des))
				{
					MsgBox::Error(L"导出宏失败");
					return;
				}
				ResetWidget();
				DisableWidget();
			}
			Qi::widget.dialogActive = false;
			});
		connect(ui.import_button, &QPushButton::clicked, this, [this] {
			if (!currentGroup) return;
			Qi::widget.dialogActive = true;
			QString src = QFileDialog::getOpenFileName(this, "导入", QString(), QString("Quickinput macro (*") + Qi::macroType + QString(")"));
			if (src.size())
			{
				QFileInfo info(src);
				if (!QDir(Qi::macroDir).exists() && !QDir(Qi::macroDir).mkdir(Qi::macroDir)) MsgBox::Error(L"创建宏目录失败");
				if (!QFile::copy(src, currentGroup->makePath(info.baseName())))
				{
					MsgBox::Error(L"导入宏失败");
					return;
				}
				QiJson::LoadMacro();
				TableUpdate();
				ResetWidget();
				DisableWidget();
			}
			Qi::widget.dialogActive = false;
			});
		connect(ui.reload_button, &QPushButton::clicked, this, [this] { Qi::popText->Show("正在加载宏"); QApplication::postEvent(this, new QEvent((QEvent::Type)e_load)); });
		connect(ui.add_group_button, &QPushButton::clicked, this, [this] {
			QString path = Qi::macroDir + Qi::macroGroups.append(MacroGroup(false, Qi::macroGroups.makeName())).name;

			if (!QDir(Qi::macroDir).exists() && !QDir(Qi::macroDir).mkdir(Qi::macroDir))
			{
				MsgBox::Error(L"创建宏目录失败");
				return;
			}

			if (!QDir(path).exists() && !QDir(path).mkdir(path)) MsgBox::Error(L"创建分组目录失败");
			currentGroup = &groups->front();
			TableUpdate();
			ResetWidget();
			DisableWidget();
			});
		connect(ui.delete_group_button, &QPushButton::clicked, this, [this] {
			if (!currentGroup) return;
			if (currentGroup->macros.not_empty() && MsgBox::Warning(L"确认删除分组的全部宏？", L"Warning", MB_ICONWARNING | MB_YESNO) != IDYES) return;
			QDir dir(currentGroup->makePath());
			dir.removeRecursively();
			QiJson::LoadMacro();
			currentGroup = &groups->front();
			TableUpdate();
			ResetWidget();
			DisableWidget();
			});
		connect(ui.delete_button, &QPushButton::clicked, this, [this] {
			if (!isMult()) return;
			for (auto& i : currentMacros) if (!QFile::remove(i->makePath())) MsgBox::Error(L"删除宏失败");

			QiJson::LoadMacro();
			TableUpdate();
			ResetWidget();
			DisableWidget();
			});
	}
	void ResetWidget()
	{
		ui.macroGroup_table->clearSelection();
	}
	void DisableWidget()
	{
		ui.edit_button->setDisabled(true);
		ui.export_button->setDisabled(true);
		ui.delete_group_button->setDisabled(true);
		ui.delete_button->setDisabled(true);
	}
	void TableUpdate()
	{
		updating = true;
		currentMacros.clear();
		if (ui.macroGroup_table->rowCount() != groups->size()) ui.macroGroup_table->setTableCount(groups->size());

		for (size_t mgPos = 0; mgPos < groups->size(); mgPos++)
		{
			MacroGroup& mg = groups->at(mgPos);
			QTableWidget* table = ui.macroGroup_table->table(mgPos);
			table->setColumnCount(1);
			table->setRowCount(mg.macros.size());
			table->setDragEnabled(true);
			table->setDragDropMode(QAbstractItemView::DragDrop);
			table->setDefaultDropAction(Qt::DropAction::IgnoreAction);
			table->setHorizontalHeaderItem(0, new QTableWidgetItem(mg.name));
			table->viewport()->installEventFilter(this);
			table->viewport()->setProperty("groupIndex", mgPos);
			for (size_t mPos = 0; mPos < mg.macros.size(); mPos++)
			{
				Macro& m = mg.macros[mPos];
				table->setItem(mPos, 0, new QTableWidgetItem(m.name));
				table->item(mPos, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				table->item(mPos, 0)->setData(DataRole::group, mgPos);
				table->item(mPos, 0)->setData(DataRole::macro, mPos);
			}
		}
		updating = false;
	}
	void RecStart(bool wnd)
	{
		if (!currentGroup) return;
		Qi::widget.dialogActive = true;
		Qi::widget.main->hide();
		WndInfo wndInfo;
		if (wnd)
		{
			wndInfo = QiFn::WindowSelection();
			if (!wndInfo.wnd)
			{
				Qi::popText->Popup(2000, "窗口已失效");
				return;
			}
		}
		if (wndInfo.wnd) RecordUi rw(currentGroup, &wndInfo);
		else RecordUi rw(currentGroup, nullptr);
		Qi::widget.dialogActive = false;
		Qi::widget.main->show();
	}
	bool isSold() const { return (currentGroup && currentMacros.size() == 1); }
	bool isMult() const { return (currentGroup && currentMacros.size()); }
private:
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
			if (!isSold()) return;
			Macro* macro = currentMacros.first();
			EditUi edit(macro, &macro->acRun);
			Qi::widget.edit = &edit;
			Qi::widget.dialogActive = Qi::debug = true;
			Qi::widget.main->hide();
			edit.exec();
			Qi::widget.main->show();
			Qi::widget.dialogActive = Qi::debug = false;
			Qi::widget.edit = nullptr;
			QiJson::SaveMacro(*macro);
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
	bool eventFilter(QObject* sender, QEvent* event)
	{
		const QString mime = "application/x-qabstractitemmodeldatalist";
		if (event->type() == QEvent::DragEnter)
		{
			QDropEvent* dropEvent = (QDropEvent*)event;
			if (dropEvent->mimeData()->hasFormat(mime)) dropEvent->acceptProposedAction();
			else return true;
		}
		else if (event->type() == QEvent::Drop)
		{
			QDropEvent* dropEvent = (QDropEvent*)event;
			if (dropEvent->mimeData()->hasFormat(mime))
			{
				QStandardItemModel model;
				model.dropMimeData(dropEvent->mimeData(), Qt::CopyAction, 0, 0, QModelIndex());
				for (size_t i = 0; i < model.rowCount(); i++)
				{
					QStandardItem* item = model.item(i);
					int currentGroupIndex = sender->property("groupIndex").toInt();
					int groupIndex = item->data(DataRole::group).toInt();
					int macroIndex = item->data(DataRole::macro).toInt();
					if (currentGroupIndex >= groups->size() || groupIndex >= groups->size() || currentGroupIndex == groupIndex) break;

					const MacroGroup& group = groups->at(currentGroupIndex);
					const Macros& macros = groups->at(groupIndex).macros;
					if (macroIndex >= macros.size()) break;
					const Macro& macro = macros.at(macroIndex);

					if (!QFile::rename(macro.makePath(), group.makePath() + group.makeName(macro.name) + Qi::macroType)) MsgBox::Error(std::wstring(L"移动失败：") + (const wchar_t*)macro.name.utf16());
				}
				QiJson::LoadMacro();
				TableUpdate();
				ResetWidget();
				DisableWidget();
			}
			return true;
		}
		return QWidget::eventFilter(sender, event);
	}
};