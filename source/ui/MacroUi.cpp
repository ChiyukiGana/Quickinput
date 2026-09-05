#include <MacroUi.h>
#include <QEditDialog.h>
#include "RecordUi.h"
#include "EditUi.h"

MacroUi::MacroUi(QWidget* parent) : QWidget(parent)
{
	Qi::widget.macro = this;
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	Init();
	Event();
	StyleGroup();
	Qi::popText->Show(lang_trans("正在加载宏"));
	QTimer::singleShot(32, [] { Qi::widget.macroLoad(); });

#ifdef Q_EDIT_HIDE
	ui.record_button->setHidden(true);
	ui.record_window_button->setHidden(true);
	ui.add_button->setHidden(true);
	ui.edit_button->setHidden(true);
	ui.export_button->setHidden(true);
	ui.import_button->setHidden(true);
	ui.add_group_button->setHidden(true);
	ui.delete_group_button->setHidden(true);
	ui.delete_button->setHidden(true);
#endif
}

enum
{
	table_groups,
	table_macros
};

void MacroUi::Init()
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
	if ("table")
	{
		ui.macroGroup_table->horizontalHeader()->setHidden(true);
		ui.macroGroup_table->verticalHeader()->setHidden(true);
		ui.macroGroup_table->setAutoScroll(false);
		ui.macroGroup_table->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked);
	}
}
void MacroUi::Event()
{
	connect(ui.macroGroup_table, &QMacroTable::currentChanged, this, &This::CurrentChanged);
	connect(ui.macroGroup_table, &QMacroTable::itemClicked, this, &This::CurrentChanged);
	connect(ui.macroGroup_table, &QMacroTable::headerClicked, this, &This::CurrentChanged);
	connect(ui.macroGroup_table, &QMacroTable::headerDoubleClicked, this, [this](int index, int) {
		if (index <= 0) return;
		QTableWidget* table = ui.macroGroup_table->table(index);
		if (!table) return;
		currentGroup = &groups->at(index);
		QEditDialog edit;
		style_set_group(edit.edit(), "line_edit");
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
	connect(ui.macroGroup_table, &QMacroTable::foldChanged, this, [this](QTableWidget* table, bool fold) {
		Qi::group.fold[table->horizontalHeaderItem(0)->text()] = fold;
		QiJson::SaveJson();
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
		if (!SelectGroup()) return;
		Macro macro;
		macro.mode = Macro::down;
		macro.count = 1;
		macro.name = currentGroup->makeName(lang_trans("宏"));
		macro.groupName = currentGroup->name;
		macro.groupBase = currentGroup->base;
		macro.storageType = Qi::set.save_type;
		currentGroup->macros.append(std::move(macro)).save();
		TableUpdate();
		ResetWidget();
		DisableWidget();
		});
	connect(ui.edit_button, &QPushButton::clicked, this, [this] { if (!isSold()) return; Qi::popText->Show(lang_trans("正在加载宏")); QTimer::singleShot(32, [] { Qi::widget.macroEdit(); }); });
	connect(ui.export_button, &QPushButton::clicked, this, [this] {
		if (!isSold()) return;
		Qi::widget.dialogActive = true;
		Macro& macro = *currentMacros.front();
		const QString exportFilter = lang_trans("Json文本宏") + "(*" + Qi::macroType + ");;" + lang_trans("Qim二进制宏") + "(*" + Qi::macroQimType + ")";
		QString des = QFileDialog::getSaveFileName(this, lang_trans("导出"), macro.name, exportFilter);
		if (des.size() > 4)
		{
			const QString back = des.mid(des.size() - 5);
			bool r = false;
			if (back.indexOf(Qi::macroQimType) != -1) r = macro.saveTo(des, Macro::StorageType::QIM);
			else if (back.indexOf(Qi::macroType) != -1) r = macro.saveTo(des, Macro::StorageType::JSON);
			else r = macro.saveTo(des, Macro::StorageType::CURRENT);
			if (!r) MsgBox::Error(lang_trans("导出宏失败").toStdWString());
		}
		Qi::widget.dialogActive = false;
		});
	connect(ui.import_button, &QPushButton::clicked, this, [this] {
		if (!SelectGroup()) return;
		Qi::widget.dialogActive = true;
		const QString importFilter = QString("Quickinput ") + lang_trans("宏") + "(*" + Qi::macroType + " * " + Qi::macroQimType + ");;" + lang_trans("Json文本宏") + "(*" + Qi::macroType + ");;" + lang_trans("Qim二进制宏") + "(*" + Qi::macroQimType + ")";
		QString src = QFileDialog::getOpenFileName(this, lang_trans("导入"), {}, importFilter);
		if (src.size() > 4)
		{
			Macro::StorageType type = Macro::StorageType::JSON;
			const QString back = src.mid(src.size() - 5);
			if (back.indexOf(Qi::macroQimType) != -1) type = Macro::StorageType::QIM;

			QFileInfo info(src);
			if (!QDir(Qi::macroDir).exists() && !QDir(Qi::macroDir).mkdir(Qi::macroDir)) MsgBox::Error(lang_trans("创建宏目录失败").toStdWString());
			if (!QFile::copy(src, currentGroup->makePath(info.baseName(), type)))
			{
				MsgBox::Error(lang_trans("导入宏失败").toStdWString());
				return;
			}
			Macro::loadAll();
			TableUpdate();
			ResetWidget();
			DisableWidget();
		}
		Qi::widget.dialogActive = false;
		});
	connect(ui.reload_button, &QPushButton::clicked, this, [this] { Qi::popText->Show(lang_trans("正在加载宏")); QTimer::singleShot(32, [] { Qi::widget.macroLoad(); }); });
	connect(ui.add_group_button, &QPushButton::clicked, this, [this] {
		QString path = Qi::macroDir + Qi::macroGroups.append(MacroGroup(false, Qi::macroGroups.makeName())).name;

		if (!QDir(Qi::macroDir).exists() && !QDir(Qi::macroDir).mkdir(Qi::macroDir))
		{
			MsgBox::Error(lang_trans("创建宏目录失败").toStdWString());
			return;
		}

		if (!QDir(path).exists() && !QDir(path).mkdir(path)) MsgBox::Error(lang_trans("创建分组目录失败").toStdWString());
		SelectGroup();
		TableUpdate();
		ResetWidget();
		DisableWidget();
		});
	connect(ui.delete_group_button, &QPushButton::clicked, this, [this] {
		if (!currentGroup) return;
		if (currentGroup->macros.not_empty() && MsgBox::Warning(lang_trans("确认删除分组的全部宏?").toStdWString(), L"Warning", MB_ICONWARNING | MB_YESNO) != IDYES) return;
		if (!currentGroup->remove())
		{
			MsgBox::Error(lang_trans("删除分组失败").toStdWString());
			return;
		}
		groups->remove_of_find(currentGroup);
		SelectGroup();
		TableUpdate();
		ResetWidget();
		DisableWidget();
		});
	connect(ui.delete_button, &QPushButton::clicked, this, [this] {
		if (!isMult()) return;
		QiVectorIndex index;
		for (auto& i : currentMacros)
		{
			size_t pos = currentGroup->macros.indexOf(i);
			if (pos != Macros::end_pos) index.push_back(pos);
			if (!QFile::moveToTrash(i->makePath())) MsgBox::Error(lang_trans("删除宏文件失败").toStdWString());
		}
		currentGroup->macros.remove(index);

		TableUpdate();
		ResetWidget();
		DisableWidget();
		});
}
void MacroUi::StyleGroup()
{
	style_set_group(ui.record_button, "macro-button");
	style_set_group(ui.record_window_button, "macro-button");
	style_set_group(ui.add_button, "macro-button");
	style_set_group(ui.edit_button, "macro-button");
	style_set_group(ui.export_button, "macro-button");
	style_set_group(ui.import_button, "macro-button");
	style_set_group(ui.reload_button, "macro-button");
	style_set_group(ui.add_group_button, "macro-button");
	style_set_group(ui.delete_group_button, "macro-button");
	style_set_group(ui.delete_button, "macro-button");
}
void MacroUi::LoadLanguage()
{
	std::call_once(lang_once, [this] {
		lang_save_t(ui.record_button);
		lang_save_t(ui.record_window_button);
		lang_save_t(ui.add_button);
		lang_save_t(ui.edit_button);
		lang_save_t(ui.export_button);
		lang_save_t(ui.import_button);
		lang_save_t(ui.reload_button);
		lang_save_t(ui.add_group_button);
		lang_save_t(ui.delete_group_button);
		lang_save_t(ui.delete_button);
		});
	lang_load_t(ui.record_button);
	lang_load_t(ui.record_window_button);
	lang_load_t(ui.add_button);
	lang_load_t(ui.edit_button);
	lang_load_t(ui.export_button);
	lang_load_t(ui.import_button);
	lang_load_t(ui.reload_button);
	lang_load_t(ui.add_group_button);
	lang_load_t(ui.delete_group_button);
	lang_load_t(ui.delete_button);
	TableUpdate();
}

void MacroUi::ResetWidget()
{
	ui.macroGroup_table->clearSelection();
}
void MacroUi::DisableWidget()
{
	ui.edit_button->setDisabled(true);
	ui.export_button->setDisabled(true);
	ui.delete_group_button->setDisabled(true);
	ui.delete_button->setDisabled(true);
}
void MacroUi::TableUpdate()
{
	updating = true;
	currentMacros.clear();
	if (ui.macroGroup_table->rowCount() != groups->size()) ui.macroGroup_table->setRowCount(groups->size());

	for (size_t mgPos = 0; mgPos < groups->size(); mgPos++)
	{
		MacroGroup& mg = groups->at(mgPos);
		QTableWidget* table = ui.macroGroup_table->table(mgPos);
		table->setColumnCount(1);
		table->setRowCount(mg.macros.size());
		table->setMinimumWidth(ui.macroGroup_table->width());
		table->setHorizontalHeaderItem(0, new QTableWidgetItem(mg.base ? lang_trans("默认分组") : mg.name));
		table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
		// group move
		table->horizontalHeader()->setAcceptDrops(true);
		table->horizontalHeader()->installEventFilter(this);
		table->horizontalHeader()->setProperty("tableType", table_groups);
		table->horizontalHeader()->setProperty("tableIndex", mgPos);
		table->horizontalHeader()->viewport()->setAcceptDrops(true);
		table->horizontalHeader()->viewport()->installEventFilter(this);
		table->horizontalHeader()->viewport()->setProperty("tableType", table_groups);
		table->horizontalHeader()->viewport()->setProperty("tableIndex", mgPos);
		// macro move
		table->setDragEnabled(true);
		table->setDragDropMode(QAbstractItemView::DragDrop);
		table->setDefaultDropAction(Qt::DropAction::IgnoreAction);
		table->viewport()->installEventFilter(this);
		table->viewport()->setProperty("tableType", table_macros);
		table->viewport()->setProperty("tableIndex", mgPos);
		for (size_t mPos = 0; mPos < mg.macros.size(); mPos++)
		{
			Macro& m = mg.macros[mPos];
			table->setItem(mPos, 0, new QTableWidgetItem(m.name));
			table->item(mPos, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			table->item(mPos, 0)->setData(DataRole::type, table_macros);
			table->item(mPos, 0)->setData(DataRole::group, mgPos);
			table->item(mPos, 0)->setData(DataRole::macro, mPos);
		}
		ui.macroGroup_table->setFold(table, Qi::group.fold[mg.name]);
	}
	Qi::widget.varViewReload();
	updating = false;
}
void MacroUi::RecStart(bool wnd)
{
	if (!SelectGroup()) return;
	Qi::widget.dialogActive = true;
	Qi::widget.main->hide();
	WndInfo wndInfo;
	if (wnd)
	{
		wndInfo = QiFn::WindowSelector();
		if (!wndInfo.wnd)
		{
			Qi::popText->Popup(2000, lang_trans("窗口已失效"));
			return;
		}
	}
	RecordUi rec(wndInfo.wnd ? &wndInfo : nullptr);
	Macro& macro = rec.Start();
	macro.groupName = currentGroup->name;
	macro.groupBase = currentGroup->base;
	macro.name = currentGroup->makeName(lang_trans(wndInfo.wnd ? "窗口录制" : "录制"));
	macro.storageType = Qi::set.save_type;
	if (macro.acRun)
	{
		currentGroup->macros.append(std::move(macro)).save();
		TableUpdate();
	}
	Qi::widget.dialogActive = false;
	Qi::widget.main->show();
}
void MacroUi::CurrentChanged(int table_index)
{
	currentMacros.clear();
	currentGroup = nullptr;
	if (!SelectGroup()) return;
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
}
bool MacroUi::SelectGroup()
{
	if (!currentGroup && groups->not_empty()) currentGroup = &groups->front();
	return currentGroup;
}

bool MacroUi::isSold() { return (currentGroup && currentMacros.size() == 1); }
bool MacroUi::isMult() { return (currentGroup && currentMacros.size()); }

void MacroUi::showEvent(QShowEvent*)
{
	ResetWidget();
	DisableWidget();
	TableUpdate();
}
void MacroUi::customEvent(QEvent* e)
{
	if (e->type() == static_cast<int>(QiEvent::mac_edit_enter))
	{
		if (!isSold()) return;
		currentEdit = currentMacros.front();
		*Qi::widget.editMacro = *currentEdit;
		Qi::widget.edit = new EditUi(Qi::widget.editMacro.get());
		Qi::widget.dialogActive = true;
		Qi::widget.main->hide();
		Qi::widget.main->setDisabled(true);
		Qi::widget.edit->show();
		Qi::popText->Hide();
	}
	else if (e->type() == static_cast<int>(QiEvent::mac_load))
	{
		Macro::loadAll();
		TableUpdate();
		ResetWidget();
		DisableWidget();
		Qi::popText->Hide();
		if (!Qi::widget.onload)
		{
			Qi::widget.onload = true;
			if (Qi::set.minMode && Qi::set.defOn) QApplication::sendEvent(Qi::widget.main, new QEvent(QEvent::WindowDeactivate));
		}
	}
	else if (e->type() == static_cast<int>(QiEvent::mac_edit_exit))
	{
		Qi::widget.main->setDisabled(false);
		Qi::widget.main->show();
		Qi::widget.dialogActive = false;
		if (Qi::widget.edit) delete Qi::widget.edit;
		Qi::widget.edit = nullptr;
		*currentEdit = std::move(*Qi::widget.editMacro);
		currentEdit->save();
		Qi::popText->Hide();
		ResetWidget();
		DisableWidget();
	}
	else if (e->type() == static_cast<int>(QiEvent::mac_edit_exit_d))
	{
		Qi::widget.main->setDisabled(false);
		Qi::widget.main->show();
		Qi::widget.dialogActive = false;
		if (Qi::widget.edit) delete Qi::widget.edit;
		Qi::widget.edit = nullptr;
		Qi::popText->Hide();
		ResetWidget();
		DisableWidget();
	}
	else if (e->type() == static_cast<int>(QiEvent::lang_reload))
	{
		LoadLanguage();
	}
}
bool MacroUi::eventFilter(QObject* sender, QEvent* event)
{
	static const QString itemMime = "application/x-qabstractitemmodeldatalist";
	static const QString headerMime = "application/x-qheaderview-section";
	static QObject* enter;

	const int tableType = sender->property("tableType").toInt();
	const int tableIndex = sender->property("tableIndex").toInt();

	if (event->type() == QEvent::MouseMove)
	{
		if (tableType == table_groups && tableIndex > 0 && sender->property("press").toBool())
		{
			QDrag* drag = new QDrag(sender);
			QMimeData* mimeData = new QMimeData;
			mimeData->setData(headerMime, QByteArray::number(tableIndex));
			drag->setMimeData(mimeData);
			drag->exec(Qt::DropAction::MoveAction);

			if (enter)
			{
				const int index = enter->property("tableIndex").toInt();
				if (index > 0 && tableIndex > 0 && index != tableIndex)
				{
					Qi::macroGroups.move(tableIndex, index);
					QiJson::SaveJson();
					TableUpdate();
					ResetWidget();
					DisableWidget();
				}
			}

			sender->setProperty("press", false);
			return true;
		}
	}
	if (event->type() == QEvent::MouseButtonPress)
	{
		if (tableType == table_groups && tableIndex > 0) sender->setProperty("press", true);
	}
	else if (event->type() == QEvent::MouseButtonRelease)
	{
		if (tableType == table_groups && tableIndex > 0) sender->setProperty("press", false);
	}
	else if (event->type() == QEvent::DragEnter)
	{
		enter = nullptr;
		QDragEnterEvent* dragEnter = (QDragEnterEvent*)event;
		if (tableType == table_macros && dragEnter->mimeData()->hasFormat(itemMime))
		{
			QStandardItemModel model;
			model.dropMimeData(dragEnter->mimeData(), Qt::CopyAction, 0, 0, QModelIndex());
			if (model.rowCount() < 1) return QWidget::eventFilter(sender, event);

			if (model.item(0)->data(DataRole::type).toInt() == table_macros && model.item(0)->data(DataRole::group).toInt() != tableIndex)
			{
				dragEnter->acceptProposedAction();
			}
		}
		else if (tableType == table_groups && dragEnter->mimeData()->hasFormat(headerMime))
		{
			const int index = dragEnter->mimeData()->data(headerMime).toInt();
			if (index > 0 && tableIndex > 0 && index != tableIndex)
			{
				dragEnter->acceptProposedAction();
				enter = sender;
				return false;
			}
		}
		return true;
	}
	else if (event->type() == QEvent::DragLeave)
	{
		if (tableType == table_groups) enter = nullptr;
	}
	else if (event->type() == QEvent::Drop)
	{
		QDropEvent* dropEvent = (QDropEvent*)event;
		if (tableType == table_macros && dropEvent->mimeData()->hasFormat(itemMime))
		{
			QStandardItemModel model;
			model.dropMimeData(dropEvent->mimeData(), Qt::CopyAction, 0, 0, QModelIndex());

			if (tableIndex >= groups->size()) return true;
			QiVectorIndex moveIndex;
			int selected_groupIndex = -1;
			for (size_t i = 0; i < model.rowCount(); i++)
			{
				QStandardItem* item = model.item(i);
				const int groupIndex = item->data(DataRole::group).toInt();
				const int macroIndex = item->data(DataRole::macro).toInt();
				if (groupIndex >= groups->size() || tableIndex == groupIndex) break;

				if (selected_groupIndex == -1) selected_groupIndex = groupIndex;
				else if (selected_groupIndex != groupIndex)
				{
					selected_groupIndex = -1;
					break;
				}
				moveIndex.push_back(macroIndex);
			}

			if (selected_groupIndex != -1)
			{
				MacroGroup& old_group = groups->at(selected_groupIndex);
				MacroGroup& new_group = groups->at(tableIndex);
				new_group.moveIn(old_group, moveIndex);
				TableUpdate();
				ResetWidget();
				DisableWidget();
			}
		}
		return true;
	}
	return QWidget::eventFilter(sender, event);
}