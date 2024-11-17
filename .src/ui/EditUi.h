#pragma once
#include <QtWinExtras/qwinfunctions.h>
#include <qbuttongroup.h>
#include <qcolordialog.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qevent.h>
#include <qtimer.h>
#include <qmenu.h>
#include "../widget/QColorSelection.h"
#include "../widget/QPointSelection.h"
#include "../widget/QRectSelection.h"
#include "../widget/QPointView.h"
#include "../widget/QRectView.h"
#include "ui_EditUi.h"
#include "header.h"

struct EditParam
{
	Macro* macro = 0;
	Actions* actions = 0;
	bool child = false;
};

class EditUi : public QDialog
{
	Q_OBJECT;
	// Value range
	const int32 posMin = -10000;
	const int32 posMax = 10000;
	const int32 moveRandMax = 9999;
	const int32 delayMax = 999999;
	const int32 loopCountMax = 9999;
	const int32 colorMax = 255;
	const int32 imageSimMax = 99;
	const int32 popTextTimeMax = 9999;

	Ui::EditUiClass ui;

	QMenu* menu;
	QAction* muDel;
	QAction* muChange;
	QAction* muCut;
	QAction* muCopy;
	QAction* muPaste;

	QPointView pv;
	QRectView rv;
	QPoint msPos;

	RgbMap rgbMap;

	Macro* macro = 0;
	Actions* actions = 0;
	bool child = false;
	bool changing = false;

public:
	EditUi(EditParam ep) : QDialog()
	{
		macro = ep.macro, actions = ep.actions, child = ep.child;

		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
	}

