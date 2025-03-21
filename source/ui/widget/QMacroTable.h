#pragma once
#include <qvariant.h>
#include <qevent.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtablewidget.h>
#include <qheaderview.h>
#include <qscrollbar.h>
#include <qlayout.h>

class QMacroTable : public QTableWidget
{
	Q_OBJECT;
	bool selfAction = false;
public:
	QMacroTable(QWidget* parent = nullptr) : QTableWidget(parent)
	{
		setFocusPolicy(Qt::NoFocus);
		setProperty("setFoldEnabled", true);
		horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
		horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
		horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
		verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		verticalHeader()->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
		verticalHeader()->setHidden(true);
		setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
		setShowGrid(false);
		setDropIndicatorShown(false);
		setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
		setStyleSheet("QTableCornerButton::section,QHeaderView::section{background-color:rgba(0,0,0,0)}QScrollBar{background:transparent}");
		if ("StyleGroup")
		{
			setProperty("group", "group_table");
			horizontalHeader()->setProperty("group", "group_table_header");
			verticalHeader()->setProperty("group", "group_table_header");
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
						corner->setProperty("group", "group_table_header");
						widget->setProperty("group", "group_table_header");
						break;
					}
					else if (name == "QLineEdit")
					{
						QLineEdit* lineEdit = (QLineEdit*)obj;
						lineEdit->setProperty("group", "group_table_header");
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
			table->setFocusPolicy(Qt::NoFocus);
			table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
			table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
			table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
			table->verticalHeader()->setDefaultSectionSize(0);
			table->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
			table->setShowGrid(false);
			table->setDropIndicatorShown(false);
			table->setCornerButtonEnabled(false);
			table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			table->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked);
			table->setColumnCount(1);
			table->setStyleSheet("QTableCornerButton::section,QHeaderView::section{background-color:rgba(0,0,0,0)}");
			table->viewport()->setStyleSheet("background-color:rgba(0,0,0,0)");
			if ("StyleGroup")
			{
				table->setProperty("group", "macro_table");
				table->horizontalHeader()->setProperty("group", "macro_table_header");
				table->verticalHeader()->setProperty("group", "macro_table_header");
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
							QLabel* label = new QLabel(corner);
							QFont font;
							font.setPixelSize(16);
							label->setFont(font);
							label->setAlignment(Qt::AlignCenter);
							label->setText(u8"▾");
							box->addWidget(label);
							box->setMargin(0);
							table->setProperty("table_corner_label", QVariant::fromValue((QLabel*)label));
							corner->setProperty("group", "macro_table_header");
							label->setProperty("group", "macro_table_header");
							break;
						}
						else if (name == "QLineEdit")
						{
							QLineEdit* lineEdit = (QLineEdit*)obj;
							lineEdit->setProperty("group", "macro_table_header");
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
				setReSelection(i);
				int row = currentRow();
				if (i != row) currentChanged(i, table->currentRow(), table->currentColumn());
				});
			connect(table->horizontalHeader(), &QHeaderView::sectionClicked, this, [this, i, table](int index) {
				if (selfAction) return;
				setReSelection(i);
				int row = currentRow();
				headerClicked(i, index);
				if (i != row) currentChanged(i, table->currentRow(), table->currentColumn());
				});
			connect(table->horizontalHeader(), &QHeaderView::sectionDoubleClicked, this, [this, i](int index) {
				if (selfAction) return;
				setReSelection(i);
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
		selfAction = true;
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
		selfAction = false;
	}

	void setReSelection(int table_index)
	{
		selfAction = true;
		setCurrentCell(table_index, 0);
		for (size_t i = 0; i < rowCount(); i++) if (i != table_index) table(i)->clearSelection();
		selfAction = false;
	}

	void tableVisable(QTableWidget* table)
	{
		selfAction = true;
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
		selfAction = false;
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