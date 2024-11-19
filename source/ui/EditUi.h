﻿#pragma execution_character_set("utf-8")
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

class EditUi : public QDialog
{
	Q_OBJECT;
	using This = EditUi;

	using JumpPoints = List<QiJumpPoint>;
	using Blocks = List<QiBlock>;

	struct Layer
	{
		Actions* actions;
		QString title;
	};

	enum TabIndex
	{
		tab_base,
		tab_loop,
		tab_state,
		tab_jump,
		tab_block,
		tab_color,
		tab_image,
		tab_text,
		tab_dialog,
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
	QAction* muEdit2;

	QTimer* testTimer;
	uint32 testCount = 0;

	// widget
	QPointView pv;
	QRectView rv;

	// image viewer
	RgbMap imageMap;

	Macro* macro = nullptr;
	Actions* actions = nullptr;
	Actions* actionsRoot = nullptr;

	List<Layer> layers;

	bool changing = false;
	bool updating = false;

public:
	EditUi(Macro* macro, Actions* actions)
	{
		Layer layer;
		this->macro = macro;
		layer.actions = this->actionsRoot = this->actions = actions;
		layer.title = QString::fromUtf8("编辑 - ") + macro->name;
		layers.Add(layer);

		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		Init();
		Event();
		StyleGroup();
		WidgetUpdate();
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
		ui.dialog_add_button->setProperty("group", "edit-add_button");
		ui.block_add_button->setProperty("group", "edit-add_button");
		ui.blockExec_add_button->setProperty("group", "edit-add_button");

		ui.bnLoopEdit->setProperty("group", "edit-edit_button");
		ui.bnTimerEdit->setProperty("group", "edit-edit_button");
		ui.bnStateEdit->setProperty("group", "edit-edit_button");
		ui.bnStateEdit2->setProperty("group", "edit-edit_button");
		ui.bnColorEdit->setProperty("group", "edit-edit_button");
		ui.bnColorEdit2->setProperty("group", "edit-edit_button");
		ui.bnImageEdit->setProperty("group", "edit-edit_button");
		ui.bnImageEdit2->setProperty("group", "edit-edit_button");
		ui.dialog_edit_button->setProperty("group", "edit-edit_button");
		ui.dialog_edit2_button->setProperty("group", "edit-edit_button");
		ui.block_edit_button->setProperty("group", "edit-edit_button");

		ui.chbMoveTrack->setProperty("group", "check");
		ui.chbColorMove->setProperty("group", "check");
		ui.chbImageMove->setProperty("group", "check");
		ui.chbPopTextWait->setProperty("group", "check");
		ui.chbWnd->setProperty("group", "check");
		ui.chbChildWnd->setProperty("group", "check");

		ui.rbRunning->setProperty("group", "radio");
		ui.rbEnding->setProperty("group", "radio");
		ui.rbClick->setProperty("group", "radio");
		ui.rbDown->setProperty("group", "radio");
		ui.rbUp->setProperty("group", "radio");
		ui.rbPos->setProperty("group", "radio");
		ui.rbMove->setProperty("group", "radio");
		ui.dialog_style_none_radio->setProperty("group", "radio");
		ui.dialog_style_warning_radio->setProperty("group", "radio");
		ui.dialog_style_error_radio->setProperty("group", "radio");

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
		ui.dialog_title_edit->setProperty("group", "line_edit");
		ui.dialog_text_edit->setProperty("group", "line_edit");
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
		ui.block_list->setProperty("group", "table");

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
			muEdit2 = new QAction("编辑2", this);

			menu->addAction(muDel);
			menu->addAction(muChange);
			menu->addAction(muCut);
			menu->addAction(muCopy);
			menu->addAction(muPaste);
			menu->addAction(muEdit);
			menu->addAction(muEdit2);

			connect(muDel, &QAction::triggered, this, &This::OnMenuDel);
			connect(muChange, &QAction::triggered, this, &This::OnMenuChange);
			connect(muCut, &QAction::triggered, this, &This::OnMenuCut);
			connect(muCopy, &QAction::triggered, this, &This::OnMenuCopy);
			connect(muPaste, &QAction::triggered, this, &This::OnMenuPaste);
			connect(muEdit, &QAction::triggered, this, &This::OnMenuEdit);
			connect(muEdit2, &QAction::triggered, this, &This::OnMenuEdit2);
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
				ui.bnLoopEdit->setText(Qi::ui.text.etEdit);
				ui.bnTimerEdit->setText(Qi::ui.text.etEdit);
				ui.block_add_button->setText(Qi::ui.text.etAdd);
				ui.blockExec_add_button->setText(Qi::ui.text.etAdd);
			}
			if ("clear shortcut")
			{
				ui.bnClose->installEventFilter(this);
				ui.bnBack->installEventFilter(this);
				ui.bnRun->installEventFilter(this);
				ui.bnPos->installEventFilter(this);
				ui.bnColorRect->installEventFilter(this);
				ui.bnColorValue->installEventFilter(this);
				ui.bnImageRect->installEventFilter(this);
				ui.bnImageShot->installEventFilter(this);

				ui.bnKeyAdd->installEventFilter(this);
				ui.bnStateAdd->installEventFilter(this);
				ui.bnMoveAdd->installEventFilter(this);
				ui.bnDelayAdd->installEventFilter(this);
				ui.bnLoopAdd->installEventFilter(this);
				ui.bnTextAdd->installEventFilter(this);
				ui.bnColorAdd->installEventFilter(this);
				ui.bnImageAdd->installEventFilter(this);
				ui.bnEndAdd->installEventFilter(this);
				ui.bnEndLoopAdd->installEventFilter(this);
				ui.bnJumpAdd->installEventFilter(this);
				ui.bnJumpPointAdd->installEventFilter(this);
				ui.dialog_add_button->installEventFilter(this);
				ui.block_add_button->installEventFilter(this);
				ui.blockExec_add_button->installEventFilter(this);

				ui.bnStateEdit->installEventFilter(this);
				ui.bnLoopEdit->installEventFilter(this);
				ui.bnTimerEdit->installEventFilter(this);
				ui.bnColorEdit->installEventFilter(this);
				ui.bnImageEdit->installEventFilter(this);
				ui.dialog_edit_button->installEventFilter(this);
				ui.dialog_edit2_button->installEventFilter(this);
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

			QButtonGroup* moveRbs = new QButtonGroup(this);
			moveRbs->addButton(ui.rbPos);
			moveRbs->addButton(ui.rbMove);
			ui.rbPos->setChecked(true);

			QButtonGroup* dialogRbs = new QButtonGroup(this);
			dialogRbs->addButton(ui.dialog_style_none_radio);
			dialogRbs->addButton(ui.dialog_style_warning_radio);
			dialogRbs->addButton(ui.dialog_style_error_radio);
			ui.dialog_style_none_radio->setChecked(true);
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
		if (macro->wndState) macro->wi.update(), SetWindowMode();
	}
	void Event()
	{
		if ("title")
		{
			connect(ui.bnClose, &QPushButton::clicked, this, &This::OnBnClose);
			connect(ui.bnBack, &QPushButton::clicked, this, &This::OnBnBack);
			connect(ui.bnRun, &QPushButton::clicked, this, &This::OnBnRun);
			connect(ui.bnWndSelect, &QPushButton::clicked, this, &This::OnBnWndSelect);
			connect(ui.chbWnd, &QPushButton::clicked, this, &This::OnChbWnd);
			connect(ui.chbChildWnd, &QPushButton::clicked, this, &This::OnChbChildWnd);
		}
		if ("table")
		{
			ui.tbActions->installEventFilter(this);
			ui.tbActions->viewport()->installEventFilter(this);
			connect(ui.rbRunning, &QRadioButton::toggled, this, &This::OnRbRunning);
			connect(ui.rbEnding, &QRadioButton::toggled, this, &This::OnRbEnding);
			connect(ui.tbActions, &QTableWidget::cellChanged, this, &This::OnTbChanged);
			connect(ui.tbActions, &QTableWidget::cellClicked, this, &This::OnTbClicked);
			connect(ui.tbActions, &QTableWidget::currentItemChanged, this, &This::OnTbSelect);
			connect(ui.tbActions, &QTableWidget::cellDoubleClicked, this, &This::OnTbDoubleClick);
			connect(ui.tbActions, &QTableWidget::customContextMenuRequested, this, &This::OnTableMenu);
		}
		if ("list")
		{
			connect(ui.lsJumpPoint, &QListWidget::itemClicked, this, &This::OnLsJumpClick);
			connect(ui.block_list, &QListWidget::itemClicked, this, &This::OnLsBlockExecClick);
		}
		if ("action widget")
		{
			connect(ui.bnKeyAdd, &QPushButton::clicked, this, [this](bool){ if (changing) ItemChange(QiType::key); else ItemAdd(QiType::key); });
			connect(ui.bnMoveAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::mouse); else ItemAdd(QiType::mouse); });
			connect(ui.bnDelayAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::delay); else ItemAdd(QiType::delay); });

			connect(ui.bnStateAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::keyState); else ItemAdd(QiType::keyState); });
			connect(ui.bnRememberPosAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::rememberPos); else ItemAdd(QiType::rememberPos); });
			connect(ui.bnRecoverPosAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::recoverPos); else ItemAdd(QiType::recoverPos); });
			connect(ui.bnEndAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::end); else ItemAdd(QiType::end); });
			connect(ui.bnEndLoopAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::loopEnd); else ItemAdd(QiType::loopEnd); });

			connect(ui.bnLoopAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::loop); else ItemAdd(QiType::loop); });
			connect(ui.bnTimerAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::timer); else ItemAdd(QiType::timer); });

			connect(ui.bnTextAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::text); else ItemAdd(QiType::text); });
			connect(ui.bnPopTextAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::popText); else ItemAdd(QiType::popText); });

			connect(ui.bnColorAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::color); else ItemAdd(QiType::color); });

			connect(ui.bnImageAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::image); else ItemAdd(QiType::image); });

			connect(ui.bnJumpAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::jump); else ItemAdd(QiType::jump); });
			connect(ui.bnJumpPointAdd, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::jumpPoint); else ItemAdd(QiType::jumpPoint); });

			connect(ui.block_add_button, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::block); else ItemAdd(QiType::block); });
			connect(ui.blockExec_add_button, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::blockExec); else ItemAdd(QiType::blockExec); });

			connect(ui.dialog_add_button, &QPushButton::clicked, this, [this](bool) { if (changing) ItemChange(QiType::dialog); else ItemAdd(QiType::dialog); });


			connect(ui.bnStateEdit, &QPushButton::clicked, this, [this](bool) { NextEdit(false); });
			connect(ui.bnStateEdit2, &QPushButton::clicked, this, [this](bool) { NextEdit(true); });

			connect(ui.bnLoopEdit, &QPushButton::clicked, this, [this](bool) { NextEdit(false); });
			connect(ui.bnTimerEdit, &QPushButton::clicked, this, [this](bool) { NextEdit(false); });

			connect(ui.bnColorEdit, &QPushButton::clicked, this, [this](bool) { NextEdit(false); });
			connect(ui.bnColorEdit2, &QPushButton::clicked, this, [this](bool) { NextEdit(true); });

			connect(ui.bnImageEdit, &QPushButton::clicked, this, [this](bool) { NextEdit(false); });
			connect(ui.bnImageEdit2, &QPushButton::clicked, this, [this](bool) { NextEdit(true); });

			connect(ui.dialog_edit_button, &QPushButton::clicked, this, [this](bool) { NextEdit(false); });
			connect(ui.dialog_edit2_button, &QPushButton::clicked, this, [this](bool) { NextEdit(true); });

			connect(ui.block_edit_button, &QPushButton::clicked, this, [this](bool) { NextEdit(false); });

			connect(ui.bnPos, &QPushButton::clicked, this, &This::OnBnMousePos);
			connect(ui.rbPos, &QRadioButton::toggled, this, &This::OnRbMousePos);
			connect(ui.rbMove, &QRadioButton::toggled, this, &This::OnRbMouseMove);
			connect(ui.etDelayMin, &QLineEdit::textChanged, this, &This::OnEtDelayMin);
			connect(ui.etCountMin, &QLineEdit::textChanged, this, &This::OnEtLoopMin);
			connect(ui.etTimerMin, &QLineEdit::textChanged, this, &This::OnEtTimerMin);
			connect(ui.bnColorRect, &QPushButton::clicked, this, &This::OnBnColorRect);
			connect(ui.bnColorValue, &QPushButton::clicked, this, &This::OnBnColorValue);
			connect(ui.bnImageRect, &QPushButton::clicked, this, &This::OnBnImageRect);
			connect(ui.bnImageShot, &QPushButton::clicked, this, &This::OnBnImageShot);
		}
		if ("timer")
		{
			connect(testTimer, &QTimer::timeout, this, &This::OnTestTimer);
		}
	}
	void DisableButtons()
	{
		ui.bnStateEdit->setDisabled(true);
		ui.bnStateEdit2->setDisabled(true);
		ui.bnLoopEdit->setDisabled(true);
		ui.bnTimerEdit->setDisabled(true);
		ui.bnColorEdit->setDisabled(true);
		ui.bnColorEdit2->setDisabled(true);
		ui.bnImageEdit->setDisabled(true);
		ui.bnImageEdit2->setDisabled(true);
		ui.bnJumpAdd->setDisabled(true);
		ui.dialog_edit_button->setDisabled(true);
		ui.dialog_edit2_button->setDisabled(true);
		ui.block_edit_button->setDisabled(true);
		ui.blockExec_add_button->setDisabled(true);
	}
	void WidgetUpdate()
	{
		setWindowTitle(layers.End()->title);

		if (layers.size() > 1)
		{
			ui.rbRunning->setVisible(false);
			ui.rbEnding->setVisible(false);
			ui.block_add_button->setDisabled(true);
			ui.block_edit_button->setDisabled(true);
		}
		else
		{
			ui.rbRunning->setVisible(true), ui.rbEnding->setVisible(true);
			ui.block_add_button->setEnabled(true);
			ui.block_edit_button->setEnabled(true);
		}

		TableUpdate();
	}
	void NextEdit(bool edit2)
	{
		SetChange(false);

		int p = ui.tbActions->currentRow(); if (p < 0) return;

		QString title;
		Actions* next = nullptr;
		bool failed = false;
		std::visit([&title, &failed, &next, edit2](auto&& var)
			{
				using T = std::decay_t<decltype(var)>;
				if constexpr (std::is_same_v<T, QiColor>)
				{
					QiColor& color = var;
					if (edit2)
					{
						next = &color.next2;
						title = "编辑 - 查找颜色（未找到）";
					}
					else
					{
						next = &color.next;
						title = "编辑 - 查找颜色（找到）";
					}
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
					if (edit2)
					{
						next = &keyState.next2;
						title = "编辑 - 按键状态（未按下）";
					}
					else
					{
						next = &keyState.next;
						title = "编辑 - 按键状态（按下）";
					}

				}
				else if constexpr (std::is_same_v<T, QiImage>)
				{
					QiImage& image = var;
					if (edit2)
					{
						next = &image.next2;
						title = "编辑 - 查找图片（未找到）";
					}
					else
					{
						next = &image.next;
						title = "编辑 - 查找图片（找到）";
					}
				}
				else if constexpr (std::is_same_v<T, QiTimer>)
				{
					QiTimer& timer = var;
					next = &timer.next;
					title = "编辑 - 定时";
				}
				else if constexpr (std::is_same_v<T, QiDialog>)
				{
					QiDialog& dialog = var;
					if (edit2)
					{
						next = &dialog.next2;
						title = "编辑 - 对话框（取消）";
					}
					else
					{
						next = &dialog.next;
						title = "编辑 - 对话框（确认）";
					}
				}
				else if constexpr (std::is_same_v<T, QiBlock>)
				{
					QiBlock& block = var;
					next = &block.next;
					title = "编辑 - 块";
				}
				else
				{
					failed = true;
				}
			}, actions->at(p));

		if (!failed)
		{
			Layer layer;
			layer.actions = next;
			layer.title = title;
			layers.Add(layer);

			actions = next;
			setWindowTitle(title);
			WidgetUpdate();
		}
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
		ui.etWndName->setText(macro->wi.wndName);
	}

	// Jump
	void LoadJumpPoints(JumpPoints& jumpPoints, const Actions& actions)
	{
		for (size_t i = 0; i < actions.size(); i++)
		{
			const Action& var = actions[i];
			if (var.index() == QiType::jumpPoint)
			{
				const QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(var);
				jumpPoints.Add(jumpPoint);
			}
			else
			{
				const QiBase& base = var.base();
				if (!base.next.empty()) LoadJumpPoints(jumpPoints, base.next);
				if (!base.next2.empty()) LoadJumpPoints(jumpPoints, base.next2);
			}
		}
	}
	bool FindJumpPoint(const JumpPoints& jumpPoints, int32 id)
	{
		for (size_t i = 0; i < jumpPoints.size(); i++)
		{
			if (jumpPoints[i].id == id) return true;
		}
		return false;
	}
	void CheckJumpInvalid(const JumpPoints& jumpPoints, Actions& actions)
	{
		for (size_t i = 0; i < actions.size(); i++)
		{
			Action& var = actions[i];
			if (var.index() == QiType::jump)
			{
				QiJump& jump= std::get<QiJump>(var);
				if (!FindJumpPoint(jumpPoints, jump.id)) jump.id = -1;
			}
			else
			{
				QiBase& base = var.base();
				if (!base.next.empty()) CheckJumpInvalid(jumpPoints, base.next);
				if (!base.next2.empty()) CheckJumpInvalid(jumpPoints, base.next2);
			}
		}
	}
	int32 JumpPointUnique(const JumpPoints& jumpPoints)
	{
		int32 id = 1;
		for (size_t i = 0; i < jumpPoints.size(); i++)
		{
			if (jumpPoints[i].id >= id) id = jumpPoints[i].id + 1;
		}
		return id;
	}
	void JumpPointUniqueActions(const JumpPoints& jumpPoints, Actions& actions)
	{
		for (size_t i = 0; i < actions.size(); i++)
		{
			Action& var = actions[i];
			if (var.index() == QiType::jumpPoint)
			{
				QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(var);
				if (FindJumpPoint(jumpPoints, jumpPoint.id))
				{
					jumpPoint.id = JumpPointUnique(jumpPoints);
				}
				else
				{
					QiBase& base = var.base();
					if (!base.next.empty()) JumpPointUniqueActions(jumpPoints, base.next);
					if (!base.next2.empty()) JumpPointUniqueActions(jumpPoints, base.next2);
				}
			}
		}
	}
	// Jump;

	// Block
	void LoadBlocks(Blocks& blocks, const Actions& actions)
	{
		for (size_t i = 0; i < actions.size(); i++)
		{
			const Action& var = actions[i];
			if (var.index() == QiType::block)
			{
				const QiBlock& block = std::get<QiBlock>(var);
				blocks.Add(block);
			}
			else
			{
				const QiBase& base = var.base();
				if (!base.next.empty()) LoadBlocks(blocks, base.next);
				if (!base.next2.empty()) LoadBlocks(blocks, base.next2);
			}
		}
	}
	bool FindBlock(const Blocks& blocks, int32 id)
	{
		for (size_t i = 0; i < blocks.size(); i++)
		{
			if (blocks[i].id == id) return true;
		}
		return false;
	}
	void CheckBlockInvalid(const Blocks& blocks, Actions& actions)
	{
		for (size_t i = 0; i < actions.size(); i++)
		{
			Action& var = actions[i];
			if (var.index() == QiType::blockExec)
			{
				QiBlockExec& blockExec = std::get<QiBlockExec>(var);
				if (!FindBlock(blocks, blockExec.id)) blockExec.id = -1;
			}
			else
			{
				QiBase& base = var.base();
				if (!base.next.empty()) CheckBlockInvalid(blocks, base.next);
				if (!base.next2.empty()) CheckBlockInvalid(blocks, base.next2);
			}
		}
	}
	int32 BlockUnique(const Blocks& blocks)
	{
		int32 id = 1;
		for (size_t i = 0; i < blocks.size(); i++)
		{
			if (blocks[i].id >= id) id = blocks[i].id + 1;
		}
		return id;
	}
	void BlockUniqueActions(const Blocks& blocks, Actions& actions)
	{
		for (size_t i = 0; i < actions.size(); i++)
		{
			Action& var = actions[i];
			if (var.index() == QiType::block)
			{
				QiBlock& block = std::get<QiBlock>(var);
				if (FindBlock(blocks, block.id))
				{
					block.id = BlockUnique(blocks);
				}
				else
				{
					QiBase& base = var.base();
					if (!base.next.empty()) BlockUniqueActions(blocks, base.next);
					if (!base.next2.empty()) BlockUniqueActions(blocks, base.next2);
				}
			}
		}
	}
	// Block;

	void ListJumpPointUpdate()
	{
		JumpPoints jumpPoints;
		LoadJumpPoints(jumpPoints, *actionsRoot);

		CheckJumpInvalid(jumpPoints, *actionsRoot);

		ui.lsJumpPoint->clear();

		for (size_t i = 0; i < jumpPoints.size(); i++)
		{
			const QiJumpPoint& jumpPoint = jumpPoints[i];
			QListWidgetItem* item = new QListWidgetItem(QString::number(jumpPoint.id) + QString::fromUtf8("　　　") + jumpPoint.mark);
			item->setData(DataRole::id, jumpPoint.id);
			ui.lsJumpPoint->addItem(item);
		}
	}
	void ListBlockUpdate()
	{
		Blocks blocks;
		LoadBlocks(blocks, *actionsRoot);

		CheckBlockInvalid(blocks, *actionsRoot);

		ui.block_list->clear();

		for (size_t i = 0; i < blocks.size(); i++)
		{
			const QiBlock& block = blocks[i];
			QListWidgetItem* item = new QListWidgetItem(QString::number(block.id) + QString::fromUtf8("　　　") + block.mark);
			item->setData(DataRole::id, block.id);
			ui.block_list->addItem(item);
		}
	}
	void TableUpdate()
	{
		DisableButtons();
		ListJumpPointUpdate();
		ListBlockUpdate();

		updating = true;
		ui.tbActions->clearMask();
		ui.tbActions->setRowCount(actions->size());
		ui.tbActions->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.tbActions->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tbActions->verticalHeader()->setDefaultSectionSize(0);

		for (size_t i = 0; i < actions->size(); i++)
		{
			const Action& var = actions->at(i);

			QString type;
			QString param;
			QString mark = var.base().mark;

			switch (var.index())
			{
			case QiType::end:
			{
				type = Qi::ui.text.acEnd;
			} break;
			case QiType::delay:
			{
				const QiDelay& delay = std::get<QiDelay>(var);
				type = Qi::ui.text.acWait;

				if (delay.min != delay.max)
				{
					param = QString::number(delay.min);
					param += " ~ ";
					param += QString::number(delay.max);
				}
				else param = QString::number(delay.min);
			} break;
			case QiType::key:
			{
				const QiKey& key = std::get<QiKey>(var);
				if (key.state == QiKey::up) type = Qi::ui.text.acUp;
				else if (key.state == QiKey::down) type = Qi::ui.text.acDown;
				else if (key.state == QiKey::click) type = Qi::ui.text.acClick;

				param = QKeyEdit::keyName(key.vk);
			} break;
			case QiType::mouse:
			{
				const QiMouse& mouse = std::get<QiMouse>(var);
				if (mouse.move) type = Qi::ui.text.acMove;
				else type = Qi::ui.text.acPos;

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
			} break;
			case QiType::text:
			{
				const QiText& text = std::get<QiText>(var);
				type = Qi::ui.text.acText;

				param = text.text.mid(0, 32);
				if (text.text.size() > 31) param += "...";
			} break;
			case QiType::color:
			{
				const QiColor& color = std::get<QiColor>(var);
				type = Qi::ui.text.acColor;

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
				param += ")";
				if (color.move) param += " 移动";
			} break;
			case QiType::loop:
			{
				const QiLoop& loop = std::get<QiLoop>(var);
				type = Qi::ui.text.acLoop;

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
			} break;
			case QiType::loopEnd:
			{
				type = Qi::ui.text.acEndLoop;
			} break;
			case QiType::keyState:
			{
				const QiKeyState& keyState = std::get<QiKeyState>(var);
				type = Qi::ui.text.acKeyState;

				param += QKeyEdit::keyName(keyState.vk);
			} break;
			case QiType::recoverPos:
			{
				type = Qi::ui.text.acRecoverPos;
			} break;
			case QiType::image:
			{
				const QiImage& image = std::get<QiImage>(var);
				type = Qi::ui.text.acImage;

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
				if (image.move) param += "移动";
			} break;
			case QiType::popText:
			{
				const QiPopText& popText = std::get<QiPopText>(var);
				type = Qi::ui.text.acPopText;

				std::wstring w;
				param = popText.text;
				param += "　时长：";
				param += QString::number(popText.time);
				if (popText.sync) param += "等待";
			} break;
			case QiType::rememberPos:
			{
				type = Qi::ui.text.acRememberPos;
			} break;
			case QiType::timer:
			{
				const QiTimer& timer = std::get<QiTimer>(var);
				type = Qi::ui.text.acTimer;

				if (timer.min == timer.max) param = QString::number(timer.min);
				else
				{
					param = QString::number(timer.min);
					param += " ~ ";
					param += QString::number(timer.max);
				}
			} break;
			case QiType::jump:
			{
				const QiJump& jump = std::get<QiJump>(var);
				type = Qi::ui.text.acJump;

				if (jump.id < 1)
				{
					param = "指定无效的锚点";
				}
				else
				{
					param = "id：";
					param += QString::number(jump.id);
				}
			} break;
			case QiType::jumpPoint:
			{
				const QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(var);
				type = Qi::ui.text.acJumpPoint;

				param = "id：";
				param += QString::number(jumpPoint.id);
			} break;
			case QiType::dialog:
			{
				const QiDialog& dialog = std::get<QiDialog>(var);
				type = Qi::ui.text.acDialog;

				param = dialog.title.mid(0, 8);
				if (dialog.title.size() > 7) param += "...";
				param += " | ";
				param += dialog.text.mid(0, 16);
				if (dialog.text.size() > 15) param += "...";
			} break;
			case QiType::block:
			{
				const QiBlock& block = std::get<QiBlock>(var);
				type = Qi::ui.text.acBlock;

				param = "id：";
				param += QString::number(block.id);
			} break;
			case QiType::blockExec:
			{
				const QiBlockExec& blockExec = std::get<QiBlockExec>(var);
				type = Qi::ui.text.acBlockExec;

				if (blockExec.id < 1)
				{
					param = "指定无效的块";
				}
				else
				{
					param = "id：";
					param += QString::number(blockExec.id);
				}
			} break;
			default: type = "加载失败"; break;
			}

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
		updating = false;
	}

private:
	void setWindowTitle(const QString& title)
	{
		QDialog::setWindowTitle(title);
		ui.lbTitle->setText(title);
	}

	bool event(QEvent* e)
	{
		if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
		{
			QKeyEvent* keyEvent = (QKeyEvent*)e;
			if ((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Space)) return true;
		}
		return QDialog::event(e);
	}
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
		else if ("buttons")
		{
			if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease)) return true;
		}
		return QDialog::eventFilter(obj, e);
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
	void OnBnBack()
	{
		layers.DelBack();
		if (layers.empty()) close();
		else
		{
			actions = layers.End()->actions;
			setWindowTitle(layers.End()->title);
			WidgetUpdate();
		}
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
		NextEdit(false);
	}
	void OnMenuEdit2()
	{
		NextEdit(true);
	}

	// List
	void OnLsJumpClick(QListWidgetItem*)
	{
		if (ui.lsJumpPoint->currentRow() < 0) ui.bnJumpAdd->setDisabled(true);
		else ui.bnJumpAdd->setEnabled(true);
	}
	void OnLsBlockExecClick(QListWidgetItem*)
	{
		if (ui.block_list->currentRow() < 0) ui.blockExec_add_button->setDisabled(true);
		else ui.blockExec_add_button->setEnabled(true);
	}

	// Table
	void OnRbRunning(bool state)
	{
		if (state && (layers.size() == 1))
		{
			layers.Begin()->actions = actionsRoot = actions = &macro->acRun;
			TableUpdate();
		}
	}
	void OnRbEnding(bool state)
	{
		if (state && (layers.size() == 1))
		{
			layers.Begin()->actions = actionsRoot = actions = &macro->acEnd;
			TableUpdate();
		}
	}
	void OnTbChanged(int row, int column)
	{
		if (updating) return;
		if (row < 0) return;
		if (column != 2) return;
		QiBase& base = actions->at(row).base();
		base.mark = ui.tbActions->item(row, 2)->text();
		if ((base.type == QiType::jumpPoint) || (base.type == QiType::block)) TableUpdate();
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
								if (!macro->wi.update())
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
						ui.bnColorEdit2->setEnabled(true);

						pv.hide();
						RECT rect;
						if (macro->wndState)
						{
							if (!macro->wi.update())
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
						ui.bnStateEdit2->setEnabled(true);
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
						ui.bnImageEdit2->setEnabled(true);

						pv.hide();
						RECT rect;
						if (macro->wndState)
						{
							if (!macro->wi.update())
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
					else if constexpr (std::is_same_v<T, QiDialog>)
					{
						const QiDialog& dialog = var;
						ui.tabWidget->setCurrentIndex(tab_dialog);
						ui.dialog_edit_button->setEnabled(true);
						ui.dialog_edit2_button->setEnabled(true);
					}
					else if constexpr (std::is_same_v<T, QiBlock>)
					{
						const QiBlock& block = var;
						ui.block_edit_button->setEnabled(true);
						ui.tabWidget->setCurrentIndex(tab_block);
					}
					else if constexpr (std::is_same_v<T, QiBlockExec>)
					{
						const QiBlockExec& blockExec = var;
						ui.tabWidget->setCurrentIndex(tab_block);
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
		muEdit2->setDisabled(true);

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
							muEdit2->setEnabled(true);
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
							muEdit2->setEnabled(true);
						}
						else if constexpr (std::is_same_v<T, QiImage>)
						{
							const QiImage& image = var;
							muEdit->setEnabled(true);
							muEdit2->setEnabled(true);
						}
						else if constexpr (std::is_same_v<T, QiTimer>)
						{
							const QiTimer& timer = var;
							muEdit->setEnabled(true);
						}
						else if constexpr (std::is_same_v<T, QiDialog>)
						{
							const QiDialog& dialog = var;
							muEdit->setEnabled(true);
							muEdit2->setEnabled(true);
						}
						else if constexpr (std::is_same_v<T, QiBlock>)
						{
							const QiBlock& block = var;
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
	void OnBnMousePos()
	{
		QPointSelection ps;
		POINT pt;
		if (macro->wndState)
		{
			if (!macro->wi.update())
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
	void OnEtDelayMin(const QString& text)
	{
		ui.etDelayMax->setText(text);
	}
	void OnEtLoopMin(const QString& text)
	{
		ui.etCountMax->setText(text);
	}
	void OnEtTimerMin(const QString& text)
	{
		ui.etTimerMax->setText(text);
	}
	void OnBnColorRect()
	{
		QRectSelection rs;
		RECT rect;

		if (macro->wndState)
		{
			if (!macro->wi.update())
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
	void OnBnImageRect()
	{
		QRectSelection rs;
		RECT rect;

		if (macro->wndState)
		{
			if (!macro->wi.update())
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
				ui.dialog_add_button->setText(Qi::ui.text.etChange);
				ui.block_add_button->setText(Qi::ui.text.etChange);
				ui.blockExec_add_button->setText(Qi::ui.text.etChange);
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
				ui.dialog_add_button->setText(Qi::ui.text.etAdd);
				ui.block_add_button->setText(Qi::ui.text.etAdd);
				ui.blockExec_add_button->setText(Qi::ui.text.etAdd);
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
				else if constexpr (std::is_same_v<T, QiDialog>)
				{
					const QiDialog& dialog = var;
					WidgetSetDialog(dialog);
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
		case QiType::dialog: action = WidgetGetDialog(); break;
		case QiType::block: action = WidgetGetBlock(); break;
		case QiType::blockExec: action = WidgetGetBlockExec(); break;
		}
		return action;
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
		base_new.next2 = std::move(base_old.next2);

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

		JumpPoints jumpPoints;
		LoadJumpPoints(jumpPoints, *actionsRoot);
		JumpPointUniqueActions(jumpPoints, Qi::clipboard);

		Blocks blocks;
		LoadBlocks(blocks, *actionsRoot);
		BlockUniqueActions(blocks, Qi::clipboard);

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
		return key;
	}
	QiKeyState WidgetGetKeyState() {
		QiKeyState keyState;
		keyState.vk = ui.hkState->key().keyCode;
		return keyState;
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
		return mouse;
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
		return delay;
	}
	QiText WidgetGetText() {
		QiText text;
		text.text = ui.etText->toPlainText();
		return text;
	}
	QiColor WidgetGetColor() {
		QiColor color;
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
		return color;
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
		return loop;
	}
	QiImage WidgetGetImage()
	{
		QiImage image;
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
		return image;
	}
	QiPopText WidgetGetPopText()
	{
		QiPopText popText;
		popText.text = ui.etPopText->text();
		int time = 1000;
		if (ui.etPopTextTime->text() != "") time = ui.etPopTextTime->text().toInt();
		if (time > popTextTimeMax) time = popTextTimeMax;
		popText.time = time;
		popText.sync = ui.chbPopTextWait->isChecked();
		return popText;
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
		return timer;
	}
	QiJump WidgetGetJump()
	{
		QiJump jump;
		int row = ui.lsJumpPoint->currentRow();
		if (row > -1) jump.id = ui.lsJumpPoint->item(row)->data(DataRole::id).toInt();
		return jump;
	}
	QiJumpPoint WidgetGetJumpPoint()
	{
		QiJumpPoint jumpPoint;

		JumpPoints jumpPoints;
		LoadJumpPoints(jumpPoints, *actionsRoot);

		jumpPoint.id = JumpPointUnique(jumpPoints);
		return jumpPoint;
	}
	QiDialog WidgetGetDialog()
	{
		QiDialog dialog;
		if (ui.dialog_style_none_radio->isChecked()) dialog.style = QiDialog::none;
		else if (ui.dialog_style_warning_radio->isChecked()) dialog.style = QiDialog::warning;
		else if (ui.dialog_style_error_radio->isChecked()) dialog.style = QiDialog::error;
		dialog.title = ui.dialog_title_edit->text();
		dialog.text = ui.dialog_text_edit->text();
		return dialog;
	}
	QiBlock WidgetGetBlock()
	{
		QiBlock block;

		Blocks blocks;
		LoadBlocks(blocks, *actionsRoot);

		block.id = BlockUnique(blocks);
		return block;
	}
	QiBlockExec WidgetGetBlockExec()
	{
		QiBlockExec blockExec;
		int row = ui.block_list->currentRow();
		if (row > -1) blockExec.id = ui.block_list->item(row)->data(DataRole::id).toInt();
		return blockExec;
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
		ui.etText->setText(text.text);
	}
	void WidgetSetColor(const QiColor& color) {
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
		ui.etPopText->setText(popText.text);
		ui.etPopTextTime->setText(QString::number(popText.time));
		ui.chbPopTextWait->setChecked(popText.sync);
	}
	void WidgetSetTimer(const QiTimer& timer)
	{
		ui.etTimerMin->setText(QString::number(timer.min));
		ui.etTimerMax->setText(QString::number(timer.max));
	}
	void WidgetSetDialog(const QiDialog& dialog)
	{
		if (dialog.style == QiDialog::none) ui.dialog_style_none_radio->setChecked(true);
		if (dialog.style == QiDialog::warning) ui.dialog_style_warning_radio->setChecked(true);
		if (dialog.style == QiDialog::error) ui.dialog_style_error_radio->setChecked(true);
		ui.dialog_title_edit->setText(dialog.title);
		ui.dialog_text_edit->setText(dialog.text);
	}
};