	void ReStyle()
	{
		setStyleSheet("");
		setStyleSheet(qis.themes[qis.set.theme].style);
	}

private:
	void WidInit()
	{
		////////// Set default state
		// Button
		{
			// text
			ui.bnKeyAdd->setText(qis.ui.etAdd);
			ui.bnStateAdd->setText(qis.ui.etAdd);
			ui.bnMoveAdd->setText(qis.ui.etAdd);
			ui.bnDelayAdd->setText(qis.ui.etAdd);
			ui.bnLoopAdd->setText(qis.ui.etAdd);
			ui.bnTextAdd->setText(qis.ui.etAdd);
			ui.bnColorAdd->setText(qis.ui.etAdd);
			ui.bnImageAdd->setText(qis.ui.etAdd);
			ui.bnEndAdd->setText(qis.ui.etAdd);
			ui.bnEndLoopAdd->setText(qis.ui.etAdd);
			ui.bnStateEdit->setText(qis.ui.etEdit);
			ui.bnLoopEdit->setText(qis.ui.etEdit);
			ui.bnColorEdit->setText(qis.ui.etEdit);
			ui.bnImageEdit->setText(qis.ui.etEdit);
			// no shortcut key
			ui.bnClose->setShortcut(Qt::Key_unknown);
			ui.bnRun->setShortcut(Qt::Key_unknown);
			ui.bnPos->setShortcut(Qt::Key_unknown);
			ui.bnColorRect->setShortcut(Qt::Key_unknown);
			ui.bnColorValue->setShortcut(Qt::Key_unknown);
			ui.bnImageRect->setShortcut(Qt::Key_unknown);
			ui.bnImageShot->setShortcut(Qt::Key_unknown);

			ui.bnKeyAdd->setShortcut(Qt::Key_unknown);
			ui.bnStateAdd->setShortcut(Qt::Key_unknown);
			ui.bnMoveAdd->setShortcut(Qt::Key_unknown);
			ui.bnDelayAdd->setShortcut(Qt::Key_unknown);
			ui.bnLoopAdd->setShortcut(Qt::Key_unknown);
			ui.bnTextAdd->setShortcut(Qt::Key_unknown);
			ui.bnColorAdd->setShortcut(Qt::Key_unknown);
			ui.bnImageAdd->setShortcut(Qt::Key_unknown);
			ui.bnEndAdd->setShortcut(Qt::Key_unknown);
			ui.bnEndLoopAdd->setShortcut(Qt::Key_unknown);
			ui.bnStateEdit->setShortcut(Qt::Key_unknown);
			ui.bnLoopEdit->setShortcut(Qt::Key_unknown);
			ui.bnColorEdit->setShortcut(Qt::Key_unknown);
			ui.bnImageEdit->setShortcut(Qt::Key_unknown);
			// edit button is default disabled
			ui.bnStateEdit->setDisabled(true);
			ui.bnLoopEdit->setDisabled(true);
			ui.bnColorEdit->setDisabled(true);
			ui.bnImageEdit->setDisabled(true);
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
			ui.rbClick->setChecked(true);

			QButtonGroup* stateRbs = new QButtonGroup(this);
			stateRbs->addButton(ui.rbStateDown);
			stateRbs->addButton(ui.rbStateUp);
			ui.rbStateDown->setChecked(true);

			QButtonGroup* moveRbs = new QButtonGroup(this);
			moveRbs->addButton(ui.rbPos);
			moveRbs->addButton(ui.rbMove);
			ui.rbPos->setChecked(true);

			QButtonGroup* colorRbs = new QButtonGroup(this);
			colorRbs->addButton(ui.rbColorFind);
			colorRbs->addButton(ui.rbColorNFind);
			ui.rbColorFind->setChecked(true);

			QButtonGroup* imageRbs = new QButtonGroup(this);
			imageRbs->addButton(ui.rbImageFind);
			imageRbs->addButton(ui.rbImageNFind);
			ui.rbImageFind->setChecked(true);
		}
		// Value range
		{
			ui.etX->setValidator(new QIntValidator(0, posMax, this));
			ui.etY->setValidator(new QIntValidator(0, posMax, this));
			ui.etMoveRand->setValidator(new QIntValidator(0, moveRandMax, this));
			ui.etDelayMin->setValidator(new QIntValidator(0, delayMax, this));
			ui.etDelayMax->setValidator(new QIntValidator(0, delayMax, this));
			ui.etCountMin->setValidator(new QIntValidator(0, loopCountMax, this));
			ui.etCountMax->setValidator(new QIntValidator(0, loopCountMax, this));
			ui.etColorRed->setValidator(new QIntValidator(0, colorMax, this));
			ui.etColorGreen->setValidator(new QIntValidator(0, colorMax, this));
			ui.etColorBlue->setValidator(new QIntValidator(0, colorMax, this));
			ui.etColorSim->setValidator(new QIntValidator(0, colorMax, this));
			ui.etColorL->setValidator(new QIntValidator(0, posMax, this));
			ui.etColorT->setValidator(new QIntValidator(0, posMax, this));
			ui.etColorR->setValidator(new QIntValidator(0, posMax, this));
			ui.etColorB->setValidator(new QIntValidator(0, posMax, this));
			ui.etImageL->setValidator(new QIntValidator(0, posMax, this));
			ui.etImageT->setValidator(new QIntValidator(0, posMax, this));
			ui.etImageR->setValidator(new QIntValidator(0, posMax, this));
			ui.etImageB->setValidator(new QIntValidator(0, posMax, this));
			ui.etImageSim->setValidator(new QIntValidator(0, imageSimMax, this));
			ui.etPopTextTime->setValidator(new QIntValidator(0, popTextTimeMax, this));
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
			ui.tbActions->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			ui.tbActions->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
			ui.tbActions->verticalHeader()->setDefaultSectionSize(0);
			ui.tbActions->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Fixed);
			ui.tbActions->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Interactive);
			ui.tbActions->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Interactive);
			ui.tbActions->setColumnWidth(0, 100);
			ui.tbActions->setColumnWidth(1, 300);
			ui.tbActions->setColumnWidth(2, 180);
		}
		// KeyEdit
		{
			ui.hkKey->Mode(0);
			ui.hkState->Mode(0);
			ui.hkKey->VirtualKey(VK_LBUTTON);
			ui.hkState->VirtualKey(VK_LBUTTON);
		}

		ui.lbImageView->setScaledContents(true);
		////////// #Set default state

		// load Window mode
		if (macro->wndState) macro->wi.Update(), SetTitle();

		// is child
		if (child) ui.rbRunning->setVisible(false), ui.rbEnding->setVisible(false);
	}
	void WidEvent()
	{
		// Title
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
		connect(ui.bnRun, SIGNAL(clicked()), this, SLOT(OnBnRun()));
		connect(ui.bnWndSelect, SIGNAL(clicked()), this, SLOT(OnBnWndSelect()));
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
		connect(ui.etDelayMin, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtDelayMin(const QString&)));
		// Text
		connect(ui.bnTextAdd, SIGNAL(clicked()), this, SLOT(OnBnTextAdd()));
		// Loop
		connect(ui.bnLoopAdd, SIGNAL(clicked()), this, SLOT(OnBnLoopAdd()));
		connect(ui.bnLoopEdit, SIGNAL(clicked()), this, SLOT(OnBnLoopEdit()));
		connect(ui.etCountMin, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtCountMin(const QString&)));
		// Color
		connect(ui.bnColorAdd, SIGNAL(clicked()), this, SLOT(OnBnColorAdd()));
		connect(ui.bnColorEdit, SIGNAL(clicked()), this, SLOT(OnBnColorEdit()));
		connect(ui.bnColorRect, SIGNAL(clicked()), this, SLOT(OnBnColorRect()));
		connect(ui.bnColorValue, SIGNAL(clicked()), this, SLOT(OnBnColorValue()));
		// End
		connect(ui.bnEndAdd, SIGNAL(clicked()), this, SLOT(OnBnEndAdd()));
		// EndLoop
		connect(ui.bnEndLoopAdd, SIGNAL(clicked()), this, SLOT(OnBnEndLoopAdd()));
		// RecoverPos
		connect(ui.bnRememberPosAdd, SIGNAL(clicked()), this, SLOT(bnRememberPosAdd()));
		connect(ui.bnRecoverPosAdd, SIGNAL(clicked()), this, SLOT(OnBnRecoverPosAdd()));
		// Image
		connect(ui.bnImageAdd, SIGNAL(clicked()), this, SLOT(OnBnImageAdd()));
		connect(ui.bnImageEdit, SIGNAL(clicked()), this, SLOT(OnBnImageEdit()));
		connect(ui.bnImageRect, SIGNAL(clicked()), this, SLOT(OnBnImageRect()));
		connect(ui.bnImageShot, SIGNAL(clicked()), this, SLOT(OnBnImageShot()));
		// PopText
		connect(ui.bnPopTextAdd, SIGNAL(clicked()), this, SLOT(OnBnPopTextAdd()));
	}

	// Window title
	void SetTitle()
	{
		ui.chbWnd->setChecked(macro->wndState); OnChbWnd();
		ui.chbChildWnd->setChecked(macro->wi.child);
		ui.etWndName->setText(QString::fromWCharArray(macro->wi.wndName.c_str()));
	}

	void NextEdit()
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		QString title;
		switch (actions->at(p).type)
		{
		case Action::_loop: title = u8"编辑 - 循环"; break;
		case Action::_color: title = u8"编辑 - 查找颜色"; break;
		case Action::_keyState: title = u8"编辑 - 按键状态"; break;
		case Action::_image: title = u8"编辑 - 查找图片"; break;
		}
		EditParam epc;
		epc.macro = macro;
		epc.child = true;
		epc.actions = &actions->at(p).next;
		EditUi edit(epc); edit.setWindowTitle(title);
		QPoint pt = pos();
		move(-10000, -10000);
		edit.move(pt);
		edit.exec();
		move(pt);
	}

	void TbUpdate()
	{
		disconnect(ui.tbActions, SIGNAL(cellChanged(int, int)), this, SLOT(OnTbChanged(int, int)));
		ui.tbActions->clearMask();
		ui.tbActions->setRowCount(actions->size());
		ui.tbActions->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.tbActions->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tbActions->verticalHeader()->setDefaultSectionSize(0);
		for (size_t i = 0; i < actions->size(); i++)
		{
			QString ps;
			switch (actions->at(i).type)
			{
			case Action::_end: ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acEnd)); break;

			case Action::_delay:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acWait));
				if (actions->at(i).d.delay.tmin != actions->at(i).d.delay.tmax)
				{
					ps = QString::number(actions->at(i).d.delay.tmin);
					ps += u8" ~ ";
					ps += QString::number(actions->at(i).d.delay.tmax);
				}
				else ps = QString::number(actions->at(i).d.delay.tmin);
			}
			break;

			case Action::_key:
			{
				if (actions->at(i).d.key.state == QiKey::up) ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acUp));
				else if (actions->at(i).d.key.state == QiKey::down) ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acDown));
				else if (actions->at(i).d.key.state == QiKey::click) ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acClick));
				ps = QString::fromWCharArray(Input::Name(actions->at(i).d.key.vk));
			}
			break;

			case Action::_mouse:
			{
				if (actions->at(i).d.mouse.move) ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acMove));
				else ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acPos));
				ps = QString::number(actions->at(i).d.mouse.x);
				ps += u8" - ";
				ps += QString::number(actions->at(i).d.mouse.y);
				if (actions->at(i).d.mouse.ex)
				{
					ps += u8"ㅤㅤ随机：";
					ps += QString::number(actions->at(i).d.mouse.ex);
				}
				if (actions->at(i).d.mouse.track)
				{
					ps += u8"ㅤㅤ轨迹：";
					ps += QString::number(actions->at(i).d.mouse.speed);
				}
			}
			break;

			case Action::_text:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acText));
				std::wstring text = actions->at(i).d.text.str.str();
				ps = QString::fromWCharArray(text.substr(0, 32).c_str());
				if (text.length() > 31) ps += u8"...";
			}
			break;

			case Action::_color:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acColor));
				ps = u8"(";
				ps += QString::number(actions->at(i).d.color.rect.left);
				ps += u8",";
				ps += QString::number(actions->at(i).d.color.rect.top);
				ps += u8",";
				ps += QString::number(actions->at(i).d.color.rect.right);
				ps += u8",";
				ps += QString::number(actions->at(i).d.color.rect.bottom);
				ps += u8")　(";
				ps += QString::number(actions->at(i).d.color.rgbe.r);
				ps += u8",";
				ps += QString::number(actions->at(i).d.color.rgbe.g);
				ps += u8",";
				ps += QString::number(actions->at(i).d.color.rgbe.b);
				ps += u8",";
				ps += QString::number(actions->at(i).d.color.rgbe.a);
				if (actions->at(i).d.color.unfind) ps += u8")　未找到";
				else
				{
					ps += u8")　找到";
					if (actions->at(i).d.color.move) ps += u8"并移动";
					else ps += u8"不移动";
				}
			}
			break;

			case Action::_loop:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acLoop));
				if ((actions->at(i).d.loop.cmin == 0 && actions->at(i).d.loop.cmax == 0))
					ps = u8"无限";
				else if (actions->at(i).d.loop.cmin == actions->at(i).d.loop.cmax)
					ps = QString::number(actions->at(i).d.loop.cmin);
				else
				{
					ps = QString::number(actions->at(i).d.loop.cmin);
					ps += u8" ~ ";
					ps += QString::number(actions->at(i).d.loop.cmax);
				}
			}
			break;

			case Action::_loopEnd: ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acEndLoop)); break;

			case Action::_keyState:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acKeyState));
				if (actions->at(i).d.keyState.state) ps = u8"按下了　";
				else ps = u8"松开了　";
				ps += QString::fromWCharArray(Input::Name(actions->at(i).d.keyState.vk));
			}
			break;

			case Action::_revocerPos: ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acRecoverPos)); break;

			case Action::_image:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acImage));
				ps = u8"(";
				ps += QString::number(actions->at(i).d.image.rect.left);
				ps += u8",";
				ps += QString::number(actions->at(i).d.image.rect.top);
				ps += u8",";
				ps += QString::number(actions->at(i).d.image.rect.right);
				ps += u8",";
				ps += QString::number(actions->at(i).d.image.rect.bottom);
				ps += u8")　(";
				ps += QString::number(actions->at(i).d.image.map.width());
				ps += u8"x";
				ps += QString::number(actions->at(i).d.image.map.height());
				ps += u8")　";
				ps += QString::number(actions->at(i).d.image.sim);
				if (actions->at(i).d.image.unfind) ps += u8"　未找到";
				else
				{
					ps += u8"　找到";
					if (actions->at(i).d.image.move) ps += u8"并移动";
					else ps += u8"不移动";
				}
			}
			break;

			case Action::_popText:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acPopText));
				ps += actions->at(i).d.popText.str.str();
			}
			break;

			case Action::_rememberPos: ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.acRememberPos)); break;

			default: ui.tbActions->setItem(i, 0, new QTableWidgetItem(u8"加载失败")); break;
			}
			ui.tbActions->setItem(i, 1, new QTableWidgetItem(ps));
			ui.tbActions->setItem(i, 2, new QTableWidgetItem(QString::fromWCharArray(actions->at(i).mark.str())));
			ui.tbActions->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbActions->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbActions->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
		connect(ui.tbActions, SIGNAL(cellChanged(int, int)), this, SLOT(OnTbChanged(int, int)));
		ui.tbActions->setStyleSheet(u8"QHeaderView::section,QScrollBar{background:transparent}");
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

				actions->Mov(before, after);
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
		ReStyle();
	}

