#pragma once
#include <src/inc_header.h>
#include <qlistview.h>
#include "ui_TriggerUi.h"
class TriggerUi : public QWidget
{
	Q_OBJECT;
	using This = TriggerUi;
	// table column
	const int tableColumn_name = 0;
	const int tableColumn_key = 1;
	const int tableColumn_mode = 2;
	const int tableColumn_state = 3;
	Ui::TriggerUiClass ui;
	MacroGroups* groups = &Qi::macroGroups;
	MacroGroup* currentGroup = &groups->front();
	Macro* currentMacro = nullptr;
	QTableWidget* currentTable = nullptr;
	int currentRow = -1;
public:
	TriggerUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		Init();
		Event();
		StyleGroup();
		TableUpdate();
	}
private:
	void StyleGroup()
	{
		ui.block_check->setProperty("group", "check");
		ui.block_cur_check->setProperty("group", "check");
		ui.mode_combo->setView(new QListView());
		ui.mode_combo->setProperty("group", "combo");
		ui.mode_combo->view()->setProperty("group", "combo_body");
		ui.count_edit->setProperty("group", "line_edit");
		ui.speed_edit->setProperty("group", "line_edit");
		ui.moveScale_x_edit->setProperty("group", "line_edit");
		ui.moveScale_y_edit->setProperty("group", "line_edit");
		ui.posScale_x_edit->setProperty("group", "line_edit");
		ui.posScale_y_edit->setProperty("group", "line_edit");
		ui.key_keyedit->setProperty("group", "line_edit");
	}
	void Init()
	{
		ui.scrollArea->setDisabled(true);
		if ("key")
		{
			ui.key_keyedit->setCombinationMode(false);
			ui.key_keyedit->setDeviceEnabled(true, true, true, true);
			ui.key_keyedit->setMaximumKeys(2);
		}
		if ("mode")
		{
			ui.mode_combo->setEditable(true);
			ui.mode_combo->lineEdit()->setReadOnly(true);
			ui.mode_combo->lineEdit()->setAlignment(Qt::AlignCenter);
			ui.mode_combo->addItem("切换");
			ui.mode_combo->addItem("按下");
			ui.mode_combo->addItem("松开");
			QStandardItemModel* model = (QStandardItemModel*)ui.mode_combo->view()->model();
			for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);
		}
		if ("table")
		{
			ui.macroGroup_table->setColumnCount(4);
			ui.macroGroup_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			ui.macroGroup_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
			ui.macroGroup_table->setColumnWidth(1, 120);
			ui.macroGroup_table->setColumnWidth(2, 80);
			ui.macroGroup_table->setColumnWidth(3, 60);
			ui.macroGroup_table->setHorizontalHeaderItem(0, new QTableWidgetItem("名称"));
			ui.macroGroup_table->setHorizontalHeaderItem(1, new QTableWidgetItem("按键"));
			ui.macroGroup_table->setHorizontalHeaderItem(2, new QTableWidgetItem("模式"));
			ui.macroGroup_table->setHorizontalHeaderItem(3, new QTableWidgetItem("状态"));
			ui.macroGroup_table->setAutoScroll(false);
		}
		if ("clear shortcut")
		{
			ui.block_check->installEventFilter(this);
			ui.block_cur_check->installEventFilter(this);
			ui.mode_combo->installEventFilter(this);
		}
	}
	void Event()
	{
		std::function currentChanged = [this](int table_index, int row = -1, int column = -1) {
			currentGroup = &groups->front();
			currentTable = nullptr;
			currentRow = -1;
			currentMacro = nullptr;
			ui.scrollArea->setDisabled(true);

			currentGroup = &groups->at(table_index);

			currentTable = ui.macroGroup_table->table(table_index);
			if (!currentTable) return;

			currentRow = currentTable->currentRow();
			if (currentRow < 0) return;
			currentMacro = &currentGroup->macros[currentRow];

			Macro& macro = groups->at(table_index).macros[currentRow];
			// state
			{
				ui.block_check->setChecked(macro.keyBlock);
				ui.block_cur_check->setChecked(macro.curBlock);
				ui.mode_combo->setCurrentIndex(macro.mode);
			}
			// key
			{
				QKeyEditKeys keys;
				if (macro.key & 0xFFFF) keys.append(macro.key & 0xFFFF);
				if (macro.key >> 16) keys.append(macro.key >> 16);
				ui.key_keyedit->setKeys(keys);
			}
			// edit
			{
				ui.count_edit->setValue(macro.count);
				ui.speed_edit->setValue(macro.speed);
				ui.moveScale_x_edit->setValue(macro.moveScaleX);
				ui.moveScale_y_edit->setValue(macro.moveScaleY);
				ui.posScale_x_edit->setValue(macro.posScaleX);
				ui.posScale_y_edit->setValue(macro.posScaleY);
			}
			ui.scrollArea->setEnabled(true);
			};
		connect(ui.macroGroup_table, &QMacroTable::itemClicked, this, [this, currentChanged](int table_index, int row, int column) {
			currentChanged(table_index, row, column);
			if (!ItemCurrented() || column != 3) return;
			currentMacro->state = !currentMacro->state;
			TableState();
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(currentTable, row, *currentMacro);
			});
		connect(ui.macroGroup_table, &QMacroTable::currentChanged, this, currentChanged);
		connect(ui.macroGroup_table, &QMacroTable::headerClicked, this, [this, currentChanged](int table_index, int column) {
			currentChanged(table_index, -1, column);
			if (!TableCurrented() || column != 3) return;
			bool state = false; for (auto& i : currentGroup->macros) { if (i.state) { state = true; break; } }
			for (size_t i = 0; i < currentGroup->macros.size(); i++)
			{
				auto& macro = currentGroup->macros[i];
				macro.state = !state;
				QiJson::SaveMacro(macro);
				SetTableItem(currentTable, i, macro);
			}
			TableState();
			});
		connect(ui.block_check, &QCheckBox::toggled, this, [this](bool state) {
			if (!ItemCurrented()) return;
			currentMacro->keyBlock = state;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(currentTable, currentRow, *currentMacro);
			});
		connect(ui.block_cur_check, &QCheckBox::toggled, this, [this](bool state) {
			if (!ItemCurrented()) return;
			currentMacro->curBlock = state;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(currentTable, currentRow, *currentMacro);
			});
		connect(ui.mode_combo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
			if (!ItemCurrented()) return;
			if (currentMacro->mode == index) return;
			currentMacro->mode = index;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(currentTable, currentRow, *currentMacro);
			});
		connect(ui.key_keyedit, &QKeyEdit::changed, this, [this] {
			if (!ItemCurrented()) return;
			QKeyEditKeys keys = ui.key_keyedit->keys();
			DWORD vk = VK_SPACE;
			if (keys.size() == 1) vk = keys.first();
			else if (keys.size() > 1) vk = keys.first() | (keys.last() << 16);
			currentMacro->key = vk;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(currentTable, currentRow, *currentMacro);
			});
		connect(ui.count_edit, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
			if (!ItemCurrented()) return;
			if (value > QiRange::macro_count_max) value = QiRange::macro_count_max;
			else if (value < 0) value = 0;
			currentMacro->count = value;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(currentTable, currentRow, *currentMacro);
			});
		connect(ui.speed_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
			if (!ItemCurrented()) return;
			if (value > QiRange::macro_speed_max) value = QiRange::macro_speed_max;
			else if (value < QiRange::macro_speed_min) value = QiRange::macro_speed_min;
			currentMacro->speed = value;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(currentTable, currentRow, *currentMacro);
			});
		connect(ui.moveScale_x_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
			if (!ItemCurrented()) return;
			if (value > QiRange::macro_moveScale_max) value = QiRange::macro_moveScale_max;
			else if (value < QiRange::macro_moveScale_min) value = QiRange::macro_moveScale_min;
			currentMacro->moveScaleX = value;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(currentTable, currentRow, *currentMacro);
			});
		connect(ui.moveScale_y_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
			if (!ItemCurrented()) return;
			if (value > QiRange::macro_moveScale_max) value = QiRange::macro_moveScale_max;
			else if (value < QiRange::macro_moveScale_min) value = QiRange::macro_moveScale_min;
			currentMacro->moveScaleY = value;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(currentTable, currentRow, *currentMacro);
			});
		connect(ui.posScale_x_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
			if (!ItemCurrented()) return;
			if (value > QiRange::macro_posScale_max) value = QiRange::macro_posScale_max;
			else if (value < QiRange::macro_posScale_min) value = QiRange::macro_posScale_min;
			currentMacro->posScaleX = value;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(currentTable, currentRow, *currentMacro);
			});
		connect(ui.posScale_y_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
			if (!ItemCurrented()) return;
			if (value > QiRange::macro_posScale_max) value = QiRange::macro_posScale_max;
			else if (value < QiRange::macro_posScale_min) value = QiRange::macro_posScale_min;
			currentMacro->posScaleY = value;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(currentTable, currentRow, *currentMacro);
			});
	}
	void SetTableItem(QTableWidget* table, int index, const Macro& macro)
	{
		// name
		table->setItem(index, 0, new QTableWidgetItem(macro.name));
		table->item(index, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		// key
		QString qs = "----";
		if ((macro.key & 0xFFFF) != 0)
		{
			qs = QKeyEdit::keyName(macro.key & 0xFFFF);
			if ((macro.key >> 16) != 0)
			{
				qs += " + ";
				qs += QKeyEdit::keyName(macro.key >> 16);
			}
		}
		table->setItem(index, 1, new QTableWidgetItem(qs));
		table->item(index, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		// mode and count
		qs = "";
		switch (macro.mode)
		{
		case Macro::sw:
			qs = "切换";
			break;
		case Macro::down:
			qs = "按下(";
			if (macro.count) qs += QString::number(macro.count);
			else qs += "无限";
			qs += ")";
			break;
		case Macro::up:
			qs = "松开(";
			if (macro.count) qs += QString::number(macro.count);
			else qs += "无限";
			qs += ")";
			break;
		}
		table->setItem(index, 2, new QTableWidgetItem(qs));
		table->item(index, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		// state
		if (macro.state) qs = Qi::ui.text.trOn;
		else qs = Qi::ui.text.trOff;
		table->setItem(index, 3, new QTableWidgetItem(qs));
		table->item(index, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	}
	bool GroupCurrented() { return currentGroup; }
	bool TableCurrented() { return currentGroup && currentTable; }
	bool MacroCurrented() { return currentGroup && currentTable && currentMacro; }
	bool ItemCurrented() { return currentGroup && currentTable && currentMacro && (currentRow > -1); }

	void TableState(int macroGroup = -1)
	{
		MacroGroup* mg;
		QTableWidget* table;
		if (macroGroup < 0)
		{
			mg = currentGroup;
			table = ui.macroGroup_table->currentTable();
		}
		else
		{
			mg = &groups->at(macroGroup);
			table = ui.macroGroup_table->table(macroGroup);
		}
		if (!mg || !table) return;
		bool state_on = true; for (auto& i : mg->macros) { if (!i.state) { state_on = false; break; } }
		bool state_off = true; for (auto& i : mg->macros) { if (i.state) { state_off = false; break; } }
		if (!table) return;
		if (state_on) table->horizontalHeaderItem(tableColumn_state)->setText(QString("全部") + Qi::ui.text.syOn);
		else if (state_off) table->horizontalHeaderItem(tableColumn_state)->setText(QString("全部") + Qi::ui.text.syOff);
		else table->horizontalHeaderItem(tableColumn_state)->setText(QString("全部") + Qi::ui.text.syAny);
	}
	void TableUpdate()
	{
		currentGroup = &groups->front();
		currentMacro = nullptr;
		if (ui.macroGroup_table->rowCount() != groups->size()) ui.macroGroup_table->setTableCount(groups->size());
		for (size_t mgPos = 0; mgPos < groups->size(); mgPos++)
		{
			MacroGroup& mg = groups->at(mgPos);
			QTableWidget* table = ui.macroGroup_table->table(mgPos);

			table->setColumnCount(4);
			table->setRowCount(mg.macros.size());

			table->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
			table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
			table->setColumnWidth(1, 120);
			table->setColumnWidth(2, 80);
			table->setColumnWidth(3, 60);
			table->setSelectionBehavior(QAbstractItemView::SelectRows);
			table->setSelectionMode(QAbstractItemView::SingleSelection);
			bool state = false;
			for (size_t mPos = 0; mPos < mg.macros.size(); mPos++)
			{
				Macro& m = mg.macros[mPos];
				SetTableItem(table, mPos, m);
				if (m.state) m.state = true;
			}
			table->setHorizontalHeaderItem(0, new QTableWidgetItem(mg.name));
			table->setHorizontalHeaderItem(1, new QTableWidgetItem(""));
			table->setHorizontalHeaderItem(2, new QTableWidgetItem(""));
			table->setHorizontalHeaderItem(3, new QTableWidgetItem("全部" + Qi::ui.text.syAny));
			TableState(mgPos);
		}
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
		ui.macroGroup_table->clearSelection();
		TableUpdate();
		ui.scrollArea->setDisabled(true);
	}
};