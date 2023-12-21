#pragma once
#pragma execution_character_set("utf-8")

#include <qevent.h>
#include "ui_RecordUi.h"
#include "QuickInputDef.h"

class RecordUi : public QDialog
{
	Q_OBJECT

public:

	RecordUi(QWidget* main = 0, QuickInputStruct* qis = 0, List<Item>* items = 0) : QDialog(main)
	{
		this->main = main;
		this->qis = qis;
		this->items = items;
		SIZE scr = System::screenSize();

		screen.setX(scr.cx);
		screen.setY(scr.cy);
		ui.setupUi(this);
		setParent(0);
		setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setMouseTracking(true);

		ui.bnStart->setText(UI::rcStart);
		ui.bnClose->setText(UI::rcClose);

		ControlEvent();
	}

	void AddItems(BYTE type, BYTE vk, POINT pos)
	{
		Item item;

		if (!begin)
		{
			tim = clock();
			begin = 1;
		}

		// delay
		item.type = 0;
		item.b = clock() - tim;
		items[0].Add(item);
		tim = clock();

		// pos
		if (Input::Type(vk))
		{
			POINT abs = RelToAbs(pos);
			item.type = 4;
			item.b = abs.x;
			item.c = abs.y;
			items[0].Add(item);
		}
		// key
		item.type = type;
		item.a = vk;
		items[0].Add(item);
	}

	bool State() { return start; }

	void EndRec()
	{
		if (start)
		{
			items[0].Add();
			items[0].Add();
		}
		else
		{
			items[0].Emp();
		}
		OnBnStart();
	}

private:

	Ui::RecordUiClass ui;
	QWidget* main;
	QPoint screen;
	QuickInputStruct* qis;
	List<Item>* items;
	long long tim = 0;
	bool start = 0, begin = 0;

	void ControlEvent()
	{
		connect(ui.bnStart, SIGNAL(clicked()), this, SLOT(OnBnStart()));
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
	}

	void mouseMoveEvent(QMouseEvent* et)
	{
		if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - QPoint(5, 15));
	}

public slots:

	void OnBnStart()
	{
		if (start)
		{
			qis->rec = 0;
			items[0].Del(items[0].len() - 1);
			items[0].Del(items[0].len() - 1);
			items[0].Del(items[0].len() - 1);
			items[0].Del(items[0].len() - 1);
			items[0].Del(items[0].len() - 1);
			items[0].Del(items[0].len() - 1);

			qis->scripts[qis->scripts.len() - 1].name = NameFilter(qis, L"录制");
			SaveScript(qis->scripts[qis->scripts.len() - 1]);
			close();
		}
		else
		{
			TipsWindow::Hide();
			items[0].Emp();
			ui.bnStart->setText(UI::rcStop);
			start = 1;
			qis->rec = this;
		}
	}

	void OnBnClose()
	{
		TipsWindow::Hide();
		qis->rec = 0;
		qis->scripts.Del(qis->scripts.len() - 1);
		main->show();
		close();
	}
};