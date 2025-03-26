#pragma once
#include <src/inc_header.h>
#include "ui_EditUi.h"
class EditUi : public QDialog
{
	Q_OBJECT;
	using This = EditUi;
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
		tab_ocr,
		tab_text,
		tab_dialog,
		tab_var,
		tab_window
	};
	enum DebugState
	{
		debug_idel,
		debug_run,
		debug_pause
	};
	// line edit range
	const int posMin = -10000;
	const int posMax = 10000;
	const int moveSpeedMax = 99;
	const int moveRandMax = 9999;
	const int delayMax = 999999;
	const int loopCountMax = 9999;
	const int timerMax = 999999;
	const int colorMax = 255;
	const int imageSimMax = 99;
	const int popTextTimeMax = 9999;
	// table column
	const int tableColumn_debug = 0;
	const int tableColumn_disable = 1;
	const int tableColumn_type = 2;
	const int tableColumn_param = 3;
	const int tableColumn_mark = 4;
	Ui::EditUiClass ui;
	// edit
	QList<Layer> layers;
	bool changing = false;
	bool updating = false;
	QiVector<int> tableCurrent;
	QiVector<int> tableCurrentPrev;
	// table context menu
	QMenu* menu;
	QAction* muDel;
	QAction* muCut;
	QAction* muCopy;
	QAction* muPaste;
	QAction* muEdit;
	QAction* muEdit2;
	// action button
	QList<QPushButton*> addButtons;
	QList<QPushButton*> changeButtons;
	QList<QPushButton*> editButtons;
	QList<QPushButton*> edit2Buttons;
	QList<QGroupBox*> actionGroups;
	// test run
	QTimer* testTimer;
	// custom widget
	QPointView pv;
	QRectView rv;
	// for image viewer
	RgbMap imageMap;
	// macro
	Macro* macro = nullptr;
	Actions* actions = nullptr;
	Actions* actionsRoot = nullptr;
public:
	EditUi(Macro* macro, Actions* actions)
	{
		// loading tip
		Qi::popText->Hide();
		// top layer
		Layer layer;
		this->macro = macro;
		layer.actions = this->actionsRoot = this->actions = actions;
		layer.title = QString::fromUtf8("编辑 - ") + macro->name;
		layers.append(layer);
		// init
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		Init();
		Event();
		StyleGroup();
		Reload();
	}
