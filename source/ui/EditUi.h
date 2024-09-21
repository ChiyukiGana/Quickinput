#pragma execution_character_set("utf-8")
#pragma once
#include <QtWinExtras/qwinfunctions.h>
#include <qbuttongroup.h>
#include <qcolordialog.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qevent.h>
#include <qtimer.h>
#include <qmenu.h>
#include "widget/QColorSelection.h"
#include "widget/QPointSelection.h"
#include "widget/QRectSelection.h"
#include "widget/QPointView.h"
#include "widget/QRectView.h"
#include "../src/minc.h"
#include "ui_EditUi.h"

struct EditParam
{
	Macro* macro = 0;
	Actions* actions = 0;
	bool child = false;
};

class EditUi : public QDialog
{
	Q_OBJECT;

	// edit box range
	const int32 posMin = -10000;
	const int32 posMax = 10000;
	const int32 moveSpeedMax = 99;
	const int32 moveRandMax = 9999;
	const int32 delayMax = 999999;
	const int32 loopCountMax = 9999;
	const int32 colorMax = 255;
	const int32 imageSimMax = 99;
	const int32 popTextTimeMax = 9999;

	Ui::EditUiClass ui;

	// rbutton context menu
	QMenu* menu;
	QAction* muDel;
	QAction* muChange;
	QAction* muCut;
	QAction* muCopy;
	QAction* muPaste;

	// widget
	QPointView pv;
	QRectView rv;

	// image viewer
	RgbMap imageMap;

	Macro* macro = 0;
	Actions* actions = 0;

	// is child editor
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
		setStyleSheet(qis.ui.themes[qis.set.theme].style);
	}

