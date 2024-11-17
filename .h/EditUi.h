#pragma once
#include <qcolordialog.h>
#include <qevent.h>
#include <qtimer.h>
#include <qmenu.h>
#include <qbuttongroup.h>
#include "QColorSelection.h"
#include "QPointSelection.h"
#include "QRectSelection.h"
#include "QPointView.h"
#include "QRectView.h"
#include "ui_EditUi.h"
#include "../static.h"

struct EditParam
{
	Macro* macro = 0; // all
	Actions* actions = 0; // currented
	bool firstLayer = false; // false: Running only / Ending only
	bool prevState = true; // false: Running state / true: Ending state
};

class EditUi : public QDialog
{
	Q_OBJECT;
	// Value range
	const int32 posMin = -10000;
	const int32 posMax = 10000;
	const int32 moveRandMax = 9999;
	const int32 delayMax = 999999;
	const int32 delayRandMax = 9999;
	const int32 loopCountMax = 9999;
	const int32 colorMax = 255;

	Ui::EditUiClass ui;

	QMenu* menu;
	QAction* muDel;
	QAction* muChange;
	QAction* muCut;
	QAction* muCopy;
	QAction* muPaste;

	QPointView pv;
	QRectView rv;
	QPoint msPos; // window move

	EditParam ep; // this Edit
	bool changing = false; // change state

public:
	// Actions is 0 = first layer
	EditUi(EditParam ep) : QDialog()
	{
		this->ep = ep;
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
		ReStyle();
	}

