#pragma once
#include <src/inc_header.h>
#include "ui_VarViewUi.h"
class VarViewUi : public QDialog
{
	Q_OBJECT;
	using This = VarViewUi;
	enum
	{
		tableColumn_name,
		tableColumn_type,
		tableColumn_value
	};
	Ui::VarViewUiClass ui;
	bool updating = false;
public:
	VarViewUi()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		connect(ui.title_close_button, &QPushButton::clicked, this, [this] { hide(); });
		StyleGroup();
		if ("table")
		{
			ui.macroGroup_table->setColumnCount(1);
			ui.macroGroup_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
			ui.macroGroup_table->horizontalHeader()->setHidden(true);
			ui.macroGroup_table->setAutoScroll(false);
		}
	}
private:
	void StyleGroup()
	{
		setProperty("group", "frame");
		ui.title_widget->setProperty("group", "title");
		ui.content_widget->setProperty("group", "client");
		ui.title_close_button->setProperty("group", "title-close_button");
	}
	void TableUpdate(QTableWidget* table, const QiVarMap varMap)
	{
		updating = true;
		table->clearContents();
		table->setRowCount(varMap.size());
		auto iter = varMap.begin();
		for (size_t i = 0; i < varMap.size(); i++, iter++)
		{
			const QString& name = ((*iter).first).c_str();
			const QiVar& var = (*iter).second;

			QTableWidgetItem* item = new QTableWidgetItem(name);
			if (var.type == QiVar::Type::t_str) item->setTextColor(QColor(255, 128, 0));
			else item->setTextColor(QColor(0, 192, 0));
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			table->setItem(i, tableColumn_name, item);

			if (var.type == QiVar::Type::t_str)
			{
				item = new QTableWidgetItem("str");
				item->setTextColor(QColor(255, 128, 0));
			}
			else
			{
				item = new QTableWidgetItem("num");
				item->setTextColor(QColor(0, 192, 0));
			}
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			table->setItem(i, tableColumn_type, item);

			item = new QTableWidgetItem(var.toString().c_str());
			if (var.type == QiVar::Type::t_str) item->setTextColor(QColor(255, 128, 0));
			else item->setTextColor(QColor(0, 192, 0));
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			table->setItem(i, tableColumn_value, item);
		}
		updating = false;
	}
	void TableUpdate()
	{
		updating = true;
		QiVector<Macro*> macros;
		for (auto& i : Qi::macroGroups) for (auto& im : i.macros) macros.append(&im);
		ui.macroGroup_table->setRowCount(macros.size() + 1);

		bool first = true;
		for (size_t i = 0; i < macros.size() + 1; i++)
		{
			QTableWidget* table = ui.macroGroup_table->table(i);
			QiVarMap* varMap;
			if ("column")
			{
				table->setColumnCount(3);
				table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
				table->horizontalHeader()->setSectionResizeMode(tableColumn_value, QHeaderView::ResizeMode::Stretch);
				table->setColumnWidth(tableColumn_name, 120);
				table->setColumnWidth(tableColumn_type, 60);
				table->setHorizontalHeaderItem(tableColumn_name, new QTableWidgetItem("全局"));
				table->setHorizontalHeaderItem(tableColumn_type, new QTableWidgetItem("类型"));
				table->setHorizontalHeaderItem(tableColumn_value, new QTableWidgetItem("值"));
				table->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
				table->setAutoScroll(false);
			}
			if (first)
			{
				first = false;
				varMap = Qi::interpreter.varMap();
			}
			else
			{
				Macro* macro = macros.at(i - 1);
				table->horizontalHeaderItem(tableColumn_name)->setText(macro->name);
				varMap = &macro->varMap;
			}

			TableUpdate(table, *varMap);
			connect(table, &QTableWidget::cellDoubleClicked, this, [this, table](int row, int column) {
				if (updating) return;
				if (column == tableColumn_value) table->editItem(table->item(row, tableColumn_value)); 
				});
			connect(table, &QTableWidget::cellChanged, this, [this, table, varMap](int row, int column) {
				if (updating) return;
				QString name = table->item(row, tableColumn_name)->text();
				QString text = table->item(row, tableColumn_value)->text();
				if (QiVar::isNumber(text.toUtf8().data())) varMap->at(name.toUtf8().data()) = QiVar(QiVar::toNumber(text.toUtf8().data()));
				else varMap->at(name.toUtf8().data()) = QiVar(text.toUtf8().data());
				TableUpdate(table, *varMap);
				});
		}
		updating = false;
	}
private:
	bool event(QEvent* e)
	{
		if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
		{
			QKeyEvent* keyEvent = (QKeyEvent*)e;
			if ((keyEvent->key() == Qt::Key_Escape) || (keyEvent->key() == Qt::Key_Return) || keyEvent->key() == Qt::Key_Enter || (keyEvent->key() == Qt::Key_Space)) return true;
		}
		return QWidget::event(e);
	}
	void showEvent(QShowEvent*)
	{
		TableUpdate();
		SetForegroundWindow((HWND)QWidget::winId());
	}
	void customEvent(QEvent* e)
	{
		if (e->type() == VarViewEvent::reload)
		{
			if (!isHidden()) TableUpdate();
		}
	}
	// window move
	QPoint mouse_positon;
	bool mouse_down = false;
	void mousePressEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			mouse_positon = e->pos();
			mouse_down = true;
			e->accept();
		}
	}
	void mouseMoveEvent(QMouseEvent* e)
	{
		if (mouse_down)
		{
			if (Distance(mouse_positon.x(), mouse_positon.y(), e->pos().x(), e->pos().y()) < 100)
			{
				move(e->pos() + pos() - mouse_positon);
				e->accept();
			}
		}
	}
	void mouseReleaseEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			mouse_down = false;
			e->accept();
		}
	}
};