private:
	void WidInit()
	{
		// Button
		{
			// text
			ui.bnKeyAdd->setText(qis.ui.text.etAdd);
			ui.bnStateAdd->setText(qis.ui.text.etAdd);
			ui.bnMoveAdd->setText(qis.ui.text.etAdd);
			ui.bnDelayAdd->setText(qis.ui.text.etAdd);
			ui.bnLoopAdd->setText(qis.ui.text.etAdd);
			ui.bnTextAdd->setText(qis.ui.text.etAdd);
			ui.bnColorAdd->setText(qis.ui.text.etAdd);
			ui.bnImageAdd->setText(qis.ui.text.etAdd);
			ui.bnEndAdd->setText(qis.ui.text.etAdd);
			ui.bnEndLoopAdd->setText(qis.ui.text.etAdd);
			ui.bnStateEdit->setText(qis.ui.text.etEdit);
			ui.bnLoopEdit->setText(qis.ui.text.etEdit);
			ui.bnColorEdit->setText(qis.ui.text.etEdit);
			ui.bnImageEdit->setText(qis.ui.text.etEdit);
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
			ui.etMoveSpeed->setValidator(new QIntValidator(0, moveSpeedMax, this));
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
			muDel = new QAction("删除", this);
			muChange = new QAction("修改", this);
			muCut = new QAction("剪切", this);
			muCopy = new QAction("复制", this);
			muPaste = new QAction("粘贴", this);

			menu->addAction(muDel);
			menu->addAction(muChange);
			menu->addAction(muCut);
			menu->addAction(muCopy);
			menu->addAction(muPaste);
			menu->setFont(QFont("Microsoft YaHei"));

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

		// enable qlable scale
		ui.lbImageView->setScaledContents(true);

		// load Window mode
		if (macro->wndState) macro->wi.Update(), SetWindowMode();

		// is child
		if (child) ui.rbRunning->setVisible(false), ui.rbEnding->setVisible(false);

		// title
		ui.lbTitle->setText(windowTitle());

		TableUpdate();
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
		connect(ui.rbRunning, SIGNAL(toggled(bool)), this, SLOT(OnRbRunning(bool)));
		connect(ui.rbEnding, SIGNAL(toggled(bool)), this, SLOT(OnRbEnding(bool)));
		connect(ui.tbActions, SIGNAL(cellChanged(int, int)), this, SLOT(OnTbChanged(int, int)));
		connect(ui.tbActions, SIGNAL(cellClicked(int, int)), this, SLOT(OnTbClicked(int, int)));
		connect(ui.tbActions, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(OnTbSelect(QTableWidgetItem*, QTableWidgetItem*)));
		connect(ui.tbActions, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(OnTbDoubleClick(int, int)));
		connect(ui.tbActions, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnTableMenu(const QPoint&)));

		// Action widget
		connect(ui.bnKeyAdd, SIGNAL(clicked()), this, SLOT(OnBnKeyAdd()));
		connect(ui.bnStateAdd, SIGNAL(clicked()), this, SLOT(OnBnKeyStateAdd()));
		connect(ui.bnStateEdit, SIGNAL(clicked()), this, SLOT(OnBnKeyStateEdit()));
		connect(ui.bnMoveAdd, SIGNAL(clicked()), this, SLOT(OnBnMouseAdd()));
		connect(ui.bnPos, SIGNAL(clicked()), this, SLOT(OnBnMousePos()));
		connect(ui.rbPos, SIGNAL(toggled(bool)), this, SLOT(OnRbMousePos(bool)));
		connect(ui.rbMove, SIGNAL(toggled(bool)), this, SLOT(OnRbMouseMove(bool)));
		connect(ui.bnDelayAdd, SIGNAL(clicked()), this, SLOT(OnBnDelayAdd()));
		connect(ui.etDelayMin, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtDelayMin(const QString&)));
		connect(ui.bnTextAdd, SIGNAL(clicked()), this, SLOT(OnBnTextAdd()));
		connect(ui.bnLoopAdd, SIGNAL(clicked()), this, SLOT(OnBnLoopAdd()));
		connect(ui.bnLoopEdit, SIGNAL(clicked()), this, SLOT(OnBnLoopEdit()));
		connect(ui.etCountMin, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtLoopMin(const QString&)));
		connect(ui.bnColorAdd, SIGNAL(clicked()), this, SLOT(OnBnColorAdd()));
		connect(ui.bnColorEdit, SIGNAL(clicked()), this, SLOT(OnBnColorEdit()));
		connect(ui.bnColorRect, SIGNAL(clicked()), this, SLOT(OnBnColorRect()));
		connect(ui.bnColorValue, SIGNAL(clicked()), this, SLOT(OnBnColorValue()));
		connect(ui.bnEndAdd, SIGNAL(clicked()), this, SLOT(OnBnEndAdd()));
		connect(ui.bnEndLoopAdd, SIGNAL(clicked()), this, SLOT(OnBnEndLoopAdd()));
		connect(ui.bnRememberPosAdd, SIGNAL(clicked()), this, SLOT(OnBnRememberPosAdd()));
		connect(ui.bnRecoverPosAdd, SIGNAL(clicked()), this, SLOT(OnBnRecoverPosAdd()));
		connect(ui.bnImageAdd, SIGNAL(clicked()), this, SLOT(OnBnImageAdd()));
		connect(ui.bnImageEdit, SIGNAL(clicked()), this, SLOT(OnBnImageEdit()));
		connect(ui.bnImageRect, SIGNAL(clicked()), this, SLOT(OnBnImageRect()));
		connect(ui.bnImageShot, SIGNAL(clicked()), this, SLOT(OnBnImageShot()));
		connect(ui.bnPopTextAdd, SIGNAL(clicked()), this, SLOT(OnBnPopTextAdd()));
	}

	void SetWindowMode()
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
		case Action::_loop: title = "编辑 - 循环"; break;
		case Action::_color: title = "编辑 - 查找颜色"; break;
		case Action::_keyState: title = "编辑 - 按键状态"; break;
		case Action::_image: title = "编辑 - 查找图片"; break;
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

	void TableUpdate()
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
			case Action::_end: ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acEnd)); break;

			case Action::_delay:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acWait));
				if (actions->at(i).d.delay.tmin != actions->at(i).d.delay.tmax)
				{
					ps = QString::number(actions->at(i).d.delay.tmin);
					ps += " ~ ";
					ps += QString::number(actions->at(i).d.delay.tmax);
				}
				else ps = QString::number(actions->at(i).d.delay.tmin);
			}
			break;

			case Action::_key:
			{
				if (actions->at(i).d.key.state == QiKey::up) ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acUp));
				else if (actions->at(i).d.key.state == QiKey::down) ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acDown));
				else if (actions->at(i).d.key.state == QiKey::click) ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acClick));
				ps = QString::fromWCharArray(Input::Name(actions->at(i).d.key.vk));
			}
			break;

			case Action::_mouse:
			{
				if (actions->at(i).d.mouse.move) ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acMove));
				else ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acPos));
				ps = QString::number(actions->at(i).d.mouse.x);
				ps += " - ";
				ps += QString::number(actions->at(i).d.mouse.y);
				if (actions->at(i).d.mouse.ex)
				{
					ps += "ㅤㅤ随机：";
					ps += QString::number(actions->at(i).d.mouse.ex);
				}
				if (actions->at(i).d.mouse.track)
				{
					ps += "ㅤㅤ轨迹：";
					ps += QString::number(actions->at(i).d.mouse.speed);
				}
			}
			break;

			case Action::_text:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acText));
				std::wstring text = actions->at(i).d.text.str.str();
				ps = QString::fromWCharArray(text.substr(0, 32).c_str());
				if (text.length() > 31) ps += "...";
			}
			break;

			case Action::_color:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acColor));
				ps = "(";
				ps += QString::number(actions->at(i).d.color.rect.left);
				ps += ",";
				ps += QString::number(actions->at(i).d.color.rect.top);
				ps += ",";
				ps += QString::number(actions->at(i).d.color.rect.right);
				ps += ",";
				ps += QString::number(actions->at(i).d.color.rect.bottom);
				ps += ")　(";
				ps += QString::number(actions->at(i).d.color.rgbe.r);
				ps += ",";
				ps += QString::number(actions->at(i).d.color.rgbe.g);
				ps += ",";
				ps += QString::number(actions->at(i).d.color.rgbe.b);
				ps += ",";
				ps += QString::number(actions->at(i).d.color.rgbe.a);
				if (actions->at(i).d.color.unfind) ps += ")　未找到";
				else
				{
					ps += ")　找到";
					if (actions->at(i).d.color.move) ps += "并移动";
					else ps += "不移动";
				}
			}
			break;

			case Action::_loop:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acLoop));
				if ((actions->at(i).d.loop.cmin == 0 && actions->at(i).d.loop.cmax == 0))
					ps = "无限";
				else if (actions->at(i).d.loop.cmin == actions->at(i).d.loop.cmax)
					ps = QString::number(actions->at(i).d.loop.cmin);
				else
				{
					ps = QString::number(actions->at(i).d.loop.cmin);
					ps += " ~ ";
					ps += QString::number(actions->at(i).d.loop.cmax);
				}
			}
			break;

			case Action::_loopEnd: ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acEndLoop)); break;

			case Action::_keyState:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acKeyState));
				if (actions->at(i).d.keyState.state) ps = "按下了　";
				else ps = "松开了　";
				ps += QString::fromWCharArray(Input::Name(actions->at(i).d.keyState.vk));
			}
			break;

			case Action::_revocerPos: ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acRecoverPos)); break;

			case Action::_image:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acImage));
				ps = "(";
				ps += QString::number(actions->at(i).d.image.rect.left);
				ps += ",";
				ps += QString::number(actions->at(i).d.image.rect.top);
				ps += ",";
				ps += QString::number(actions->at(i).d.image.rect.right);
				ps += ",";
				ps += QString::number(actions->at(i).d.image.rect.bottom);
				ps += ")　(";
				ps += QString::number(actions->at(i).d.image.map.width());
				ps += "x";
				ps += QString::number(actions->at(i).d.image.map.height());
				ps += ")　";
				ps += QString::number(actions->at(i).d.image.sim);
				if (actions->at(i).d.image.unfind) ps += "　未找到";
				else
				{
					ps += "　找到";
					if (actions->at(i).d.image.move) ps += "并移动";
					else ps += "不移动";
				}
			}
			break;

			case Action::_popText:
			{
				ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acPopText));
				ps = QString::fromWCharArray(actions->at(i).d.popText.str.str());
				ps += "　时长：";
				ps += QString::number(actions->at(i).d.popText.time);
			}
			break;

			case Action::_rememberPos: ui.tbActions->setItem(i, 0, new QTableWidgetItem(qis.ui.text.acRememberPos)); break;

			default: ui.tbActions->setItem(i, 0, new QTableWidgetItem("加载失败")); break;
			}
			ui.tbActions->setItem(i, 1, new QTableWidgetItem(ps));
			ui.tbActions->setItem(i, 2, new QTableWidgetItem(QString::fromWCharArray(actions->at(i).mark.str())));
			ui.tbActions->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbActions->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbActions->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
		connect(ui.tbActions, SIGNAL(cellChanged(int, int)), this, SLOT(OnTbChanged(int, int)));
		ui.tbActions->setStyleSheet("QHeaderView::section,QScrollBar{background:transparent}");
	}