	void ReStyle()
	{
		setStyleSheet("");
		setStyleSheet(Global::qi.styles[Global::qi.set.style].style);
		ui.extab0->setStyleSheet(Global::qi.styles[Global::qi.set.style].style);
		ui.extab1->setStyleSheet(Global::qi.styles[Global::qi.set.style].style);
		ui.extab2->setStyleSheet(Global::qi.styles[Global::qi.set.style].style);
	}

private:
	void WidInit()
	{
		////////// Set default state
		// Button
		{
			// text
			ui.bnKeyAdd->setText(UI::etAdd);
			ui.bnStateAdd->setText(UI::etAdd);
			ui.bnMoveAdd->setText(UI::etAdd);
			ui.bnDelayAdd->setText(UI::etAdd);
			ui.bnLoopAdd->setText(UI::etAdd);
			ui.bnTextAdd->setText(UI::etAdd);
			ui.bnColorAdd->setText(UI::etAdd);
			ui.bnEndAdd->setText(UI::etAdd);
			ui.bnEndLoopAdd->setText(UI::etAdd);

			ui.bnStateEdit->setText(UI::etEdit);
			ui.bnLoopEdit->setText(UI::etEdit);
			ui.bnColorEdit->setText(UI::etEdit);
			ui.bnDel->setText(UI::etDel);
			// edit button is default disabled
			ui.bnStateEdit->setDisabled(true);
			ui.bnLoopEdit->setDisabled(true);
			ui.bnColorEdit->setDisabled(true);
		}
		// RadioButton group
		{
			QButtonGroup* actionRbs = new QButtonGroup(this);
			actionRbs->addButton(ui.rbRunning);
			actionRbs->addButton(ui.rbEnding);
			ui.rbRunning->setChecked(true);

			QButtonGroup* keyRbs = new QButtonGroup(this);
			keyRbs->addButton(ui.rbDown);
			keyRbs->addButton(ui.rbUp);
			keyRbs->addButton(ui.rbClick);
			ui.rbDown->setChecked(true);

			QButtonGroup* stateRbs = new QButtonGroup(this);
			stateRbs->addButton(ui.rbStateDown);
			stateRbs->addButton(ui.rbStateUp);
			ui.rbStateDown->setChecked(true);

			QButtonGroup* moveRbs = new QButtonGroup(this);
			moveRbs->addButton(ui.rbPos);
			moveRbs->addButton(ui.rbMove);
			ui.rbPos->setChecked(true);

			QButtonGroup* colorRbs = new QButtonGroup(this);
			colorRbs->addButton(ui.rbColorGet);
			colorRbs->addButton(ui.rbColorNot);
			ui.rbColorGet->setChecked(true);
		}
		// Value range
		{
			ui.etX->setValidator(new QIntValidator(0, posMax, this));
			ui.etY->setValidator(new QIntValidator(0, posMax, this));
			ui.etMoveRand->setValidator(new QIntValidator(0, moveRandMax, this));
			ui.etTime->setValidator(new QIntValidator(0, delayMax, this));
			ui.etDelayRand->setValidator(new QIntValidator(0, delayRandMax, this));
			ui.etCount->setValidator(new QIntValidator(0, loopCountMax, this));
			ui.etCountRand->setValidator(new QIntValidator(0, loopCountMax, this));
			ui.etR->setValidator(new QIntValidator(0, colorMax, this));
			ui.etG->setValidator(new QIntValidator(0, colorMax, this));
			ui.etB->setValidator(new QIntValidator(0, colorMax, this));
			ui.etCX->setValidator(new QIntValidator(0, colorMax, this));
			ui.etColorL->setValidator(new QIntValidator(0, posMax, this));
			ui.etColorT->setValidator(new QIntValidator(0, posMax, this));
			ui.etColorR->setValidator(new QIntValidator(0, posMax, this));
			ui.etColorB->setValidator(new QIntValidator(0, posMax, this));
		}
		// Table Menu
		{
			menu = new QMenu(this);
			muDel = new QAction(u8"删除", this);
			muChange = new QAction(u8"修改", this);
			muCut = new QAction(u8"剪切", this);
			muCopy = new QAction(u8"复制", this);
			muPaste = new QAction(u8"粘贴", this);

			menu->addAction(muDel);
			menu->addAction(muChange);
			menu->addAction(muCut);
			menu->addAction(muCopy);
			menu->addAction(muPaste);
			menu->setFont(QFont(u8"Microsoft YaHei"));

			connect(muDel, SIGNAL(triggered()), this, SLOT(OnMenuDel()));
			connect(muChange, SIGNAL(triggered()), this, SLOT(OnMenuChange()));
			connect(muCut, SIGNAL(triggered()), this, SLOT(OnMenuCut()));
			connect(muCopy, SIGNAL(triggered()), this, SLOT(OnMenuCopy()));
			connect(muPaste, SIGNAL(triggered()), this, SLOT(OnMenuPaste()));
		}
		// Table
		{
			ui.tbActions->setContextMenuPolicy(Qt::CustomContextMenu);
			ui.tbActions->setColumnCount(3);
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"功能"));
				ui.tbActions->setHorizontalHeaderItem(0, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"参数"));
				ui.tbActions->setHorizontalHeaderItem(1, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"备注"));
				ui.tbActions->setHorizontalHeaderItem(2, tbi);
			}
			ui.tbActions->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			ui.tbActions->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
			ui.tbActions->verticalHeader()->setDefaultSectionSize(0);
			ui.tbActions->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Fixed);
			ui.tbActions->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Interactive);
			ui.tbActions->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Interactive);
			ui.tbActions->setColumnWidth(0, 100);
			ui.tbActions->setColumnWidth(1, 300);
			ui.tbActions->setColumnWidth(2, 85);
		}
		// KeyEdit
		{
			ui.hkKey->Mode(0);
			ui.hkState->Mode(0);
			ui.hkKey->VirtualKey(VK_LBUTTON);
			ui.hkState->VirtualKey(VK_LBUTTON);
		}
		////////// #Set default state

		// load Window mode
		if (ep.macro->wndState) ep.macro->wi.Update(), SetTitle();

		// load Actions state
		if (!ep.firstLayer)
		{
			if (ep.prevState) ui.rbEnding->setChecked(true);
			else ui.rbRunning->setChecked(true);
			ui.rbRunning->setDisabled(true);
			ui.rbEnding->setDisabled(true);
		}
	}
	void WidEvent()
	{
		// Title
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
		connect(ui.bnWnd, SIGNAL(clicked()), this, SLOT(OnBnWnd()));
		connect(ui.chbWnd, SIGNAL(clicked()), this, SLOT(OnChbWnd()));
		connect(ui.chbChildWnd, SIGNAL(clicked()), this, SLOT(OnChbChildWnd()));

		// Table
		ui.tbActions->installEventFilter(this);
		ui.tbActions->viewport()->installEventFilter(this);
		connect(ui.rbRunning, SIGNAL(toggled(bool)), this, SLOT(OnRbRunning(bool))); // Running page
		connect(ui.rbEnding, SIGNAL(toggled(bool)), this, SLOT(OnRbEnding(bool))); // Ending page
		connect(ui.tbActions, SIGNAL(cellChanged(int, int)), this, SLOT(OnTbChanged(int, int)));
		connect(ui.tbActions, SIGNAL(cellClicked(int, int)), this, SLOT(OnTbClicked(int, int)));
		connect(ui.tbActions, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(OnTbSelect(QTableWidgetItem*, QTableWidgetItem*)));
		connect(ui.tbActions, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(OnTbDoubleClick(int, int))); // Action Changing
		connect(ui.tbActions, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnTableMenu(const QPoint&))); // Menu

		// Key
		connect(ui.bnKeyAdd, SIGNAL(clicked()), this, SLOT(OnBnKeyAdd()));
		// Key state
		connect(ui.bnStateAdd, SIGNAL(clicked()), this, SLOT(OnBnKeyStateAdd()));
		connect(ui.bnStateEdit, SIGNAL(clicked()), this, SLOT(OnBnKeyStateEdit()));
		// Mouse
		connect(ui.bnMoveAdd, SIGNAL(clicked()), this, SLOT(OnBnMoveAdd()));
		connect(ui.bnPos, SIGNAL(clicked()), this, SLOT(OnBnPos()));
		connect(ui.rbPos, SIGNAL(toggled(bool)), this, SLOT(OnRbPos(bool)));
		connect(ui.rbMove, SIGNAL(toggled(bool)), this, SLOT(OnRbMove(bool)));
		// Delay
		connect(ui.bnDelayAdd, SIGNAL(clicked()), this, SLOT(OnBnDelayAdd()));
		// Text
		connect(ui.bnTextAdd, SIGNAL(clicked()), this, SLOT(OnBnTextAdd()));
		// Loop
		connect(ui.bnLoopAdd, SIGNAL(clicked()), this, SLOT(OnBnLoopAdd()));
		connect(ui.bnLoopEdit, SIGNAL(clicked()), this, SLOT(OnBnLoopEdit()));
		// Color
		connect(ui.bnColorAdd, SIGNAL(clicked()), this, SLOT(OnBnColorAdd()));
		connect(ui.bnColorEdit, SIGNAL(clicked()), this, SLOT(OnBnColorEdit()));
		connect(ui.bnColorRect, SIGNAL(clicked()), this, SLOT(OnBnColorRect()));
		connect(ui.bnColorValue, SIGNAL(clicked()), this, SLOT(OnBnColorValue()));
		// Delete
		connect(ui.bnDel, SIGNAL(clicked()), this, SLOT(OnBnDel()));
		// End
		connect(ui.bnEndAdd, SIGNAL(clicked()), this, SLOT(OnBnEndAdd()));
		// EndLoop
		connect(ui.bnEndLoopAdd, SIGNAL(clicked()), this, SLOT(OnBnEndLoopAdd()));
		// RecoverPos
		connect(ui.bnRecoverPosAdd, SIGNAL(clicked()), this, SLOT(OnBnRecoverPosAdd()));
	}

	void TbUpdate()
	{
		disconnect(ui.tbActions, SIGNAL(cellChanged(int, int)), this, SLOT(OnTbChanged(int, int)));
		ui.tbActions->clearMask();
		ui.tbActions->setRowCount(ep.actions->size());
		ui.tbActions->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.tbActions->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tbActions->verticalHeader()->setDefaultSectionSize(0);

		for (uint32 u = 0; u < ep.actions->size(); u++)
		{
			QString ps;
			switch (ep.actions[0][u].type)
			{
			case Action::_end: ui.tbActions->setItem(u, 0, new QTableWidgetItem(UI::acEnd)); break;

			case Action::_delay:
			{
				ui.tbActions->setItem(u, 0, new QTableWidgetItem(UI::acWait));
				ps = QString::number(ep.actions[0][u].delay.ms);
				ps += u8"ㅤㅤ";
				ps += QString::number(ep.actions[0][u].delay.ex);
			}
			break;

			case Action::_key:
			{
				if (ep.actions[0][u].key.state == QiKey::up) ui.tbActions->setItem(u, 0, new QTableWidgetItem(UI::acUp));
				else if (ep.actions[0][u].key.state == QiKey::down) ui.tbActions->setItem(u, 0, new QTableWidgetItem(UI::acDown));
				else if (ep.actions[0][u].key.state == QiKey::click) ui.tbActions->setItem(u, 0, new QTableWidgetItem(UI::acClick));
				ps = QString::fromWCharArray(Input::Name(ep.actions[0][u].key.vk));
			}
			break;

			case Action::_mouse:
			{
				if (ep.actions[0][u].mouse.move) ui.tbActions->setItem(u, 0, new QTableWidgetItem(UI::acMove));
				else ui.tbActions->setItem(u, 0, new QTableWidgetItem(UI::acPos));
				ps = QString::number(ep.actions[0][u].mouse.x);
				ps += u8", ";
				ps += QString::number(ep.actions[0][u].mouse.y);
				ps += u8"ㅤㅤ";
				ps += QString::number(ep.actions[0][u].mouse.ex);
			}
			break;

			case Action::_text:
			{
				ui.tbActions->setItem(u, 0, new QTableWidgetItem(UI::acText));
				std::wstring text = ep.actions[0][u].text.str.str();
				ps = QString::fromWCharArray(text.substr(0, 32).c_str());
				if (text.length() > 31) ps += u8"...";
			}
			break;

			case Action::_color:
			{
				ui.tbActions->setItem(u, 0, new QTableWidgetItem(UI::acColor));
				ps = u8"(";
				ps += QString::number(ep.actions[0][u].color.rect.left);
				ps += u8",";
				ps += QString::number(ep.actions[0][u].color.rect.top);
				ps += u8",";
				ps += QString::number(ep.actions[0][u].color.rect.right);
				ps += u8",";
				ps += QString::number(ep.actions[0][u].color.rect.bottom);
				ps += u8")　(";
				ps += QString::number(ep.actions[0][u].color.rgbe.r);
				ps += u8",";
				ps += QString::number(ep.actions[0][u].color.rgbe.g);
				ps += u8",";
				ps += QString::number(ep.actions[0][u].color.rgbe.b);
				ps += u8",";
				ps += QString::number(ep.actions[0][u].color.rgbe.a);
				if (ep.actions[0][u].color.unfind) ps += u8")　未找到";
				else
				{
					ps += u8")　找到";
					if (ep.actions[0][u].color.move) ps += u8"并移动";
					else ps += u8"不移动";
				}
			}
			break;

			case Action::_loop:
			{
				ui.tbActions->setItem(u, 0, new QTableWidgetItem(UI::acLoop));
				if (ep.actions[0][u].loop.count)
				{
					ps = QString::number(ep.actions[0][u].loop.count);
					if (ep.actions[0][u].loop.rand)
					{
						ps += u8" ~ ";
						ps += QString::number(ep.actions[0][u].loop.rand);
					}
				}
				else if (ep.actions[0][u].loop.rand)
				{
					ps += u8"0 ~ ";
					ps += QString::number(ep.actions[0][u].loop.rand);
				}
				else ps = u8"无限";
			}
			break;

			case Action::_loopEnd: ui.tbActions->setItem(u, 0, new QTableWidgetItem(UI::acEndLoop)); break;

			case Action::_keyState:
			{
				ui.tbActions->setItem(u, 0, new QTableWidgetItem(UI::acKeyState));
				if (ep.actions[0][u].keyState.state) ps = u8"按下了　";
				else ps = u8"松开了　";
				ps += QString::fromWCharArray(Input::Name(ep.actions[0][u].keyState.vk));
			}
			break;

			case Action::_revocerPos: ui.tbActions->setItem(u, 0, new QTableWidgetItem(UI::acRecoverPos)); break;

			default: ui.tbActions->setItem(u, 0, new QTableWidgetItem(u8"加载失败")); break;
			}
			ui.tbActions->setItem(u, 1, new QTableWidgetItem(ps));
			ui.tbActions->setItem(u, 2, new QTableWidgetItem(QString::fromWCharArray(ep.actions[0][u].mark.c_str())));
			ui.tbActions->item(u, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbActions->item(u, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbActions->item(u, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
		connect(ui.tbActions, SIGNAL(cellChanged(int, int)), this, SLOT(OnTbChanged(int, int)));
		ui.tbActions->setStyleSheet(u8"QHeaderView::section,QScrollBar{background:transparent}");
	}

	// Window title
	void SetTitle()
	{
		ui.chbWnd->setChecked(ep.macro->wndState); OnChbWnd();
		ui.chbChildWnd->setChecked(ep.macro->wi.child);
		ui.etWndName->setText(QString::fromWCharArray(ep.macro->wi.wndName.c_str()));
	}

private: // Event
	// Window move event
	void mousePressEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) msPos = et->pos(); }
	void mouseMoveEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - msPos); }

	bool eventFilter(QObject* obj, QEvent* et)
	{
		if (obj == ui.tbActions)
		{
			if (et->type() == QEvent::KeyPress)
			{
				QKeyEvent* key = (QKeyEvent*)et;
				if (key->modifiers() == Qt::NoModifier)
				{
					if (key->key() == Qt::Key_Backspace)
					{
						ItemDel();
						return true;
					}
					else if (key->key() == Qt::Key_Delete)
					{
						ItemDel();
						return true;
					}
				}
				else if (key->modifiers() == Qt::ControlModifier)
				{
					if (key->key() == Qt::Key_C)
					{
						ItemCopy();
						return true;
					}
					else if (key->key() == Qt::Key_X)
					{
						ItemCut();
						return true;
					}
					else if (key->key() == Qt::Key_V)
					{
						ItemPaste();
						return true;
					}
				}
				else if (key->modifiers() == Qt::ShiftModifier)
				{
					if (key->key() == Qt::Key_Up)
					{
						ItemMove(true);
						return true;
					}
					else if (key->key() == Qt::Key_Down)
					{
						ItemMove(false);
						return true;
					}
				}
			}
		}
		else if (obj == ui.tbActions->viewport()) // move drop
		{
			if (et->type() == QEvent::Drop)
			{
				QDropEvent* drop = static_cast<QDropEvent*>(et);
				QTableWidgetItem* item = ui.tbActions->itemAt(drop->pos());
				int before = 0, after = 0;
				before = ui.tbActions->currentRow();
				if (before < 0) return 0;

				if (item) after = item->row();
				else after = ui.tbActions->rowCount() - 1;

				if (after < 0) return 0;

				ep.actions->Mov(before, after);
				TbUpdate();
				ui.tbActions->setCurrentItem(ui.tbActions->item(after, 0));
				return true;
			}
		}
		return false;
	}
	void showEvent(QShowEvent*)
	{
		ui.lbTitle->setText(windowTitle());
		TbUpdate();
	}