private:
	// >>>> new action set here
	void StyleGroup()
	{
		if ("this window")
		{
			setProperty("group", "frame");
			ui.title_widget->setProperty("group", "title");
			ui.content_widget->setProperty("group", "client");
			ui.title_run_button->setProperty("group", "title-title_run_button");
			ui.title_back_button->setProperty("group", "title-back_button");
			ui.title_close_button->setProperty("group", "title-close_button");
		}
		if ("action widget button")
		{
			ui.mouse_position_button->setProperty("group", "get_button");
			ui.color_rect_button->setProperty("group", "get_button");
			ui.image_rect_button->setProperty("group", "get_button");
			ui.image_shot_button->setProperty("group", "get_button");
			ui.ocr_rect_button->setProperty("group", "get_button");
			ui.window_select_button->setProperty("group", "get_button");
		}
		if ("action button")
		{
			for (auto& i : addButtons) i->setProperty("group", "edit-add_button");
			for (auto& i : changeButtons) i->setProperty("group", "edit-add_button");
			for (auto& i : editButtons) i->setProperty("group", "edit-edit_button");
			for (auto& i : edit2Buttons) i->setProperty("group", "edit-edit_button");
			ui.ocr_test_button->setProperty("group", "edit-edit_button");
		}
		if ("check box")
		{
			ui.tab_lock_check->setProperty("group", "check");
			ui.tab_hideTip_check->setProperty("group", "check");
			ui.mouse_track_check->setProperty("group", "check");
			ui.color_move_check->setProperty("group", "check");
			ui.image_move_check->setProperty("group", "check");
			ui.popText_wait_check->setProperty("group", "check");
			ui.window_state_check->setProperty("group", "check");
			ui.window_child_check->setProperty("group", "check");
			ui.keyBlock_move_check->setProperty("group", "check");
			ui.ocr_match_check->setProperty("group", "check");
		}
		if ("radio button")
		{
			ui.action_running_radio->setProperty("group", "radio");
			ui.action_ending_radio->setProperty("group", "radio");
			ui.key_click_radio->setProperty("group", "radio");
			ui.key_down_radio->setProperty("group", "radio");
			ui.key_up_radio->setProperty("group", "radio");
			ui.mouse_position_radio->setProperty("group", "radio");
			ui.mouse_move_radio->setProperty("group", "radio");
			ui.dialog_style_none_radio->setProperty("group", "radio");
			ui.dialog_style_warning_radio->setProperty("group", "radio");
			ui.dialog_style_error_radio->setProperty("group", "radio");
			ui.keyBlock_on_radio->setProperty("group", "radio");
			ui.keyBlock_off_radio->setProperty("group", "radio");
		}
		if ("line edit")
		{
			ui.quickInput_text_edit->setProperty("group", "line_edit");
			ui.mouse_x_edit->setProperty("group", "line_edit");
			ui.mouse_y_edit->setProperty("group", "line_edit");
			ui.mouse_speed_edit->setProperty("group", "line_edit");
			ui.mouse_rand_edit->setProperty("group", "line_edit");
			ui.delay_min_edit->setProperty("group", "line_edit");
			ui.delay_max_edit->setProperty("group", "line_edit");
			ui.loop_min_edit->setProperty("group", "line_edit");
			ui.loop_max_edit->setProperty("group", "line_edit");
			ui.timer_min_edit->setProperty("group", "line_edit");
			ui.timer_max_edit->setProperty("group", "line_edit");
			ui.color_left_edit->setProperty("group", "line_edit");
			ui.color_top_edit->setProperty("group", "line_edit");
			ui.color_right_edit->setProperty("group", "line_edit");
			ui.color_bottom_edit->setProperty("group", "line_edit");
			ui.color_red_edit->setProperty("group", "line_edit");
			ui.color_green_edit->setProperty("group", "line_edit");
			ui.color_blue_edit->setProperty("group", "line_edit");
			ui.color_sim_edit->setProperty("group", "line_edit");
			ui.image_left_edit->setProperty("group", "line_edit");
			ui.image_top_edit->setProperty("group", "line_edit");
			ui.image_right_edit->setProperty("group", "line_edit");
			ui.image_bottom_edit->setProperty("group", "line_edit");
			ui.image_sim_edit->setProperty("group", "line_edit");
			ui.popText_text_edit->setProperty("group", "line_edit");
			ui.popText_time_edit->setProperty("group", "line_edit");
			ui.dialog_title_edit->setProperty("group", "line_edit");
			ui.dialog_text_edit->setProperty("group", "line_edit");
			ui.clock_time_edit->setProperty("group", "line_edit");
			ui.ocr_left_edit->setProperty("group", "line_edit");
			ui.ocr_top_edit->setProperty("group", "line_edit");
			ui.ocr_right_edit->setProperty("group", "line_edit");
			ui.ocr_bottom_edit->setProperty("group", "line_edit");
			ui.ocr_text_edit->setProperty("group", "line_edit");
			ui.ocr_var_edit->setProperty("group", "line_edit");
			ui.varCondition_edit->setProperty("group", "line_edit");
			ui.window_name_edit->setProperty("group", "line_edit");
		}
		if ("text edit")
		{
			ui.copyText_text->setProperty("group", "text_edit");
			ui.varOperator_edit->setProperty("group", "text_edit");
		}
		if ("hotkey edit")
		{
			ui.key_keyedit->setProperty("group", "key_edit");
			ui.keyState_keyedit->setProperty("group", "key_edit");
			ui.keyBlock_keyedit->setProperty("group", "key_edit");
		}
		if ("action tab widget")
		{
			ui.tabWidget->setProperty("group", "tab_widget");
			ui.tabWidget->tabBar()->setProperty("group", "tab_widget_bar");
		}
		if ("action list")
		{
			ui.jumpPoint_list->setProperty("group", "action_table_header");
			ui.block_list->setProperty("group", "action_table_header");
		}
		if ("action table")
		{
			ui.action_table->setProperty("group", "action_table");
			ui.action_table->horizontalHeader()->setProperty("group", "action_table_header");
			ui.action_table->verticalHeader()->setProperty("group", "action_table_header");
			menu->setProperty("group", "context_menu");
		}
		if ("table corner button")
		{
			const QObjectList& objs = ui.action_table->children();
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
					corner->setProperty("group", "action_table_header");
					widget->setProperty("group", "action_table_header");
					break;
				}
				else if (name == "QLineEdit")
				{
					QLineEdit* lineEdit = (QLineEdit*)obj;
					lineEdit->setProperty("group", "action_table_header");
				}
			}
		}
	}
	// >>>> new action set here
	void Init()
	{
		if ("table context menu")
		{
			menu = new QMenu(this);
			muDel = new QAction("删除", this);
			muCut = new QAction("剪切", this);
			muCopy = new QAction("复制", this);
			muPaste = new QAction("粘贴", this);
			muEdit = new QAction("编辑", this);
			muEdit2 = new QAction("编辑2", this);
			menu->addAction(muDel);
			menu->addAction(muCut);
			menu->addAction(muCopy);
			menu->addAction(muPaste);
			menu->addAction(muEdit);
			menu->addAction(muEdit2);
		}
		if ("button")
		{
			if ("add/change")
			{
				int type = QiType::none;

				type = QiType::end;
				ui.end_add_button->setProperty("qit", type);
				ui.end_change_button->setProperty("qit", type);
				addButtons.append(ui.end_add_button);
				changeButtons.append(ui.end_change_button);

				type = QiType::delay;
				ui.delay_add_button->setProperty("qit", type);
				ui.delay_change_button->setProperty("qit", type);
				addButtons.append(ui.delay_add_button);
				changeButtons.append(ui.delay_change_button);

				type = QiType::key;
				ui.key_add_button->setProperty("qit", type);
				ui.key_change_button->setProperty("qit", type);
				addButtons.append(ui.key_add_button);
				changeButtons.append(ui.key_change_button);

				type = QiType::mouse;
				ui.mouse_add_button->setProperty("qit", type);
				ui.mouse_change_button->setProperty("qit", type);
				addButtons.append(ui.mouse_add_button);
				changeButtons.append(ui.mouse_change_button);

				type = QiType::copyText;
				ui.copyText_add_button->setProperty("qit", type);
				ui.copyText_change_button->setProperty("qit", type);
				addButtons.append(ui.copyText_add_button);
				changeButtons.append(ui.copyText_change_button);

				type = QiType::color;
				ui.color_add_button->setProperty("qit", type);
				ui.color_change_button->setProperty("qit", type);
				addButtons.append(ui.color_add_button);
				changeButtons.append(ui.color_change_button);

				type = QiType::loop;
				ui.loop_add_button->setProperty("qit", type);
				ui.loop_change_button->setProperty("qit", type);
				addButtons.append(ui.loop_add_button);
				changeButtons.append(ui.loop_change_button);

				type = QiType::loopEnd;
				ui.endLoop_add_button->setProperty("qit", type);
				ui.endLoop_change_button->setProperty("qit", type);
				addButtons.append(ui.endLoop_add_button);
				changeButtons.append(ui.endLoop_change_button);

				type = QiType::keyState;
				ui.keyState_add_button->setProperty("qit", type);
				ui.keyState_change_button->setProperty("qit", type);
				addButtons.append(ui.keyState_add_button);
				changeButtons.append(ui.keyState_change_button);

				type = QiType::resetPos;
				ui.resetPos_add_button->setProperty("qit", type);
				ui.resetPos_change_button->setProperty("qit", type);
				addButtons.append(ui.resetPos_add_button);
				changeButtons.append(ui.resetPos_change_button);

				type = QiType::image;
				ui.image_add_button->setProperty("qit", type);
				ui.image_change_button->setProperty("qit", type);
				addButtons.append(ui.image_add_button);
				changeButtons.append(ui.image_change_button);

				type = QiType::popText;
				ui.popText_add_button->setProperty("qit", type);
				ui.popText_change_button->setProperty("qit", type);
				addButtons.append(ui.popText_add_button);
				changeButtons.append(ui.popText_change_button);

				type = QiType::savePos;
				ui.savePos_add_button->setProperty("qit", type);
				ui.savePos_change_button->setProperty("qit", type);
				addButtons.append(ui.savePos_add_button);
				changeButtons.append(ui.savePos_change_button);

				type = QiType::timer;
				ui.timer_add_button->setProperty("qit", type);
				ui.timer_change_button->setProperty("qit", type);
				addButtons.append(ui.timer_add_button);
				changeButtons.append(ui.timer_change_button);

				type = QiType::jump;
				ui.jump_add_button->setProperty("qit", type);
				ui.jump_change_button->setProperty("qit", type);
				addButtons.append(ui.jump_add_button);
				changeButtons.append(ui.jump_change_button);

				type = QiType::jumpPoint;
				ui.jumpPoint_add_button->setProperty("qit", type);
				ui.jumpPoint_change_button->setProperty("qit", type);
				addButtons.append(ui.jumpPoint_add_button);
				changeButtons.append(ui.jumpPoint_change_button);

				type = QiType::dialog;
				ui.dialog_add_button->setProperty("qit", type);
				ui.dialog_change_button->setProperty("qit", type);
				addButtons.append(ui.dialog_add_button);
				changeButtons.append(ui.dialog_change_button);

				type = QiType::block;
				ui.block_add_button->setProperty("qit", type);
				ui.block_change_button->setProperty("qit", type);
				addButtons.append(ui.block_add_button);
				changeButtons.append(ui.block_change_button);

				type = QiType::blockExec;
				ui.blockExec_add_button->setProperty("qit", type);
				ui.blockExec_change_button->setProperty("qit", type);
				addButtons.append(ui.blockExec_add_button);
				changeButtons.append(ui.blockExec_change_button);

				type = QiType::quickInput;
				ui.quickInput_add_button->setProperty("qit", type);
				ui.quickInput_change_button->setProperty("qit", type);
				addButtons.append(ui.quickInput_add_button);
				changeButtons.append(ui.quickInput_change_button);

				type = QiType::keyBlock;
				ui.keyBlock_add_button->setProperty("qit", type);
				ui.keyBlock_change_button->setProperty("qit", type);
				addButtons.append(ui.keyBlock_add_button);
				changeButtons.append(ui.keyBlock_change_button);

				type = QiType::clock;
				ui.clock_add_button->setProperty("qit", type);
				ui.clock_change_button->setProperty("qit", type);
				addButtons.append(ui.clock_add_button);
				changeButtons.append(ui.clock_change_button);

				type = QiType::ocr;
				ui.ocr_add_button->setProperty("qit", type);
				ui.ocr_change_button->setProperty("qit", type);
				addButtons.append(ui.ocr_add_button);
				changeButtons.append(ui.ocr_change_button);

				type = QiType::varOperator;
				ui.varOperator_add_button->setProperty("qit", type);
				ui.varOperator_change_button->setProperty("qit", type);
				addButtons.append(ui.varOperator_add_button);
				changeButtons.append(ui.varOperator_change_button);

				type = QiType::varCondition;
				ui.varCondition_add_button->setProperty("qit", type);
				ui.varCondition_change_button->setProperty("qit", type);
				addButtons.append(ui.varCondition_add_button);
				changeButtons.append(ui.varCondition_change_button);
			}
			if ("edit")
			{
				editButtons.append(ui.keyState_edit_button);
				editButtons.append(ui.loop_edit_button);
				editButtons.append(ui.timer_edit_button);
				editButtons.append(ui.color_edit_button);
				editButtons.append(ui.image_edit_button);
				editButtons.append(ui.dialog_edit_button);
				editButtons.append(ui.block_edit_button);
				editButtons.append(ui.clock_edit_button);
				editButtons.append(ui.ocr_edit_button);
				editButtons.append(ui.varCondition_edit_button);

				edit2Buttons.append(ui.keyState_edit2_button);
				edit2Buttons.append(ui.color_edit2_button);
				edit2Buttons.append(ui.image_edit2_button);
				edit2Buttons.append(ui.dialog_edit2_button);
				edit2Buttons.append(ui.clock_edit2_button);
				edit2Buttons.append(ui.ocr_edit2_button);
				edit2Buttons.append(ui.varCondition_edit2_button);
			}
			if ("clear shortcut")
			{
				// action add
				for (auto& i : addButtons) i->installEventFilter(this);
				for (auto& i : changeButtons) i->installEventFilter(this);
				for (auto& i : editButtons) i->installEventFilter(this);
				for (auto& i : edit2Buttons) i->installEventFilter(this);

				ui.jump_add_button->installEventFilter(this);
				ui.blockExec_add_button->installEventFilter(this);

				// title
				ui.title_close_button->installEventFilter(this);
				ui.title_back_button->installEventFilter(this);
				ui.title_run_button->installEventFilter(this);
				// action
				ui.mouse_position_button->installEventFilter(this);
				ui.color_rect_button->installEventFilter(this);
				ui.color_rgb_button->installEventFilter(this);
				ui.image_rect_button->installEventFilter(this);
				ui.image_shot_button->installEventFilter(this);
			}
			for (auto& i : changeButtons) i->setDisabled(true);
		}
		if ("tooltip")
		{
			actionGroups.append(ui.grp_key);
			actionGroups.append(ui.grp_quickInput);
			actionGroups.append(ui.grp_mouse);
			actionGroups.append(ui.grp_delay);
			actionGroups.append(ui.grp_loop);
			actionGroups.append(ui.grp_timer);
			actionGroups.append(ui.grp_end);
			actionGroups.append(ui.grp_keyState);
			actionGroups.append(ui.grp_clock);
			actionGroups.append(ui.grp_keyBlock);
			actionGroups.append(ui.grp_mousePos);
			actionGroups.append(ui.grp_jump);
			actionGroups.append(ui.grp_block);
			actionGroups.append(ui.grp_color);
			actionGroups.append(ui.grp_image);
			actionGroups.append(ui.grp_ocr);
			actionGroups.append(ui.grp_copyText);
			actionGroups.append(ui.grp_popText);
			actionGroups.append(ui.dialog_group);
			actionGroups.append(ui.grp_varOperator);
			actionGroups.append(ui.grp_varCondition);
			actionGroups.append(ui.grp_window);
		}
		if ("radio group")
		{
			QButtonGroup* actionRbs = new QButtonGroup(this);
			actionRbs->addButton(ui.action_running_radio);
			actionRbs->addButton(ui.action_ending_radio);
			ui.action_running_radio->setChecked(true);
			QButtonGroup* keyRbs = new QButtonGroup(this);
			keyRbs->addButton(ui.key_down_radio);
			keyRbs->addButton(ui.key_up_radio);
			keyRbs->addButton(ui.key_click_radio);
			ui.key_click_radio->setChecked(true);
			QButtonGroup* moveRbs = new QButtonGroup(this);
			moveRbs->addButton(ui.mouse_position_radio);
			moveRbs->addButton(ui.mouse_move_radio);
			ui.mouse_position_radio->setChecked(true);
			QButtonGroup* dialogRbs = new QButtonGroup(this);
			dialogRbs->addButton(ui.dialog_style_none_radio);
			dialogRbs->addButton(ui.dialog_style_warning_radio);
			dialogRbs->addButton(ui.dialog_style_error_radio);
			ui.dialog_style_none_radio->setChecked(true);
			QButtonGroup* keyBlockRbs = new QButtonGroup(this);
			keyBlockRbs->addButton(ui.keyBlock_on_radio);
			keyBlockRbs->addButton(ui.keyBlock_off_radio);
			ui.keyBlock_on_radio->setChecked(true);
		}
		if ("line edit range")
		{
			ui.mouse_x_edit->setValidator(new QIntValidator(0, posMax, this));
			ui.mouse_y_edit->setValidator(new QIntValidator(0, posMax, this));
			ui.mouse_speed_edit->setValidator(new QIntValidator(0, moveSpeedMax, this));
			ui.mouse_rand_edit->setValidator(new QIntValidator(0, moveRandMax, this));
			ui.delay_min_edit->setValidator(new QIntValidator(0, delayMax, this));
			ui.delay_max_edit->setValidator(new QIntValidator(0, delayMax, this));
			ui.loop_min_edit->setValidator(new QIntValidator(0, loopCountMax, this));
			ui.loop_max_edit->setValidator(new QIntValidator(0, loopCountMax, this));
			ui.timer_max_edit->setValidator(new QIntValidator(0, timerMax, this));
			ui.timer_min_edit->setValidator(new QIntValidator(0, timerMax, this));
			ui.color_red_edit->setValidator(new QIntValidator(0, colorMax, this));
			ui.color_green_edit->setValidator(new QIntValidator(0, colorMax, this));
			ui.color_blue_edit->setValidator(new QIntValidator(0, colorMax, this));
			ui.color_sim_edit->setValidator(new QIntValidator(0, colorMax, this));
			ui.color_left_edit->setValidator(new QIntValidator(0, posMax, this));
			ui.color_top_edit->setValidator(new QIntValidator(0, posMax, this));
			ui.color_right_edit->setValidator(new QIntValidator(0, posMax, this));
			ui.color_bottom_edit->setValidator(new QIntValidator(0, posMax, this));
			ui.image_left_edit->setValidator(new QIntValidator(0, posMax, this));
			ui.image_top_edit->setValidator(new QIntValidator(0, posMax, this));
			ui.image_right_edit->setValidator(new QIntValidator(0, posMax, this));
			ui.image_bottom_edit->setValidator(new QIntValidator(0, posMax, this));
			ui.image_sim_edit->setValidator(new QIntValidator(0, imageSimMax, this));
			ui.popText_time_edit->setValidator(new QIntValidator(0, popTextTimeMax, this));
		}
		if ("table")
		{
			ui.action_table->setContextMenuPolicy(Qt::CustomContextMenu);
			ui.action_table->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
			ui.action_table->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
			ui.action_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			ui.action_table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
			ui.action_table->verticalHeader()->setDefaultSectionSize(0);
			ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_debug, QHeaderView::ResizeMode::Fixed);
			ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_disable, QHeaderView::ResizeMode::Fixed);
			ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_type, QHeaderView::ResizeMode::Fixed);
			ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_param, QHeaderView::ResizeMode::Interactive);
			ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_mark, QHeaderView::ResizeMode::Interactive);
			ui.action_table->setColumnWidth(tableColumn_debug, 35);
			ui.action_table->setColumnWidth(tableColumn_disable, 50);
			ui.action_table->setColumnWidth(tableColumn_type, 100);
			ui.action_table->setColumnWidth(tableColumn_param, 300);
			ui.action_table->setColumnWidth(tableColumn_mark, 100);
			ui.action_table->setStyleSheet("QTableCornerButton::section,QHeaderView::section{background-color:rgba(0,0,0,0)}QScrollBar{background:transparent}");
		}
		if ("key edit")
		{
			ui.key_keyedit->setCombinationMode(false);
			ui.key_keyedit->setDeviceEnabled(true, true, true, false);
			ui.key_keyedit->setKey(VK_LBUTTON);

			ui.keyState_keyedit->setCombinationMode(false);
			ui.keyState_keyedit->setDeviceEnabled(true, true, true, true);
			ui.keyState_keyedit->setKey(VK_LBUTTON);

			ui.keyBlock_keyedit->setCombinationMode(false);
			ui.keyBlock_keyedit->setDeviceEnabled(true, true, true, false);
			ui.keyBlock_keyedit->setKey(VK_LBUTTON);
		}
		if ("timer")
		{
			testTimer = new QTimer(this);
		}
		if ("check")
		{
			ui.tab_lock_check->setChecked(Qi::set.tabLock);
			ui.tab_hideTip_check->setChecked(Qi::set.tabHideTip);
			if (Qi::set.tabHideTip) for (auto& i : actionGroups) i->setToolTipDuration(1);
			else for (auto& i : actionGroups) i->setToolTipDuration(0);
		}
		// enable qlable scale
		ui.image_view_label->setScaledContents(true);
		// load Window mode
		if (macro->wndState) macro->wndInfo.update(), SetWindowMode();
	}
	// >>>> new action set here
	void Event()
	{
		if ("title")
		{
			connect(ui.title_close_button, &QPushButton::clicked, this, [this] {
				Qi::popText->Show("正在保存宏");
				QApplication::postEvent(this, new QEvent((QEvent::Type)EditEvent::close));
				});
			connect(ui.title_back_button, &QPushButton::clicked, this, [this] {
				layers.removeLast();
				if (layers.empty())
				{
					Qi::popText->Show("正在保存宏");
					QApplication::postEvent(this, new QEvent((QEvent::Type)EditEvent::close));
				}
				else
				{
					actions = layers.last().actions;
					setWindowTitle(layers.last().title);
					Reload();
				}
				});
			connect(ui.title_run_button, &QPushButton::clicked, this, [this] {
				if (QiThread::MacroRunActive(macro) || QiThread::MacroEndActive(macro))
				{
					macro->interpreter->DebugContinue();
				}
				else
				{
					Qi::run = true;
					timeBeginPeriod(1);
					if (ui.action_running_radio->isChecked()) QiThread::StartMacroRun(macro);
					else QiThread::StartMacroEnd(macro);
					testTimer->start(16);
				}
				SetDebugState(debug_run);
				});
			connect(ui.window_select_button, &QPushButton::clicked, this, [this] { SelectWindow(); });
			connect(ui.window_state_check, &QCheckBox::toggled, this, [this](bool state) {
				if (state)
				{
					macro->wndState = true;
					ui.window_select_button->setDisabled(false);
					ui.window_child_check->setDisabled(false);
					ui.window_name_edit->setDisabled(false);
				}
				else
				{
					macro->wndState = false;
					ui.window_select_button->setDisabled(true);
					ui.window_child_check->setDisabled(true);
					ui.window_name_edit->setDisabled(true);
				}
				});
			connect(ui.window_child_check, &QCheckBox::toggled, this, [this](bool state) { macro->wndInfo.child = state; });
		}
		// >>>> new action set here
		if ("action widget")
		{
			connect(ui.tab_lock_check, &QCheckBox::toggled, this, [this](bool state) {
				Qi::set.tabLock = state;
				QiJson::SaveJson();
				});
			connect(ui.tab_hideTip_check, &QCheckBox::toggled, this, [this](bool state) {
				if (state) for (auto& i : actionGroups) i->setToolTipDuration(1);
				else for (auto& i : actionGroups) i->setToolTipDuration(0);
				Qi::set.tabHideTip = state;
				QiJson::SaveJson();
				});
			for (auto& i : addButtons) connect(i, &QPushButton::clicked, this, [this, i] { ItemAdd(i->property("qit").toInt()); });
			for (auto& i : changeButtons) connect(i, &QPushButton::clicked, this, [this, i] { ItemChange(i->property("qit").toInt()); });
			for (auto& i : editButtons) connect(i, &QPushButton::clicked, this, [this] { NextEdit(false); });
			for (auto& i : edit2Buttons) connect(i, &QPushButton::clicked, this, [this] { NextEdit(true); });
			// move
			connect(ui.mouse_position_button, &QPushButton::clicked, this, [this] {
				QPointSelection ps;
				POINT pt;
				if (macro->wndState)
				{
					if (!macro->wndInfo.update())
					{
						SelectWindow();
					}
					RECT wrect = Window::rect(macro->wndInfo.wnd);
					pt = ps.Start(wrect);
					pt = QiFn::WRTA({ pt.x, pt.y }, macro->wndInfo.wnd);
				}
				else
				{
					pt = ps.Start();
					pt = QiFn::RTA({ pt.x, pt.y });
				}
				QiMouse mouse(WidgetGetMouse());
				mouse.x = pt.x;
				mouse.y = pt.y;
				WidgetSet(mouse);
				});
			connect(ui.mouse_position_radio, &QRadioButton::toggled, this, [this](bool state) {
				if (state)
				{
					ui.mouse_x_edit->setValidator(new QIntValidator(0, posMax, this));
					ui.mouse_y_edit->setValidator(new QIntValidator(0, posMax, this));
					if (ui.mouse_x_edit->text().toInt() < 0) ui.mouse_x_edit->setText("0");
					if (ui.mouse_y_edit->text().toInt() < 0) ui.mouse_y_edit->setText("0");
					ui.mouse_position_button->setText("+");
					ui.mouse_position_button->setDisabled(0);
				}
				});
			connect(ui.mouse_move_radio, &QRadioButton::toggled, this, [this](bool state) {
				if (state)
				{
					ui.mouse_x_edit->setValidator(new QIntValidator(posMin, posMax, this));
					ui.mouse_y_edit->setValidator(new QIntValidator(posMin, posMax, this));
					ui.mouse_position_button->setText("×");
					ui.mouse_position_button->setDisabled(true);
				}
				});
			// delay
			connect(ui.delay_min_edit, &QLineEdit::textChanged, this, [this](const QString& text) { ui.delay_max_edit->setText(text); });
			// loop
			connect(ui.loop_min_edit, &QLineEdit::textChanged, this, [this](const QString& text) { ui.loop_max_edit->setText(text); });
			// timer
			connect(ui.timer_min_edit, &QLineEdit::textChanged, this, [this](const QString& text) { ui.timer_max_edit->setText(text); });
			// color
			connect(ui.color_rect_button, &QPushButton::clicked, this, [this] {
				QRectSelection rs;
				RECT rect;
				Rgba rgba;
				if (macro->wndState)
				{
					if (!macro->wndInfo.update())
					{
						SelectWindow();
					}
					RECT wrect = Window::rect(macro->wndInfo.wnd);
					rect = rs.Start(wrect);
					if ((rect.right - rect.left == 0) && (rect.bottom - rect.top == 0))
					{
						HDC hdc = GetDC(nullptr);
						rgba = Rgba(GetPixel(hdc, rect.left, rect.top));
						rgba.a = 1;
						ReleaseDC(nullptr, hdc);
					}
					rect = QiFn::WRTAR(rect, macro->wndInfo.wnd);
				}
				else
				{
					rect = rs.Start();
					if ((rect.right - rect.left == 0) && (rect.bottom - rect.top == 0))
					{
						HDC hdc = GetDC(nullptr);
						rgba = Rgba(GetPixel(hdc, rect.left, rect.top));
						rgba.a = 1;
						ReleaseDC(nullptr, hdc);
					}
					rect = QiFn::RTAR(rect);
				}
				QiColor color(WidgetGetColor());
				color.rect = rect;
				if (rgba.a)
				{
					color.rgbe.r = rgba.r;
					color.rgbe.g = rgba.g;
					color.rgbe.b = rgba.b;
				}
				WidgetSet(color);
				});
			connect(ui.color_rgb_button, &QPushButton::clicked, this, [this] {
				QColorSelection cs;
				QColorDialog cd(cs.Start(), this);
				cd.setStyleSheet(Qi::ui.dialogStyle);
				cd.exec();
				QiColor color(WidgetGetColor());
				color.rgbe.r = cd.currentColor().red();
				color.rgbe.g = cd.currentColor().green();
				color.rgbe.b = cd.currentColor().blue();
				WidgetSet(color);
				});
			// image
			connect(ui.image_rect_button, &QPushButton::clicked, this, [this] {
				QRectSelection rs;
				RECT rect;
				if (macro->wndState)
				{
					if (!macro->wndInfo.update())
					{
						SelectWindow();
					}
					RECT wrect = Window::rect(macro->wndInfo.wnd);
					rect = rs.Start(wrect);
					rect = QiFn::WRTAR(rect, macro->wndInfo.wnd);
				}
				else
				{
					rect = rs.Start();
					rect = QiFn::RTAR(rect);
				}
				QiImage image(WidgetGetImage());
				image.rect = rect;
				WidgetSet(image);
				});
			connect(ui.image_shot_button, &QPushButton::clicked, this, [this] {
				QRectSelection rs;
				RECT rect = rs.Start();
				QiImage image(WidgetGetImage());
				Image::ScreenRgbMap(imageMap, rect);
				image.map = imageMap;
				RECT before = QiFn::ATRR(image.rect);
				if ((!image.rect.left && !image.rect.top && !image.rect.right && !image.rect.bottom) || rect.left < before.left || rect.top < before.top || rect.right > before.right || rect.bottom > before.bottom) image.rect = QiFn::RTAR(rect);
				WidgetSet(image);
				});
			// ocr
			connect(ui.ocr_rect_button, &QPushButton::clicked, this, [this] {
				QRectSelection rs;
				RECT rect;
				if (macro->wndState)
				{
					if (!macro->wndInfo.update())
					{
						SelectWindow();
					}
					RECT wrect = Window::rect(macro->wndInfo.wnd);
					rect = rs.Start(wrect);
					rect = QiFn::WRTAR(rect, macro->wndInfo.wnd);
				}
				else
				{
					rect = rs.Start();
					rect = QiFn::RTAR(rect);
				}
				QiOcr ocr(WidgetGetOcr());
				ocr.rect = rect;
				WidgetSet(ocr);
				});
			connect(ui.ocr_test_button, &QPushButton::clicked, this, [this] {
				if (!Qi::ocr)
				{
					MsgBox::Warning(L"没有安装OCR组件，无法使用文字识别功能");
					return;
				}
				std::string text = Qi::ocr->scan(QiFn::ATRR(WidgetGetOcr().rect));
				if (text.empty())
				{
					Qi::popText->Popup("没有识别到内容");
				}
				else
				{
					QString str = QString::fromStdString(text);
					str.replace(QChar('\n'), "");
					Qi::popText->Popup(str);
				}
				});
		}
		// >>>> new action set here
		if ("action table")
		{
			// hotkey
			ui.action_table->installEventFilter(this);
			// drag
			ui.action_table->viewport()->installEventFilter(this);
			connect(ui.action_running_radio, &QRadioButton::toggled, this, [this](bool state) {
				if (state && (layers.size() == 1))
				{
					layers.first().actions = actionsRoot = actions = &macro->acRun;
					TableReload();
				}
				});
			connect(ui.action_ending_radio, &QRadioButton::toggled, this, [this](bool state) {
				if (state && (layers.size() == 1))
				{
					layers.first().actions = actionsRoot = actions = &macro->acEnd;
					TableReload();
				}
				});
			// mark
			connect(ui.action_table, &QTableWidget::cellChanged, this, [this](int row, int column) {
				if (updating) return;
				if (row < 0) return;
				if (column != tableColumn_mark) return;
				QiBase& base = actions->at(row).base();
				base.mark = ui.action_table->item(row, tableColumn_mark)->text();
				if ((base.type == QiType::jumpPoint) || (base.type == QiType::block)) TableUpdate(row);
				});
			// mark, disable, debug
			connect(ui.action_table, &QTableWidget::cellClicked, this, [this](int row, int column) {
				if (column == tableColumn_mark)
				{
					ui.action_table->editItem(ui.action_table->item(row, tableColumn_mark));
					TableUpdate(row);
				}
				else if (column == tableColumn_disable)
				{
					if (tableCurrentPrev.empty())
					{
						QiBase& base = actions->at(row).base();
						base.disable = !base.disable;
						TableUpdate(row);
					}
					else
					{
						bool exist = false;
						for (auto& i : tableCurrentPrev)
						{
							if (row == i)
							{
								exist = true;
								break;
							}
						}
						if (exist)
						{
							QiVector<int> selection = tableCurrentPrev;
							for (auto& i : selection)
							{
								QiBase& base = actions->at(i).base();
								base.disable = !base.disable;
								TableUpdate(i);
								TableSelection(selection);
							}
						}
						else
						{
							QiBase& base = actions->at(row).base();
							base.disable = !base.disable;
							TableUpdate(row);
						}

					}
				}
				else if (column == tableColumn_debug)
				{
					QiBase& base = actions->at(row).base();
					if (Input::state(VK_CONTROL))
					{
						IterActions(*actionsRoot, [](Action& action) { if (action.base().debug_entry) action.base().debug_entry = false; return false; });
						base.debug_entry = true;
						base.debug_break = base.debug_exit = false;
					}
					else if (Input::state(VK_SHIFT))
					{
						IterActions(*actionsRoot, [](Action& action) { if (action.base().debug_exit) action.base().debug_exit = false; return false; });
						base.debug_exit = true;
						base.debug_break = base.debug_entry = false;
					}
					else
					{
						if (base.debug_break || base.debug_entry || base.debug_exit)
						{
							base.debug_break = base.debug_entry = base.debug_exit = false;
						}
						else
						{
							base.debug_break = true;
							base.debug_entry = base.debug_exit = false;
						}
					}
					TableReload();
				}
				});
			// selection
			// >>>> new action set here
			connect(ui.action_table, &QTableWidget::itemSelectionChanged, this, [this] {
				tableCurrentPrev = tableCurrent;
				tableCurrent.clear();
				QList<QTableWidgetItem*> items = ui.action_table->selectedItems();
				for (auto& i : items) if (i->column() == 0) tableCurrent.append(i->row());
				tableCurrent.sort([](const int& v1, const int& v2) { return v1 < v2; });
				DisableMenus();
				DisableButtons();
				pv.hide();
				rv.hide();
				// selction is solid
				if (tableCurrent.empty())
				{
					for (auto& i : changeButtons) i->setDisabled(true);
					return;
				}
				for (auto& i : changeButtons) i->setEnabled(true);
				if (tableCurrent.size() == 1)
				{
					bool tabLock = ui.tab_lock_check->isChecked();
					int tab = tab_base;
					const Action& var = actions->at(tableCurrent.front());
					switch (var.index())
					{
					case QiType::end: tab = tab_loop; break;
					case QiType::delay: tab = tab_base; break;
					case QiType::key: tab = tab_base; break;
					case QiType::mouse: tab = tab_base;
					{
						const QiMouse& mouse = std::get<QiMouse>(var);
						if (!mouse.move)
						{
							POINT pt;
							if (macro->wndState)
							{
								if (!macro->wndInfo.update()) SelectWindow();
								POINT rpt = QiFn::WATR({ mouse.x, mouse.y }, macro->wndInfo.wnd);
								pt = Window::pos(macro->wndInfo.wnd);
								pt.x += rpt.x, pt.y += rpt.y;
							}
							else pt = QiFn::ATR({ mouse.x, mouse.y });
							pv.Show(pt);
						}
					} break;
					case QiType::copyText: tab = tab_text; break;
					case QiType::color: tab = tab_color;
					{
						const QiColor& color = std::get<QiColor>(var);
						ui.color_edit_button->setEnabled(true);
						ui.color_edit2_button->setEnabled(true);
						RECT rect;
						if (macro->wndState)
						{
							if (!macro->wndInfo.update()) SelectWindow();
							rect = QiFn::WATRR(color.rect, macro->wndInfo.wnd);
							POINT pt = Window::pos(macro->wndInfo.wnd);
							rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
						}
						else rect = QiFn::ATRR(color.rect);
						rv.Show(rect);
					} break;
					case QiType::loop: tab = tab_loop;
					{
						ui.loop_edit_button->setEnabled(true);
					} break;
					case QiType::loopEnd: tab = tab_loop; break;
					case QiType::keyState: tab = tab_state;
					{
						ui.keyState_edit_button->setEnabled(true);
						ui.keyState_edit2_button->setEnabled(true);
					} break;
					case QiType::resetPos: tab = tab_state; break;
					case QiType::image: tab = tab_color;
					{
						const QiImage& image = std::get<QiImage>(var);
						ui.image_edit_button->setEnabled(true);
						ui.image_edit2_button->setEnabled(true);
						RECT rect;
						if (macro->wndState)
						{
							if (!macro->wndInfo.update()) SelectWindow();
							rect = QiFn::WATRR(image.rect, macro->wndInfo.wnd);
							POINT pt = Window::pos(macro->wndInfo.wnd);
							rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
						}
						else rect = QiFn::ATRR(image.rect);
						rv.Show(rect);
						WidgetSet(image);
					} break;
					case QiType::popText: tab = tab_text; break;
					case QiType::savePos: tab = tab_state; break;
					case QiType::timer: tab = tab_loop;
					{
						ui.timer_edit_button->setEnabled(true);
					} break;
					case QiType::jump: tab = tab_jump; break;
					case QiType::jumpPoint: tab = tab_jump; break;
					case QiType::dialog: tab = tab_dialog;
					{
						ui.dialog_edit_button->setEnabled(true);
						ui.dialog_edit2_button->setEnabled(true);
					} break;
					case QiType::block: tab = tab_block;
					{
						ui.block_edit_button->setEnabled(true);
					} break;
					case QiType::blockExec: tab = tab_block; break;
					case QiType::quickInput: tab = tab_base; break;
					case QiType::keyBlock: tab = tab_state; break;
					case QiType::clock: tab = tab_state;
					{
						ui.clock_edit_button->setEnabled(true);
						ui.clock_edit2_button->setEnabled(true);
					} break;
					case QiType::ocr: tab = tab_ocr;
					{
						const QiOcr& ocr = std::get<QiOcr>(var);
						ui.ocr_edit_button->setEnabled(true);
						ui.ocr_edit2_button->setEnabled(true);
						RECT rect;
						if (macro->wndState)
						{
							if (!macro->wndInfo.update()) SelectWindow();
							rect = QiFn::WATRR(ocr.rect, macro->wndInfo.wnd);
							POINT pt = Window::pos(macro->wndInfo.wnd);
							rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
						}
						else rect = QiFn::ATRR(ocr.rect);
						rv.Show(rect);
					} break;
					case QiType::varOperator: tab = tab_var; break;
					case QiType::varCondition: tab = tab_var;
					{
						ui.varCondition_edit_button->setEnabled(true);
						ui.varCondition_edit2_button->setEnabled(true);
					} break;
					}
					if (!tabLock) ui.tabWidget->setCurrentIndex(tab);
				}
				});
			// value
			connect(ui.action_table, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) { if (row < 0) return; ItemUse(row); });
			// mouse rbutton menu
			// >>>> new action set here (include edit)
			connect(ui.action_table, &QTableWidget::customContextMenuRequested, this, [this] {
				if ("set menu item state")
				{
					muPaste->setEnabled(Qi::clipboard.not_empty());
					if (tableCurrent.empty())
					{
						menu->exec(QCursor::pos());
						return;
					}
					// selction is not empty
					muDel->setEnabled(true);
					muCut->setEnabled(true);
					muCopy->setEnabled(true);
					// selction is solid
					if (tableCurrent.size() == 1)
					{
						if ("have next")
						{
							bool edit = false;
							bool edit2 = false;
							const Action& var = actions->at(tableCurrent.front());
							switch (var.index())
							{
							case QiType::color: edit = edit2 = true; break;
							case QiType::loop: edit = true; break;
							case QiType::keyState: edit = edit2 = true; break;
							case QiType::image: edit = edit2 = true; break;
							case QiType::timer: edit = true; break;
							case QiType::dialog: edit = edit2 = true; break;
							case QiType::block: edit = true; break;
							case QiType::ocr: edit = edit2 = true; break;
							case QiType::varCondition: edit = edit2 = true; break;
							}
							muEdit->setEnabled(edit);
							muEdit2->setEnabled(edit2);
						}
					}
					menu->exec(QCursor::pos());
				}
				});
		}
		if ("action table menu")
		{
			connect(muDel, &QAction::triggered, this, [this] { ItemDel(); });
			connect(muCut, &QAction::triggered, this, [this] { ItemCut(); });
			connect(muCopy, &QAction::triggered, this, [this] { ItemCopy(); });
			connect(muPaste, &QAction::triggered, this, [this] { ItemPaste(); });
			connect(muEdit, &QAction::triggered, this, [this] { NextEdit(false); });
			connect(muEdit2, &QAction::triggered, this, [this] { NextEdit(true); });
		}
		if ("action list")
		{
			connect(ui.jumpPoint_list, &QListWidget::itemClicked, this, [this] {
				if (ui.jumpPoint_list->currentRow() < 0) ui.jump_add_button->setDisabled(true);
				else ui.jump_add_button->setEnabled(true);
				});
			connect(ui.block_list, &QListWidget::itemClicked, this, [this] {
				if (ui.block_list->currentRow() < 0) ui.blockExec_add_button->setDisabled(true);
				else ui.blockExec_add_button->setEnabled(true);
				});
		}
		if ("timer")
		{
			connect(testTimer, &QTimer::timeout, this, [this]
				{
					if (QiThread::MacroRunActive(macro) || QiThread::MacroEndActive(macro))
					{
						if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_F10))
						{
							macro->interpreter->DebugContinue();
							if (ui.action_running_radio->isChecked()) QiThread::ExitMacroRun(macro);
							else QiThread::ExitMacroEnd(macro);
						}
					}
					else
					{
						testTimer->stop();
						timeEndPeriod(1);
						Qi::run = false;
						Sleep(300);
						SetDebugState(debug_idel);
					}
				}
			);
		}
	}
	void DisableMenus()
	{
		muDel->setDisabled(true);
		muCut->setDisabled(true);
		muCopy->setDisabled(true);
		muEdit->setDisabled(true);
		muEdit2->setDisabled(true);
	}
	void DisableButtons()
	{
		for (auto& i : editButtons) i->setDisabled(true);
		for (auto& i : edit2Buttons) i->setDisabled(this);
		ui.jump_add_button->setDisabled(this);
		ui.blockExec_add_button->setDisabled(this);
	}
	void Reload()
	{
		setWindowTitle(layers.last().title);
		if (layers.size() > 1)
		{
			ui.action_running_radio->setVisible(false);
			ui.action_ending_radio->setVisible(false);
			ui.block_add_button->setDisabled(true);
			ui.block_edit_button->setDisabled(true);
		}
		else
		{
			ui.action_running_radio->setVisible(true), ui.action_ending_radio->setVisible(true);
			ui.block_add_button->setEnabled(true);
			ui.block_edit_button->setEnabled(true);
		}
		TableReload();
	}
	// >>>> new action set here (include edit)
	void SetDebugState(int debugState)
	{
		if (debugState == debug_idel)
		{
			ui.title_run_label->setText("运行");
			ui.title_run_button->setStyleSheet("QPushButton{background-color:#0E0;border-radius:10px}QPushButton:hover{background-color:#0C0}");
			ui.action_running_radio->setDisabled(false);
			ui.action_ending_radio->setDisabled(false);
			ui.title_close_button->setDisabled(false);
			ui.title_back_button->setDisabled(false);
			ui.content_widget->setDisabled(false);
		}
		else if (debugState == debug_run)
		{
			ui.title_run_label->setText("运行中（Shift F10停止）");
			ui.title_run_button->setStyleSheet("QPushButton{background-color:#0E0;border-radius:10px}QPushButton:hover{background-color:#0C0}");
			ui.action_running_radio->setDisabled(true);
			ui.action_ending_radio->setDisabled(true);
			ui.title_close_button->setDisabled(true);
			ui.title_back_button->setDisabled(true);
			ui.content_widget->setDisabled(true);
		}
		else if (debugState == debug_pause)
		{
			ui.title_run_label->setText("已暂停，点击继续（Shift F10停止）");
			ui.title_run_button->setStyleSheet("QPushButton{background-color:#FC0;border-radius:10px}QPushButton:hover{background-color:#DA0}");
		}
	}
	void NextEdit(bool edit2)
	{
		int p = ui.action_table->currentRow(); if (p < 0) return;
		QString title;
		Actions* next = nullptr;
		Action& var = actions->at(p);
		switch (var.index())
		{
		case QiType::color:
		{
			QiColor& color = std::get<QiColor>(var);
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
		} break;
		case QiType::loop:
		{
			QiLoop& loop = std::get<QiLoop>(var);
			next = &loop.next;
			title = "编辑 - 循环";
		} break;
		case QiType::keyState:
		{
			QiKeyState& keyState = std::get<QiKeyState>(var);
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
		} break;
		case QiType::image:
		{
			QiImage& image = std::get<QiImage>(var);
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
		} break;
		case QiType::timer:
		{
			QiTimer& timer = std::get<QiTimer>(var);
			next = &timer.next;
			title = "编辑 - 定时";
		} break;
		case QiType::jump:
		{
			QiJump& jump = std::get<QiJump>(var);
		} break;
		case QiType::jumpPoint:
		{
			QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(var);
		} break;
		case QiType::dialog:
		{
			QiDialog& dialog = std::get<QiDialog>(var);
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
		} break;
		case QiType::block:
		{
			QiBlock& block = std::get<QiBlock>(var);
			next = &block.next;
			title = "编辑 - 块";
		} break;
		case QiType::clock:
		{
			QiClock& clock = std::get<QiClock>(var);
			if (edit2)
			{
				next = &clock.next2;
				title = "编辑 - 时钟（未经过）";
			}
			else
			{
				next = &clock.next;
				title = "编辑 - 时钟（已经过）";
			}
		} break;
		case QiType::ocr:
		{
			QiOcr& clock = std::get<QiOcr>(var);
			if (edit2)
			{
				next = &clock.next2;
				title = "编辑 - 文字识别（失败）";
			}
			else
			{
				next = &clock.next;
				title = "编辑 - 文字识别（成功）";
			}
		} break;
		case QiType::varCondition:
		{
			QiVarCondition& clock = std::get<QiVarCondition>(var);
			if (edit2)
			{
				next = &clock.next2;
				title = "编辑 - 变量条件（否）";
			}
			else
			{
				next = &clock.next;
				title = "编辑 - 变量条件（是）";
			}
		} break;
		}
		if (next)
		{
			Layer layer;
			layer.actions = next;
			layer.title = title;
			layers.append(layer);
			actions = next;
			setWindowTitle(title);
			Reload();
		}
	}
	void SetWindowMode()
	{
		ui.window_state_check->setChecked(macro->wndState);
		ui.window_child_check->setChecked(macro->wndInfo.child);
		ui.window_name_edit->setText(macro->wndInfo.wndName);
	}
	void setWindowTitle(const QString& title)
	{
		QDialog::setWindowTitle(title);
		ui.title_label->setText(title);
	}
	void SelectWindow()
	{
		QPoint pt = pos();
		move(-1000, -1000);
		macro->wndInfo = QiFn::WindowSelection();
		move(pt);
		SetWindowMode();
	}

	// ids
	bool IterActions(Actions& actions, std::function<bool(Action&)> callBack, int type = QiType::none)
	{
		for (auto& i : actions)
		{
			if ((type == QiType::none || i.index() == type) && callBack(i)) return true;
			if (i.base().next.not_empty() && IterActions(i.base().next, callBack, type)) return true;
			if (i.base().next2.not_empty() && IterActions(i.base().next2, callBack, type)) return true;
		}
		return false;
	}
	// ids
	bool IterActions(const Actions& actions, std::function<bool(const Action&)> callBack, int type = QiType::none)
	{
		for (auto& i : actions)
		{
			if ((type == QiType::none || i.index() == type) && callBack(i)) return true;
			if (i.base().next.not_empty() && IterActions(i.base().next, callBack, type)) return true;
			if (i.base().next2.not_empty() && IterActions(i.base().next2, callBack, type)) return true;
		}
		return false;
	}
	// type: jumpPoint, jump, block, blockExec
	QiVector<int> LoadIds(const Actions& actions, int type)
	{
		QiVector<int> ids;
		IterActions(actions, [&ids, type](const Action& action) {
			if (action.index() == type)
			{
				switch (type)
				{
				case QiType::jumpPoint:
				{
					const QiJumpPoint& ref = std::get<QiJumpPoint>(action);
					ids.append(ref.id);
				} break;
				case QiType::jump:
				{
					const QiJump& ref = std::get<QiJump>(action);
					ids.append(ref.id);
				} break;
				case QiType::block:
				{
					const QiBlock& ref = std::get<QiBlock>(action);
					ids.append(ref.id);
				} break;
				case QiType::blockExec:
				{
					const QiBlockExec& ref = std::get<QiBlockExec>(action);
					ids.append(ref.id);
				} break;
				}
			}
			return false;
			});
		return ids;
	}
	Actions LoadType(const Actions& actions, int type)
	{
		Actions result;
		IterActions(actions, [&result, type](const Action& action) {
			if (action.index() == type) result.append(action);
			return false;
			}, type);
		return result;
	}
	bool FindId(const QiVector<int>& ids, int id)
	{
		for (auto& i : ids) if (i == id) return true; return false;
	}
	// type: jump, blockExec
	void InvalidId(Actions& actions, int type)
	{
		if (type == QiType::jump)
		{
			QiVector<int> ids = LoadIds(actions, QiType::jumpPoint);
			IterActions(actions, [this, &ids](Action& action) {
				QiJump& jump = std::get<QiJump>(action);
				if (!FindId(ids, jump.id)) jump.id = -1;
				return false;
				}, type);
		}
		else if (type == QiType::blockExec)
		{
			QiVector<int> ids = LoadIds(actions, QiType::block);
			IterActions(actions, [this, &ids](Action& action) {
				QiBlockExec& blockExec = std::get<QiBlockExec>(action);
				if (!FindId(ids, blockExec.id)) blockExec.id = -1;
				return false;
				}, type);
		}
	}
	int UniqueId(const QiVector<int>& ids)
	{
		int id = 1;
		for (auto& i : ids) if (i >= id) id = i + 1;
		return id;
	}
	// type: jumpPoint, block
	void UniqueActionsId(QiVector<int> ids, Actions& actions, int type)
	{
		if (type == QiType::jumpPoint)
		{
			IterActions(actions, [this, &ids, &actions](Action& action) {
				QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(action);
				if (FindId(ids, jumpPoint.id))
				{
					int id_res = jumpPoint.id;
					jumpPoint.id = UniqueId(ids);
					ids.append(jumpPoint.id);
					IterActions(actions, [id_res, jumpPoint](Action& action) {
						QiJump& jump = std::get<QiJump>(action);
						if (id_res == jump.id) jump.id = jumpPoint.id;
						return false;
						}, QiType::jump);
				}
				return false;
				}, type);
		}
		else if (type == QiType::block)
		{
			IterActions(actions, [this, &ids, &actions](Action& action) {
				QiBlock& block = std::get<QiBlock>(action);
				if (FindId(ids, block.id))
				{
					int id_res = block.id;
					block.id = UniqueId(ids);
					ids.append(block.id);
					IterActions(actions, [id_res, block](Action& action) {
						QiBlockExec& blockExec = std::get<QiBlockExec>(action);
						if (id_res == blockExec.id) blockExec.id = block.id;
						return false;
						}, QiType::blockExec);
				}
				return false;
				}, type);
		}
	}
	void ListJumpPointReload()
	{
		InvalidId(*actionsRoot, QiType::jump);
		ui.jumpPoint_list->clear();
		Actions jumpPoints = LoadType(*actionsRoot, QiType::jumpPoint);
		for (auto& i : jumpPoints)
		{
			const QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(i);
			QListWidgetItem* item = new QListWidgetItem(QString::number(jumpPoint.id) + QString::fromUtf8("　　　") + jumpPoint.mark);
			item->setData(DataRole::id, jumpPoint.id);
			ui.jumpPoint_list->addItem(item);
		}
	}
	void ListBlockReload()
	{
		InvalidId(*actionsRoot, QiType::blockExec);
		ui.block_list->clear();
		Actions blocks = LoadType(*actionsRoot, QiType::block);
		for (auto& i : blocks)
		{
			const QiBlock& block = std::get<QiBlock>(i);
			QListWidgetItem* item = new QListWidgetItem(QString::number(block.id) + QString::fromUtf8("　　　") + block.mark);
			item->setData(DataRole::id, block.id);
			ui.block_list->addItem(item);
		}
	}
	// table
	// >>>> new action set here
	void TableUpdate(int index)
	{
		updating = true;
		const Action& var = actions->at(index);
		QString type;
		QString param;
		switch (var.index())
		{
		case QiType::end:
		{
			type = Qi::ui.text.acEnd;
		} break;
		case QiType::delay:
		{
			const QiDelay& ref = std::get<QiDelay>(var);
			type = Qi::ui.text.acWait;

			if (ref.min != ref.max)
			{
				param = QString::number(ref.min);
				param += " ~ ";
				param += QString::number(ref.max);
			}
			else param = QString::number(ref.min);
		} break;
		case QiType::key:
		{
			const QiKey& ref = std::get<QiKey>(var);
			if (ref.state == QiKey::up) type = Qi::ui.text.acUp;
			else if (ref.state == QiKey::down) type = Qi::ui.text.acDown;
			else if (ref.state == QiKey::click) type = Qi::ui.text.acClick;

			param = QKeyEdit::keyName(ref.vk);
		} break;
		case QiType::mouse:
		{
			const QiMouse& ref = std::get<QiMouse>(var);
			if (ref.move) type = Qi::ui.text.acMove;
			else type = Qi::ui.text.acPos;

			param = QString::number(ref.x);
			param += " - ";
			param += QString::number(ref.y);
			if (ref.ex)
			{
				param += "ㅤㅤ随机：";
				param += QString::number(ref.ex);
			}
			if (ref.track)
			{
				param += "ㅤㅤ轨迹：";
				param += QString::number(ref.speed);
			}
		} break;
		case QiType::copyText:
		{
			const QiCopyText& ref = std::get<QiCopyText>(var);
			type = Qi::ui.text.acText;

			param = ref.text.mid(0, 32);
			if (ref.text.size() > 31) param += "...";
		} break;
		case QiType::color:
		{
			const QiColor& ref = std::get<QiColor>(var);
			type = Qi::ui.text.acColor;

			param = "(";
			param += QString::number(ref.rect.left);
			param += ",";
			param += QString::number(ref.rect.top);
			param += ",";
			param += QString::number(ref.rect.right);
			param += ",";
			param += QString::number(ref.rect.bottom);
			param += ")　(";
			param += QString::number(ref.rgbe.r);
			param += ",";
			param += QString::number(ref.rgbe.g);
			param += ",";
			param += QString::number(ref.rgbe.b);
			param += ",";
			param += QString::number(ref.rgbe.a);
			param += ")";
			if (ref.move) param += " 移动";
		} break;
		case QiType::loop:
		{
			const QiLoop& ref = std::get<QiLoop>(var);
			type = Qi::ui.text.acLoop;

			if ((ref.min == 0 && ref.max == 0))
				param = "无限";
			else if (ref.min == ref.max)
				param = QString::number(ref.min);
			else
			{
				param = QString::number(ref.min);
				param += " ~ ";
				param += QString::number(ref.max);
			}
		} break;
		case QiType::loopEnd:
		{
			type = Qi::ui.text.acEndLoop;
		} break;
		case QiType::keyState:
		{
			const QiKeyState& ref = std::get<QiKeyState>(var);
			type = Qi::ui.text.acKeyState;

			param += QKeyEdit::keyName(ref.vk);
		} break;
		case QiType::resetPos:
		{
			type = Qi::ui.text.acResetPos;
		} break;
		case QiType::image:
		{
			const QiImage& ref = std::get<QiImage>(var);
			type = Qi::ui.text.acImage;

			param = "(";
			param += QString::number(ref.rect.left);
			param += ",";
			param += QString::number(ref.rect.top);
			param += ",";
			param += QString::number(ref.rect.right);
			param += ",";
			param += QString::number(ref.rect.bottom);
			param += ")　(";
			param += QString::number(ref.map.width());
			param += "x";
			param += QString::number(ref.map.height());
			param += ")　";
			param += QString::number(ref.sim);
			if (ref.move) param += "移动";
		} break;
		case QiType::popText:
		{
			const QiPopText& ref = std::get<QiPopText>(var);
			type = Qi::ui.text.acPopText;

			std::wstring w;
			param = ref.text;
			param += "　时长：";
			param += QString::number(ref.time);
			if (ref.sync) param += "等待";
		} break;
		case QiType::savePos:
		{
			type = Qi::ui.text.acSavePos;
		} break;
		case QiType::timer:
		{
			const QiTimer& ref = std::get<QiTimer>(var);
			type = Qi::ui.text.acTimer;

			if (ref.min == ref.max) param = QString::number(ref.min);
			else
			{
				param = QString::number(ref.min);
				param += " ~ ";
				param += QString::number(ref.max);
			}
		} break;
		case QiType::jump:
		{
			const QiJump& ref = std::get<QiJump>(var);
			type = Qi::ui.text.acJump;

			if (ref.id < 1)
			{
				param = "指定无效的锚点";
			}
			else
			{
				param = "id：";
				param += QString::number(ref.id);
			}
		} break;
		case QiType::jumpPoint:
		{
			const QiJumpPoint& ref = std::get<QiJumpPoint>(var);
			type = Qi::ui.text.acJumpPoint;

			param = "id：";
			param += QString::number(ref.id);
		} break;
		case QiType::dialog:
		{
			const QiDialog& ref = std::get<QiDialog>(var);
			type = Qi::ui.text.acDialog;

			param = ref.title.mid(0, 8);
			if (ref.title.size() > 7) param += "...";
			param += " | ";
			param += ref.text.mid(0, 16);
			if (ref.text.size() > 15) param += "...";
		} break;
		case QiType::block:
		{
			const QiBlock& ref = std::get<QiBlock>(var);
			type = Qi::ui.text.acBlock;

			param = "id：";
			param += QString::number(ref.id);
		} break;
		case QiType::blockExec:
		{
			const QiBlockExec& ref = std::get<QiBlockExec>(var);
			type = Qi::ui.text.acBlockExec;

			if (ref.id < 1)
			{
				param = "指定无效的块";
			}
			else
			{
				param = "id：";
				param += QString::number(ref.id);
			}
		} break;
		case QiType::quickInput:
		{
			const QiQuickInput& ref = std::get<QiQuickInput>(var);
			type = Qi::ui.text.acQuickInput;

			param = KeyToString(ref.chars);
		} break;
		case QiType::keyBlock:
		{
			const QiKeyBlock& ref = std::get<QiKeyBlock>(var);
			type = Qi::ui.text.acKeyBlock;

			if (ref.block) param = "屏蔽：";
			else param = "解除：";
			if (ref.vk) param += QKeyEdit::keyName(ref.vk);
			else param += "鼠标移动";

		} break;
		case QiType::clock:
		{
			const QiClock& ref = std::get<QiClock>(var);
			type = Qi::ui.text.acClock;

			param = QString::fromStdString(QiTime::toString(ref.time));
		} break;
		case QiType::ocr:
		{
			const QiOcr& ref = std::get<QiOcr>(var);
			type = Qi::ui.text.acOcr;

			param = "(";
			param += QString::number(ref.rect.left);
			param += ",";
			param += QString::number(ref.rect.top);
			param += ",";
			param += QString::number(ref.rect.right);
			param += ",";
			param += QString::number(ref.rect.bottom);
			param += ")　";
			if (ref.match) param += "匹配：";
			else param += "搜索：";
			param += ref.text.mid(0, 16);
			if (ref.text.size() > 15) param += "...";
		} break;
		case QiType::varOperator:
		{
			const QiVarOperator& ref = std::get<QiVarOperator>(var);
			type = Qi::ui.text.acVarOperator;

			param = ref.script.mid(0, 32);
			if (ref.script.size() > 31) param += "...";
		} break;
		case QiType::varCondition:
		{
			const QiVarCondition& ref = std::get<QiVarCondition>(var);
			type = Qi::ui.text.acVarCondition;

			param = ref.script.mid(0, 32);
			if (ref.script.size() > 31) param += "...";
		} break;
		}

		if (type.isEmpty())
		{
			ui.action_table->item(index, tableColumn_type)->setText("无效类型");
		}
		else
		{
			if (var.base().debug_entry) ui.action_table->item(index, tableColumn_debug)->setText(Qi::ui.text.syEntry);
			else
			{
				if (var.base().debug_break) ui.action_table->item(index, tableColumn_debug)->setText(Qi::ui.text.syPause);
				else
				{
					if (var.base().debug_exit) ui.action_table->item(index, tableColumn_debug)->setText(Qi::ui.text.syExit);
					else ui.action_table->item(index, tableColumn_debug)->setText("");
				}
			}

			if (var.base().disable) ui.action_table->item(index, tableColumn_disable)->setText(Qi::ui.text.syOff);
			else ui.action_table->item(index, tableColumn_disable)->setText(Qi::ui.text.syOn);

			ui.action_table->item(index, tableColumn_type)->setText(type);

			ui.action_table->item(index, tableColumn_param)->setText(param);

			ui.action_table->item(index, tableColumn_mark)->setText(var.base().mark);
		}
		updating = false;
	}
	void TableUpdate(int begin, int end)
	{
		for (size_t i = begin; i <= end; i++) TableUpdate(i);
	}
	void TableReload()
	{
		tableCurrent.clear();
		tableCurrentPrev.clear();
		DisableMenus();
		DisableButtons();
		ListJumpPointReload();
		ListBlockReload();
		ui.action_table->setRowCount(actions->size());
		if (ui.action_table->rowCount() > 0)
		{
			for (size_t i = 0; i < ui.action_table->rowCount(); i++)
			{
				updating = true;
				ui.action_table->setItem(i, tableColumn_debug, new QTableWidgetItem());
				ui.action_table->item(i, tableColumn_debug)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.action_table->setItem(i, tableColumn_disable, new QTableWidgetItem());
				ui.action_table->item(i, tableColumn_disable)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.action_table->setItem(i, tableColumn_type, new QTableWidgetItem());
				ui.action_table->item(i, tableColumn_type)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.action_table->setItem(i, tableColumn_param, new QTableWidgetItem());
				ui.action_table->item(i, tableColumn_param)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.action_table->setItem(i, tableColumn_mark, new QTableWidgetItem());
				ui.action_table->item(i, tableColumn_mark)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				TableUpdate(i);
				updating = false;
			}
		}
	}
	void TableInsert(int index)
	{
		ui.action_table->insertRow(index);
		TableUpdate(index);
	}
	void TableRemove(int index)
	{
		ui.action_table->removeRow(index);
	}
	void TableSelection(const QiVector<int> selection)
	{
		for (auto& i : selection) for (size_t c = 0; c < ui.action_table->columnCount(); c++) ui.action_table->setItemSelected(ui.action_table->item(i, c), true);
	}

	// quickinput
	QiVector<unsigned char> StringToKey(const QString str)
	{
		QiVector<unsigned char> keys;
		for (auto& i : str)
		{
			unsigned char c = i.toLatin1();
			unsigned char converted = 0;
			if (InRange(c, '0', '9', 0) || InRange(c, 'A', 'Z', 0)) converted = c;
			else if (InRange(c, 'a', 'z', 0))  converted = toupper(c);
			else
			{
				switch (c)
				{
				case '`': converted = VK_OEM_3; break;
				case '~': converted = VK_OEM_3; break;
				case '-': converted = VK_OEM_MINUS; break;
				case '_': converted = VK_OEM_MINUS; break;
				case '=': converted = VK_OEM_PLUS; break;
				case '+': converted = VK_OEM_PLUS; break;
				case '[': converted = VK_OEM_4; break;
				case '{': converted = VK_OEM_4; break;
				case ']': converted = VK_OEM_6; break;
				case '}': converted = VK_OEM_6; break;
				case ';': converted = VK_OEM_1; break;
				case ':': converted = VK_OEM_1; break;
				case '\'': converted = VK_OEM_7; break;
				case '\"': converted = VK_OEM_7; break;
				case ',': converted = VK_OEM_COMMA; break;
				case '<': converted = VK_OEM_COMMA; break;
				case '.': converted = VK_OEM_PERIOD; break;
				case '>': converted = VK_OEM_PERIOD; break;
				case '/': converted = VK_OEM_2; break;
				case '?': converted = VK_OEM_2; break;
				case '\\': converted = VK_OEM_5; break;
				case '|': converted = VK_OEM_5; break;
				}
			}
			if (converted) keys.append(converted);
		}
		return keys;
	}
	QString KeyToString(const QiVector<unsigned char >& keys)
	{
		QString str;
		for (auto& i : keys)
		{
			unsigned char c = 0;
			if (InRange(i, '0', '9', 0) || InRange(i, 'A', 'Z', 0)) c = i;
			else
			{
				switch (i)
				{
				case VK_OEM_3: c = '`'; break;
				case VK_OEM_MINUS: c = '-'; break;
				case VK_OEM_PLUS: c = '='; break;
				case VK_OEM_4: c = '['; break;
				case VK_OEM_6: c = ']'; break;
				case VK_OEM_1:c = ';'; break;
				case VK_OEM_7: c = '\''; break;
				case VK_OEM_COMMA: c = ','; break;
				case VK_OEM_PERIOD: c = '.'; break;
				case VK_OEM_2: c = '/'; break;
				case VK_OEM_5: c = '\\'; break;
				}
			}
			if (c) str += c;
		}
		return str;
	}
