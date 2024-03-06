#pragma once
#include <qevent.h>
#include "ui_RecordUi.h"
#include "../static.h"

class RecordUi : public QDialog
{
	Q_OBJECT;
	Ui::RecordUiClass ui;
	Actions* actions = 0;
	HWND wnd = 0;
	long long tim = 0;
	bool start = 0, keyTrigger = 0;

public:
	RecordUi(Actions* actions) : QDialog()
	{
		this->actions = actions;
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setMouseTracking(true);

		ui.bnStart->setText(UI::rcStart);
		ui.bnClose->setText(UI::rcClose);

		WidEvent();
	}

	void Start(HWND wnd)
	{
		if (Global::qi.set.recKey)
		{
			std::wstring text(L"按下");
			text += Input::Name(Global::qi.set.recKey);
			text += L"开始录制";
			TipsWindow::Show(text, RGB(0x20, 0xFF, 0x20));
		}

		this->wnd = wnd;
		tim = 0;
		if (wnd)
		{
			POINT wpt = Window::pos(wnd);
			move(wpt.x, wpt.y);
			WndLock::Lock(wnd);
			exec();
			WndLock::UnLock();
		}
		else exec();
	}

	void AddItems(byte vk, bool state, POINT pt = { 0 })
	{
		if (!start) return;
		if (tim)
		{
			// delay
			actions->AddNull();
			actions->Get().type = Action::_delay;
			actions->Get().delay.ms = clock() - tim;
			tim = clock();
		}
		else tim = clock();

		// pos
		if (Input::Type(vk))
		{
			actions->AddNull();
			actions->Get().type = Action::_mouse;
			if (wnd)
			{
				POINT wpt = Window::pos(wnd);
				pt = WRTA({ pt.x - wpt.x, pt.y - wpt.y }, wnd);
			}
			else pt = RTA(pt);
			actions->Get().mouse.x = pt.x;
			actions->Get().mouse.y = pt.y;
		}

		// key
		{
			actions->AddNull();
			actions->Get().type = Action::_key;
			actions->Get().key.vk = vk;
			if (state) actions->Get().key.state = QiKey::down;
			else actions->Get().key.state = QiKey::up;
		}
	}

	bool State() const { return start; }

	void BeginRec() { keyTrigger = 1; OnBnStart(); keyTrigger = 0; }

private:
	void WidEvent()
	{
		connect(ui.bnStart, SIGNAL(clicked()), this, SLOT(OnBnStart()));
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
	}

	void mouseMoveEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - QPoint(5, 15)); }

public slots:
	void OnBnStart()
	{
		if (start)
		{
			Global::qi.rec = 0;
			if (!keyTrigger) actions->DelBack(6);

			if (wnd) Global::qi.macros.Get().name = NameFilter(L"窗口录制");
			else Global::qi.macros.Get().name = NameFilter(L"录制");
			SaveMacro(Global::qi.macros.Get());
			TipsWindow::Hide();
			close();
		}
		else
		{
			if (Global::qi.set.recKey)
			{
				std::wstring text(L"按下");
				text += Input::Name(Global::qi.set.recKey);
				text += L"停止录制";
				TipsWindow::Show(text);
			}

			ui.bnStart->setText(UI::rcStop);
			start = 1;
		}
	}
	void OnBnClose()
	{
		TipsWindow::Hide();
		Global::qi.rec = 0;
		Global::qi.macros.DelBack();
		close();
	}
};