private slots:
	//////////////////////////// Title
	void OnBnClose() { close(); }
	void OnBnWnd()
	{
		ep.macro->wi = WindowSelection();
		SetTitle();
	}
	void OnChbWnd()
	{
		if (ui.chbWnd->isChecked())
		{
			ep.macro->wndState = true;
			ui.bnWnd->setDisabled(false);
			ui.chbChildWnd->setDisabled(false);
			ui.etWndName->setDisabled(false);
		}
		else
		{
			ep.macro->wndState = false;
			ui.bnWnd->setDisabled(true);
			ui.chbChildWnd->setDisabled(true);
			ui.etWndName->setDisabled(true);
		}
	}
	void OnChbChildWnd()
	{
		ep.macro->wi.child = ui.chbChildWnd->isChecked();
	}
	//////////////////////////// #Title


	//////////////////////////// Menu
	void OnMenuDel() { ItemDel(); }
	void OnMenuChange() { Changing(true); }
	void OnMenuCut() { ItemCut(); }
	void OnMenuCopy() { ItemCopy(); }
	void OnMenuPaste() { ItemPaste(); }
	//////////////////////////// #Menu


	//////////////////////////// Table
	void OnRbRunning(bool state)
	{
		if (state && ep.firstLayer)
		{
			ep.actions = &ep.macro->actions;
			TbUpdate();
		}
	}
	void OnRbEnding(bool state)
	{
		if (state && ep.firstLayer)
		{
			ep.actions = &ep.macro->actionsEnding;
			TbUpdate();
		}
	}
	void OnTbChanged(int row, int column)
	{
		if (row < 0) return;
		if (column != 2) return;
		ep.actions[0][row].mark = (LPCWSTR)(ui.tbActions->item(row, 2)->text().utf16());
	}
	void OnTbClicked(int row, int column)
	{
		ui.bnColorEdit->setDisabled(true);
		ui.bnLoopEdit->setDisabled(true);
		ui.bnStateEdit->setDisabled(true);

		if (row < 0) return;
		if (column == 2) ui.tbActions->editItem(ui.tbActions->item(row, column));
		else
		{
			if (ep.actions[0][row].type == Action::_mouse && !ep.actions[0][row].mouse.move)
			{
				rv.hide();
				POINT pt;
				if (ep.macro->wndState)
				{
					if (!ep.macro->wi.Update()) { ep.macro->wi = WindowSelection(); SetTitle(); }
					POINT rpt = WATR({ ep.actions[0][row].mouse.x, ep.actions[0][row].mouse.y }, ep.macro->wi.wnd);
					pt = Window::pos(ep.macro->wi.wnd);
					pt.x += rpt.x, pt.y += rpt.y;
				}
				else
				{
					pt = ATR({ ep.actions[0][row].mouse.x, ep.actions[0][row].mouse.y });
				}
				pv.Show(pt);
			}
			else if (ep.actions[0][row].type == Action::_color)
			{
				pv.hide();
				RECT rect;
				if (ep.macro->wndState)
				{
					if (!ep.macro->wi.Update()) { ep.macro->wi = WindowSelection(); SetTitle(); }
					rect = WATRR(ep.actions[0][row].color.rect, ep.macro->wi.wnd);
					POINT pt = Window::pos(ep.macro->wi.wnd);
					rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
				}
				else
				{
					rect = ATRR(ep.actions[0][row].color.rect);
				}
				rv.Show(rect);
				ui.bnColorEdit->setDisabled(false);
				ui.extabBox->setCurrentIndex(1);
			}
			else if (ep.actions[0][row].type == Action::_loop)
			{
				ui.bnLoopEdit->setDisabled(false);
			}
			else if (ep.actions[0][row].type == Action::_keyState)
			{
				ui.bnStateEdit->setDisabled(false);
				ui.extabBox->setCurrentIndex(0);
			}
			else if (ep.actions[0][row].type == Action::_text)
			{
				ui.extabBox->setCurrentIndex(2);
			}
		}
	}
	void OnTbSelect(QTableWidgetItem*, QTableWidgetItem*) { OnTbClicked(ui.tbActions->currentRow(), ui.tbActions->currentColumn()); }
	void OnTbDoubleClick(int row, int column) { Changing(true); }
	void OnTableMenu(const QPoint& pt)
	{
		QList<QTableWidgetItem*> items = ui.tbActions->selectedItems();
		if (items.size())
		{
			muDel->setEnabled(true);
			muChange->setEnabled(true);
			muCut->setEnabled(true);
			muCopy->setEnabled(true);
		}
		else
		{
			muDel->setDisabled(true);
			muChange->setDisabled(true);
			muCut->setDisabled(true);
			muCopy->setDisabled(true);
		}

		if (Global::qi.clipboard.size())
		{
			muPaste->setEnabled(true);
		}
		else
		{
			muPaste->setDisabled(true);
		}

		menu->exec(QCursor::pos());
	}
	//////////////////////////// #Table


	//////////////////////////// Action Button
	// Key
	void OnBnKeyAdd() { if (changing) ItemChange(Action::_key); else ItemAdd(Action::_key); }
	// Key state
	void OnBnKeyStateAdd() { if (changing) ItemChange(Action::_keyState); else ItemAdd(Action::_keyState); }
	void OnBnKeyStateEdit() {
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;

		if (ep.actions[0][pos].type != Action::_keyState) return;
		EditParam epc;
		epc.macro = ep.macro;
		epc.actions = &ep.actions[0][pos].keyState.next;
		epc.firstLayer = false;
		epc.prevState = ui.rbEnding->isChecked();
		EditUi edit(epc);
		edit.setWindowTitle(u8"编辑 - 按键状态");
		edit.exec();
		SetTitle();
	}
	// Mouse
	void OnBnMoveAdd() { if (changing) ItemChange(Action::_mouse); else ItemAdd(Action::_mouse); }
	void OnBnPos()
	{
		QPointSelection ps;
		POINT pt;
		if (ep.macro->wndState)
		{
			if (!ep.macro->wi.Update()) { ep.macro->wi = WindowSelection(); SetTitle(); }
			RECT wrect = Window::rect(ep.macro->wi.wnd);
			pt = ps.Start(wrect);
			pt = WRTA({ pt.x, pt.y }, ep.macro->wi.wnd);
		}
		else
		{
			pt = ps.Start();
			pt = RTA({ pt.x, pt.y });
		}
		SetEtPos({ pt.x, pt.y, -1 });
	}
	void OnRbPos(bool state)
	{
		if (state)
		{
			ui.etX->setValidator(new QIntValidator(0, posMax, this));
			ui.etY->setValidator(new QIntValidator(0, posMax, this));
			if (ui.etX->text().toInt() < 0) ui.etX->setText(u8"0");
			if (ui.etY->text().toInt() < 0) ui.etY->setText(u8"0");
			ui.bnPos->setText(u8"+");
			ui.bnPos->setDisabled(0);
		}
	}
	void OnRbMove(bool state)
	{
		if (state)
		{
			ui.etX->setValidator(new QIntValidator(posMin, posMax, this));
			ui.etY->setValidator(new QIntValidator(posMin, posMax, this));
			ui.bnPos->setText(u8"×");
			ui.bnPos->setDisabled(1);
		}
	}
	// Delay
	void OnBnDelayAdd() { if (changing) ItemChange(Action::_delay); else ItemAdd(Action::_delay); }
	// Text
	void OnBnTextAdd() { if (changing) ItemChange(Action::_text); else ItemAdd(Action::_text); }
	// Loop
	void OnBnLoopAdd() { if (changing) ItemChange(Action::_loop); else ItemAdd(Action::_loop); }
	void OnBnLoopEdit()
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;

		if (ep.actions[0][pos].type != Action::_loop) return;
		EditParam epc;
		epc.macro = ep.macro;
		epc.actions = &ep.actions[0][pos].loop.next;
		epc.firstLayer = false;
		epc.prevState = ui.rbEnding->isChecked();
		EditUi edit(epc);
		edit.setWindowTitle(u8"编辑 - 循环");
		edit.exec();
		SetTitle();
	}
	// Color
	void OnBnColorAdd() { if (changing) ItemChange(Action::_color); else ItemAdd(Action::_color); }
	void OnBnColorEdit()
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;

		if (ep.actions[0][pos].type != Action::_color) return;
		EditParam epc;
		epc.macro = ep.macro;
		epc.actions = &ep.actions[0][pos].color.next;
		epc.firstLayer = false;
		epc.prevState = ui.rbEnding->isChecked();
		EditUi edit(epc);
		edit.setWindowTitle(u8"编辑 - 查找颜色");
		edit.exec();
		SetTitle();
	}
	void OnBnColorRect()
	{
		QRectSelection rs;
		RECT rect;

		if (ep.macro->wndState)
		{
			if (!ep.macro->wi.Update()) { ep.macro->wi = WindowSelection(); SetTitle(); }
			RECT wrect = Window::rect(ep.macro->wi.wnd);
			rect = rs.Start(wrect);
			rect = WRTAR(rect, ep.macro->wi.wnd);
		}
		else
		{
			rect = rs.Start();
			rect = RTAR(rect);
		}

		SetEtColorRect(rect);
	}
	void OnBnColorValue()
	{
		QColorSelection cs;
		QColorDialog cd(cs.Start(), this);
		cd.exec();
		SetColor(RGB(cd.currentColor().red(), cd.currentColor().green(), cd.currentColor().blue()));
	}
	// Delete
	void OnBnDel() { ItemDel(); }
	// End
	void OnBnEndAdd() { if (changing) ItemChange(Action::_end); else ItemAdd(Action::_end); }
	// EndLoop
	void OnBnEndLoopAdd() { if (changing) ItemChange(Action::_loopEnd); else ItemAdd(Action::_loopEnd); }
	// RecoverPos
	void OnBnRecoverPosAdd() { if (changing) ItemChange(Action::_revocerPos); else ItemAdd(Action::_revocerPos); }
	//////////////////////////// #Action Button