private:
	bool event(QEvent* e)
	{
		if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
		{
			QKeyEvent* keyEvent = (QKeyEvent*)e;
			if ((keyEvent->key() == Qt::Key_Escape) || (keyEvent->key() == Qt::Key_Return) || keyEvent->key() == Qt::Key_Enter || (keyEvent->key() == Qt::Key_Space)) return true;
		}
		return QDialog::event(e);
	}
	bool eventFilter(QObject* obj, QEvent* e)
	{
		if (obj == ui.action_table)
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
						ItemMove(true, 1);
						return true;
					}
					else if (key->key() == Qt::Key_Down)
					{
						ItemMove(false, 1);
						return true;
					}
				}
			}
		}
		else if (obj == ui.action_table->viewport()) // move drop
		{
			if (e->type() == QEvent::Drop)
			{
				QDropEvent* drop = static_cast<QDropEvent*>(e);
				QTableWidgetItem* item = ui.action_table->itemAt(drop->pos());
				int before = 0, after = 0;
				before = ui.action_table->currentRow();
				if (before < 0) return false;
				if (item) after = item->row();
				else after = ui.action_table->rowCount() - 1;
				if (after < 0) return false;

				int len = after - before;
				if (len > 0) ItemMove(false, len);
				else if (len < 0) ItemMove(true, (~len) + 1);

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
	void customEvent(QEvent* e)
	{
		if (e->type() == EditEvent::close) close();
		if (e->type() == EditEvent::debug_pause) SetDebugState(debug_pause);
	}
	// window move
	QPoint mouse_positon;
	bool mouse_down = false;
	void mousePressEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			mouse_positon = e->pos();
			mouse_down = true;
			e->accept();
		}
	}
	void mouseMoveEvent(QMouseEvent* e)
	{
		if (mouse_down)
		{
			if (Distance(mouse_positon.x(), mouse_positon.y(), e->pos().x(), e->pos().y()) < 100)
			{
				move(e->pos() + pos() - mouse_positon);
				e->accept();
			}
		}
	}
	void mouseReleaseEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			mouse_down = false;
			e->accept();
		}
	}
