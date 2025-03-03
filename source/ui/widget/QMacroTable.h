#pragma once
#include <qvariant.h>
#include <qevent.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtablewidget.h>
#include <qheaderview.h>
#include <qlayout.h>

class QMacroTable : public QTableWidget
{
	Q_OBJECT;
	bool selfAction = false;
public:
	QMacroTable(QWidget* parent = nullptr) : QTableWidget(parent)
	{
		setProperty("setFoldEnabled", true);
		horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
		horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
		verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		verticalHeader()->setHidden(true);
		setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
		setShowGrid(false);
		setDropIndicatorShown(false);
		setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
		setStyleSheet("QHeaderView::section,QScrollBar{background:transparent}");
		if ("style group")
		{
			setProperty("group", "table");
			horizontalHeader()->setProperty("group", "table_header");
			verticalHeader()->setProperty("group", "table_header");
			if ("StyleGroup")
			{
				setProperty("group", "table");
				horizontalHeader()->setProperty("group", "table_header");
				verticalHeader()->setProperty("group", "table_header");
				if ("table corner button")
				{
					const QObjectList& objs = children();
					for (size_t i = 0; i < objs.size(); i++)
					{
						const QObject* obj = objs[i];
						QString name = obj->metaObject()->className();
						if (name == "QTableCornerButton")
						{
							QWidget* corner = (QWidget*)obj;
							QHBoxLayout* box = new QHBoxLayout(corner);
							box->setMargin(0);
							QWidget* widget = new QWidget(corner);
							box->addWidget(widget);
							widget->setProperty("group", "table_header");
							break;
						}
						else if (name == "QLineEdit")
						{
							QLineEdit* lineEdit = (QLineEdit*)obj;
							lineEdit->setProperty("group", "table_header");
						}
					}
				}
			}
		}
	}

