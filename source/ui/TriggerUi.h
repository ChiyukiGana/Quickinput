#pragma once
#include <src/inc_header.h>
#include "ui_TriggerUi.h"
class TriggerUi : public QWidget
{
	Q_OBJECT;
	using This = TriggerUi;
	const int countMax = 9999;
	Ui::TriggerUiClass ui;
	Macros* macros = &Qi::macros;
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
			ui.macro_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			ui.macro_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			ui.macro_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
			ui.macro_table->setColumnWidth(1, 120);
			ui.macro_table->setColumnWidth(2, 80);
			ui.macro_table->setColumnWidth(3, 60);
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
		connect(ui.macro_table, &QTableWidget::cellClicked, this, [this](int row, int column) {
			if (row < 0 || column != 3) return;
			macros->at(row).state = !macros->at(row).state;
			TableUpdate();
			QiJson::SaveMacro(macros->at(row));
			});
		connect(ui.macro_table, &QTableWidget::itemSelectionChanged, this, [this]() {
			ResetWidget();
			QList<QTableWidgetItem*> items = ui.macro_table->selectedItems();
			if (items.size() == ui.macro_table->columnCount())
			{
				int row = items.first()->row();
				Macro& macro = macros->at(row);
				// state
				ui.block_check->setChecked(macro.block);
				ui.block_cur_check->setChecked(macro.blockCur);
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
			}
			else ui.param_widget->setDisabled(true);
			});
		connect(ui.block_check, &QCheckBox::clicked, this, [this](bool state) {
			int p = ui.macro_table->currentRow(); if (p < 0) return;
			macros->at(p).block = state;
			TableUpdate();
			QiJson::SaveMacro(macros->at(p));
			});
		connect(ui.block_cur_check, &QCheckBox::clicked, this, [this](bool state) {
			int p = ui.macro_table->currentRow(); if (p < 0) return;
			macros->at(p).blockCur = state;
			TableUpdate();
			QiJson::SaveMacro(macros->at(p));
			});

		connect(ui.mode_combo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
			int p = ui.macro_table->currentRow(); if (p < 0) return;
			switch (index)
			{
			case Macro::sw:
				ui.count_edit->setText("无限");
				ui.count_edit->setDisabled(1);
				macros->at(p).count = 0;
				macros->at(p).mode = Macro::sw;
				break;
			case Macro::down:
				ui.count_edit->setText("");
				ui.count_edit->setDisabled(0);
				macros->at(p).count = 1;
				macros->at(p).mode = Macro::down;
				break;
			case Macro::up:
				ui.count_edit->setText("");
				ui.count_edit->setDisabled(0);
				macros->at(p).count = 1;
				macros->at(p).mode = Macro::up;
				break;
			}
			TableUpdate();
			QiJson::SaveMacro(macros->at(p));
			});
		connect(ui.key_keyedit, &QKeyEdit::changed, this, [this] {
			int p = ui.macro_table->currentRow();
			if (p < 0) return;
			QList<QKeyEdit::Key> keys = ui.key_keyedit->keys();
			DWORD vk = VK_SPACE;
			if (keys.size() == 1) vk = keys[0].keyCode;
			else if (keys.size() == 2) vk = keys[0].keyCode | (keys[1].keyCode << 16);
			macros->at(p).key = vk;
			QiJson::SaveMacro(macros->at(p));
			TableUpdate();
			});
		connect(ui.count_edit, &QLineEdit::textEdited, this, [this](const QString& text) {
			int p = ui.macro_table->currentRow(); if (p < 0) return;
			int n = text.toInt();
			if (n > countMax) n = countMax;
			macros->at(p).count = n;
			TableUpdate();
			QiJson::SaveMacro(macros->at(p));
			});
	}
	void ResetWidget()
	{
		ui.block_check->setChecked(false);
		ui.block_cur_check->setChecked(false);
		ui.key_keyedit->clear();
		ui.count_edit->setText("");
	}
	void TableUpdate()
	{
		ui.macro_table->clearMask();
		ui.macro_table->setRowCount(macros->size());
		ui.macro_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.macro_table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.macro_table->verticalHeader()->setDefaultSectionSize(0);
		for (size_t i = 0; i < macros->size(); i++) {
			// name
			ui.macro_table->setItem(i, 0, new QTableWidgetItem(macros->at(i).name));
			ui.macro_table->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			// key
			QString qs = "----";
			if ((macros->at(i).key & 0xFFFF) != 0)
			{
				qs = QKeyEdit::keyName(macros->at(i).key & 0xFFFF);
				if ((macros->at(i).key >> 16) != 0)
				{
					qs += " + ";
					qs += QKeyEdit::keyName(macros->at(i).key >> 16);
				}
			}
			ui.macro_table->setItem(i, 1, new QTableWidgetItem(qs));
			ui.macro_table->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			// mode and count
			qs = "";
			switch (macros->at(i).mode)
			{
			case Macro::sw:
				qs = "切换";
				break;
			case Macro::down:
				qs = "按下(";
				if (macros->at(i).count) qs += QString::number(macros->at(i).count);
				else qs += "无限";
				qs += ")";
				break;
			case Macro::up:
				qs = "松开(";
				if (macros->at(i).count) qs += QString::number(macros->at(i).count);
				else qs += "无限";
				qs += ")";
				break;
			}
			ui.macro_table->setItem(i, 2, new QTableWidgetItem(qs));
			ui.macro_table->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			// state
			if (macros->at(i).state) qs = Qi::ui.text.trOn;
			else qs = Qi::ui.text.trOff;
			ui.macro_table->setItem(i, 3, new QTableWidgetItem(qs));
			ui.macro_table->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
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
		ui.macro_table->setCurrentItem(0);
		TableUpdate();
		ResetWidget();
		ui.param_widget->setDisabled(true);
	}
};