private:
	// >>>> new action set here
	void ItemUse(int p)
	{
		const Action& var = actions->at(p);
		switch (var.index())
		{
		case QiType::delay: { const QiDelay& ref = std::get<QiDelay>(var); WidgetSet(ref); } break;
		case QiType::key: { const QiKey& ref = std::get<QiKey>(var); WidgetSet(ref); } break;
		case QiType::mouse: { const QiMouse& ref = std::get<QiMouse>(var); WidgetSet(ref); } break;
		case QiType::copyText: { const QiCopyText& ref = std::get<QiCopyText>(var); WidgetSet(ref); } break;
		case QiType::color: { const QiColor& ref = std::get<QiColor>(var); WidgetSet(ref); } break;
		case QiType::loop: { const QiLoop& ref = std::get<QiLoop>(var); WidgetSet(ref); } break;
		case QiType::keyState: { const QiKeyState& ref = std::get<QiKeyState>(var); WidgetSet(ref); } break;
		case QiType::image: { const QiImage& ref = std::get<QiImage>(var); WidgetSet(ref); } break;
		case QiType::popText: { const QiPopText& ref = std::get<QiPopText>(var); WidgetSet(ref); } break;
		case QiType::timer: { const QiTimer& ref = std::get<QiTimer>(var); WidgetSet(ref); } break;
		case QiType::dialog: { const QiDialog& ref = std::get<QiDialog>(var); WidgetSet(ref); } break;
		case QiType::quickInput: { const QiQuickInput& ref = std::get<QiQuickInput>(var); WidgetSet(ref); } break;
		case QiType::keyBlock: { const QiKeyBlock& ref = std::get<QiKeyBlock>(var); WidgetSet(ref); } break;
		case QiType::clock: { const QiClock& ref = std::get<QiClock>(var); WidgetSet(ref); } break;
		case QiType::ocr: { const QiOcr& ref = std::get<QiOcr>(var); WidgetSet(ref); } break;
		case QiType::varOperator: { const QiVarOperator& ref = std::get<QiVarOperator>(var); WidgetSet(ref); } break;
		case QiType::varCondition: { const QiVarCondition& ref = std::get<QiVarCondition>(var); WidgetSet(ref); } break;
		}
	}
	// >>>> new action set here
	Action ItemGet(int type)
	{
		Action action;
		switch (type)
		{
		case QiType::end: action = QiEnd(); break;
		case QiType::delay: action = WidgetGetDelay(); break;
		case QiType::key: action = WidgetGetKey(); break;
		case QiType::mouse: action = WidgetGetMouse(); break;
		case QiType::copyText: action = WidgetGetText(); break;
		case QiType::color: action = WidgetGetColor(); break;
		case QiType::loop: action = WidgetGetLoop(); break;
		case QiType::loopEnd: action = QiLoopEnd(); break;
		case QiType::keyState: action = WidgetGetKeyState(); break;
		case QiType::resetPos: action = QiResetPos(); break;
		case QiType::image: action = WidgetGetImage(); break;
		case QiType::popText: action = WidgetGetPopText(); break;
		case QiType::savePos: action = QiSavePos(); break;
		case QiType::timer: action = WidgetGetTimer(); break;
		case QiType::jump: action = WidgetGetJump(); break;
		case QiType::jumpPoint: action = WidgetGetJumpPoint(); break;
		case QiType::dialog: action = WidgetGetDialog(); break;
		case QiType::block: action = WidgetGetBlock(); break;
		case QiType::blockExec: action = WidgetGetBlockExec(); break;
		case QiType::quickInput: action = WidgetGetQuickInput(); break;
		case QiType::keyBlock: action = WidgetGetKeyBlock(); break;
		case QiType::clock: action = WidgetGetClock(); break;
		case QiType::ocr: action = WidgetGetOcr(); break;
		case QiType::varOperator: action = WidgetGetVarOperator(); break;
		case QiType::varCondition: action = WidgetGetVarCondition(); break;
		}
		return action;
	}
	void ItemMove(bool up, int len)
	{
		int count = ui.action_table->rowCount();
		if (count < 1) return;
		if (tableCurrent.not_empty())
		{
			QiVector<int> after;
			for (int i = 0, p = 0, t = 0, over = 0; i < tableCurrent.size(); i++)
			{
				if (up)
				{
					over = 0 - (tableCurrent.front() - len);
					if (over < 0) over = 0;
					p = tableCurrent[i];
					t = p - len + over;
					after.append(t);
					actions->move(p, t);
				}
				else
				{
					over = count - (tableCurrent.back() + len + 1);
					if (over > 0) over = 0;
					p = tableCurrent[tableCurrent.size() - i - 1];
					t = p + len + over;
					after.append(t);
					actions->move(p, t);
				}
			}
			TableReload();
			ui.action_table->clearSelection();
			TableSelection(after);
		}
	}
	void ItemAdd(int type)
	{
		int p = ui.action_table->currentRow();
		if (p < 0) p = actions->size();
		else p++;
		actions->insert(p, ItemGet(type));
		TableReload();
		ui.action_table->setCurrentItem(ui.action_table->item(p, tableColumn_type));
	}
	void ItemChange(int type)
	{
		for (auto& i : tableCurrent)
		{
			Action& current = actions->at(i);
			QiBase base_old(std::move(current.base()));
			current = std::move(ItemGet(type));
			QiBase& base_new = current.base();
			base_new.disable = std::move(base_old.disable);
			base_new.mark = std::move(base_old.mark);
			base_new.next = std::move(base_old.next);
			base_new.next2 = std::move(base_old.next2);
			TableReload();
		}
		ui.action_table->clearSelection();
	}
	void ItemDel()
	{
		if (tableCurrent.empty()) return;
		std::vector<size_t> positions;
		for (auto& i : tableCurrent) positions.push_back(i);
		actions->remove(positions);
		TableReload();
		ui.action_table->setCurrentItem(0);
	}
	void ItemCut()
	{
		ItemCopy();
		ItemDel();
	}
	void ItemCopy()
	{
		if (tableCurrent.empty()) return;
		Qi::clipboard.clear();
		for (auto& i : tableCurrent) Qi::clipboard.append(actions->at(i));
	}
	void ItemPaste()
	{
		int p = ui.action_table->currentRow();
		if (p < 0) p = actions->size();
		else p++;
		// reset jump id
		UniqueActionsId(LoadIds(*actionsRoot, QiType::jumpPoint), Qi::clipboard, QiType::jumpPoint);
		// reset block id
		UniqueActionsId(LoadIds(*actionsRoot, QiType::block), Qi::clipboard, QiType::block);
		actions->insert(p, Qi::clipboard);
		TableReload();
	}
	// get params from widget
	QiKey WidgetGetKey() {
		QiKey key;
		if (ui.key_down_radio->isChecked()) key.state = QiKey::down;
		else if (ui.key_up_radio->isChecked()) key.state = QiKey::up;
		else if (ui.key_click_radio->isChecked()) key.state = QiKey::click;
		key.vk = ui.key_keyedit->key();
		return key;
	}
	QiKeyState WidgetGetKeyState() {
		QiKeyState keyState;
		keyState.vk = ui.keyState_keyedit->key();
		return keyState;
	}
	QiMouse WidgetGetMouse() {
		QiMouse mouse;
		mouse.move = ui.mouse_move_radio->isChecked();
		mouse.track = ui.mouse_track_check->isChecked();
		int x = ui.mouse_x_edit->text().toInt();
		int y = ui.mouse_y_edit->text().toInt();
		int r = ui.mouse_rand_edit->text().toInt();
		int s = 50;
		if (ui.mouse_speed_edit->text() != "") s = ui.mouse_speed_edit->text().toInt();
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
		if (ui.delay_min_edit->text() != "") mn = ui.delay_min_edit->text().toInt();
		int mx = ui.delay_max_edit->text().toInt();
		if (mn > delayMax) mn = delayMax;
		if (mx > delayMax) mx = delayMax;
		if (mx < mn) mx = mn;
		delay.min = mn;
		delay.max = mx;
		return delay;
	}
	QiCopyText WidgetGetText() {
		QiCopyText text;
		text.text = ui.copyText_text->toPlainText();
		return text;
	}
	QiColor WidgetGetColor() {
		QiColor color;
		color.move = ui.color_move_check->isChecked();
		{
			int l = ui.color_left_edit->text().toInt();
			int t = ui.color_top_edit->text().toInt();
			int r = ui.color_right_edit->text().toInt();
			int b = ui.color_bottom_edit->text().toInt();
			if (l > posMax) l = posMax;
			if (t > posMax) t = posMax;
			if (r > posMax) r = posMax;
			if (b > posMax) b = posMax;
			color.rect = { l, t, r, b };
		}
		{
			int r = ui.color_red_edit->text().toInt();
			int g = ui.color_green_edit->text().toInt();
			int b = ui.color_blue_edit->text().toInt();
			int e = 5;
			if (ui.color_sim_edit->text() != "") e = ui.color_sim_edit->text().toInt();
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
		if (ui.loop_min_edit->text() != "") mn = ui.loop_min_edit->text().toInt();
		int mx = ui.loop_max_edit->text().toInt();
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
		image.move = ui.image_move_check->isChecked();
		{
			int l = ui.image_left_edit->text().toInt();
			int t = ui.image_top_edit->text().toInt();
			int r = ui.image_right_edit->text().toInt();
			int b = ui.image_bottom_edit->text().toInt();
			if (l > posMax) l = posMax;
			if (t > posMax) t = posMax;
			if (r > posMax) r = posMax;
			if (b > posMax) b = posMax;
			image.rect = { l, t, r, b };
		}
		{
			int s = 80;
			if (ui.image_sim_edit->text() != "") s = ui.image_sim_edit->text().toInt();
			if (s > imageSimMax) s = imageSimMax;
			image.sim = s;
		}
		image.map = imageMap;
		return image;
	}
	QiPopText WidgetGetPopText()
	{
		QiPopText popText;
		popText.text = ui.popText_text_edit->text();
		int time = 1000;
		if (ui.popText_time_edit->text() != "") time = ui.popText_time_edit->text().toInt();
		if (time > popTextTimeMax) time = popTextTimeMax;
		popText.time = time;
		popText.sync = ui.popText_wait_check->isChecked();
		return popText;
	}
	QiTimer WidgetGetTimer()
	{
		QiTimer timer;
		int mn = 1;
		if (ui.timer_min_edit->text() != "") mn = ui.timer_min_edit->text().toInt();
		int mx = ui.timer_max_edit->text().toInt();
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
		int row = ui.jumpPoint_list->currentRow();
		if (row > -1) jump.id = ui.jumpPoint_list->item(row)->data(DataRole::id).toInt();
		return jump;
	}
	QiJumpPoint WidgetGetJumpPoint()
	{
		QiJumpPoint jumpPoint;
		QiVector<int> ids = LoadIds(*actionsRoot, QiType::jumpPoint);
		jumpPoint.id = UniqueId(ids);
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
		QiVector<int> ids = LoadIds(*actionsRoot, QiType::block);
		block.id = UniqueId(ids);
		return block;
	}
	QiBlockExec WidgetGetBlockExec()
	{
		QiBlockExec blockExec;
		int row = ui.block_list->currentRow();
		if (row > -1) blockExec.id = ui.block_list->item(row)->data(DataRole::id).toInt();
		return blockExec;
	}
	QiQuickInput WidgetGetQuickInput()
	{
		QiQuickInput quickInput;
		quickInput.chars = StringToKey(ui.quickInput_text_edit->text());
		return quickInput;
	}
	QiKeyBlock WidgetGetKeyBlock()
	{
		QiKeyBlock keyBlock;
		if (ui.keyBlock_move_check->isChecked()) keyBlock.vk = 0;
		else keyBlock.vk = ui.keyBlock_keyedit->key();
		keyBlock.block = ui.keyBlock_on_radio->isChecked();
		return keyBlock;
	}
	QiClock WidgetGetClock()
	{
		QiClock clock;
		QTime time = ui.clock_time_edit->time();
		clock.time = QiTime::toTimeStamp(time.hour(), time.minute(), time.second());
		return clock;
	}
	QiOcr WidgetGetOcr()
	{
		QiOcr ocr;
		int l = ui.ocr_left_edit->text().toInt();
		int t = ui.ocr_top_edit->text().toInt();
		int r = ui.ocr_right_edit->text().toInt();
		int b = ui.ocr_bottom_edit->text().toInt();
		if (l > posMax) l = posMax;
		if (t > posMax) t = posMax;
		if (r > posMax) r = posMax;
		if (b > posMax) b = posMax;
		ocr.rect = { l, t, r, b };

		ocr.text = ui.ocr_text_edit->text();
		ocr.var = ui.ocr_var_edit->text();
		ocr.match = ui.ocr_match_check->isChecked();
		return ocr;
	}
	QiVarOperator WidgetGetVarOperator()
	{
		QiVarOperator varOperator;
		varOperator.script = ui.varOperator_edit->toPlainText();
		return varOperator;
	}
	QiVarCondition WidgetGetVarCondition()
	{
		QiVarCondition varCondition;
		varCondition.script = ui.varCondition_edit->text();
		return varCondition;
	}
	// load params to widget
	void WidgetSet(const QiKey& key)
	{
		if (key.state == QiKey::down) ui.key_down_radio->setChecked(true);
		else if (key.state == QiKey::up) ui.key_up_radio->setChecked(true);
		else if (key.state == QiKey::click) ui.key_click_radio->setChecked(true);
		ui.key_keyedit->setKey(key.vk);
	}
	void WidgetSet(const QiKeyState& keyState)
	{
		ui.keyState_keyedit->setKey(keyState.vk);
	}
	void WidgetSet(const QiMouse& mouse)
	{
		mouse.move ? ui.mouse_move_radio->setChecked(true) : ui.mouse_position_radio->setChecked(true);
		ui.mouse_track_check->setChecked(mouse.track);
		ui.mouse_speed_edit->setText(QString::number(mouse.speed));
		ui.mouse_x_edit->setText(QString::number(mouse.x)); ui.mouse_y_edit->setText(QString::number(mouse.y)); if (mouse.ex > -1) ui.mouse_rand_edit->setText(QString::number(mouse.ex));
	}
	void WidgetSet(const QiDelay& delay)
	{
		ui.delay_min_edit->setText(QString::number(delay.min));
		ui.delay_max_edit->setText(QString::number(delay.max));
	}
	void WidgetSet(const QiCopyText& text)
	{
		ui.copyText_text->setPlainText(text.text);
	}
	void WidgetSet(const QiColor& color) {
		ui.color_move_check->setChecked(color.move);
		ui.color_left_edit->setText(QString::number(color.rect.left));
		ui.color_top_edit->setText(QString::number(color.rect.top));
		ui.color_right_edit->setText(QString::number(color.rect.right));
		ui.color_bottom_edit->setText(QString::number(color.rect.bottom));
		ui.color_sim_edit->setText(QString::number(color.rgbe.a));
		ui.color_red_edit->setText(QString::number(color.rgbe.r));
		ui.color_green_edit->setText(QString::number(color.rgbe.g));
		ui.color_blue_edit->setText(QString::number(color.rgbe.b));
		QString style = "background-color:rgb(";
		style += QString::number(color.rgbe.r);
		style += ",";
		style += QString::number(color.rgbe.g);
		style += ",";
		style += QString::number(color.rgbe.b);
		style += ");";
		ui.color_rgb_button->setStyleSheet(style);
	}
	void WidgetSet(const QiLoop& loop)
	{
		ui.loop_min_edit->setText(QString::number(loop.min));
		ui.loop_max_edit->setText(QString::number(loop.max));
	}
	void WidgetSet(const QiImage& image) {
		ui.image_left_edit->setText(QString::number(image.rect.left));
		ui.image_top_edit->setText(QString::number(image.rect.top));
		ui.image_right_edit->setText(QString::number(image.rect.right));
		ui.image_bottom_edit->setText(QString::number(image.rect.bottom));
		ui.image_sim_edit->setText(QString::number(image.sim));
		ui.image_move_check->setChecked(image.move);
		imageMap = image.map;
		HBITMAP hbmp = Image::toBmp32(imageMap);
		if (hbmp) ui.image_view_label->setPixmap(QtWin::fromHBITMAP(hbmp));
	}
	void WidgetSet(const QiPopText& popText) {
		ui.popText_text_edit->setText(popText.text);
		ui.popText_time_edit->setText(QString::number(popText.time));
		ui.popText_wait_check->setChecked(popText.sync);
	}
	void WidgetSet(const QiTimer& timer)
	{
		ui.timer_min_edit->setText(QString::number(timer.min));
		ui.timer_max_edit->setText(QString::number(timer.max));
	}
	void WidgetSet(const QiDialog& dialog)
	{
		if (dialog.style == QiDialog::none) ui.dialog_style_none_radio->setChecked(true);
		if (dialog.style == QiDialog::warning) ui.dialog_style_warning_radio->setChecked(true);
		if (dialog.style == QiDialog::error) ui.dialog_style_error_radio->setChecked(true);
		ui.dialog_title_edit->setText(dialog.title);
		ui.dialog_text_edit->setText(dialog.text);
	}
	void WidgetSet(const QiQuickInput& quickInput)
	{
		ui.quickInput_text_edit->setText(KeyToString(quickInput.chars));
	}
	void WidgetSet(const QiKeyBlock& keyBlock)
	{
		if (keyBlock.vk) ui.keyBlock_keyedit->setKey(keyBlock.vk);
		ui.keyBlock_move_check->setChecked(!keyBlock.vk);
		keyBlock.block ? ui.keyBlock_on_radio->setChecked(true) : ui.keyBlock_off_radio->setChecked(true);
	}
	void WidgetSet(const QiClock& clock)
	{
		ui.clock_time_edit->setTime(QTime(QiTime::h(clock.time), QiTime::m(clock.time), QiTime::s(clock.time)));
	}
	void WidgetSet(const QiOcr& ocr)
	{
		ui.ocr_left_edit->setText(QString::number(ocr.rect.left));
		ui.ocr_top_edit->setText(QString::number(ocr.rect.top));
		ui.ocr_right_edit->setText(QString::number(ocr.rect.right));
		ui.ocr_bottom_edit->setText(QString::number(ocr.rect.bottom));
		ui.ocr_text_edit->setText(ocr.text);
		ui.ocr_var_edit->setText(ocr.var);
		ui.ocr_match_check->setChecked(ocr.match);
	}
	void WidgetSet(const QiVarOperator& varOperator)
	{
		ui.varOperator_edit->setPlainText(varOperator.script);
	}
	void WidgetSet(const QiVarCondition& varCondition)
	{
		ui.varCondition_edit->setText(varCondition.script);
	}
};