private:
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
				TableUpdate();
				ui.tbActions->setCurrentItem(ui.tbActions->item(after, 0));
				return true;
			}
		}
		return false;
	}
	void showEvent(QShowEvent*)
	{
		ReStyle();
		SetForegroundWindow((HWND)QWidget::winId());
	}
	QPoint msPos; bool mouseDown = false; void mousePressEvent(QMouseEvent* et) { if (et->button() == Qt::LeftButton) msPos = et->pos(), mouseDown = true; et->accept(); }void mouseMoveEvent(QMouseEvent* et) { if (mouseDown) move(et->pos() + pos() - msPos); }void mouseReleaseEvent(QMouseEvent* et) { if (et->button() == Qt::LeftButton) mouseDown = false; }
private slots:
	// Title
	void OnBnClose()
	{
		close();
	}
	void OnBnRun()
	{
		ui.bnRun->setDisabled(true);
		uint32 count = macro->count;
		macro->count = 1;
		qis.run = true;
		timeBeginPeriod(1);
		QiThread::StartMacroRun(macro);
		while (QiThread::MacroRunActive(macro))
		{
			if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_F10))
			{
				QiThread::ExitMacroRun(macro);
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

	// Rutton context menu
	void OnMenuDel()
	{
		ItemDel();
	}
	void OnMenuChange()
	{
		changing ? Changing(false) : Changing(true);
	}
	void OnMenuCut()
	{
		ItemCut();
	}
	void OnMenuCopy()
	{
		ItemCopy();
	}
	void OnMenuPaste()
	{
		ItemPaste();
	}

	// Table
	void OnRbRunning(bool state)
	{
		if (state && (!child))
		{
			actions = &macro->acRun;
			TableUpdate();
		}
	}
	void OnRbEnding(bool state)
	{
		if (state && (!child))
		{
			actions = &macro->acEnd;
			TableUpdate();
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

		pv.hide();
		rv.hide();

		if (row < 0) return;
		if (column == 2) ui.tbActions->editItem(ui.tbActions->item(row, column));
		else
		{
			switch (actions->at(row).type)
			{
			case Action::_end:
			{
				ui.tabWidget->setCurrentIndex(1);
				break;
			}
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
						if (!macro->wi.Update())
						{
							macro->wi = QiFn::WindowSelection();
							SetWindowMode();
						}
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
					if (!macro->wi.Update())
					{
						macro->wi = QiFn::WindowSelection();
						SetWindowMode();
					}
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
			case Action::_loopEnd:
			{
				ui.tabWidget->setCurrentIndex(1);
				break;
			}
			case Action::_keyState:
			{
				ui.bnStateEdit->setEnabled(true);
				ui.tabWidget->setCurrentIndex(1);
				break;
			}
			case Action::_revocerPos:
			{
				ui.tabWidget->setCurrentIndex(1);
				break;
			}
			case Action::_image:
			{
				pv.hide();
				RECT rect;
				if (macro->wndState)
				{
					if (!macro->wi.Update())
					{
						macro->wi = QiFn::WindowSelection();
						SetWindowMode();
					}
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
				WidgetSetImage(actions->at(row));
				break;
			}
			case Action::_popText:
			{
				ui.tabWidget->setCurrentIndex(4);
				break;
			}
			case Action::_rememberPos:
			{
				ui.tabWidget->setCurrentIndex(1);
				break;
			}
			}
		}
	}
	void OnTbSelect(QTableWidgetItem*, QTableWidgetItem*)
	{
		OnTbClicked(ui.tbActions->currentRow(), ui.tbActions->currentColumn());
	}
	void OnTbDoubleClick(int row, int column)
	{
		changing ? Changing(false) : Changing(true);
	}
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

	// Window mode
	void OnBnWndSelect()
	{
		macro->wi = QiFn::WindowSelection();
		SetWindowMode();
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
	void OnChbChildWnd()
	{
		macro->wi.child = ui.chbChildWnd->isChecked();
	}

	// Action widget
	void OnBnKeyAdd()
	{
		if (changing) ItemChange(Action::_key);
		else ItemAdd(Action::_key);
	}
	void OnBnKeyStateAdd()
	{
		if (changing) ItemChange(Action::_keyState);
		else ItemAdd(Action::_keyState);
	}
	void OnBnKeyStateEdit()
	{
		NextEdit();
	}
	void OnBnMouseAdd()
	{
		if (changing) ItemChange(Action::_mouse);
		else ItemAdd(Action::_mouse);
	}
	void OnBnMousePos()
	{
		QPointSelection ps;
		POINT pt;
		if (macro->wndState)
		{
			if (!macro->wi.Update())
			{
				macro->wi = QiFn::WindowSelection();
				SetWindowMode();
			}
			RECT wrect = Window::rect(macro->wi.wnd);
			pt = ps.Start(wrect);
			pt = QiFn::WRTA({ pt.x, pt.y }, macro->wi.wnd);
		}
		else
		{
			pt = ps.Start();
			pt = QiFn::RTA({ pt.x, pt.y });
		}
		Action action = WidgetGetMouse();
		action.d.mouse.x = pt.x;
		action.d.mouse.y = pt.y;
		WidgetSetMouse(action);
	}
	void OnRbMousePos(bool state)
	{
		if (state)
		{
			ui.etX->setValidator(new QIntValidator(0, posMax, this));
			ui.etY->setValidator(new QIntValidator(0, posMax, this));
			if (ui.etX->text().toInt() < 0) ui.etX->setText("0");
			if (ui.etY->text().toInt() < 0) ui.etY->setText("0");
			ui.bnPos->setText("+");
			ui.bnPos->setDisabled(0);
		}
	}
	void OnRbMouseMove(bool state)
	{
		if (state)
		{
			ui.etX->setValidator(new QIntValidator(posMin, posMax, this));
			ui.etY->setValidator(new QIntValidator(posMin, posMax, this));
			ui.bnPos->setText("×");
			ui.bnPos->setDisabled(true);
		}
	}
	void OnBnDelayAdd()
	{
		if (changing) ItemChange(Action::_delay);
		else ItemAdd(Action::_delay);
	}
	void OnEtDelayMin(const QString& text)
	{
		ui.etDelayMax->setText(text);
	}
	void OnBnTextAdd()
	{
		if (changing) ItemChange(Action::_text);
		else ItemAdd(Action::_text);
	}
	void OnBnLoopAdd()
	{
		if (changing) ItemChange(Action::_loop);
		else ItemAdd(Action::_loop);
	}
	void OnBnLoopEdit()
	{
		NextEdit();
	}
	void OnEtLoopMin(const QString& text)
	{
		ui.etCountMax->setText(text);
	}
	void OnBnColorAdd()
	{
		if (changing) ItemChange(Action::_color);
		else ItemAdd(Action::_color);
	}
	void OnBnColorEdit()
	{
		NextEdit();
	}
	void OnBnColorRect()
	{
		QRectSelection rs;
		RECT rect;

		if (macro->wndState)
		{
			if (!macro->wi.Update())
			{
				macro->wi = QiFn::WindowSelection();
				SetWindowMode();
			}
			RECT wrect = Window::rect(macro->wi.wnd);
			rect = rs.Start(wrect);
			rect = QiFn::WRTAR(rect, macro->wi.wnd);
		}
		else
		{
			rect = rs.Start();
			rect = QiFn::RTAR(rect);
		}

		Action action = WidgetGetColor();
		action.d.color.rect = rect;
		WidgetSetColor(action);
	}
	void OnBnColorValue()
	{
		QColorSelection cs;
		QColorDialog cd(cs.Start());
		cd.exec();
		Action action = WidgetGetColor();
		action.d.color.rgbe.r = cd.currentColor().red();
		action.d.color.rgbe.g = cd.currentColor().green();
		action.d.color.rgbe.b = cd.currentColor().blue();
		WidgetSetColor(action);
	}
	void OnBnEndAdd()
	{
		if (changing) ItemChange(Action::_end);
		else ItemAdd(Action::_end);
	}
	void OnBnEndLoopAdd()
	{
		if (changing) ItemChange(Action::_loopEnd);
		else ItemAdd(Action::_loopEnd);
	}
	void OnBnRememberPosAdd()
	{
		if (changing) ItemChange(Action::_rememberPos);
		else ItemAdd(Action::_rememberPos);
	}
	void OnBnRecoverPosAdd()
	{
		if (changing) ItemChange(Action::_revocerPos);
		else ItemAdd(Action::_revocerPos);
	}
	void OnBnImageAdd()
	{
		if (changing) ItemChange(Action::_image);
		else ItemAdd(Action::_image);
	}
	void OnBnImageEdit()
	{
		NextEdit();
	}
	void OnBnImageRect()
	{
		QRectSelection rs;
		RECT rect;

		if (macro->wndState)
		{
			if (!macro->wi.Update())
			{
				macro->wi = QiFn::WindowSelection();
				SetWindowMode();
			}
			RECT wrect = Window::rect(macro->wi.wnd);
			rect = rs.Start(wrect);
			rect = QiFn::WRTAR(rect, macro->wi.wnd);
		}
		else
		{
			rect = rs.Start();
			rect = QiFn::RTAR(rect);
		}

		Action action = WidgetGetImage();
		action.d.image.rect = rect;
		WidgetSetImage(action);
	}
	void OnBnImageShot()
	{
		QRectSelection rs;
		RECT rect = rs.Start();
		Action action = WidgetGetImage();
		Image::ScreenRgbMap(imageMap, rect);
		action.d.image.map = imageMap;
		WidgetSetImage(action);
	}
	void OnBnPopTextAdd()
	{
		if (changing) ItemChange(Action::_popText);
		else ItemAdd(Action::_popText);
	}

private:
	// Action
	void Changing(bool state)
	{
		if (state)
		{
			int p = ui.tbActions->currentRow();  if (p < 0) return;
			ItemGet(p);
			changing = state;
			{
				ui.bnKeyAdd->setText(qis.ui.text.etChange);
				ui.bnStateAdd->setText(qis.ui.text.etChange);
				ui.bnMoveAdd->setText(qis.ui.text.etChange);
				ui.bnDelayAdd->setText(qis.ui.text.etChange);
				ui.bnLoopAdd->setText(qis.ui.text.etChange);
				ui.bnTextAdd->setText(qis.ui.text.etChange);
				ui.bnColorAdd->setText(qis.ui.text.etChange);
				ui.bnEndAdd->setText(qis.ui.text.etChange);
				ui.bnEndLoopAdd->setText(qis.ui.text.etChange);
				ui.bnRecoverPosAdd->setText(qis.ui.text.etChange);
				ui.bnImageAdd->setText(qis.ui.text.etChange);
				ui.bnPopTextAdd->setText(qis.ui.text.etChange);
				ui.bnRememberPosAdd->setText(qis.ui.text.etChange);
			}
		}
		else
		{
			changing = state;
			{
				ui.bnKeyAdd->setText(qis.ui.text.etAdd);
				ui.bnStateAdd->setText(qis.ui.text.etAdd);
				ui.bnMoveAdd->setText(qis.ui.text.etAdd);
				ui.bnDelayAdd->setText(qis.ui.text.etAdd);
				ui.bnLoopAdd->setText(qis.ui.text.etAdd);
				ui.bnTextAdd->setText(qis.ui.text.etAdd);
				ui.bnColorAdd->setText(qis.ui.text.etAdd);
				ui.bnEndAdd->setText(qis.ui.text.etAdd);
				ui.bnEndLoopAdd->setText(qis.ui.text.etAdd);
				ui.bnRecoverPosAdd->setText(qis.ui.text.etAdd);
				ui.bnImageAdd->setText(qis.ui.text.etAdd);
				ui.bnPopTextAdd->setText(qis.ui.text.etAdd);
				ui.bnRememberPosAdd->setText(qis.ui.text.etAdd);
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
		TableUpdate();
	}
	void ItemGet(int32 p)
	{
		switch (actions->at(p).type)
		{
		case Action::_delay: WidgetSetDelay(actions->at(p)); break;
		case Action::_key: WidgetSetKey(actions->at(p)); break;
		case Action::_mouse: WidgetSetMouse(actions->at(p)); break;
		case Action::_text: WidgetSetText(actions->at(p)); break;
		case Action::_color: WidgetSetColor(actions->at(p)); break;
		case Action::_loop: WidgetSetLoop(actions->at(p)); break;
		case Action::_keyState: WidgetSetKeyState(actions->at(p)); break;
		case Action::_image: WidgetSetImage(actions->at(p)); break;
		case Action::_popText: WidgetSetPopText(actions->at(p)); break;
		}
	}
	void ItemSet(Action::ActionType type, int32 p)
	{
		wcstr mark(actions->at(p).mark);
		Action action;
		switch (type)
		{
		case Action::_end: action.type = Action::_end; break;
		case Action::_delay: action = WidgetGetDelay(); break;
		case Action::_key: action = WidgetGetKey(); break;
		case Action::_mouse: action = WidgetGetMouse(); break;
		case Action::_text: action = WidgetGetText(); break;
		case Action::_color: action = WidgetGetColor(); break;
		case Action::_loop: action = WidgetGetLoop(); break;
		case Action::_loopEnd: action.type = Action::_loopEnd; break;
		case Action::_keyState: action = WidgetGetKeyState(); break;
		case Action::_revocerPos: action.type = Action::_revocerPos; break;
		case Action::_image: action = WidgetGetImage(); break;
		case Action::_popText: action = WidgetGetPopText(); break;
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
		TableUpdate();
		ui.tbActions->setCurrentItem(ui.tbActions->item(p, 0));
	}
	void ItemChange(Action::ActionType type)
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;
		ItemSet(type, p);
		TableUpdate();
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

		TableUpdate();
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
		TableUpdate();
	}

	// Get widget data
	Action WidgetGetKey() {
		Action action(Action::_key);
		if (ui.rbDown->isChecked()) action.d.key.state = QiKey::down;
		else if (ui.rbUp->isChecked()) action.d.key.state = QiKey::up;
		else if (ui.rbClick->isChecked()) action.d.key.state = QiKey::click;
		action.d.key.vk = ui.hkKey->virtualKey();
		return action;
	}
	Action WidgetGetKeyState() {
		Action action(Action::_keyState);
		action.d.keyState.state = ui.rbStateDown->isChecked();
		action.d.keyState.vk = ui.hkState->virtualKey();
		return action;
	}
	Action WidgetGetMouse() {
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
	Action WidgetGetDelay() {
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
	Action WidgetGetText() {
		Action action(Action::_text);
		action.d.text.str.copy((PCWSTR)(ui.etText->toPlainText().utf16()));
		return action;
	}
	Action WidgetGetColor() {
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
	Action WidgetGetLoop()
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
	Action WidgetGetImage()
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
		action.d.image.map = imageMap;
		return action;
	}
	Action WidgetGetPopText()
	{
		Action action(Action::_popText);
		action.d.popText.str.copy((PCWSTR)(ui.etPopText->text().utf16()));
		int time = 1000;
		if (ui.etPopTextTime->text() != "") time = ui.etPopTextTime->text().toInt();
		if (time > popTextTimeMax) time = popTextTimeMax;
		action.d.popText.time = time;
		return action;
	}

	// Load widget data
	void WidgetSetKey(const Action& action) {
		if (action.d.key.state == QiKey::down) ui.rbDown->setChecked(1);
		else if (action.d.key.state == QiKey::up) ui.rbUp->setChecked(1);
		else if (action.d.key.state == QiKey::click) ui.rbClick->setChecked(1);
		ui.hkKey->VirtualKey(action.d.key.vk);
	}
	void WidgetSetKeyState(const Action& action)
	{
		if (action.d.keyState.state) ui.rbStateDown->setChecked(true);
		if (!action.d.keyState.state) ui.rbStateUp->setChecked(true);
		ui.hkState->VirtualKey(action.d.keyState.vk);
	}
	void WidgetSetMouse(const Action& action) {
		if (action.d.mouse.move) ui.rbMove->setChecked(true), OnRbMouseMove(true);
		else ui.rbPos->setChecked(true), OnRbMousePos(true);
		ui.rbMoveTrack->setChecked(action.d.mouse.track);
		ui.etMoveSpeed->setText(QString::number(action.d.mouse.speed));
		ui.etX->setText(QString::number(action.d.mouse.x)); ui.etY->setText(QString::number(action.d.mouse.y)); if (action.d.mouse.ex > -1) ui.etMoveRand->setText(QString::number(action.d.mouse.ex));
	}
	void WidgetSetDelay(const Action& action) {
		ui.etDelayMin->setText(QString::number(action.d.delay.tmin));
		ui.etDelayMax->setText(QString::number(action.d.delay.tmax));
	}
	void WidgetSetText(const Action& action) {
		ui.etText->setText(QString::fromWCharArray(action.d.text.str.str()));
	}
	void WidgetSetColor(const Action& action) {
		if (action.d.color.unfind) ui.rbColorNFind->setChecked(true);
		else ui.rbColorFind->setChecked(true);
		ui.chbColorMove->setChecked(action.d.color.move);
		// Rect
		{
			ui.etColorL->setText(QString::number(action.d.color.rect.left));
			ui.etColorT->setText(QString::number(action.d.color.rect.top));
			ui.etColorR->setText(QString::number(action.d.color.rect.right));
			ui.etColorB->setText(QString::number(action.d.color.rect.bottom));
		}
		// Color
		{
			ui.etColorSim->setText(QString::number(action.d.color.rgbe.a));
			ui.etColorRed->setText(QString::number(action.d.color.rgbe.r));
			ui.etColorGreen->setText(QString::number(action.d.color.rgbe.g));
			ui.etColorBlue->setText(QString::number(action.d.color.rgbe.b));
			QString style = "background-color:rgb(";
			style += QString::number(action.d.color.rgbe.r);
			style += ",";
			style += QString::number(action.d.color.rgbe.g);
			style += ",";
			style += QString::number(action.d.color.rgbe.b);
			style += ");";
			ui.bnColorValue->setStyleSheet(style);
		}
	}
	void WidgetSetLoop(const Action& action) {
		ui.etCountMin->setText(QString::number(action.d.loop.cmin));
		ui.etCountMax->setText(QString::number(action.d.loop.cmax));
	}
	void WidgetSetImage(const Action& action) {
		if (action.d.image.unfind) ui.rbImageNFind->setChecked(true);
		else ui.rbImageFind->setChecked(true);
		ui.chbImageMove->setChecked(action.d.image.move);
		// Rect
		{
			ui.etImageL->setText(QString::number(action.d.image.rect.left));
			ui.etImageT->setText(QString::number(action.d.image.rect.top));
			ui.etImageR->setText(QString::number(action.d.image.rect.right));
			ui.etImageB->setText(QString::number(action.d.image.rect.bottom));
		}
		// Image
		{
			ui.etImageSim->setText(QString::number(action.d.image.sim));
			imageMap = action.d.image.map;
			HBITMAP hbmp = Image::toBmp32(imageMap);
			if (hbmp) ui.lbImageView->setPixmap(QtWin::fromHBITMAP(hbmp));
		}
	}
	void WidgetSetPopText(const Action& action) {
		ui.etPopText->setText(QString::fromWCharArray(action.d.popText.str.str()));
		ui.etPopTextTime->setText(QString::number(action.d.popText.time));
	}
};