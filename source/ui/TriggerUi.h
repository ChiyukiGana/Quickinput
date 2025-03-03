#pragma once
#include <src/inc_header.h>
#include "ui_TriggerUi.h"
class TriggerUi : public QWidget
{
	Q_OBJECT;
	using This = TriggerUi;
	const int countMax = 9999;
	Ui::TriggerUiClass ui;
	MacroGroups* groups = &Qi::macroGroups;
	MacroGroup* currentGroup = &groups->front();
	Macro* currentMacro;
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
		ui.content_widget->setProperty("group", "client");
		ui.block_check->setProperty("group", "check");
		ui.block_cur_check->setProperty("group", "check");
		ui.mode_combo->setProperty("group", "combo");
		ui.count_edit->setProperty("group", "line_edit");
		ui.key_keyedit->setProperty("group", "line_edit");
		ui.mode_combo->view()->setProperty("group", "table");
	}
	void Init()
	{
		ui.param_widget->setDisabled(true);
		if ("key")
		{
			ui.key_keyedit->setMode(QKeyEdit::Mode::solid);
			ui.key_keyedit->setMaxKeys(2);
			ui.key_keyedit->setMouseEnable(true);
			ui.key_keyedit->setWheelEnable(true);
			ui.key_keyedit->setPadEnable(true);
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
		if ("count")
		{
			ui.count_edit->setValidator(new QIntValidator(0, countMax, this));
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
		std::function currentChanged = [this](int table_index) {
			currentGroup = &groups->front();
			currentMacro = nullptr;
			ui.param_widget->setDisabled(true);

			QTableWidget* table = ui.macroGroup_table->currentTable();
			if (!table) return;
			int row = table->currentRow();
			if (row < 0) return;

			currentGroup = &groups->at(table_index);
			currentMacro = &currentGroup->macros[row];
			ResetWidget();
			Macro& macro = groups->at(table_index).macros[row];
			// state
			ui.block_check->setChecked(macro.keyBlock);
			ui.block_cur_check->setChecked(macro.curBlock);
			ui.mode_combo->setCurrentIndex(macro.mode);
			// key
			{
				QList<QKeyEdit::Key> keys;
				QKeyEdit::Key key;
				key.keyCode = macro.key & 0xFFFF;
				keys.push_back(key);
				key.keyCode = macro.key >> 16;
				keys.push_back(key);
				ui.key_keyedit->setKeys(keys);
			}
			// count
			if (macro.mode >= Macro::down)
			{
				ui.count_edit->setText(QString::number(macro.count));
				ui.count_edit->setEnabled(true);
			}
			ui.param_widget->setEnabled(true);
			};
		connect(ui.macroGroup_table, &QMacroTable::itemClicked, this, [this, currentChanged](int table_index, int row, int column) {
			currentChanged(table_index);
			if (!currentMacro || table_index < 0 || row < 0 || column != 3) return;
			QTableWidget* table = ui.macroGroup_table->table(table_index);
			if (!table) return;

			currentMacro->state = !currentMacro->state;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(table, row, *currentMacro);
			});
		connect(ui.macroGroup_table, &QMacroTable::currentChanged, this, currentChanged);
		connect(ui.macroGroup_table, &QMacroTable::headerClicked, this, [this, currentChanged](int table_index, int column) {
			currentChanged(table_index);
			if (!currentGroup || column != 3) return;
			QTableWidget* table = ui.macroGroup_table->table(table_index);
			if (!table) return;
			bool state = false;
			for (auto& i : currentGroup->macros)
			{
				if (i.state)
				{
					state = true;
					break;
				}
			}
			for (size_t i = 0; i < currentGroup->macros.size(); i++)
			{
				auto& macro = currentGroup->macros[i];
				macro.state = !state;
				QiJson::SaveMacro(macro);
				SetTableItem(table, i, macro);
			}
			});
		connect(ui.block_check, &QCheckBox::clicked, this, [this](bool state) {
			if (!currentMacro) return;
			QTableWidget* table = ui.macroGroup_table->currentTable();
			if (!table) return;
			int row = table->currentRow();
			if (row < 0) return;

			currentMacro->keyBlock = state;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(table, row, *currentMacro);
			});
		connect(ui.block_cur_check, &QCheckBox::clicked, this, [this](bool state) {
			if (!currentMacro) return;
			QTableWidget* table = ui.macroGroup_table->currentTable();
			if (!table) return;
			int row = table->currentRow();
			if (row < 0) return;

			currentMacro->curBlock = state;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(table, row, *currentMacro);
			});

		connect(ui.mode_combo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
			if (!currentMacro) return;
			QTableWidget* table = ui.macroGroup_table->currentTable();
			if (!table) return;
			int row = table->currentRow();
			if (row < 0) return;

			switch (index)
			{
			case Macro::sw:
				ui.count_edit->setText("无限");
				ui.count_edit->setDisabled(1);
				currentMacro->count = 0;
				currentMacro->mode = Macro::sw;
				break;
			case Macro::down:
				ui.count_edit->setText("");
				ui.count_edit->setDisabled(0);
				currentMacro->count = 1;
				currentMacro->mode = Macro::down;
				break;
			case Macro::up:
				ui.count_edit->setText("");
				ui.count_edit->setDisabled(0);
				currentMacro->count = 1;
				currentMacro->mode = Macro::up;
				break;
			}
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(table, row, *currentMacro);
			});
		connect(ui.key_keyedit, &QKeyEdit::changed, this, [this] {
			if (!currentMacro) return;
			QTableWidget* table = ui.macroGroup_table->currentTable();
			if (!table) return;
			int row = table->currentRow();
			if (row < 0) return;

			QList<QKeyEdit::Key> keys = ui.key_keyedit->keys();
			DWORD vk = VK_SPACE;
			if (keys.size() == 1) vk = keys[0].keyCode;
			else if (keys.size() == 2) vk = keys[0].keyCode | (keys[1].keyCode << 16);
			currentMacro->key = vk;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(table, row, *currentMacro);
			});
		connect(ui.count_edit, &QLineEdit::textEdited, this, [this](const QString& text) {
			if (!currentMacro) return;
			QTableWidget* table = ui.macroGroup_table->currentTable();
			if (!table) return;
			int row = table->currentRow();
			if (row < 0) return;

			int n = text.toInt();
			if (n > countMax) n = countMax;
			currentMacro->count = n;
			QiJson::SaveMacro(*currentMacro);
			SetTableItem(table, row, *currentMacro);
			});
	}
	void ResetWidget()
	{
		ui.block_check->setChecked(false);
		ui.block_cur_check->setChecked(false);
		ui.key_keyedit->clear();
		ui.count_edit->setText("");
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
		ResetWidget();
		ui.param_widget->setDisabled(true);
	}
};