	void setTableCount(int count)
	{
		selfAction = true;
		clearContents();
		setRowCount(count);
		if (!columnCount()) setColumnCount(1);
		for (size_t i = 0; i < count; i++)
		{
			QTableWidget* table = new QTableWidget(this);
			table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
			table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
			table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
			table->verticalHeader()->setDefaultSectionSize(0);
			table->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
			table->setShowGrid(false);
			table->setDropIndicatorShown(false);
			table->setCornerButtonEnabled(false);
			table->setStyleSheet("QHeaderView::section,QScrollBar{background:transparent;font-weight:bold}");
			table->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked);
			table->setColumnCount(1);
			if ("StyleGroup")
			{
				table->setProperty("group", "table");
				table->horizontalHeader()->setProperty("group", "table_header");
				table->verticalHeader()->setProperty("group", "table_header");
				if ("table corner button")
				{
					const QObjectList& objs = table->children();
					for (size_t i = 0; i < objs.size(); i++)
					{
						const QObject* obj = objs[i];
						QString name = obj->metaObject()->className();
						if (name == "QTableCornerButton")
						{
							QWidget* corner = (QWidget*)obj;
							corner->installEventFilter(this);
							corner->setProperty("parent_table", QVariant::fromValue((QTableWidget*)table));
							QHBoxLayout* box = new QHBoxLayout(corner);
							box->setMargin(0);
							QLabel* label = new QLabel(corner);
							QFont font;
							font.setPixelSize(16);
							label->setFont(font);
							label->setAlignment(Qt::AlignCenter);
							label->setText(u8"▾");
							box->addWidget(label);
							table->setProperty("table_corner_label", QVariant::fromValue((QLabel*)label));
							label->setProperty("group", "table_header");
							break;
						}
						else if (name == "QLineEdit")
						{
							QLineEdit* lineEdit = (QLineEdit*)obj;
							lineEdit->setProperty("group", "table_header");
						}
					}
				}
			}
			setCellWidget(i, 0, table);
			setSpan(i, 0, 1, columnCount());
			connect(table, &QTableWidget::itemSelectionChanged, this, [this, i, table]() {
				if (selfAction) return;
				setReSelection(i);
				currentChanged(i, table->currentRow(), table->currentColumn());
				});
			connect(table, &QTableWidget::cellClicked, this, [this, i](int row, int column) {
				if (selfAction) return;
				setReSelection(i);
				itemClicked(i, row, column);
				});
			connect(table, &QTableWidget::cellChanged, this, [this, i](int row, int column) {
				if (selfAction) return;
				setReSelection(i);
				itemChanged(i, row, column);
				});
			connect(table->verticalHeader(), &QHeaderView::sectionClicked, this, [this, i, table](int index) {
				if (selfAction) return;
				int row = currentRow();
				setReSelection(i);
				if (i != row) currentChanged(i, table->currentRow(), table->currentColumn());
				});
			connect(table->horizontalHeader(), &QHeaderView::sectionClicked, this, [this, i, table](int index) {
				if (selfAction) return;
				int row = currentRow();
				setReSelection(i);
				headerClicked(i, index);
				if (i != row) currentChanged(i, table->currentRow(), table->currentColumn());
				});
			connect(table->horizontalHeader(), &QHeaderView::sectionDoubleClicked, this, [this, i](int index) {
				if (selfAction) return;
				headerDoubleClicked(i, index);
				});
		}
		setReSize();
		selfAction = false;
	}

	QTableWidget* table(int table_index)
	{
		return (QTableWidget*)cellWidget(table_index, 0);
	}

	QTableWidget* currentTable()
	{
		if (currentRow() < 0) return nullptr;
		return table(currentRow());
	}

	QList<int> currentIndex(int table_index)
	{
		QList<int> index;
		QTableWidget* table_pointer = table(table_index);
		if (table_pointer)
		{
			QList<QTableWidgetItem*> items = table_pointer->selectedItems();
			for (const auto& i : items)
			{
				if (i->column() == 0) index.append(i->row());
			}
		}
		return index;
	}

	bool currentItem(int& table_index, int& row, int& column)
	{
		table_index = row = column = -1;
		table_index = currentRow();
		if (table_index > -1)
		{
			QTableWidget* t = table(table_index);
			if (t)
			{
				row = t->currentRow();
				column = t->currentColumn();
				if (row > -1 && column > -1) return true;
			}
		}
		return false;
	}

private:
	void setReSize()
	{
		for (int i = 0; i < rowCount(); i++)
		{
			QTableWidget* t = table(i);
			int rowHeight = t->horizontalHeader()->height();
			if (!t->viewport()->isHidden())
			{
				int rowCount = t->rowCount();
				for (int row = 0; row < rowCount; row++) rowHeight += t->rowHeight(row);
			}
			setRowHeight(i, rowHeight);
		}
	}

	void setReSelection(int table_index)
	{
		setCurrentCell(table_index, 0);
		for (size_t i = 0; i < rowCount(); i++) if (i != table_index) table(i)->clearSelection();
	}

	void tableVisable(QTableWidget* table)
	{
		if (table->viewport()->isHidden())
		{
			table->viewport()->setHidden(false);
			table->property("table_corner_label").value<QLabel*>()->setText(u8"▾");
		}
		else
		{
			table->viewport()->setHidden(true);
			table->property("table_corner_label").value<QLabel*>()->setText(u8"▸");
		}
	}

	bool eventFilter(QObject* sender, QEvent* event)
	{
		if (event->type() == QEvent::Timer) setReSize();
		if (event->type() == QEvent::MouseButtonRelease)
		{
			QVariant var = sender->property("parent_table");
			if (var.isValid())
			{
				tableVisable(var.value<QTableWidget*>());
				setReSize();
			}
		}
		return QTableWidget::eventFilter(sender, event);
	}
Q_SIGNALS:
	void currentChanged(int table, int row, int column) const;
	void itemClicked(int table, int row, int column) const;
	void itemChanged(int table, int row, int column) const;
	void headerClicked(int table, int column) const;
	void headerDoubleClicked(int table, int column) const;
};