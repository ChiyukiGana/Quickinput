#include <ClickUi.h>

enum TableColumn
{
	TableColumn_control,
	TableColumn_click,
	TableColumn_mode,
	TableColumn_interval,
	TableColumn_state,
	TableColumn_add,
	TableColumn_count
};

ClickUi::ClickUi(QWidget* parent) : QWidget(parent)
{
	Qi::widget.click = this;
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	Init();
	Event();
	StyleGroup();
	LoadLanguage();
	TableReload();
}

void ClickUi::Init()
{
	ui.table->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
	ui.table->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);

	ui.table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
	ui.table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
	ui.table->verticalHeader()->setDefaultSectionSize(28);

	ui.table->horizontalHeader()->setSectionResizeMode(TableColumn_control, QHeaderView::ResizeMode::Stretch);
	ui.table->horizontalHeader()->setSectionResizeMode(TableColumn_click, QHeaderView::ResizeMode::Stretch);
	ui.table->horizontalHeader()->setSectionResizeMode(TableColumn_mode, QHeaderView::ResizeMode::Stretch);
	ui.table->horizontalHeader()->setSectionResizeMode(TableColumn_interval, QHeaderView::ResizeMode::Fixed);
	ui.table->horizontalHeader()->setSectionResizeMode(TableColumn_state, QHeaderView::ResizeMode::Fixed);
	ui.table->horizontalHeader()->setSectionResizeMode(TableColumn_add, QHeaderView::ResizeMode::Fixed);

	ui.table->setColumnWidth(TableColumn_interval, 80);
	ui.table->setColumnWidth(TableColumn_state, 50);
	ui.table->setColumnWidth(TableColumn_add, 50);
}

void ClickUi::Event()
{
	connect(ui.table->horizontalHeader(), &QHeaderView::sectionClicked, this, [this](int index) {
		if (index != TableColumn_add) return;
		Qi::click.clickers.append(Clicker());
		TableAppend(Qi::click.clickers.size() - 1);
		QiJson::SaveJson();
		});
	connect(ui.table, &QTableWidget::cellClicked, this, [this](int row, int col) {
		if (row < 0) return;
		if (col == TableColumn_add)
		{
			Qi::click.clickers.remove(row);
			TableRemove(row);
			QiJson::SaveJson();
		}
		else if (col == TableColumn_state)
		{
			Qi::click.clickers[row].state = !Qi::click.clickers[row].state;
			TableUpdate(row);
			QiJson::SaveJson();
		}
		});
}

void ClickUi::StyleGroup()
{
	style_set_group(ui.table, "action_table");
	style_set_group(ui.table->horizontalHeader(), "action_table_header");
	style_set_group(ui.table->verticalHeader(), "action_table_header");
	ui.table->setStyleSheet("QTableCornerButton::section,QHeaderView::section,QScrollBar,QScrollBar::sub-line,QScrollBar::add-line{background-color:rgba(0,0,0,0);border:none}QScrollBar::handle{background-color:rgba(128,128,128,0.3);border:none}");
}

void ClickUi::LoadLanguage()
{
	std::call_once(lang_once, [this] {
		lang_save_tpl(ui.table);
		lang_save_tb(ui.table->horizontalHeaderItem(TableColumn_control));
		lang_save_tb(ui.table->horizontalHeaderItem(TableColumn_click));
		lang_save_tb(ui.table->horizontalHeaderItem(TableColumn_mode));
		lang_save_tb(ui.table->horizontalHeaderItem(TableColumn_interval));
		lang_save_tb(ui.table->horizontalHeaderItem(TableColumn_state));
		});
	lang_load_tpl(ui.table,"点击提示");
	lang_load_tb(ui.table->horizontalHeaderItem(TableColumn_control));
	lang_load_tb(ui.table->horizontalHeaderItem(TableColumn_click));
	lang_load_tb(ui.table->horizontalHeaderItem(TableColumn_mode));
	lang_load_tb(ui.table->horizontalHeaderItem(TableColumn_interval));
	lang_load_tb(ui.table->horizontalHeaderItem(TableColumn_state));
}

void ClickUi::TableRemove(int index)
{
	ui.table->removeRow(index);
}

