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

	enum TabIndex
	{
		tab_base,
		tab_loop,
		tab_state,
		tab_jump,
		tab_color,
		tab_image,
		tab_text,
		tab_window
	};

	// edit box range
	const int32 posMin = -10000;
	const int32 posMax = 10000;
	const int32 moveSpeedMax = 99;
	const int32 moveRandMax = 9999;
	const int32 delayMax = 999999;
	const int32 loopCountMax = 9999;
	const int32 timerMax = 999999;
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
	QAction* muEdit;

	QTimer* testTimer;
	uint32 testCount = 0;

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
	bool updating = false;

public:
	EditUi(EditParam ep) : QDialog()
	{
		macro = ep.macro, actions = ep.actions, child = ep.child;
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		Init();
		Event();
		StyleGroup();
	}

	void StyleGroup()
	{
		setProperty("group", "frame");
		ui.titleWidget->setProperty("group", "title");
		ui.clientWidget->setProperty("group", "client");

		ui.bnRun->setProperty("group", "title-run_button");
		ui.bnClose->setProperty("group", "title-close_button");

		ui.bnPos->setProperty("group", "get_button");
		ui.bnColorRect->setProperty("group", "get_button");
		ui.bnImageRect->setProperty("group", "get_button");
		ui.bnImageShot->setProperty("group", "get_button");
		ui.bnWndSelect->setProperty("group", "get_button");

		ui.bnKeyAdd->setProperty("group", "edit-add_button");
		ui.bnMoveAdd->setProperty("group", "edit-add_button");
		ui.bnDelayAdd->setProperty("group", "edit-add_button");
		ui.bnLoopAdd->setProperty("group", "edit-add_button");
		ui.bnTimerAdd->setProperty("group", "edit-add_button");
		ui.bnStateAdd->setProperty("group", "edit-add_button");
		ui.bnRememberPosAdd->setProperty("group", "edit-add_button");
		ui.bnRecoverPosAdd->setProperty("group", "edit-add_button");
		ui.bnEndLoopAdd->setProperty("group", "edit-add_button");
		ui.bnEndAdd->setProperty("group", "edit-add_button");
		ui.bnColorAdd->setProperty("group", "edit-add_button");
		ui.bnImageAdd->setProperty("group", "edit-add_button");
		ui.bnTextAdd->setProperty("group", "edit-add_button");
		ui.bnPopTextAdd->setProperty("group", "edit-add_button");
		ui.bnJumpAdd->setProperty("group", "edit-add_button");
		ui.bnJumpPointAdd->setProperty("group", "edit-add_button");

		ui.bnLoopEdit->setProperty("group", "edit-edit_button");
		ui.bnTimerEdit->setProperty("group", "edit-edit_button");
		ui.bnStateEdit->setProperty("group", "edit-edit_button");
		ui.bnColorEdit->setProperty("group", "edit-edit_button");
		ui.bnImageEdit->setProperty("group", "edit-edit_button");

		ui.chbMoveTrack->setProperty("group", "check");
		ui.chbColorMove->setProperty("group", "check");
		ui.chbImageMove->setProperty("group", "check");
		ui.chbPopTextWait->setProperty("group", "check");
		ui.chbWnd->setProperty("group", "check");
		ui.chbChildWnd->setProperty("group", "check");

		ui.rbRunning->setProperty("group", "radio");
		ui.rbEnding->setProperty("group", "radio");
		ui.rbStateDown->setProperty("group", "radio");
		ui.rbClick->setProperty("group", "radio");
		ui.rbDown->setProperty("group", "radio");
		ui.rbUp->setProperty("group", "radio");
		ui.rbPos->setProperty("group", "radio");
		ui.rbMove->setProperty("group", "radio");
		ui.rbStateDown->setProperty("group", "radio");
		ui.rbStateUp->setProperty("group", "radio");
		ui.rbColorFind->setProperty("group", "radio");
		ui.rbColorNFind->setProperty("group", "radio");
		ui.rbImageFind->setProperty("group", "radio");
		ui.rbImageNFind->setProperty("group", "radio");

		menu->setProperty("group", "context_menu");

		ui.etX->setProperty("group", "line_edit");
		ui.etY->setProperty("group", "line_edit");
		ui.etMoveSpeed->setProperty("group", "line_edit");
		ui.etMoveRand->setProperty("group", "line_edit");
		ui.etDelayMin->setProperty("group", "line_edit");
		ui.etDelayMax->setProperty("group", "line_edit");
		ui.etCountMin->setProperty("group", "line_edit");
		ui.etCountMax->setProperty("group", "line_edit");
		ui.etTimerMin->setProperty("group", "line_edit");
		ui.etTimerMax->setProperty("group", "line_edit");
		ui.etColorL->setProperty("group", "line_edit");
		ui.etColorT->setProperty("group", "line_edit");
		ui.etColorR->setProperty("group", "line_edit");
		ui.etColorB->setProperty("group", "line_edit");
		ui.etColorRed->setProperty("group", "line_edit");
		ui.etColorGreen->setProperty("group", "line_edit");
		ui.etColorBlue->setProperty("group", "line_edit");
		ui.etColorSim->setProperty("group", "line_edit");
		ui.etImageL->setProperty("group", "line_edit");
		ui.etImageT->setProperty("group", "line_edit");
		ui.etImageR->setProperty("group", "line_edit");
		ui.etImageB->setProperty("group", "line_edit");
		ui.etImageSim->setProperty("group", "line_edit");
		ui.etPopText->setProperty("group", "line_edit");
		ui.etPopTextTime->setProperty("group", "line_edit");
		ui.etWndName->setProperty("group", "line_edit");

		ui.etText->setProperty("group", "text_edit");

		ui.hkKey->setProperty("group", "key_edit");
		ui.hkState->setProperty("group", "key_edit");

		ui.tabWidget->setProperty("group", "tab_widget");
		ui.tabWidget->tabBar()->setProperty("group", "tab_widget_bar");

		ui.tbActions->setProperty("group", "table");
		ui.tbActions->horizontalHeader()->setProperty("group", "table_header");
		ui.tbActions->verticalHeader()->setProperty("group", "table_header");

		ui.lsJumpPoint->setProperty("group", "table");

		const QObjectList& objs = ui.tbActions->children();
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

private:
	void Init()
	{
		if ("table context menu")
		{
			menu = new QMenu(this);
			muDel = new QAction("删除", this);
			muChange = new QAction("修改", this);
			muCut = new QAction("剪切", this);
			muCopy = new QAction("复制", this);
			muPaste = new QAction("粘贴", this);
			muEdit = new QAction("编辑", this);

			menu->addAction(muDel);
			menu->addAction(muChange);
			menu->addAction(muCut);
			menu->addAction(muCopy);
			menu->addAction(muPaste);
			menu->addAction(muEdit);

			connect(muDel, SIGNAL(triggered()), this, SLOT(OnMenuDel()));
			connect(muChange, SIGNAL(triggered()), this, SLOT(OnMenuChange()));
			connect(muCut, SIGNAL(triggered()), this, SLOT(OnMenuCut()));
			connect(muCopy, SIGNAL(triggered()), this, SLOT(OnMenuCopy()));
			connect(muPaste, SIGNAL(triggered()), this, SLOT(OnMenuPaste()));
			connect(muEdit, SIGNAL(triggered()), this, SLOT(OnMenuEdit()));
		}
		if ("buttons")
		{
			if ("text")
			{
				ui.bnKeyAdd->setText(Qi::ui.text.etAdd);
				ui.bnStateAdd->setText(Qi::ui.text.etAdd);
				ui.bnMoveAdd->setText(Qi::ui.text.etAdd);
				ui.bnDelayAdd->setText(Qi::ui.text.etAdd);
				ui.bnLoopAdd->setText(Qi::ui.text.etAdd);
				ui.bnTextAdd->setText(Qi::ui.text.etAdd);
				ui.bnColorAdd->setText(Qi::ui.text.etAdd);
				ui.bnImageAdd->setText(Qi::ui.text.etAdd);
				ui.bnEndAdd->setText(Qi::ui.text.etAdd);
				ui.bnEndLoopAdd->setText(Qi::ui.text.etAdd);
				ui.bnJumpAdd->setText(Qi::ui.text.etAdd);
				ui.bnJumpPointAdd->setText(Qi::ui.text.etAdd);
				ui.bnStateEdit->setText(Qi::ui.text.etEdit);
				ui.bnLoopEdit->setText(Qi::ui.text.etEdit);
				ui.bnColorEdit->setText(Qi::ui.text.etEdit);
				ui.bnImageEdit->setText(Qi::ui.text.etEdit);
				ui.bnTimerEdit->setText(Qi::ui.text.etEdit);
			}
			if ("clear shortcut")
			{
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
				ui.bnTimerEdit->setShortcut(Qt::Key_unknown);
				ui.bnJumpAdd->setShortcut(Qt::Key_unknown);
				ui.bnJumpPointAdd->setShortcut(Qt::Key_unknown);
				ui.bnStateEdit->setShortcut(Qt::Key_unknown);
				ui.bnLoopEdit->setShortcut(Qt::Key_unknown);
				ui.bnColorEdit->setShortcut(Qt::Key_unknown);
				ui.bnImageEdit->setShortcut(Qt::Key_unknown);
			}
			if ("disable edit buttons")
			{
				DisableButtons();
			}
		}
		if ("set radio group")
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
		if ("line edit range")
		{
			ui.etX->setValidator(new QIntValidator(0, posMax, this));
			ui.etY->setValidator(new QIntValidator(0, posMax, this));
			ui.etMoveSpeed->setValidator(new QIntValidator(0, moveSpeedMax, this));
			ui.etMoveRand->setValidator(new QIntValidator(0, moveRandMax, this));
			ui.etDelayMin->setValidator(new QIntValidator(0, delayMax, this));
			ui.etDelayMax->setValidator(new QIntValidator(0, delayMax, this));
			ui.etCountMin->setValidator(new QIntValidator(0, loopCountMax, this));
			ui.etCountMax->setValidator(new QIntValidator(0, loopCountMax, this));
			ui.etTimerMax->setValidator(new QIntValidator(0, timerMax, this));
			ui.etTimerMin->setValidator(new QIntValidator(0, timerMax, this));
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
		if ("table")
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
		if ("key edit")
		{
			ui.hkKey->setMode(QKeyEdit::Mode::solid);
			ui.hkKey->setMouseEnable(true);
			ui.hkKey->setWheelEnable(true);
			ui.hkKey->setKey(QKeyEdit::Key(VK_LBUTTON));

			ui.hkState->setMode(QKeyEdit::Mode::solid);
			ui.hkState->setMouseEnable(true);
			ui.hkState->setWheelEnable(true);
			ui.hkState->setPadEnable(true);
			ui.hkState->setKey(QKeyEdit::Key(VK_LBUTTON));
		}
		if ("timer")
		{
			testTimer = new QTimer(this);
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
	void Event()
	{
		if ("title")
		{
			connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
			connect(ui.bnRun, SIGNAL(clicked()), this, SLOT(OnBnRun()));
			connect(ui.bnWndSelect, SIGNAL(clicked()), this, SLOT(OnBnWndSelect()));
			connect(ui.chbWnd, SIGNAL(clicked()), this, SLOT(OnChbWnd()));
			connect(ui.chbChildWnd, SIGNAL(clicked()), this, SLOT(OnChbChildWnd()));
		}
		if ("table")
		{
			ui.tbActions->installEventFilter(this);
			ui.tbActions->viewport()->installEventFilter(this);
			connect(ui.rbRunning, SIGNAL(toggled(bool)), this, SLOT(OnRbRunning(bool)));
			connect(ui.rbEnding, SIGNAL(toggled(bool)), this, SLOT(OnRbEnding(bool)));
			connect(ui.tbActions, SIGNAL(cellChanged(int, int)), this, SLOT(OnTbChanged(int, int)));
			connect(ui.tbActions, SIGNAL(cellClicked(int, int)), this, SLOT(OnTbClicked(int, int)));
			connect(ui.tbActions, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(OnTbSelect(QTableWidgetItem*, QTableWidgetItem*)));
			connect(ui.tbActions, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(OnTbDoubleClick(int, int)));
			connect(ui.tbActions, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnTableMenu(const QPoint&)));
		}
		if ("list")
		{
			connect(ui.lsJumpPoint, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(OnLsJumpClick(QListWidgetItem*)));
		}
		if ("action widget")
		{
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
			connect(ui.bnTimerAdd, SIGNAL(clicked()), this, SLOT(OnBnTimerAdd()));
			connect(ui.bnTimerEdit, SIGNAL(clicked()), this, SLOT(OnBnTimerEdit()));
			connect(ui.etTimerMin, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtTimerMin(const QString&)));
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
			connect(ui.bnJumpAdd, SIGNAL(clicked()), this, SLOT(OnBnJumpAdd()));
			connect(ui.bnJumpPointAdd, SIGNAL(clicked()), this, SLOT(OnBnJumpPointAdd()));
		}
		if ("timer")
		{
			connect(testTimer, SIGNAL(timeout()), this, SLOT(OnTestTimer()));
		}
	}

	void DisableButtons()
	{
		ui.bnStateEdit->setDisabled(true);
		ui.bnLoopEdit->setDisabled(true);
		ui.bnTimerEdit->setDisabled(true);
		ui.bnColorEdit->setDisabled(true);
		ui.bnImageEdit->setDisabled(true);
		ui.bnJumpAdd->setDisabled(true);
	}

	void SelectWindow()
	{
		QPoint pt = pos();
		move(-1000, -1000);
		macro->wi = QiFn::WindowSelection();
		move(pt);
		SetWindowMode();
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
		Actions* next = nullptr;
		bool failed = false;
		std::visit([&title, &failed, &next](auto&& var)
			{
				using T = std::decay_t<decltype(var)>;
				if constexpr (std::is_same_v<T, QiColor>)
				{
					QiColor& color = var;
					next = &color.next;
					title = "编辑 - 查找颜色";
				}
				else if constexpr (std::is_same_v<T, QiLoop>)
				{
					QiLoop& loop = var;
					next = &loop.next;
					title = "编辑 - 循环";
				}
				else if constexpr (std::is_same_v<T, QiKeyState>)
				{
					QiKeyState& keyState = var;
					next = &keyState.next;
					title = "编辑 - 按键状态";
				}
				else if constexpr (std::is_same_v<T, QiImage>)
				{
					QiImage& image = var;
					next = &image.next;
					title = "编辑 - 查找图片";
				}
				else if constexpr (std::is_same_v<T, QiTimer>)
				{
					QiTimer& timer = var;
					next = &timer.next;
					title = "编辑 - 定时";
				}
				else
				{
					failed = true;
				}
			}, actions->at(p));

		if (!failed)
		{
			EditParam epc;
			epc.macro = macro;
			epc.child = true;
			epc.actions = next;
			
			EditUi edit(epc);
			edit.setWindowTitle(title);
			QPoint pt = pos();
			move(-10000, -10000);
			edit.move(pt);
			edit.exec();
			move(pt);
			JumpPointUpdate();
		}
	}

	void JumpPointCall(const Actions& actions)
	{
		for (size_t i = 0; i < actions.size(); i++)
		{
			std::visit([&](auto&& var)
				{
					using T = std::decay_t<decltype(var)>;
					if constexpr (std::is_same_v<T, QiJumpPoint>)
					{
						const QiJumpPoint& jumpPoint = var;
						QListWidgetItem* item = new QListWidgetItem(QString::number(jumpPoint.id) + QString::fromUtf8("　　　") + WToQString(jumpPoint.mark));
						item->setData(DataRole::id, jumpPoint.id);
						ui.lsJumpPoint->addItem(item);
					}
					else
					{
						const Actions* next = nullptr;

						if constexpr (std::is_same_v<T, QiColor>)
						{
							const QiColor& color = var;
							if (color.next.size()) next = &color.next;
						}
						else if constexpr (std::is_same_v<T, QiLoop>)
						{
							const QiLoop& loop = var;
							if (loop.next.size()) next = &loop.next;
						}
						else if constexpr (std::is_same_v<T, QiKeyState>)
						{
							const QiKeyState& keyState = var;
							if (keyState.next.size()) next = &keyState.next;
						}
						else if constexpr (std::is_same_v<T, QiImage>)
						{
							const QiImage& image = var;
							if (image.next.size()) next = &image.next;
						}
						else if constexpr (std::is_same_v<T, QiTimer>)
						{
							const QiTimer& timer = var;
							if (timer.next.size()) next = &timer.next;
						}

						if (next) JumpPointCall(*next);
					}
				}, actions.at(i));
		}
	}
	void JumpPointUpdate()
	{
		Actions* actions = &macro->acRun;
		if (ui.rbEnding->isChecked()) actions = &macro->acEnd;
		ui.lsJumpPoint->clear();
		JumpPointCall(*actions);
	}

	void TableUpdate()
	{
		DisableButtons();
		updating = true;
		ui.tbActions->clearMask();
		ui.tbActions->setRowCount(actions->size());
		ui.tbActions->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.tbActions->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tbActions->verticalHeader()->setDefaultSectionSize(0);

		for (size_t i = 0; i < actions->size(); i++)
		{
			QString type;
			QString param;
			QString mark;
			std::visit([&type, &param, &mark](auto&& var)
				{
					using T = std::decay_t<decltype(var)>;
					if constexpr (std::is_same_v<T, QiEnd>)
					{
						const QiEnd& end = var;
						type = Qi::ui.text.acEnd;
						mark = WToQString(end.mark);
					}
					else if constexpr (std::is_same_v<T, QiDelay>)
					{
						const QiDelay& delay = var;
						type = Qi::ui.text.acWait;
						mark = WToQString(delay.mark);

						if (delay.min != delay.max)
						{
							param = QString::number(delay.min);
							param += " ~ ";
							param += QString::number(delay.max);
						}
						else param = QString::number(delay.min);
					}
					else if constexpr (std::is_same_v<T, QiKey>)
					{
						const QiKey& key = var;
						if (key.state == QiKey::up) type = Qi::ui.text.acUp;
						else if (key.state == QiKey::down) type = Qi::ui.text.acDown;
						else if (key.state == QiKey::click) type = Qi::ui.text.acClick;
						mark = WToQString(key.mark);

						param = QKeyEdit::keyName(key.vk);
					}
					else if constexpr (std::is_same_v<T, QiMouse>)
					{
						const QiMouse& mouse = var;
						if (mouse.move) type = Qi::ui.text.acMove;
						else type = Qi::ui.text.acPos;
						mark = WToQString(mouse.mark);

						param = QString::number(mouse.x);
						param += " - ";
						param += QString::number(mouse.y);
						if (mouse.ex)
						{
							param += "ㅤㅤ随机：";
							param += QString::number(mouse.ex);
						}
						if (mouse.track)
						{
							param += "ㅤㅤ轨迹：";
							param += QString::number(mouse.speed);
						}
					}
					else if constexpr (std::is_same_v<T, QiText>)
					{
						const QiText& text = var;
						type = Qi::ui.text.acText;
						mark = WToQString(text.mark);

						param = WToQString(text.str.substr(0, 32));
						if (text.str.size() > 31) param += "...";
					}
					else if constexpr (std::is_same_v<T, QiColor>)
					{
						const QiColor& color = var;
						type = Qi::ui.text.acColor;
						mark = WToQString(color.mark);

						param = "(";
						param += QString::number(color.rect.left);
						param += ",";
						param += QString::number(color.rect.top);
						param += ",";
						param += QString::number(color.rect.right);
						param += ",";
						param += QString::number(color.rect.bottom);
						param += ")　(";
						param += QString::number(color.rgbe.r);
						param += ",";
						param += QString::number(color.rgbe.g);
						param += ",";
						param += QString::number(color.rgbe.b);
						param += ",";
						param += QString::number(color.rgbe.a);
						if (color.unfind) param += ")　未找到";
						else
						{
							param += ")　找到";
							if (color.move) param += "并移动";
							else param += "不移动";
						}
					}
					else if constexpr (std::is_same_v<T, QiLoop>)
					{
						const QiLoop& loop = var;
						type = Qi::ui.text.acLoop;
						mark = WToQString(loop.mark);

						if ((loop.min == 0 && loop.max == 0))
							param = "无限";
						else if (loop.min == loop.max)
							param = QString::number(loop.min);
						else
						{
							param = QString::number(loop.min);
							param += " ~ ";
							param += QString::number(loop.max);
						}
					}
					else if constexpr (std::is_same_v<T, QiLoopEnd>)
					{
						const QiLoopEnd& loopEnd = var;
						type = Qi::ui.text.acEndLoop;
						mark = WToQString(loopEnd.mark);
					}
					else if constexpr (std::is_same_v<T, QiKeyState>)
					{
						const QiKeyState& keyState = var;
						type = Qi::ui.text.acKeyState;
						mark = WToQString(keyState.mark);

						if (keyState.state) param = "按下了　";
						else param = "松开了　";
						param += QKeyEdit::keyName(keyState.vk);
					}
					else if constexpr (std::is_same_v<T, QiRecoverPos>)
					{
						const QiRecoverPos& recoverPos = var;
						type = Qi::ui.text.acRecoverPos;
						mark = WToQString(recoverPos.mark);
					}
					else if constexpr (std::is_same_v<T, QiImage>)
					{
						const QiImage& image = var;
						type = Qi::ui.text.acImage;
						mark = WToQString(image.mark);

						param = "(";
						param += QString::number(image.rect.left);
						param += ",";
						param += QString::number(image.rect.top);
						param += ",";
						param += QString::number(image.rect.right);
						param += ",";
						param += QString::number(image.rect.bottom);
						param += ")　(";
						param += QString::number(image.map.width());
						param += "x";
						param += QString::number(image.map.height());
						param += ")　";
						param += QString::number(image.sim);
						if (image.unfind) param += "　未找到";
						else
						{
							param += "　找到";
							if (image.move) param += "并移动";
							else param += "不移动";
						}
					}
					else if constexpr (std::is_same_v<T, QiPopText>)
					{
						const QiPopText& popText = var;
						type = Qi::ui.text.acPopText;
						mark = WToQString(popText.mark);

						std::wstring w;
						param = WToQString(popText.str);
						param += "　时长：";
						param += QString::number(popText.time);
					}
					else if constexpr (std::is_same_v<T, QiRememberPos>)
					{
						const QiRememberPos& rememberPos = var;
						type = Qi::ui.text.acRememberPos;
						mark = WToQString(rememberPos.mark);
					}
					else if constexpr (std::is_same_v<T, QiTimer>)
					{
						const QiTimer& timer = var;
						type = Qi::ui.text.acTimer;
						mark = WToQString(timer.mark);

						if (timer.min == timer.max) param = QString::number(timer.min);
						else
						{
							param = QString::number(timer.min);
							param += " ~ ";
							param += QString::number(timer.max);
						}
					}
					else if constexpr (std::is_same_v<T, QiJump>)
					{
						const QiJump& jump = var;
						type = Qi::ui.text.acJump;
						mark = WToQString(jump.mark);

						param = "id：";
						param += QString::number(jump.id);
					}
					else if constexpr (std::is_same_v<T, QiJumpPoint>)
					{
						const QiJumpPoint& jumpPoint = var;
						type = Qi::ui.text.acJumpPoint;
						mark = WToQString(jumpPoint.mark);

						param = "id：";
						param += QString::number(jumpPoint.id);
					}
					else
					{
						type = "加载失败";
					}
				}, actions->at(i));

			QTableWidgetItem* item = new QTableWidgetItem(type);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbActions->setItem(i, 0, item);

			item = new QTableWidgetItem(param);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbActions->setItem(i, 1, item);

			item = new QTableWidgetItem(mark);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbActions->setItem(i, 2, item);
		}
		JumpPointUpdate();
		updating = false;
	}

private:
	bool eventFilter(QObject* obj, QEvent* e)
	{
		if (obj == ui.tbActions)
		{
			if (e->type() == QEvent::KeyPress)
			{
				QKeyEvent* key = (QKeyEvent*)e;
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
			if (e->type() == QEvent::Drop)
			{
				QDropEvent* drop = static_cast<QDropEvent*>(e);
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
		Qi::application->setStyleSheet(Qi::ui.themes[Qi::set.theme].style);
		SetForegroundWindow((HWND)QWidget::winId());
	}
	QPoint msPos; bool mouseDown = false; void mousePressEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) msPos = e->pos(), mouseDown = true; e->accept(); }void mouseMoveEvent(QMouseEvent* e) { if (mouseDown) move(e->pos() + pos() - msPos); }void mouseReleaseEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) mouseDown = false; }
private Q_SLOTS:
	void OnTestTimer()
	{
		if (QiThread::MacroRunActive(macro))
		{
			if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_F10))
			{
				QiThread::ExitMacroRun(macro);
			}
		}
		else
		{
			testTimer->stop();
			timeEndPeriod(1);
			Qi::run = false;
			macro->count = testCount;
			Thread::Sleep(300);
			setEnabled(true);
		}
	}

	// Title
	void OnBnClose()
	{
		close();
	}
	void OnBnRun()
	{
		setDisabled(true);
		testCount = macro->count;
		macro->count = 1;
		Qi::run = true;
		timeBeginPeriod(1);
		QiThread::StartMacroRun(macro);
		testTimer->start(16);
	}

	// Rutton context menu
	void OnMenuDel()
	{
		ItemDel();
	}
	void OnMenuChange()
	{
		changing ? SetChange(false) : SetChange(true);
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
	void OnMenuEdit()
	{
		NextEdit();
	}

	// List
	void OnLsJumpClick(QListWidgetItem*)
	{
		if (ui.lsJumpPoint->currentRow() < 0) ui.bnJumpAdd->setDisabled(true);
		else ui.bnJumpAdd->setEnabled(true);
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
		if (updating) return;
		if (row < 0) return;
		if (column != 2) return;
		QiBase& base = actions->at(row).base();
		base.mark = QStringToW(ui.tbActions->item(row, 2)->text());
		if (actions->at(row).base().type == QiType::jumpPoint) JumpPointUpdate();
	}
	void OnTbClicked(int row, int column)
	{
		DisableButtons();

		pv.hide();
		rv.hide();

		if (row < 0) return;
		if (column == 2) ui.tbActions->editItem(ui.tbActions->item(row, column));
		else
		{
			std::visit([this](auto&& var)
				{
					using T = std::decay_t<decltype(var)>;
					if constexpr (std::is_same_v<T, QiEnd>)
					{
						const QiEnd& end = var;
						ui.tabWidget->setCurrentIndex(tab_state);
					}
					else if constexpr (std::is_same_v<T, QiDelay>)
					{
						const QiDelay& delay = var;
						ui.tabWidget->setCurrentIndex(tab_base);
					}
					else if constexpr (std::is_same_v<T, QiKey>)
					{
						const QiKey& key = var;
						ui.tabWidget->setCurrentIndex(tab_base);
					}
					else if constexpr (std::is_same_v<T, QiMouse>)
					{
						const QiMouse& mouse = var;
						ui.tabWidget->setCurrentIndex(tab_base);

						if (!mouse.move)
						{
							rv.hide();
							POINT pt;
							if (macro->wndState)
							{
								if (!macro->wi.Update())
								{
									SelectWindow();
								}
								POINT rpt = QiFn::WATR({ mouse.x, mouse.y }, macro->wi.wnd);
								pt = Window::pos(macro->wi.wnd);
								pt.x += rpt.x, pt.y += rpt.y;
							}
							else
							{
								pt = QiFn::ATR({ mouse.x, mouse.y });
							}
							pv.Show(pt);
						}
					}
					else if constexpr (std::is_same_v<T, QiText>)
					{
						const QiText& text = var;
						ui.tabWidget->setCurrentIndex(tab_text);
					}
					else if constexpr (std::is_same_v<T, QiColor>)
					{
						const QiColor& color = var;
						ui.tabWidget->setCurrentIndex(tab_color);

						ui.bnColorEdit->setEnabled(true);

						pv.hide();
						RECT rect;
						if (macro->wndState)
						{
							if (!macro->wi.Update())
							{
								SelectWindow();
							}
							rect = QiFn::WATRR(color.rect, macro->wi.wnd);
							POINT pt = Window::pos(macro->wi.wnd);
							rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
						}
						else
						{
							rect = QiFn::ATRR(color.rect);
						}
						rv.Show(rect);
					}
					else if constexpr (std::is_same_v<T, QiLoop>)
					{
						const QiLoop& loop = var;
						ui.tabWidget->setCurrentIndex(tab_loop);

						ui.bnLoopEdit->setEnabled(true);
					}
					else if constexpr (std::is_same_v<T, QiLoopEnd>)
					{
						const QiLoopEnd& loopEnd = var;
						ui.tabWidget->setCurrentIndex(tab_state);
					}
					else if constexpr (std::is_same_v<T, QiKeyState>)
					{
						const QiKeyState& keyState = var;
						ui.tabWidget->setCurrentIndex(tab_state);

						ui.bnStateEdit->setEnabled(true);
					}
					else if constexpr (std::is_same_v<T, QiRecoverPos>)
					{
						const QiRecoverPos& recoverPos = var;
						ui.tabWidget->setCurrentIndex(tab_state);
					}
					else if constexpr (std::is_same_v<T, QiImage>)
					{
						const QiImage& image = var;
						ui.tabWidget->setCurrentIndex(tab_image);

						ui.bnImageEdit->setEnabled(true);

						pv.hide();
						RECT rect;
						if (macro->wndState)
						{
							if (!macro->wi.Update())
							{
								SelectWindow();
							}
							rect = QiFn::WATRR(image.rect, macro->wi.wnd);
							POINT pt = Window::pos(macro->wi.wnd);
							rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
						}
						else
						{
							rect = QiFn::ATRR(image.rect);
						}
						rv.Show(rect);

						WidgetSetImage(image);
					}
					else if constexpr (std::is_same_v<T, QiPopText>)
					{
						const QiPopText& popText = var;
						ui.tabWidget->setCurrentIndex(tab_text);
					}
					else if constexpr (std::is_same_v<T, QiRememberPos>)
					{
						const QiRememberPos& rememberPos = var;
						ui.tabWidget->setCurrentIndex(tab_state);
					}
					else if constexpr (std::is_same_v<T, QiTimer>)
					{
						const QiTimer& timer = var;
						ui.tabWidget->setCurrentIndex(tab_loop);

						ui.bnTimerEdit->setEnabled(true);
					}
					else if constexpr (std::is_same_v<T, QiJump>)
					{
						const QiJump& jump = var;
						ui.tabWidget->setCurrentIndex(tab_jump);
					}
					else if constexpr (std::is_same_v<T, QiJumpPoint>)
					{
						const QiJumpPoint& jumpPoint = var;
						ui.tabWidget->setCurrentIndex(tab_jump);
					}
					else
					{
						ui.tabWidget->setCurrentIndex(tab_base);
					}
				},
			actions->at(row));
		}
	}
	void OnTbSelect(QTableWidgetItem*, QTableWidgetItem*)
	{
		OnTbClicked(ui.tbActions->currentRow(), ui.tbActions->currentColumn());
	}
	void OnTbDoubleClick(int row, int column)
	{
		changing ? SetChange(false) : SetChange(true);
	}
	void OnTableMenu(const QPoint& pt)
	{
		muDel->setDisabled(true);
		muChange->setDisabled(true);
		muCut->setDisabled(true);
		muCopy->setDisabled(true);
		muPaste->setDisabled(true);
		muEdit->setDisabled(true);

		QList<QTableWidgetItem*> items = ui.tbActions->selectedItems();
		if (items.size())
		{
			muDel->setEnabled(true);
			muChange->setEnabled(true);
			muCut->setEnabled(true);
			muCopy->setEnabled(true);
			if (items.size() == ui.tbActions->columnCount())
			{
				std::visit([this](auto&& var)
					{
						using T = std::decay_t<decltype(var)>;
						if constexpr (std::is_same_v<T, QiColor>)
						{
							const QiColor& color = var;
							muEdit->setEnabled(true);
						}
						else if constexpr (std::is_same_v<T, QiLoop>)
						{
							const QiLoop& loop = var;
							muEdit->setEnabled(true);
						}
						else if constexpr (std::is_same_v<T, QiKeyState>)
						{
							const QiKeyState& keyState = var;
							muEdit->setEnabled(true);
						}
						else if constexpr (std::is_same_v<T, QiImage>)
						{
							const QiImage& image = var;
							muEdit->setEnabled(true);
						}
						else if constexpr (std::is_same_v<T, QiTimer>)
						{
							const QiTimer& timer = var;
							muEdit->setEnabled(true);
						}
					}, actions->at(items[0]->row()));
			}
		}

		if (Qi::clipboard.size()) muPaste->setEnabled(true);

		menu->exec(QCursor::pos());
	}

	// Window mode
	void OnBnWndSelect()
	{
		SelectWindow();
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
		if (changing) ItemChange(QiType::key);
		else ItemAdd(QiType::key);
	}
	void OnBnKeyStateAdd()
	{
		if (changing) ItemChange(QiType::keyState);
		else ItemAdd(QiType::keyState);
	}
	void OnBnKeyStateEdit()
	{
		NextEdit();
	}
	void OnBnMouseAdd()
	{
		if (changing) ItemChange(QiType::mouse);
		else ItemAdd(QiType::mouse);
	}
	void OnBnMousePos()
	{
		QPointSelection ps;
		POINT pt;
		if (macro->wndState)
		{
			if (!macro->wi.Update())
			{
				SelectWindow();
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
		QiMouse mouse(WidgetGetMouse());
		mouse.x = pt.x;
		mouse.y = pt.y;
		WidgetSetMouse(mouse);
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
		if (changing) ItemChange(QiType::delay);
		else ItemAdd(QiType::delay);
	}
	void OnEtDelayMin(const QString& text)
	{
		ui.etDelayMax->setText(text);
	}
	void OnBnTextAdd()
	{
		if (changing) ItemChange(QiType::text);
		else ItemAdd(QiType::text);
	}
	void OnBnLoopAdd()
	{
		if (changing) ItemChange(QiType::loop);
		else ItemAdd(QiType::loop);
	}
	void OnBnLoopEdit()
	{
		NextEdit();
	}
	void OnEtLoopMin(const QString& text)
	{
		ui.etCountMax->setText(text);
	}
	void OnBnTimerAdd()
	{
		if (changing) ItemChange(QiType::timer);
		else ItemAdd(QiType::timer);
	}
	void OnBnTimerEdit()
	{
		NextEdit();
	}
	void OnEtTimerMin(const QString& text)
	{
		ui.etTimerMax->setText(text);
	}
	void OnBnColorAdd()
	{
		if (changing) ItemChange(QiType::color);
		else ItemAdd(QiType::color);
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
				SelectWindow();
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

		QiColor color(WidgetGetColor());
		color.rect = rect;
		WidgetSetColor(color);
	}
	void OnBnColorValue()
	{
		QColorSelection cs;
		QColorDialog cd(cs.Start(), this);
		cd.setStyleSheet(Qi::ui.dialogStyle);
		cd.exec();
		QiColor color(WidgetGetColor());
		color.rgbe.r = cd.currentColor().red();
		color.rgbe.g = cd.currentColor().green();
		color.rgbe.b = cd.currentColor().blue();
		WidgetSetColor(color);
	}
	void OnBnEndAdd()
	{
		if (changing) ItemChange(QiType::end);
		else ItemAdd(QiType::end);
	}
	void OnBnEndLoopAdd()
	{
		if (changing) ItemChange(QiType::loopEnd);
		else ItemAdd(QiType::loopEnd);
	}
	void OnBnRememberPosAdd()
	{
		if (changing) ItemChange(QiType::rememberPos);
		else ItemAdd(QiType::rememberPos);
	}
	void OnBnRecoverPosAdd()
	{
		if (changing) ItemChange(QiType::recoverPos);
		else ItemAdd(QiType::recoverPos);
	}
	void OnBnImageAdd()
	{
		if (changing) ItemChange(QiType::image);
		else ItemAdd(QiType::image);
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
				SelectWindow();
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

		QiImage image(WidgetGetImage());
		image.rect = rect;
		WidgetSetImage(image);
	}
	void OnBnImageShot()
	{
		QRectSelection rs;
		RECT rect = rs.Start();
		QiImage image(WidgetGetImage());
		Image::ScreenRgbMap(imageMap, rect);
		image.map = imageMap;
		WidgetSetImage(image);
	}
	void OnBnPopTextAdd()
	{
		if (changing) ItemChange(QiType::popText);
		else ItemAdd(QiType::popText);
	}
	void OnBnJumpAdd()
	{
		if (changing) ItemChange(QiType::jump);
		else ItemAdd(QiType::jump);
	}
	void OnBnJumpPointAdd()
	{
		if (changing) ItemChange(QiType::jumpPoint);
		else ItemAdd(QiType::jumpPoint);
	}

private:
	// Action
	void SetChange(bool state)
	{
		if (state)
		{
			int p = ui.tbActions->currentRow();  if (p < 0) return;
			ItemUse(p);
			changing = state;
			{
				ui.bnKeyAdd->setText(Qi::ui.text.etChange);
				ui.bnStateAdd->setText(Qi::ui.text.etChange);
				ui.bnMoveAdd->setText(Qi::ui.text.etChange);
				ui.bnDelayAdd->setText(Qi::ui.text.etChange);
				ui.bnLoopAdd->setText(Qi::ui.text.etChange);
				ui.bnTimerAdd->setText(Qi::ui.text.etChange);
				ui.bnTextAdd->setText(Qi::ui.text.etChange);
				ui.bnColorAdd->setText(Qi::ui.text.etChange);
				ui.bnEndAdd->setText(Qi::ui.text.etChange);
				ui.bnEndLoopAdd->setText(Qi::ui.text.etChange);
				ui.bnRecoverPosAdd->setText(Qi::ui.text.etChange);
				ui.bnImageAdd->setText(Qi::ui.text.etChange);
				ui.bnPopTextAdd->setText(Qi::ui.text.etChange);
				ui.bnRememberPosAdd->setText(Qi::ui.text.etChange);
				ui.bnJumpAdd->setText(Qi::ui.text.etChange);
				ui.bnJumpPointAdd->setText(Qi::ui.text.etChange);
			}
		}
		else
		{
			changing = state;
			{
				ui.bnKeyAdd->setText(Qi::ui.text.etAdd);
				ui.bnStateAdd->setText(Qi::ui.text.etAdd);
				ui.bnMoveAdd->setText(Qi::ui.text.etAdd);
				ui.bnDelayAdd->setText(Qi::ui.text.etAdd);
				ui.bnLoopAdd->setText(Qi::ui.text.etAdd);
				ui.bnTimerAdd->setText(Qi::ui.text.etAdd);
				ui.bnTextAdd->setText(Qi::ui.text.etAdd);
				ui.bnColorAdd->setText(Qi::ui.text.etAdd);
				ui.bnEndAdd->setText(Qi::ui.text.etAdd);
				ui.bnEndLoopAdd->setText(Qi::ui.text.etAdd);
				ui.bnRecoverPosAdd->setText(Qi::ui.text.etAdd);
				ui.bnImageAdd->setText(Qi::ui.text.etAdd);
				ui.bnPopTextAdd->setText(Qi::ui.text.etAdd);
				ui.bnRememberPosAdd->setText(Qi::ui.text.etAdd);
				ui.bnJumpAdd->setText(Qi::ui.text.etAdd);
				ui.bnJumpPointAdd->setText(Qi::ui.text.etAdd);
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
	void ItemUse(int32 p)
	{
		std::visit([this](auto&& var)
			{
				using T = std::decay_t<decltype(var)>;
				if constexpr (std::is_same_v<T, QiDelay>)
				{
					const QiDelay& delay = var;
					WidgetSetDelay(delay);
				}
				else if constexpr (std::is_same_v<T, QiKey>)
				{
					const QiKey& key = var;
					WidgetSetKey(key);
				}
				else if constexpr (std::is_same_v<T, QiMouse>)
				{
					const QiMouse& mouse = var;
					WidgetSetMouse(mouse);
				}
				else if constexpr (std::is_same_v<T, QiText>)
				{
					const QiText& text = var;
					WidgetSetText(text);
				}
				else if constexpr (std::is_same_v<T, QiColor>)
				{
					const QiColor& color = var;
					WidgetSetColor(color);
				}
				else if constexpr (std::is_same_v<T, QiLoop>)
				{
					const QiLoop& loop = var;
					WidgetSetLoop(loop);
				}
				else if constexpr (std::is_same_v<T, QiKeyState>)
				{
					const QiKeyState& keyState = var;
					WidgetSetKeyState(keyState);
				}
				else if constexpr (std::is_same_v<T, QiImage>)
				{
					const QiImage& image = var;
					WidgetSetImage(image);
				}
				else if constexpr (std::is_same_v<T, QiPopText>)
				{
					const QiPopText& popText = var;
					WidgetSetPopText(popText);
				}
				else if constexpr (std::is_same_v<T, QiTimer>)
				{
					const QiTimer& timer = var;
					WidgetSetTimer(timer);
				}
			}, actions->at(p)
		);
	}
	Action ItemGet(uint32 type)
	{
		Action action;
		switch (type)
		{
		case QiType::end: action = QiEnd(); break;
		case QiType::delay: action = WidgetGetDelay(); break;
		case QiType::key: action = WidgetGetKey(); break;
		case QiType::mouse: action = WidgetGetMouse(); break;
		case QiType::text: action = WidgetGetText(); break;
		case QiType::color: action = WidgetGetColor(); break;
		case QiType::loop: action = WidgetGetLoop(); break;
		case QiType::loopEnd: action = QiLoopEnd(); break;
		case QiType::keyState: action = WidgetGetKeyState(); break;
		case QiType::recoverPos: action = QiRecoverPos(); break;
		case QiType::image: action = WidgetGetImage(); break;
		case QiType::popText: action = WidgetGetPopText(); break;
		case QiType::rememberPos: action = QiRememberPos(); break;
		case QiType::timer: action = WidgetGetTimer(); break;
		case QiType::jump: action = WidgetGetJump(); break;
		case QiType::jumpPoint: action = WidgetGetJumpPoint(); break;
		}
		return std::move(action);
	}
	void ItemAdd(uint32 type)
	{
		int p = ui.tbActions->currentRow();
		if (p < 0) p = actions->size();
		else p++;
		actions->Ins(ItemGet(type), p);
		TableUpdate();
		ui.tbActions->setCurrentItem(ui.tbActions->item(p, 0));
	}
	void ItemChange(uint32 type)
	{
		int p = ui.tbActions->currentRow(); if (p < 0) return;

		Action& current = actions->at(p);
		QiBase base_old = current.base();

		current = std::move(ItemGet(type));
		QiBase& base_new = current.base();

		base_new.mark = std::move(base_old.mark);
		base_new.next = std::move(base_old.next);

		TableUpdate();
		ui.tbActions->setCurrentItem(0);
		SetChange(false);
	}
	void ItemDel()
	{
		QList<QTableWidgetItem*> items = ui.tbActions->selectedItems();
		if (!items.size()) return;

		List<size_t> itemPos;
		for (size_t i = 0; i < items.size(); i++) if (items[i]->column() == 0) itemPos.Add(items[i]->row());
		actions->Del(itemPos);

		TableUpdate();
		ui.tbActions->setCurrentItem(0);

		SetChange(false);
	}
	void ItemCut()
	{
		QList<QTableWidgetItem*> items = ui.tbActions->selectedItems();
		if (!items.size()) return;

		Qi::clipboard.resize(0);
		for (size_t i = 0; i < items.size(); i++) if (items[i]->column() == 0) Qi::clipboard.Add(actions->at(items[i]->row()));

		ItemDel();
	}
	void ItemCopy()
	{
		QList<QTableWidgetItem*> items = ui.tbActions->selectedItems();
		if (!items.size()) return;

		Qi::clipboard.clear();
		for (size_t i = 0; i < items.size(); i++) if (items[i]->column() == 0) Qi::clipboard.Add(actions->at(items[i]->row()));
	}
	void ItemPaste()
	{
		int p = ui.tbActions->currentRow();
		if (p < 0) p = actions->size();
		else p++;
		for (size_t i = Qi::clipboard.size(); i > 0; i--)
		{
			actions->Ins(Qi::clipboard[i - 1], p);
			if (i < 1) break;
		}
		TableUpdate();
	}

	// Get widget data
	QiKey WidgetGetKey() {
		QiKey key;
		if (ui.rbDown->isChecked()) key.state = QiKey::down;
		else if (ui.rbUp->isChecked()) key.state = QiKey::up;
		else if (ui.rbClick->isChecked()) key.state = QiKey::click;
		key.vk = ui.hkKey->key().keyCode;
		return std::move(key);
	}
	QiKeyState WidgetGetKeyState() {
		QiKeyState keyState;
		keyState.state = ui.rbStateDown->isChecked();
		keyState.vk = ui.hkState->key().keyCode;
		return std::move(keyState);
	}
	QiMouse WidgetGetMouse() {
		QiMouse mouse;
		mouse.move = ui.rbMove->isChecked();
		mouse.track = ui.chbMoveTrack->isChecked();
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
		mouse.x = x;
		mouse.y = y;
		mouse.ex = r;
		mouse.speed = s;
		return std::move(mouse);
	}
	QiDelay WidgetGetDelay() {
		QiDelay delay;
		int mn = 10;
		if (ui.etDelayMin->text() != "") mn = ui.etDelayMin->text().toInt();
		int mx = ui.etDelayMax->text().toInt();
		if (mn > delayMax) mn = delayMax;
		if (mx > delayMax) mx = delayMax;
		if (mx < mn) mx = mn;
		delay.min = mn;
		delay.max = mx;
		return std::move(delay);
	}
	QiText WidgetGetText() {
		QiText text;
		text.str = (const wchar_t*)(ui.etText->toPlainText().utf16());
		return std::move(text);
	}
	QiColor WidgetGetColor() {
		QiColor color;
		color.unfind = ui.rbColorNFind->isChecked();
		color.move = ui.chbColorMove->isChecked();
		{
			int l = ui.etColorL->text().toInt();
			int t = ui.etColorT->text().toInt();
			int r = ui.etColorR->text().toInt();
			int b = ui.etColorB->text().toInt();
			if (l > posMax) l = posMax;
			if (t > posMax) t = posMax;
			if (r > posMax) r = posMax;
			if (b > posMax) b = posMax;
			color.rect = { l, t, r, b };
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
			color.rgbe.set(r, g, b, e);
		}
		return std::move(color);
	}
	QiLoop WidgetGetLoop()
	{
		QiLoop loop;
		int mn = 1;
		if (ui.etCountMin->text() != "") mn = ui.etCountMin->text().toInt();
		int mx = ui.etCountMax->text().toInt();
		if (mn > loopCountMax) mn = loopCountMax;
		if (mx > loopCountMax) mx = loopCountMax;
		if (mx < mn) mx = mn;
		loop.min = mn;
		loop.max = mx;
		return std::move(loop);
	}
	QiImage WidgetGetImage()
	{
		QiImage image;
		image.unfind = ui.rbImageNFind->isChecked();
		image.move = ui.chbImageMove->isChecked();
		{
			int l = ui.etImageL->text().toInt();
			int t = ui.etImageT->text().toInt();
			int r = ui.etImageR->text().toInt();
			int b = ui.etImageB->text().toInt();
			if (l > posMax) l = posMax;
			if (t > posMax) t = posMax;
			if (r > posMax) r = posMax;
			if (b > posMax) b = posMax;
			image.rect = { l, t, r, b };
		}
		{
			int s = 80;
			if (ui.etImageSim->text() != "") s = ui.etImageSim->text().toInt();
			if (s > imageSimMax) s = imageSimMax;
			image.sim = s;
		}
		image.map = imageMap;
		return std::move(image);
	}
	QiPopText WidgetGetPopText()
	{
		QiPopText popText;
		popText.str = (const wchar_t*)(ui.etPopText->text().utf16());
		int time = 1000;
		if (ui.etPopTextTime->text() != "") time = ui.etPopTextTime->text().toInt();
		if (time > popTextTimeMax) time = popTextTimeMax;
		popText.time = time;
		popText.sync = ui.chbPopTextWait->isChecked();
		return std::move(popText);
	}
	QiTimer WidgetGetTimer()
	{
		QiTimer timer;
		int mn = 1;
		if (ui.etTimerMin->text() != "") mn = ui.etTimerMin->text().toInt();
		int mx = ui.etTimerMax->text().toInt();
		if (mn > timerMax) mn = timerMax;
		if (mx > timerMax) mx = timerMax;
		if (mx < mn) mx = mn;
		timer.min = mn;
		timer.max = mx;
		return std::move(timer);
	}
	QiJump WidgetGetJump()
	{
		QiJump jump;
		int row = ui.lsJumpPoint->currentRow();
		if (row > -1) jump.id = ui.lsJumpPoint->item(row)->data(DataRole::id).toUInt();
		return std::move(jump);
	}
	QiJumpPoint WidgetGetJumpPoint()
	{
		QiJumpPoint jumpPoint;
		if (ui.rbEnding->isChecked()) jumpPoint.id = QiFn::UniqueJumpPoint(macro->acEnd);
		else jumpPoint.id = QiFn::UniqueJumpPoint(macro->acRun);
		return std::move(jumpPoint);
	}

	// Load widget data
	void WidgetSetKey(const QiKey& key) {
		if (key.state == QiKey::down) ui.rbDown->setChecked(true);
		else if (key.state == QiKey::up) ui.rbUp->setChecked(true);
		else if (key.state == QiKey::click) ui.rbClick->setChecked(true);
		ui.hkKey->setKey(QKeyEdit::Key(key.vk));
	}
	void WidgetSetKeyState(const QiKeyState& keyState)
	{
		if (keyState.state) ui.rbStateDown->setChecked(true);
		if (!keyState.state) ui.rbStateUp->setChecked(true);
		ui.hkState->setKey(QKeyEdit::Key(keyState.vk));
	}
	void WidgetSetMouse(const QiMouse& mouse) {
		if (mouse.move) ui.rbMove->setChecked(true), OnRbMouseMove(true);
		else ui.rbPos->setChecked(true), OnRbMousePos(true);
		ui.chbMoveTrack->setChecked(mouse.track);
		ui.etMoveSpeed->setText(QString::number(mouse.speed));
		ui.etX->setText(QString::number(mouse.x)); ui.etY->setText(QString::number(mouse.y)); if (mouse.ex > -1) ui.etMoveRand->setText(QString::number(mouse.ex));
	}
	void WidgetSetDelay(const QiDelay& delay) {
		ui.etDelayMin->setText(QString::number(delay.min));
		ui.etDelayMax->setText(QString::number(delay.max));
	}
	void WidgetSetText(const QiText& text) {
		ui.etText->setText(WToQString(text.str));
	}
	void WidgetSetColor(const QiColor& color) {
		if (color.unfind) ui.rbColorNFind->setChecked(true);
		else ui.rbColorFind->setChecked(true);
		ui.chbColorMove->setChecked(color.move);
		// Rect
		{
			ui.etColorL->setText(QString::number(color.rect.left));
			ui.etColorT->setText(QString::number(color.rect.top));
			ui.etColorR->setText(QString::number(color.rect.right));
			ui.etColorB->setText(QString::number(color.rect.bottom));
		}
		// Color
		{
			ui.etColorSim->setText(QString::number(color.rgbe.a));
			ui.etColorRed->setText(QString::number(color.rgbe.r));
			ui.etColorGreen->setText(QString::number(color.rgbe.g));
			ui.etColorBlue->setText(QString::number(color.rgbe.b));
			QString style = "background-color:rgb(";
			style += QString::number(color.rgbe.r);
			style += ",";
			style += QString::number(color.rgbe.g);
			style += ",";
			style += QString::number(color.rgbe.b);
			style += ");";
			ui.bnColorValue->setStyleSheet(style);
		}
	}
	void WidgetSetLoop(const QiLoop& loop) {
		ui.etCountMin->setText(QString::number(loop.min));
		ui.etCountMax->setText(QString::number(loop.max));
	}
	void WidgetSetImage(const QiImage& image) {
		if (image.unfind) ui.rbImageNFind->setChecked(true);
		else ui.rbImageFind->setChecked(true);
		ui.chbImageMove->setChecked(image.move);
		// Rect
		{
			ui.etImageL->setText(QString::number(image.rect.left));
			ui.etImageT->setText(QString::number(image.rect.top));
			ui.etImageR->setText(QString::number(image.rect.right));
			ui.etImageB->setText(QString::number(image.rect.bottom));
		}
		// Image
		{
			ui.etImageSim->setText(QString::number(image.sim));
			imageMap = image.map;
			HBITMAP hbmp = Image::toBmp32(imageMap);
			if (hbmp) ui.lbImageView->setPixmap(QtWin::fromHBITMAP(hbmp));
		}
	}
	void WidgetSetPopText(const QiPopText& popText) {
		ui.etPopText->setText(WToQString(popText.str));
		ui.etPopTextTime->setText(QString::number(popText.time));
		ui.chbPopTextWait->setChecked(popText.sync);
	}
	void WidgetSetTimer(const QiTimer& timer)
	{
		ui.etTimerMin->setText(QString::number(timer.min));
		ui.etTimerMax->setText(QString::number(timer.max));
	}
};