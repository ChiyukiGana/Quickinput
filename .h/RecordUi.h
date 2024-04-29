#pragma once
#include <qevent.h>
#include "ui_RecordUi.h"
#include "../static.h"

class RecordUi : public QDialog
{
	Q_OBJECT;
	Ui::RecordUiClass ui;
	Macro macro;
	HWND wnd = 0;
	long long tim = 0;
	bool start = false, keyTrigger = false;

public:
	RecordUi() : QDialog()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setMouseTracking(true);

		ui.bnStart->setText(UI::rcStart);
		ui.bnClose->setText(UI::rcClose);

		WidEvent();
		ReStyle();
	}

	void ReStyle()
	{
		setStyleSheet("");
		setStyleSheet(Global::qi.styles[Global::qi.set.style].style);
	}

	void Start(WndInfo* wi)
	{
		if (Global::qi.set.recKey)
		{
			std::wstring text(L"按下");
			text += Input::Name(Global::qi.set.recKey);
			text += L"开始录制";
			TipsWindow::Show(text, RGB(0x20, 0xFF, 0x20));
		}

		macro.mode = Macro::down;
		macro.count = 1;
		tim = 0;
		if (wi)
		{
			wnd = wi->wnd;
			macro.wi = *wi;
			macro.wndState = true;
			macro.name = NameFilter(L"窗口录制");
			POINT wpt = Window::pos(wnd);
			move(wpt.x, wpt.y);
			WndLock::Lock(wnd);
			exec();
			WndLock::UnLock();
		}
		else
		{
			macro.name = NameFilter(L"录制");
			exec();
		}
	}

	void AddItems(byte vk, bool state, POINT pt = { 0 })
	{
		if (!start) return;
		if (wnd && !IsWindowVisible(wnd))
		{
			start = false;
			Global::qi.rec = 0;
			TipsWindow::Hide();
			MsgBox::Error(L"窗口已失效");
			close();
		}

		if (tim)
		{
			// delay
			macro.actions.AddNull();
			macro.actions.Get().type = Action::_delay;
			macro.actions.Get().delay.ms = clock() - tim;
			tim = clock();
		}
		else tim = clock();

		// pos
		if (Input::Type(vk))
		{
			macro.actions.AddNull();
			macro.actions.Get().type = Action::_mouse;
			if (wnd)
			{
				POINT wpt = Window::pos(wnd);
				pt = WRTA({ pt.x - wpt.x, pt.y - wpt.y }, wnd);
			}
			else pt = RTA(pt);
			macro.actions.Get().mouse.x = pt.x;
			macro.actions.Get().mouse.y = pt.y;
		}

		// key
		{
			macro.actions.AddNull();
			macro.actions.Get().type = Action::_key;
			macro.actions.Get().key.vk = vk;
			if (state) macro.actions.Get().key.state = QiKey::down;
			else macro.actions.Get().key.state = QiKey::up;
		}
	}

	bool State() const { return start; }

	void BeginRec() { keyTrigger = true; OnBnStart(); keyTrigger = false; }

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
		// end
		if (start)
		{
			Global::qi.rec = 0;

#ifdef _DEBUG
			MsgWnd::log(L"Rec ending");
#endif

			TipsWindow::Hide();
			if (!keyTrigger) macro.actions.DelBack(6);
			Global::qi.macros.Add(macro);
			SaveMacro(Global::qi.macros.Get());

#ifdef _DEBUG
			MsgWnd::log(L"Rec saved");
#endif

			close();
		}
		// start
		else
		{

#ifdef _DEBUG
			MsgWnd::log(L"Rec starting");
#endif

			if (wnd && !IsWindowVisible(wnd))
			{
				Global::qi.rec = 0;
				TipsWindow::Hide();
				MsgBox::Error(L"窗口已失效");
				close();
			}
			if (Global::qi.set.recKey)
			{
				std::wstring text(L"按下");
				text += Input::Name(Global::qi.set.recKey);
				text += L"停止录制";
				TipsWindow::Show(text);
			}

			ui.bnStart->setText(UI::rcStop);
			start = true;
		}
	}
	void OnBnClose()
	{

#ifdef _DEBUG
		MsgWnd(L"Rec close");
#endif

		TipsWindow::Hide();
		Global::qi.rec = 0;
		Global::qi.macros.DelBack();
		close();
	}
};