void ClickUi::TableUpdate(int index)
{
	const Clicker& click = Qi::click.clickers[index];
	{
		QKeyEdit* key = reinterpret_cast<QKeyEdit*>(ui.table->cellWidget(index, TableColumn_control));
		if (!key)
		{
			ui.table->setCellWidget(index, TableColumn_control, key = new QKeyEdit());
			key->setCombinationMode(false);
			key->setMaximumKeys(1);
			key->setDeviceEnabled(true, true, true, true);
			style_set_group(key, "line_edit");
			key->style()->unpolish(key);
			key->style()->polish(key);
			key->update();
			key->setStyleSheet("margin-left:5px;margin-right:5px;margin-top:2px;margin-bottom:2px");

			connect(key, &QKeyEdit::changed, this, [this, key, index] { Qi::click.clickers[index].key1 = key->key(); QiJson::SaveJson(); });
		}
		key->setKey(click.key1);
	}
	{
		QKeyEdit* key = reinterpret_cast<QKeyEdit*>(ui.table->cellWidget(index, TableColumn_click));
		if (!key)
		{
			ui.table->setCellWidget(index, TableColumn_click, key = new QKeyEdit());
			key->setCombinationMode(false);
			key->setMaximumKeys(1);
			key->setDeviceEnabled(true, true, true, false);
			style_set_group(key, "line_edit");
			key->style()->unpolish(key);
			key->style()->polish(key);
			key->update();
			key->setStyleSheet("margin-left:5px;margin-right:5px;margin-top:2px;margin-bottom:2px");

			connect(key, &QKeyEdit::changed, this, [this, key, index] { Qi::click.clickers[index].key2 = key->key(); QiJson::SaveJson(); });
		}
		key->setKey(click.key2);
	}
	{
		QComboBox* mode = reinterpret_cast<QComboBox*>(ui.table->cellWidget(index, TableColumn_mode));
		if (!mode)
		{
			ui.table->setCellWidget(index, TableColumn_mode, mode = new QComboBox());
			mode->setEditable(true);
			mode->lineEdit()->setReadOnly(true);
			mode->lineEdit()->setAlignment(Qt::AlignCenter);
			mode->addItem("按下");
			mode->addItem("切换");
			QStandardItemModel* model = reinterpret_cast<QStandardItemModel*>(mode->view()->model());
			for (size_t i = 0; i < model->rowCount(); i++) model->item(i)->setTextAlignment(Qt::AlignCenter);
			mode->setView(new QListView());
			mode->setStyleSheet("QComboBox{padding-left:30px;margin-left:5px;margin-right:5px;margin-top:2px;margin-bottom:2px}QComboBox::drop-down{width:30px;border:none}QComboBox::down-arrow{image:url(:/arrow.png);width:14px;height:14px}");
			style_set_group(mode, "combo");
			style_set_group(mode->view(), "combo_body");
			mode->view()->style()->unpolish(mode->view());
			mode->view()->style()->polish(mode->view());
			mode->view()->update();
			mode->style()->unpolish(mode);
			mode->style()->polish(mode);
			mode->update();

			connect(mode, &QComboBox::currentIndexChanged, this, [this, index](int value) { if (index < 0) return; Qi::click.clickers[index].mode = value; QiJson::SaveJson(); });
		}
		mode->setCurrentIndex(click.mode);
	}
	{
		QSpinBox* interval = reinterpret_cast<QSpinBox*>(ui.table->cellWidget(index, TableColumn_interval));
		if (!interval)
		{
			ui.table->setCellWidget(index, TableColumn_interval, interval = new QSpinBox());
			interval->setButtonSymbols(QAbstractSpinBox::NoButtons);
			interval->setMinimum(0);
			interval->setMaximum(1000000);
			style_set_group(interval, "line_edit");
			interval->style()->unpolish(interval);
			interval->style()->polish(interval);
			interval->update();
			interval->setStyleSheet("margin-left:5px;margin-right:5px;margin-top:2px;margin-bottom:2px");

			connect(interval, &QSpinBox::valueChanged, this, [this, index](int value) { Qi::click.clickers[index].interval = value; QiJson::SaveJson(); });
		}
		interval->setValue(click.interval);
	}
	{
		QTableWidgetItem* item = ui.table->item(index, TableColumn_state);
		if (!item) ui.table->setItem(index, TableColumn_state, item = new QTableWidgetItem());
		item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		item->setText(click.state ? QiUi::Symbol::On : QiUi::Symbol::Off);
	}
	{
		QTableWidgetItem* item = ui.table->item(index, TableColumn_add);
		if (!item) ui.table->setItem(index, TableColumn_add, item = new QTableWidgetItem());
		item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		item->setText("x");
	}
}

void ClickUi::TableAppend(int index)
{
	ui.table->insertRow(index);
	TableUpdate(index);
}

void ClickUi::TableReload()
{
	ui.table->setRowCount(Qi::click.clickers.size());
	for (size_t i = 0; i < ui.table->rowCount(); i++) TableUpdate(i);
}

void ClickUi::customEvent(QEvent* e)
{
	if (e->type() == static_cast<int>(QiEvent::lang_reload))
	{
		LoadLanguage();
	}
}