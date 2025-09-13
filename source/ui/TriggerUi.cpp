#include "TriggerUi.h"
#include "QTextDialog.h"
TriggerUi::TriggerUi(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	Init();
	Event();
	StyleGroup();
	TableUpdate();
}

void TriggerUi::Init()
{
	ui.scrollArea->setDisabled(true);
	if ("key")
	{
		ui.key_keyedit->setCombinationMode(false);
		ui.key_keyedit->setDeviceEnabled(true, true, true, Qi::set.pad);
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
void TriggerUi::Event()
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
			ui.timer_check->setChecked(macro.timer);
		}
		// key
		{
			QKeyEditKeys keys;
			if (macro.key1) keys.append(macro.key1);
			if (macro.key2) keys.append(macro.key2);
			ui.key_keyedit->setKeys(keys);
		}
		// edit
		{
			ui.var_edit->setText(macro.script);
			ui.count_edit->setValue(macro.count);
			ui.speed_edit->setValue(macro.speed);
			ui.timer_start_edit->setTime(QTime(QiTime::h(macro.timerStart), QiTime::m(macro.timerStart), QiTime::s(macro.timerStart)));
			ui.timer_end_edit->setTime(QTime(QiTime::h(macro.timerEnd), QiTime::m(macro.timerEnd), QiTime::s(macro.timerEnd)));
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
	connect(ui.macroGroup_table, &QMacroTable::foldChanged, this, [this](QTableWidget* table, bool fold) {
		Qi::group.fold[table->horizontalHeaderItem(tableColumn_name)->text()] = fold;
		QiJson::SaveJson();
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
		if (!keys.isEmpty()) currentMacro->key1 = keys.at(0);
		if (keys.size() > 1) currentMacro->key2 = keys.at(1);
		QiJson::SaveMacro(*currentMacro);
		SetTableItem(currentTable, currentRow, *currentMacro);
		});
	connect(ui.count_edit, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
		if (!ItemCurrented()) return;
		currentMacro->count = QiRange::Restricted(value, Macro::range_count);
		QiJson::SaveMacro(*currentMacro);
		SetTableItem(currentTable, currentRow, *currentMacro);
		});
	connect(ui.speed_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		if (!ItemCurrented()) return;
		currentMacro->speed = QiRange::Restricted(value, Macro::range_speed);
		QiJson::SaveMacro(*currentMacro);
		SetTableItem(currentTable, currentRow, *currentMacro);
		});

	connect(ui.var_button, &QPushButton::clicked, this, [this] {
		if (!ItemCurrented()) return;
		QTextDialog edit(true);
		ui.var_edit->setText(edit.Start(ui.var_edit->text()));
		});
	connect(ui.var_edit, &QLineEdit::textChanged, this, [this](const QString& text) {
		if (!ItemCurrented()) return;
		currentMacro->script = text;
		QiJson::SaveMacro(*currentMacro);
		});

	connect(ui.moveScale_x_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		if (!ItemCurrented()) return;
		currentMacro->moveScaleX = QiRange::Restricted(value, Macro::range_moveScale);
		QiJson::SaveMacro(*currentMacro);
		SetTableItem(currentTable, currentRow, *currentMacro);
		});
	connect(ui.moveScale_y_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		if (!ItemCurrented()) return;
		currentMacro->moveScaleY = QiRange::Restricted(value, Macro::range_moveScale);
		QiJson::SaveMacro(*currentMacro);
		SetTableItem(currentTable, currentRow, *currentMacro);
		});

	connect(ui.posScale_x_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		if (!ItemCurrented()) return;
		currentMacro->posScaleX = QiRange::Restricted(value, Macro::range_posScale);
		QiJson::SaveMacro(*currentMacro);
		SetTableItem(currentTable, currentRow, *currentMacro);
		});
	connect(ui.posScale_y_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		if (!ItemCurrented()) return;
		currentMacro->posScaleY = QiRange::Restricted(value, Macro::range_posScale);
		QiJson::SaveMacro(*currentMacro);
		SetTableItem(currentTable, currentRow, *currentMacro);
		});

	connect(ui.timer_start_edit, &QTimeEdit::userTimeChanged, this, [this](const QTime& time) {
		if (!ItemCurrented()) return;
		currentMacro->timerStart = QiRange::Restricted((int)QiTime::toTimeStamp(time.hour(), time.minute(), time.second()), Macro::range_timer);
		QiJson::SaveMacro(*currentMacro);
		SetTableItem(currentTable, currentRow, *currentMacro);
		});
	connect(ui.timer_end_edit, &QTimeEdit::userTimeChanged, this, [this](const QTime& time) {
		if (!ItemCurrented()) return;
		currentMacro->timerEnd = QiRange::Restricted((int)QiTime::toTimeStamp(time.hour(), time.minute(), time.second()), Macro::range_timer);
		QiJson::SaveMacro(*currentMacro);
		SetTableItem(currentTable, currentRow, *currentMacro);
		});
	connect(ui.timer_check, &QCheckBox::toggled, this, [this](bool state) {
		if (!ItemCurrented()) return;
		currentMacro->timer = state;
		QiJson::SaveMacro(*currentMacro);
		SetTableItem(currentTable, currentRow, *currentMacro);
		});
	connect(ui.timer_start_button, &QPushButton::clicked, this, [this] {
		time_t current = QiTime::current();
		ui.timer_start_edit->setTime(QTime(QiTime::h(current), QiTime::m(current), QiTime::s(current)));
		});
	connect(ui.timer_end_button, &QPushButton::clicked, this, [this] {
		time_t current = QiTime::current();
		ui.timer_end_edit->setTime(QTime(QiTime::h(current), QiTime::m(current), QiTime::s(current)));
		});
}
void TriggerUi::StyleGroup()
{
	ui.block_check->setProperty("group", "check");
	ui.block_cur_check->setProperty("group", "check");
	ui.mode_combo->setView(new QListView());
	ui.mode_combo->setProperty("group", "combo");
	ui.mode_combo->view()->setProperty("group", "combo_body");
	ui.var_button->setProperty("group", "get_button");
	ui.var_edit->setProperty("group", "line_edit");
	ui.count_edit->setProperty("group", "line_edit");
	ui.speed_edit->setProperty("group", "line_edit");
	ui.timer_start_edit->setProperty("group", "line_edit");
	ui.timer_end_edit->setProperty("group", "line_edit");
	ui.moveScale_x_edit->setProperty("group", "line_edit");
	ui.moveScale_y_edit->setProperty("group", "line_edit");
	ui.posScale_x_edit->setProperty("group", "line_edit");
	ui.posScale_y_edit->setProperty("group", "line_edit");
	ui.key_keyedit->setProperty("group", "line_edit");
	ui.timer_check->setProperty("group", "check");
	ui.timer_start_button->setProperty("group", "get_button");
	ui.timer_end_button->setProperty("group", "get_button");
	ui.scrollArea->setStyleSheet("QScrollArea,QScrollBar,QScrollBar::sub-line,QScrollBar::add-line{background-color:rgba(0,0,0,0);border:none}QScrollBar::handle{background-color:rgba(128,128,128,0.3);border:none}");
}

