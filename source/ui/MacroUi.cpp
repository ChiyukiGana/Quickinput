#include <MacroUi.h>
#include <QEditDialog.h>
#include "RecordUi.h"
#include "EditUi.h"
MacroUi::MacroUi(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	Init();
	Event();
	StyleGroup();
	Qi::popText->Show("正在加载宏");
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
	ui.macroGroup_table->horizontalHeader()->setHidden(true);
	ui.macroGroup_table->verticalHeader()->setHidden(true);
	ui.macroGroup_table->setAutoScroll(false);
	ui.macroGroup_table->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked);
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
		macro.name = currentGroup->makeName("宏");
		macro.groupName = currentGroup->name;
		macro.groupBase = currentGroup->base;
		QiJson::SaveMacro(currentGroup->macros.append(std::move(macro)));
		TableUpdate();
		ResetWidget();
		DisableWidget();
		});
	connect(ui.edit_button, &QPushButton::clicked, this, [this] { if (!isSold()) return; Qi::popText->Show("正在加载宏"); QTimer::singleShot(32, [] { Qi::widget.macroEdit(); }); });
	connect(ui.export_button, &QPushButton::clicked, this, [this] {
		if (!isSold()) return;
		Qi::widget.dialogActive = true;
		Macro& macro = *currentMacros.front();
		QString des = QFileDialog::getSaveFileName(this, "导出", macro.name + Qi::macroType, QString("Quickinput macro (*") + Qi::macroType + QString(")"));
		if (des.size())
		{
			QFile::remove(des);
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
		if (!SelectGroup()) return;
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
	connect(ui.reload_button, &QPushButton::clicked, this, [this] { Qi::popText->Show("正在加载宏"); QTimer::singleShot(32, [] { Qi::widget.macroLoad(); }); });
	connect(ui.add_group_button, &QPushButton::clicked, this, [this] {
		QString path = Qi::macroDir + Qi::macroGroups.append(MacroGroup(false, Qi::macroGroups.makeName())).name;

		if (!QDir(Qi::macroDir).exists() && !QDir(Qi::macroDir).mkdir(Qi::macroDir))
		{
			MsgBox::Error(L"创建宏目录失败");
			return;
		}

		if (!QDir(path).exists() && !QDir(path).mkdir(path)) MsgBox::Error(L"创建分组目录失败");
		SelectGroup();
		TableUpdate();
		ResetWidget();
		DisableWidget();
		});
	connect(ui.delete_group_button, &QPushButton::clicked, this, [this] {
		if (!currentGroup) return;
		if (currentGroup->macros.not_empty() && MsgBox::Warning(L"确认删除分组的全部宏？", L"Warning", MB_ICONWARNING | MB_YESNO) != IDYES) return;
		if (!QFile::moveToTrash(currentGroup->makePath())) MsgBox::Error(L"删除分组失败");
		QiJson::LoadMacro();
		SelectGroup();
		TableUpdate();
		ResetWidget();
		DisableWidget();
		});
	connect(ui.delete_button, &QPushButton::clicked, this, [this] {
		if (!isMult()) return;
		for (auto& i : currentMacros) if (!QFile::moveToTrash(i->makePath())) MsgBox::Error(L"删除宏失败");

		QiJson::LoadMacro();
		TableUpdate();
		ResetWidget();
		DisableWidget();
		});
}
void MacroUi::StyleGroup()
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
		table->setHorizontalHeaderItem(0, new QTableWidgetItem(mg.name));
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
			table->item(mPos, 0)->setData(static_cast<int>(DataRole::type), table_macros);
			table->item(mPos, 0)->setData(static_cast<int>(DataRole::group), mgPos);
			table->item(mPos, 0)->setData(static_cast<int>(DataRole::macro), mPos);
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
		wndInfo = QiFn::WindowSelection();
		if (!wndInfo.wnd)
		{
			Qi::popText->Popup(2000, "窗口已失效");
			return;
		}
	}
	if (wndInfo.wnd)
	{
		RecordUi rw(&wndInfo);
		Macro macro = rw.Start();
		macro.groupName = currentGroup->name;
		macro.groupBase = currentGroup->base;
		macro.name = currentGroup->makeName("窗口录制");
		if (macro.acRun)
		{
			QiJson::SaveMacro(currentGroup->macros.append(std::move(macro)));
			TableUpdate();
		}
	}
	else
	{
		RecordUi rw(nullptr);
		Macro macro = rw.Start();
		macro.groupName = currentGroup->name;
		macro.groupBase = currentGroup->base;
		macro.name = currentGroup->makeName("录制");
		if (macro.acRun)
		{
			QiJson::SaveMacro(currentGroup->macros.append(std::move(macro)));
			TableUpdate();
		}
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
	static Macro* edit;
	if (e->type() == static_cast<int>(QiEvent::mac_edit_enter))
	{
		if (!isSold()) return;
		edit = currentMacros.front();
		Qi::widget.editMacro = edit->copy();
		Qi::widget.edit = new EditUi(&Qi::widget.editMacro);
		Qi::widget.dialogActive = Qi::debug = true;
		Qi::widget.main->hide();
		Qi::widget.main->setDisabled(true);
		Qi::widget.edit->show();
		Qi::popText->Hide();
	}
	else if (e->type() == static_cast<int>(QiEvent::mac_load))
	{
		QiJson::LoadMacro();
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
		Qi::widget.dialogActive = Qi::debug = false;
		if (Qi::widget.edit) delete Qi::widget.edit;
		Qi::widget.edit = nullptr;
		*edit = std::move(Qi::widget.editMacro);
		QiJson::SaveMacro(*edit);
		Qi::popText->Hide();
		ResetWidget();
		DisableWidget();
	}
	else if (e->type() == static_cast<int>(QiEvent::mac_edit_exit_d))
	{
		Qi::widget.main->setDisabled(false);
		Qi::widget.main->show();
		Qi::widget.dialogActive = Qi::debug = false;
		if (Qi::widget.edit) delete Qi::widget.edit;
		Qi::widget.edit = nullptr;
		Qi::popText->Hide();
		ResetWidget();
		DisableWidget();
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

			if (model.item(0)->data(static_cast<int>(DataRole::type)).toInt() == table_macros && model.item(0)->data(static_cast<int>(DataRole::group)).toInt() != tableIndex)
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
			for (size_t i = 0; i < model.rowCount(); i++)
			{
				QStandardItem* item = model.item(i);
				int groupIndex = item->data(static_cast<int>(DataRole::group)).toInt();
				int macroIndex = item->data(static_cast<int>(DataRole::macro)).toInt();
				if (tableIndex >= groups->size() || groupIndex >= groups->size() || tableIndex == groupIndex) break;

				const MacroGroup& group = groups->at(tableIndex);
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