private slots:
	//////////////////////////// Title
	void OnBnClose() { close(); }
	void OnBnRun()
	{
		ui.bnRun->setDisabled(true);
		uint32 count = macro->count;
		macro->count = 1;
		qis.run = true;
		timeBeginPeriod(1);
		macro->thRun = Thread::Start(QiFn::ThreadMacro, macro);
		DWORD exitCode = STILL_ACTIVE;
		while (exitCode == STILL_ACTIVE) {
			GetExitCodeThread(macro->thRun, &exitCode);
			if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_F10))
			{
				PostThreadMessageW(GetThreadId(macro->thRun), WM_USER, 0, 0);
				break;
			}
			Thread::Sleep(10);
		}
		timeEndPeriod(1);
		qis.run = false;
		macro->count = count;
		Thread::Sleep(300);
		ui.bnRun->setEnabled(true);
	}
	void OnBnWndSelect()
	{
		macro->wi = QiFn::WindowSelection();
		SetTitle();
	}
	void OnChbWnd()
	{
		if (ui.chbWnd->isChecked())
		{
			macro->wndState = true;
			ui.bnWndSelect->setDisabled(false);
			ui.chbChildWnd->setDisabled(false);
			ui.etWndName->setDisabled(false);
		}
		else
		{
			macro->wndState = false;
			ui.bnWndSelect->setDisabled(true);
			ui.chbChildWnd->setDisabled(true);
			ui.etWndName->setDisabled(true);
		}
	}
	void OnChbChildWnd() { macro->wi.child = ui.chbChildWnd->isChecked(); }
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
		if (state && (!child))
		{
			actions = &macro->acRun;
			TbUpdate();
		}
	}
	void OnRbEnding(bool state)
	{
		if (state && (!child))
		{
			actions = &macro->acEnd;
			TbUpdate();
		}
	}
	void OnTbChanged(int row, int column)
	{
		if (row < 0) return;
		if (column != 2) return;
		actions->at(row).mark = (PCWSTR)(ui.tbActions->item(row, 2)->text().utf16());
	}
	void OnTbClicked(int row, int column)
	{
		ui.bnColorEdit->setDisabled(true);
		ui.bnLoopEdit->setDisabled(true);
		ui.bnStateEdit->setDisabled(true);
		ui.bnImageEdit->setDisabled(true);

		if (row < 0) return;
		if (column == 2) ui.tbActions->editItem(ui.tbActions->item(row, column));
		else
		{
			switch (actions->at(row).type)
			{
			case Action::_delay:
			{
				ui.tabWidget->setCurrentIndex(0);
				break;
			}
			case Action::_key:
			{
				ui.tabWidget->setCurrentIndex(0);
				break;
			}
			case Action::_mouse:
			{
				if (!actions->at(row).d.mouse.move)
				{
					rv.hide();
					POINT pt;
					if (macro->wndState)
					{
						if (!macro->wi.Update()) { macro->wi = QiFn::WindowSelection(); SetTitle(); }
						POINT rpt = QiFn::WATR({ actions->at(row).d.mouse.x, actions->at(row).d.mouse.y }, macro->wi.wnd);
						pt = Window::pos(macro->wi.wnd);
						pt.x += rpt.x, pt.y += rpt.y;
					}
					else
					{
						pt = QiFn::ATR({ actions->at(row).d.mouse.x, actions->at(row).d.mouse.y });
					}
					pv.Show(pt);
				}
				ui.tabWidget->setCurrentIndex(0);
				break;
			}
			case Action::_text:
			{
				ui.tabWidget->setCurrentIndex(4);
				break;
			}
			case Action::_color:
			{
				pv.hide();
				RECT rect;
				if (macro->wndState)
				{
					if (!macro->wi.Update()) { macro->wi = QiFn::WindowSelection(); SetTitle(); }
					rect = QiFn::WATRR(actions->at(row).d.color.rect, macro->wi.wnd);
					POINT pt = Window::pos(macro->wi.wnd);
					rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
				}
				else
				{
					rect = QiFn::ATRR(actions->at(row).d.color.rect);
				}
				rv.Show(rect);
				ui.bnColorEdit->setEnabled(true);
				ui.tabWidget->setCurrentIndex(2);
				break;
			}
			case Action::_loop:
			{
				ui.bnLoopEdit->setEnabled(true);
				ui.tabWidget->setCurrentIndex(0);
				break;
			}
			case Action::_keyState:
			{
				ui.bnStateEdit->setEnabled(true);
				ui.tabWidget->setCurrentIndex(1);
				break;
			}
			case Action::_image:
			{
				pv.hide();
				RECT rect;
				if (macro->wndState)
				{
					if (!macro->wi.Update()) { macro->wi = QiFn::WindowSelection(); SetTitle(); }
					rect = QiFn::WATRR(actions->at(row).d.image.rect, macro->wi.wnd);
					POINT pt = Window::pos(macro->wi.wnd);
					rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
				}
				else
				{
					rect = QiFn::ATRR(actions->at(row).d.image.rect);
				}
				rv.Show(rect);
				ui.bnImageEdit->setEnabled(true);
				ui.tabWidget->setCurrentIndex(3);
				SetLbImageView(actions->at(row).d.image.map);
				break;
			}
			case Action::_popText:
			{
				ui.tabWidget->setCurrentIndex(4);
				break;
			}
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

		if (qis.clipboard.size()) muPaste->setEnabled(true);
		else muPaste->setDisabled(true);

		menu->exec(QCursor::pos());
	}
	//////////////////////////// #Table


	//////////////////////////// Action Button
	// Delete
	void OnBnDel() { ItemDel(); }
	// Key
	void OnBnKeyAdd() { if (changing) ItemChange(Action::_key); else ItemAdd(Action::_key); }
	// Key state
	void OnBnKeyStateAdd() { if (changing) ItemChange(Action::_keyState); else ItemAdd(Action::_keyState); }
	void OnBnKeyStateEdit() { NextEdit(); }
	// Mouse
	void OnBnMoveAdd() { if (changing) ItemChange(Action::_mouse); else ItemAdd(Action::_mouse); }
	void OnBnPos()
	{
		QPointSelection ps;
		POINT pt;
		if (macro->wndState)
		{
			if (!macro->wi.Update()) { macro->wi = QiFn::WindowSelection(); SetTitle(); }
			RECT wrect = Window::rect(macro->wi.wnd);
			pt = ps.Start(wrect);
			pt = QiFn::WRTA({ pt.x, pt.y }, macro->wi.wnd);
		}
		else
		{
			pt = ps.Start();
			pt = QiFn::RTA({ pt.x, pt.y });
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
			ui.bnPos->setDisabled(true);
		}
	}
	// Delay
	void OnBnDelayAdd() { if (changing) ItemChange(Action::_delay); else ItemAdd(Action::_delay); }
	void OnEtDelayMin(const QString& text) { ui.etDelayMax->setText(text); }
	// Text
	void OnBnTextAdd() { if (changing) ItemChange(Action::_text); else ItemAdd(Action::_text); }
	// Loop
	void OnBnLoopAdd() { if (changing) ItemChange(Action::_loop); else ItemAdd(Action::_loop); }
	void OnBnLoopEdit() { NextEdit(); }
	void OnEtCountMin(const QString& text) { ui.etCountMax->setText(text); }
	// Color
	void OnBnColorAdd() { if (changing) ItemChange(Action::_color); else ItemAdd(Action::_color); }
	void OnBnColorEdit() { NextEdit(); }
	void OnBnColorRect()
	{
		QRectSelection rs;
		RECT rect;

		if (macro->wndState)
		{
			if (!macro->wi.Update()) { macro->wi = QiFn::WindowSelection(); SetTitle(); }
			RECT wrect = Window::rect(macro->wi.wnd);
			rect = rs.Start(wrect);
			rect = QiFn::WRTAR(rect, macro->wi.wnd);
		}
		else
		{
			rect = rs.Start();
			rect = QiFn::RTAR(rect);
		}

		SetEtColorRect(rect);
	}
	void OnBnColorValue()
	{
		QColorSelection cs;
		QColorDialog cd(cs.Start());
		cd.exec();
		SetColor(RGB(cd.currentColor().red(), cd.currentColor().green(), cd.currentColor().blue()));
	}
	// End
	void OnBnEndAdd() { if (changing) ItemChange(Action::_end); else ItemAdd(Action::_end); }
	// EndLoop
	void OnBnEndLoopAdd() { if (changing) ItemChange(Action::_loopEnd); else ItemAdd(Action::_loopEnd); }
	// RememberPos
	void bnRememberPosAdd() { if (changing) ItemChange(Action::_rememberPos); else ItemAdd(Action::_rememberPos); }
	// RecoverPos
	void OnBnRecoverPosAdd() { if (changing) ItemChange(Action::_revocerPos); else ItemAdd(Action::_revocerPos); }
	// Image
	void OnBnImageAdd() { if (changing) ItemChange(Action::_image); else ItemAdd(Action::_image); }
	void OnBnImageEdit() { NextEdit(); }
	void OnBnImageRect()
	{
		QRectSelection rs;
		RECT rect;

		if (macro->wndState)
		{
			if (!macro->wi.Update()) { macro->wi = QiFn::WindowSelection(); SetTitle(); }
			RECT wrect = Window::rect(macro->wi.wnd);
			rect = rs.Start(wrect);
			rect = QiFn::WRTAR(rect, macro->wi.wnd);
		}
		else
		{
			rect = rs.Start();
			rect = QiFn::RTAR(rect);
		}

		SetEtImageRect(rect);
	}
	void OnBnImageShot()
	{
		QRectSelection rs;
		RECT rect = rs.Start();
		Image::ScreenRgbMap(rgbMap, rect);
		SetLbImageView();
	}
	// PopText
	void OnBnPopTextAdd() { if (changing) ItemChange(Action::_popText); else ItemAdd(Action::_popText); }
	//////////////////////////// #Action Button

private: // Widget data
	// Item
	void Changing(bool state)
	{
		if (state)
		{
			int p = ui.tbActions->currentRow();  if (p < 0) return;
			ItemGet(p);
			changing = state;
			{
				ui.bnKeyAdd->setText(qis.ui.etChange);
				ui.bnStateAdd->setText(qis.ui.etChange);
				ui.bnMoveAdd->setText(qis.ui.etChange);
				ui.bnDelayAdd->setText(qis.ui.etChange);
				ui.bnLoopAdd->setText(qis.ui.etChange);
				ui.bnTextAdd->setText(qis.ui.etChange);
				ui.bnColorAdd->setText(qis.ui.etChange);
				ui.bnEndAdd->setText(qis.ui.etChange);
				ui.bnEndLoopAdd->setText(qis.ui.etChange);
				ui.bnRecoverPosAdd->setText(qis.ui.etChange);
				ui.bnImageAdd->setText(qis.ui.etChange);
				ui.bnPopTextAdd->setText(qis.ui.etChange);
				ui.bnRememberPosAdd->setText(qis.ui.etChange);
			}
		}
		else
		{
			changing = state;
			{
				ui.bnKeyAdd->setText(qis.ui.etAdd);
				ui.bnStateAdd->setText(qis.ui.etAdd);
				ui.bnMoveAdd->setText(qis.ui.etAdd);
				ui.bnDelayAdd->setText(qis.ui.etAdd);
				ui.bnLoopAdd->setText(qis.ui.etAdd);
				ui.bnTextAdd->setText(qis.ui.etAdd);
				ui.bnColorAdd->setText(qis.ui.etAdd);
				ui.bnEndAdd->setText(qis.ui.etAdd);
				ui.bnEndLoopAdd->setText(qis.ui.etAdd);
				ui.bnRecoverPosAdd->setText(qis.ui.etAdd);
				ui.bnImageAdd->setText(qis.ui.etAdd);
				ui.bnPopTextAdd->setText(qis.ui.etAdd);
				ui.bnRememberPosAdd->setText(qis.ui.etAdd);
			}
		}
	}
	void ItemMove(bool up)
	{
		int p = ui.tbActions->currentRow();
		if (p < 0) return;

		if (up && ((p - 1) >= 0))
		{
			actions->Swp(p, p - 1);
			ui.tbActions->setCurrentItem(ui.tbActions->item(p - 1, 0));
		}
		else if (!up && (p + 1) < ui.tbActions->rowCount())
		{
			actions->Swp(p, p + 1);
			ui.tbActions->setCurrentItem(ui.tbActions->item(p + 1, 0));
		}
		TbUpdate();
	}
	void ItemGet(int32 p)
	{
		switch (actions->at(p).type)
		{
		case Action::_delay: LoadDelay(actions->at(p)); break;
		case Action::_key: LoadKey(actions->at(p)); break;
		case Action::_mouse: LoadMouse(actions->at(p)); break;
		case Action::_text: LoadText(actions->at(p)); break;
		case Action::_color: LoadColor(actions->at(p)); break;
		case Action::_loop: LoadLoop(actions->at(p)); break;
		case Action::_keyState: LoadKeyState(actions->at(p)); break;
		case Action::_image: LoadImg(actions->at(p)); break;
		case Action::_popText: LoadPopText(actions->at(p)); break;
		}
	}
	void ItemSet(Action::ActionType type, int32 p)
	{
		wcstr mark(actions->at(p).mark);
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
		case Action::_image: action = GetImg(); break;
		case Action::_popText: action = GetPopText(); break;
		case Action::_rememberPos: action.type = Action::_rememberPos; break;
		default: action.type = Action::_none; break;
		}
		action.mark.copy(mark);
		action.next = actions->at(p).next;
		actions->at(p) = action;
	}
	void ItemAdd(Action::ActionType type)
	{
		int p = ui.tbActions->currentRow();
		if (p < 0) p = actions->size();
		else p++;
		actions->InsNull(p);
		ItemSet(type, p);
		TbUpdate();
		ui.tbActions->setCurrentItem(ui.tbActions->item(p, 0));
	}
	void ItemChange(Action::ActionType type)
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		ItemSet(type, p);
		TbUpdate();
		ui.tbActions->setCurrentItem(0);
		Changing(false);
	}
	void ItemDel()
	{
		QList<QTableWidgetItem*> items = ui.tbActions->selectedItems();
		if (!items.size()) return;

		List<size_t> itemPos;
		for (size_t i = 0; i < items.size(); i++) if (!items[i]->column()) itemPos.Add(items[i]->row());
		actions->Del(itemPos);

		TbUpdate();
		ui.tbActions->setCurrentItem(0);

		Changing(false);
	}
	void ItemCut()
	{
		QList<QTableWidgetItem*> items = ui.tbActions->selectedItems();
		if (!items.size()) return;

		qis.clipboard.resize(0);
		for (size_t i = 0; i < items.size(); i++) if (items[i]->column() == 0) qis.clipboard.Add(actions->at(items[i]->row()));

		ItemDel();
	}
	void ItemCopy()
	{
		QList<QTableWidgetItem*> items = ui.tbActions->selectedItems();
		if (!items.size()) return;

		qis.clipboard.resize(0);
		for (size_t i = 0; i < items.size(); i++) if (items[i]->column() == 0) qis.clipboard.Add(actions->at(items[i]->row()));
	}
	void ItemPaste()
	{
		int p = ui.tbActions->currentRow();

		if (p < 0) p = actions->size();
		else if ((p + 1) <= actions->size()) p++;

		for (uint32 i = 0; i < qis.clipboard.size(); i++) actions->Ins(qis.clipboard[i], p + i);
		TbUpdate();
	}

	// Set Wid data
	void SetHkKey(byte key) { ui.hkKey->VirtualKey(key); }
	void SetHkState(byte key) { ui.hkState->VirtualKey(key); }
	void SetEtPos(const RECT& pt) { ui.etX->setText(QString::number(pt.left)); ui.etY->setText(QString::number(pt.top)); if (pt.right > -1) ui.etMoveRand->setText(QString::number(pt.right)); }
	void SetEtDelay(const POINT& pt) { ui.etDelayMin->setText(QString::number(pt.x)); ui.etDelayMax->setText(QString::number(pt.y)); }

	void SetRbColorMode(bool mode) { if (mode) ui.rbColorNFind->setChecked(true); else ui.rbColorFind->setChecked(true); }
	void SetChbColorMove(bool state) { ui.chbColorMove->setChecked(state); }
	void SetEtColorRect(const RECT& rect) { ui.etColorL->setText(QString::number(rect.left)); ui.etColorT->setText(QString::number(rect.top)); ui.etColorR->setText(QString::number(rect.right)); ui.etColorB->setText(QString::number(rect.bottom)); }
	void SetEtColorValue(const Rgba& color) { SetColor(color); ui.etColorSim->setText(QString::number(color.a)); }
	void SetColor(const Rgba& color) {
		ui.etColorRed->setText(QString::number(color.r));
		ui.etColorGreen->setText(QString::number(color.g));
		ui.etColorBlue->setText(QString::number(color.b));
		QString style = u8"background-color:rgb(";
		style += QString::number(color.r);
		style += u8",";
		style += QString::number(color.g);
		style += u8",";
		style += QString::number(color.b);
		style += u8");";
		ui.bnColorValue->setStyleSheet(style);
	}

	void SetEtLoopCountMin(const uint32& count) { ui.etCountMin->setText(QString::number(count)); }
	void SetEtLoopCountMax(const uint32& count) { ui.etCountMax->setText(QString::number(count)); }

	void SetRbImageMode(bool mode) { if (mode) ui.rbImageNFind->setChecked(true); else ui.rbImageFind->setChecked(true); }
	void SetChbImageMove(bool state) { ui.chbImageMove->setChecked(state); }
	void SetEtImageRect(const RECT& rect) { ui.etImageL->setText(QString::number(rect.left)); ui.etImageT->setText(QString::number(rect.top)); ui.etImageR->setText(QString::number(rect.right)); ui.etImageB->setText(QString::number(rect.bottom)); }
	void SetLbImageView() { HBITMAP hbmp = Image::toBmp32(rgbMap); if (hbmp) ui.lbImageView->setPixmap(QtWin::fromHBITMAP(hbmp)); }
	void SetLbImageView(const RgbMap& map) { rgbMap.copy(map); SetLbImageView(); }
	void SetEtImageSim(byte sim) { ui.etImageSim->setText(QString::number((int)sim)); }

	void SetEtPopText(QString text) { ui.etPopText->setText(text); }
	void SetEtPopTextTime(uint32 time) { ui.etPopTextTime->setText(QString::number(time)); }

	// Get data by Wid
	Action GetKey() {
		Action action(Action::_key);
		if (ui.rbDown->isChecked()) action.d.key.state = QiKey::down;
		else if (ui.rbUp->isChecked()) action.d.key.state = QiKey::up;
		else if (ui.rbClick->isChecked()) action.d.key.state = QiKey::click;
		action.d.key.vk = ui.hkKey->virtualKey();
		return action;
	}
	Action GetKeyState() {
		Action action(Action::_keyState);
		action.d.keyState.state = ui.rbStateDown->isChecked();
		action.d.keyState.vk = ui.hkState->virtualKey();
		return action;
	}
	Action GetMouse() {
		Action action(Action::_mouse);
		action.d.mouse.move = ui.rbMove->isChecked();
		action.d.mouse.track = ui.rbMoveTrack->isChecked();
		int x = ui.etX->text().toInt();
		int y = ui.etY->text().toInt();
		int r = ui.etMoveRand->text().toInt();
		int s = 50;
		if (ui.etMoveSpeed->text() != "") s = ui.etMoveSpeed->text().toInt();
		if (x > posMax) x = posMax;
		if (y > posMax) y = posMax;
		if (r > posMax) r = posMax;
		if (x < posMin) x = posMin;
		if (y < posMin) y = posMin;
		if (!s) s = 1;
		action.d.mouse.x = x;
		action.d.mouse.y = y;
		action.d.mouse.ex = r;
		action.d.mouse.speed = s;
		return action;
	}
	Action GetDelay() {
		Action action(Action::_delay);
		int mn = 10;
		if (ui.etDelayMin->text() != "") mn = ui.etDelayMin->text().toInt();
		int mx = ui.etDelayMax->text().toInt();
		if (mn > delayMax) mn = delayMax;
		if (mx > delayMax) mx = delayMax;
		if (mx < mn) mx = mn;
		action.d.delay.tmin = mn;
		action.d.delay.tmax = mx;
		return action;
	}
	Action GetText() {
		Action action(Action::_text);
		action.d.text.str.copy((PCWSTR)(ui.etText->toPlainText().utf16()));
		return action;
	}
	Action GetColor() {
		Action action(Action::_color);
		action.d.color.unfind = ui.rbColorNFind->isChecked();
		action.d.color.move = ui.chbColorMove->isChecked();
		{
			int l = ui.etColorL->text().toInt();
			int t = ui.etColorT->text().toInt();
			int r = ui.etColorR->text().toInt();
			int b = ui.etColorB->text().toInt();
			if (l > posMax) l = posMax;
			if (t > posMax) t = posMax;
			if (r > posMax) r = posMax;
			if (b > posMax) b = posMax;
			action.d.color.rect = { l, t, r, b };
		}
		{
			int r = ui.etColorRed->text().toInt();
			int g = ui.etColorGreen->text().toInt();
			int b = ui.etColorBlue->text().toInt();
			int e = 5;
			if (ui.etColorSim->text() != "") e = ui.etColorSim->text().toInt();
			if (r > colorMax) r = colorMax;
			if (g > colorMax) g = colorMax;
			if (b > colorMax) b = colorMax;
			if (e > colorMax) e = colorMax;
			action.d.color.rgbe.set(r, g, b, e);
		}
		return action;
	}
	Action GetLoop()
	{
		Action action(Action::_loop);
		int mn = 1;
		if (ui.etCountMin->text() != "") mn = ui.etCountMin->text().toInt();
		int mx = ui.etCountMax->text().toInt();
		if (mn > loopCountMax) mn = loopCountMax;
		if (mx > loopCountMax) mx = loopCountMax;
		if (mx < mn) mx = mn;
		action.d.loop.cmin = mn;
		action.d.loop.cmax = mx;
		return action;
	}
	Action GetImg()
	{
		Action action(Action::_image);
		action.d.image.unfind = ui.rbImageNFind->isChecked();
		action.d.image.move = ui.chbImageMove->isChecked();
		{
			int l = ui.etImageL->text().toInt();
			int t = ui.etImageT->text().toInt();
			int r = ui.etImageR->text().toInt();
			int b = ui.etImageB->text().toInt();
			if (l > posMax) l = posMax;
			if (t > posMax) t = posMax;
			if (r > posMax) r = posMax;
			if (b > posMax) b = posMax;
			action.d.image.rect = { l, t, r, b };
		}
		{
			int s = 80;
			if (ui.etImageSim->text() != "") s = ui.etImageSim->text().toInt();
			if (s > imageSimMax) s = imageSimMax;
			action.d.image.sim = s;
		}
		action.d.image.map = rgbMap;
		return action;
	}
	Action GetPopText()
	{
		Action action(Action::_popText);
		action.d.popText.str.copy((PCWSTR)(ui.etPopText->text().utf16()));
		int time = 1000;
		if (ui.etPopTextTime->text() != "") time = ui.etPopTextTime->text().toInt();
		if (time > popTextTimeMax) time = popTextTimeMax;
		action.d.popText.time = time;
		return action;
	}

	// Set Wid data by Action
	void LoadKey(const Action& action) {
		if (action.d.key.state == QiKey::down) ui.rbDown->setChecked(1);
		else if (action.d.key.state == QiKey::up) ui.rbUp->setChecked(1);
		else if (action.d.key.state == QiKey::click) ui.rbClick->setChecked(1);
		SetHkKey(action.d.key.vk);
	}
	void LoadKeyState(const Action& action)
	{
		if (action.d.keyState.state) ui.rbStateDown->setChecked(true);
		if (!action.d.keyState.state) ui.rbStateUp->setChecked(true);
		SetHkState(action.d.keyState.vk);
	}
	void LoadMouse(const Action& action) {
		if (action.d.mouse.move)
		{
			ui.rbMove->setChecked(true);
			OnRbMove(true);
		}
		else
		{
			ui.rbPos->setChecked(true);
			OnRbPos(true);
		}
		ui.rbMoveTrack->setChecked(action.d.mouse.track);
		ui.etMoveSpeed->setText(QString::number(action.d.mouse.speed));
		SetEtPos({ (long)action.d.mouse.x, (long)action.d.mouse.y, (long)action.d.mouse.ex, 0 });
	}
	void LoadDelay(const Action& action) { SetEtDelay({ (long)action.d.delay.tmin, (long)action.d.delay.tmax }); }
	void LoadText(const Action& action) { ui.etText->setText(QString::fromWCharArray(action.d.text.str.str())); }
	void LoadColor(const Action& action) { SetRbColorMode(action.d.color.unfind); SetChbColorMove(action.d.color.move); SetEtColorRect(action.d.color.rect); SetEtColorValue(action.d.color.rgbe); }
	void LoadLoop(const Action& action) { SetEtLoopCountMin(action.d.loop.cmin); SetEtLoopCountMax(action.d.loop.cmax); }
	void LoadImg(const Action& action) { SetRbImageMode(action.d.image.unfind); SetChbImageMove(action.d.image.move); SetEtImageRect(action.d.image.rect); SetEtImageSim(action.d.image.sim); }
	void LoadPopText(const Action& action) { SetEtPopText(QString::fromWCharArray(action.d.popText.str.str())); SetEtPopTextTime(action.d.popText.time); }
};