bool TriggerUi::GroupCurrented() { return currentGroup; }
bool TriggerUi::TableCurrented() { return currentGroup && currentTable; }
bool TriggerUi::MacroCurrented() { return currentGroup && currentTable && currentMacro; }
bool TriggerUi::ItemCurrented() { return currentGroup && currentTable && currentMacro && (currentRow > -1); }

void TriggerUi::TableState(int macroGroup)
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
	if (state_on && state_off) table->horizontalHeaderItem(tableColumn_state)->setText(QString("全部") + QiUi::Symbol::Off);
	else if (state_on) table->horizontalHeaderItem(tableColumn_state)->setText(QString("全部") + QiUi::Symbol::On);
	else if (state_off) table->horizontalHeaderItem(tableColumn_state)->setText(QString("全部") + QiUi::Symbol::Off);
	else table->horizontalHeaderItem(tableColumn_state)->setText(QString("全部") + QiUi::Symbol::Any);
}
void TriggerUi::SetTableItem(QTableWidget* table, int index, const Macro& macro)
{
	// name
	table->setItem(index, 0, new QTableWidgetItem(macro.name));
	table->item(index, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	// key
	QString qs = "----";
	if (macro.key1 && macro.key2) qs = QKeyEdit::keyName(macro.key1) + " + " + QKeyEdit::keyName(macro.key2);
	else if (macro.key1) qs = QKeyEdit::keyName(macro.key1);
	else if (macro.key2) qs = QKeyEdit::keyName(macro.key2);
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
	if (macro.state) qs = QiUi::Text::trOn;
	else qs = QiUi::Text::trOff;
	table->setItem(index, 3, new QTableWidgetItem(qs));
	table->item(index, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

void TriggerUi::TableUpdate()
{
	currentGroup = &groups->front();
	currentMacro = nullptr;
	if (ui.macroGroup_table->rowCount() != groups->size()) ui.macroGroup_table->setRowCount(groups->size());
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
		table->setHorizontalHeaderItem(tableColumn_name, new QTableWidgetItem(mg.name));
		table->setHorizontalHeaderItem(tableColumn_key, new QTableWidgetItem(""));
		table->setHorizontalHeaderItem(tableColumn_mode, new QTableWidgetItem(""));
		table->setHorizontalHeaderItem(tableColumn_state, new QTableWidgetItem("全部" + QiUi::Symbol::Any));
		TableState(mgPos);
		ui.macroGroup_table->setFold(table, Qi::group.fold[mg.name]);
	}
}

bool TriggerUi::event(QEvent* e)
{
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
	{
		QKeyEvent* keyEvent = (QKeyEvent*)e;
		if ((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Space)) return true;
	}
	return QWidget::event(e);
}
bool TriggerUi::eventFilter(QObject* obj, QEvent* e)
{
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease)) return true;
	return QWidget::eventFilter(obj, e);
}
void TriggerUi::showEvent(QShowEvent*)
{
	ui.macroGroup_table->clearSelection();
	TableUpdate();
	ui.scrollArea->setDisabled(true);
}
void TriggerUi::customEvent(QEvent* e)
{
	if (e->type() == static_cast<int>(QiEvent::key_reset))
	{
		ui.key_keyedit->setPadEnabled(Qi::set.pad);
	}
}