private: // Widget data
	// Item
	void Changing(bool state)
	{
		if (state)
		{
			int pos = ui.tbActions->currentRow();
			if (pos < 0) return;

			ItemGet(pos);

			changing = state;
			{
				ui.bnKeyAdd->setText(UI::etChange);
				ui.bnStateAdd->setText(UI::etChange);
				ui.bnMoveAdd->setText(UI::etChange);
				ui.bnDelayAdd->setText(UI::etChange);
				ui.bnLoopAdd->setText(UI::etChange);
				ui.bnTextAdd->setText(UI::etChange);
				ui.bnColorAdd->setText(UI::etChange);
				ui.bnEndAdd->setText(UI::etChange);
				ui.bnEndLoopAdd->setText(UI::etChange);
				ui.bnRecoverPosAdd->setText(UI::etChange);
			}
		}
		else
		{
			changing = state;
			{
				ui.bnKeyAdd->setText(UI::etAdd);
				ui.bnStateAdd->setText(UI::etAdd);
				ui.bnMoveAdd->setText(UI::etAdd);
				ui.bnDelayAdd->setText(UI::etAdd);
				ui.bnLoopAdd->setText(UI::etAdd);
				ui.bnTextAdd->setText(UI::etAdd);
				ui.bnColorAdd->setText(UI::etAdd);
				ui.bnEndAdd->setText(UI::etAdd);
				ui.bnEndLoopAdd->setText(UI::etAdd);
				ui.bnRecoverPosAdd->setText(UI::etAdd);
			}
		}
	}
	void ItemMove(bool up)
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;

		if (up && ((pos - 1) >= 0))
		{
			ep.actions->Swp(pos, pos - 1);
			ui.tbActions->setCurrentItem(ui.tbActions->item(pos - 1, 0));
		}
		else if (!up && (pos + 1) < ui.tbActions->rowCount())
		{
			ep.actions->Swp(pos, pos + 1);
			ui.tbActions->setCurrentItem(ui.tbActions->item(pos + 1, 0));
		}
		TbUpdate();
	}
	void ItemGet(int32 pos)
	{
		switch (ep.actions[0][pos].type)
		{
		case Action::_delay: LoadDelay(ep.actions[0][pos]); break;
		case Action::_key: LoadKey(ep.actions[0][pos]); break;
		case Action::_mouse: LoadMouse(ep.actions[0][pos]); break;
		case Action::_text: LoadText(ep.actions[0][pos]); break;
		case Action::_color: LoadColor(ep.actions[0][pos]); break;
		case Action::_loop: LoadLoop(ep.actions[0][pos]); break;
		case Action::_keyState: LoadKeyState(ep.actions[0][pos]); break;
		}
	}
	void ItemSet(Action::ActionType type, int32 pos)
	{
		Action action;
		switch (type)
		{
		case Action::_end: action.type = Action::_end; break;
		case Action::_delay: action = GetDelay(); break;
		case Action::_key: action = GetKey(); break;
		case Action::_mouse: action = GetMouse(); break;
		case Action::_text: action = GetText(); break;
		case Action::_color: action = GetColor(); break;
		case Action::_loop: action = GetLoop(); break;
		case Action::_loopEnd: action.type = Action::_loopEnd; break;
		case Action::_keyState: action = GetKeyState(); break;
		case Action::_revocerPos: action.type = Action::_revocerPos; break;
		default: action.type = Action::_none; break;
		}

		// Save child
		if (ep.actions[0][pos].type == Action::_color)
		{
			if (type == Action::_color) action.color.next = ep.actions[0][pos].color.next;
			if (type == Action::_loop) action.loop.next = ep.actions[0][pos].color.next;
			if (type == Action::_keyState) action.keyState.next = ep.actions[0][pos].color.next;
		}
		else if (ep.actions[0][pos].type == Action::_loop)
		{
			if (type == Action::_color) action.color.next = ep.actions[0][pos].loop.next;
			if (type == Action::_loop) action.loop.next = ep.actions[0][pos].loop.next;
			if (type == Action::_keyState) action.loop.next = ep.actions[0][pos].loop.next;
		}
		else if (ep.actions[0][pos].type == Action::_keyState)
		{
			if (type == Action::_color) action.color.next = ep.actions[0][pos].keyState.next;
			if (type == Action::_loop) action.loop.next = ep.actions[0][pos].keyState.next;
			if (type == Action::_keyState) action.keyState.next = ep.actions[0][pos].keyState.next;
		}
		ep.actions[0][pos] = action;
	}
	void ItemAdd(Action::ActionType type)
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) pos = ep.actions->size();
		else if ((pos + 1) <= ep.actions->size()) pos++;

		ep.actions->InsNull(pos);
		ItemSet(type, pos);
		TbUpdate();
		ui.tbActions->setCurrentItem(ui.tbActions->item(pos, 0));
	}
	void ItemAdd(Action& action)
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) pos = ep.actions->size();
		else if ((pos + 1) <= ep.actions->size()) pos++;

		ep.actions->Ins(action, pos);

		TbUpdate();
		ui.tbActions->setCurrentItem(ui.tbActions->item(pos, 0));
	}
	void ItemChange(Action::ActionType type)
	{
		int pos = ui.tbActions->currentRow();
		if (pos < 0) return;

		std::wstring mark = ep.actions[0][pos].mark;
		ItemSet(type, pos);
		ep.actions[0][pos].mark = mark;

		TbUpdate();
		ui.tbActions->setCurrentItem(0);

		Changing(false);
	}
	void ItemDel()
	{
		QList<QTableWidgetItem*> items = ui.tbActions->selectedItems();
		if (!items.size()) return;

		List<uint32> itemPos;
		for (uint32 u = 0; u < items.size(); u++) if (items[u]->column() == 0) itemPos.Add(items[u]->row());
		ep.actions->Del(itemPos);

		TbUpdate();
		ui.tbActions->setCurrentItem(0);

		Changing(false);
	}
	void ItemCut()
	{
		QList<QTableWidgetItem*> items = ui.tbActions->selectedItems();
		if (!items.size()) return;

		Global::qi.clipboard.resize(0);
		for (uint32 u = 0; u < items.size(); u++) if (items[u]->column() == 0) Global::qi.clipboard.Add(ep.actions[0][items[u]->row()]);

		ItemDel();
	}
	void ItemCopy()
	{
		QList<QTableWidgetItem*> items = ui.tbActions->selectedItems();
		if (!items.size()) return;

		Global::qi.clipboard.resize(0);
		for (uint32 u = 0; u < items.size(); u++) if (items[u]->column() == 0) Global::qi.clipboard.Add(ep.actions[0][items[u]->row()]);
	}
	void ItemPaste()
	{
		int pos = ui.tbActions->currentRow();

		if (pos < 0) pos = ep.actions->size();
		else if ((pos + 1) <= ep.actions->size()) pos++;

		for (uint32 u = 0; u < Global::qi.clipboard.size(); u++)
		{
			ep.actions->Ins(Global::qi.clipboard[u], pos);
			pos++;
		}
		TbUpdate();
	}

	// Set Wid data
	void SetHkKey(byte key) { ui.hkKey->VirtualKey(key); }
	void SetHkState(byte key) { ui.hkState->VirtualKey(key); }
	void SetEtPos(RECT pt) { ui.etX->setText(QString::number(pt.left)); ui.etY->setText(QString::number(pt.top)); if (pt.right > -1) ui.etMoveRand->setText(QString::number(pt.right)); }
	void SetEtDelay(POINT pt) { ui.etTime->setText(QString::number(pt.x)); ui.etDelayRand->setText(QString::number(pt.y)); }
	void SetRbColorMode(bool mode) { if (mode) ui.rbColorNot->setChecked(1); else ui.rbColorGet->setChecked(1); }
	void SetChbColorMove(bool state) { ui.chbColorMove->setChecked(state); }
	void SetEtColorRect(RECT rect) { ui.etColorL->setText(QString::number(rect.left)); ui.etColorT->setText(QString::number(rect.top)); ui.etColorR->setText(QString::number(rect.right)); ui.etColorB->setText(QString::number(rect.bottom)); }
	void SetEtColorValue(Rgba color) {
		SetColor(color);
		ui.etCX->setText(QString::number(color.a));
	}
	void SetColor(Rgba color) {
		ui.etR->setText(QString::number(color.r));
		ui.etG->setText(QString::number(color.g));
		ui.etB->setText(QString::number(color.b));
		QString style = u8"background-color:rgb(";
		style += QString::number(color.r);
		style += u8",";
		style += QString::number(color.g);
		style += u8",";
		style += QString::number(color.b);
		style += u8");";
		ui.bnColorValue->setStyleSheet(style);
	}
	void SetEtLoopCount(uint32 count) { ui.etCount->setText(QString::number(count)); }
	void SetEtLoopCountRand(uint32 count) { ui.etCountRand->setText(QString::number(count)); }

	// Get data by Wid
	Action GetKey() {
		Action action(Action::_key);
		if (ui.rbDown->isChecked()) action.key.state = QiKey::down;
		else if (ui.rbUp->isChecked()) action.key.state = QiKey::up;
		else if (ui.rbClick->isChecked()) action.key.state = QiKey::click;
		action.key.vk = ui.hkKey->virtualKey();
		return action;
	}
	Action GetKeyState() {
		Action action(Action::_keyState);
		action.keyState.state = ui.rbStateDown->isChecked();
		action.keyState.vk = ui.hkState->virtualKey();
		return action;
	}
	Action GetMouse() {
		Action action(Action::_mouse);
		action.mouse.move = ui.rbMove->isChecked();
		int x = ui.etX->text().toInt();
		int y = ui.etY->text().toInt();
		int ex = ui.etMoveRand->text().toInt();
		if (x > posMax) x = posMax;
		if (y > posMax) y = posMax;
		if (ex > posMax) ex = posMax;
		if (x < posMin) x = posMin;
		if (y < posMin) y = posMin;
		action.mouse.x = x;
		action.mouse.y = y;
		action.mouse.ex = ex;
		return action;
	}
	Action GetDelay() {
		Action action(Action::_delay);
		int ms = ui.etTime->text().toInt();
		int ex = ui.etDelayRand->text().toInt();
		if (ms > delayMax) ms = delayMax;
		if (ex > delayMax) ex = delayRandMax;
		action.delay.ms = ms;
		action.delay.ex = ex;
		return action;
	}
	Action GetText() {
		Action action(Action::_text);
		action.text.str.cpy((LPCWSTR)(ui.etText->toPlainText().utf16()));
		return action;
	}
	Action GetColor() {
		Action action(Action::_color);
		action.color.unfind = ui.rbColorNot->isChecked();
		action.color.move = ui.chbColorMove->isChecked();
		{
			int l = ui.etColorL->text().toInt();
			int t = ui.etColorT->text().toInt();
			int r = ui.etColorR->text().toInt();
			int b = ui.etColorB->text().toInt();
			if (l > posMax) l = posMax;
			if (t > posMax) t = posMax;
			if (r > posMax) r = posMax;
			if (b > posMax) b = posMax;
			action.color.rect = { l, t, r, b };
		}
		{
			int r = ui.etR->text().toInt();
			int g = ui.etG->text().toInt();
			int b = ui.etB->text().toInt();
			int ex = 0;
			if (ui.etCX->text() == "") ex = 5;
			else ex = ui.etCX->text().toInt();
			if (r > colorMax) r = colorMax;
			if (g > colorMax) g = colorMax;
			if (b > colorMax) b = colorMax;
			if (ex > colorMax) ex = colorMax;
			action.color.rgbe.set(r, g, b, ex);
		}
		return action;
	}
	Action GetLoop()
	{
		Action action(Action::_loop);
		{
			int i = 1;
			if (ui.etCount->text() != "") i = ui.etCount->text().toInt();
			if (i > loopCountMax) i = loopCountMax;
			action.loop.count = i;
		}
		{
			int i = 0;
			if (ui.etCountRand->text() != "") i = ui.etCountRand->text().toInt();
			if (i > loopCountMax) i = loopCountMax;
			if (i < action.loop.count) i = action.loop.count;
			action.loop.rand = i;
		}
		return action;
	}

	// Set Wid data by Action
	void LoadKey(const Action& action) {
		if (action.key.state == QiKey::down) ui.rbDown->setChecked(1);
		else if (action.key.state == QiKey::up) ui.rbUp->setChecked(1);
		else if (action.key.state == QiKey::click) ui.rbClick->setChecked(1);
		SetHkKey(action.key.vk);
	}
	void LoadKeyState(const Action& action)
	{
		if (action.keyState.state) ui.rbStateDown->setChecked(true);
		if (!action.keyState.state) ui.rbStateUp->setChecked(true);
		SetHkState(action.keyState.vk);
	}
	void LoadMouse(const Action& action) {
		if (action.mouse.move)
		{
			ui.rbMove->setChecked(1);
			OnRbMove(1);
		}
		else
		{
			ui.rbPos->setChecked(1);
			OnRbPos(1);
		}
		SetEtPos({ (long)action.mouse.x, (long)action.mouse.y, (long)action.mouse.ex, 0 });
	}
	void LoadDelay(const Action& action) { SetEtDelay({ (long)action.delay.ms, (long)action.delay.ex }); }
	void LoadText(const Action& action) { ui.etText->setText(QString::fromWCharArray(action.text.str.str())); }
	void LoadColor(const Action& action) { SetRbColorMode(action.color.unfind); SetChbColorMove(action.color.move); SetEtColorRect(action.color.rect); SetEtColorValue(action.color.rgbe); }
	void LoadLoop(const Action& action) { SetEtLoopCount(action.loop.count); SetEtLoopCountRand(action.loop.rand); }
};