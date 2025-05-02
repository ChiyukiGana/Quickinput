#include <EditUi.h>
DefWindowMove(EditUi) EditUi::EditUi(Macro* macro, Actions* actions)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);

	// loading tip
	Qi::popText->Hide();

	// set as top layer
	this->macro = macro;
	this->actionsRoot = this->actions = actions;
	layers.append(Layer(QString::fromUtf8("编辑 - ") + macro->name, this->actionsRoot));

	Init();
	Event();
	StyleGroup();
	Reload();
}


void EditUi::Init()
{
	Init_Bind();

	if ("table")
	{
		ui.action_table->setColumnCount(tableColumn_count);
		ui.action_table->setContextMenuPolicy(Qt::CustomContextMenu);
		ui.action_table->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
		ui.action_table->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
		ui.action_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.action_table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.action_table->verticalHeader()->setDefaultSectionSize(0);
		ui.action_table->setHorizontalHeaderItem(tableColumn_debug, new QTableWidgetItem("调试"));
		ui.action_table->setHorizontalHeaderItem(tableColumn_disable, new QTableWidgetItem("调试"));
		ui.action_table->setHorizontalHeaderItem(tableColumn_type, new QTableWidgetItem("功能"));
		ui.action_table->setHorizontalHeaderItem(tableColumn_param, new QTableWidgetItem("参数"));
		ui.action_table->setHorizontalHeaderItem(tableColumn_mark, new QTableWidgetItem("备注"));
		ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_debug, QHeaderView::ResizeMode::Fixed);
		ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_disable, QHeaderView::ResizeMode::Fixed);
		ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_type, QHeaderView::ResizeMode::Fixed);
		ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_param, QHeaderView::ResizeMode::Interactive);
		ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_mark, QHeaderView::ResizeMode::Interactive);
		ui.action_table->setColumnWidth(tableColumn_debug, 35);
		ui.action_table->setColumnWidth(tableColumn_disable, 50);
		ui.action_table->setColumnWidth(tableColumn_type, 100);
		ui.action_table->setColumnWidth(tableColumn_param, 300);
		ui.action_table->setColumnWidth(tableColumn_mark, 200);
	}
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
		if ("clear shortcut")
		{
			// action add
			BindSafeIter(bind_add_button, [this](QPushButton* p, size_t i) { p->installEventFilter(this); });
			BindSafeIter(bind_chg_button, [this](QPushButton* p, size_t i) { p->installEventFilter(this); });
			BindSafeIter(bind_edt_button, [this](QPushButton* p, size_t i) { p->installEventFilter(this); });
			BindSafeIter(bind_edt2_button, [this](QPushButton* p, size_t i) { p->installEventFilter(this); });

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

		if ("move")
		{
			ui.mouse_move_button->setText("×");
			ui.mouse_move_button->setDisabled(true);
		}
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

		QButtonGroup* soundPlayRbs = new QButtonGroup(this);
		soundPlayRbs->addButton(ui.soundPlay_play_radio);
		soundPlayRbs->addButton(ui.soundPlay_pause_radio);
		soundPlayRbs->addButton(ui.soundPlay_resume_radio);
		soundPlayRbs->addButton(ui.soundPlay_stop_radio);
		ui.soundPlay_play_radio->setChecked(true);
	}
	if ("file edit")
	{
		ui.soundPlay_edit->setDialog("选择音频", "音频文件(*.mp3 *.wav)", "C:/Windows/Media");
		ui.soundPlay_edit->setFiles(Qi::ui.sounds);
	}
	if ("line edit range")
	{
		ui.mouse_x_edit->setValidator(new QIntValidator(QiMouse::range_pos.first, QiMouse::range_pos.second, this));
		ui.mouse_y_edit->setValidator(new QIntValidator(QiMouse::range_pos.first, QiMouse::range_pos.second, this));
		ui.mouse_speed_edit->setValidator(new QIntValidator(QiMouse::range_speed.first, QiMouse::range_speed.second, this));
		ui.mouse_rand_edit->setValidator(new QIntValidator(QiMouse::range_rand.first, QiMouse::range_rand.second, this));
		//ui.delay_min_edit->setValidator(new QRegExpValidator(QRegExp(QiIntVarRegex), this));
		//ui.delay_max_edit->setValidator(new QRegExpValidator(QRegExp(QiIntVarRegex), this));
		//ui.loop_min_edit->setValidator(new QRegExpValidator(QRegExp(QiIntVarRegex), this));
		//ui.loop_max_edit->setValidator(new QRegExpValidator(QRegExp(QiIntVarRegex), this));
		//ui.timer_max_edit->setValidator(new QRegExpValidator(QRegExp(QiIntVarRegex), this));
		//ui.timer_min_edit->setValidator(new QRegExpValidator(QRegExp(QiIntVarRegex), this));
		ui.color_red_edit->setValidator(new QIntValidator(QiColor::range_rgb.first, QiColor::range_rgb.second, this));
		ui.color_green_edit->setValidator(new QIntValidator(QiColor::range_rgb.first, QiColor::range_rgb.second, this));
		ui.color_blue_edit->setValidator(new QIntValidator(QiColor::range_rgb.first, QiColor::range_rgb.second, this));
		ui.color_sim_edit->setValidator(new QIntValidator(QiColor::range_rgb.first, QiColor::range_rgb.second, this));
		ui.color_left_edit->setValidator(new QIntValidator(QiColor::range_rect.first, QiColor::range_rect.second, this));
		ui.color_top_edit->setValidator(new QIntValidator(QiColor::range_rect.first, QiColor::range_rect.second, this));
		ui.color_right_edit->setValidator(new QIntValidator(QiColor::range_rect.first, QiColor::range_rect.second, this));
		ui.color_bottom_edit->setValidator(new QIntValidator(QiColor::range_rect.first, QiColor::range_rect.second, this));
		ui.image_left_edit->setValidator(new QIntValidator(QiImage::range_rect.first, QiImage::range_rect.second, this));
		ui.image_top_edit->setValidator(new QIntValidator(QiImage::range_rect.first, QiImage::range_rect.second, this));
		ui.image_right_edit->setValidator(new QIntValidator(QiImage::range_rect.first, QiImage::range_rect.second, this));
		ui.image_bottom_edit->setValidator(new QIntValidator(QiImage::range_rect.first, QiImage::range_rect.second, this));
		ui.image_sim_edit->setValidator(new QIntValidator(QiImage::range_sim.first, QiImage::range_sim.second, this));
		ui.popText_time_edit->setValidator(new QIntValidator(QiPopText::range_time.first, QiPopText::range_time.second, this));
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
		DisableTip(Qi::set.tabHideTip);
	}
	// enable qlable scale
	ui.image_view_label->setScaledContents(true);
	// load Window mode
	if (macro->wndState) macro->wndInfo.update(), SetWindowMode();
}
void EditUi::Init_Bind()
{
	if ("group of type")
	{
		bind_type_group.at(QiType::none) = nullptr;
		bind_type_group.at(QiType::end) = ui.grp_end;
		bind_type_group.at(QiType::delay) = ui.grp_delay;
		bind_type_group.at(QiType::key) = ui.grp_key;
		bind_type_group.at(QiType::mouse) = ui.grp_mouse;
		bind_type_group.at(QiType::copyText) = ui.grp_copyText;
		bind_type_group.at(QiType::color) = ui.grp_color;
		bind_type_group.at(QiType::loop) = ui.grp_loop;
		bind_type_group.at(QiType::loopEnd) = ui.grp_loopEnd;
		bind_type_group.at(QiType::keyState) = ui.grp_keyState;
		bind_type_group.at(QiType::resetPos) = ui.grp_resetPos;
		bind_type_group.at(QiType::image) = ui.grp_image;
		bind_type_group.at(QiType::popText) = ui.grp_popText;
		bind_type_group.at(QiType::savePos) = ui.grp_savePos;
		bind_type_group.at(QiType::timer) = ui.grp_timer;
		bind_type_group.at(QiType::jump) = ui.grp_jump;
		bind_type_group.at(QiType::jumpPoint) = ui.grp_jumpPoint;
		bind_type_group.at(QiType::dialog) = ui.grp_dialog;
		bind_type_group.at(QiType::block) = ui.grp_block;
		bind_type_group.at(QiType::blockExec) = ui.grp_blockExec;
		bind_type_group.at(QiType::quickInput) = ui.grp_quickInput;
		bind_type_group.at(QiType::keyBlock) = ui.grp_keyBlock;
		bind_type_group.at(QiType::clock) = ui.grp_clock;
		bind_type_group.at(QiType::ocr) = ui.grp_ocr;
		bind_type_group.at(QiType::varOperator) = ui.grp_varOperator;
		bind_type_group.at(QiType::varCondition) = ui.grp_varCondition;
		bind_type_group.at(QiType::mouseTrack) = nullptr;
		bind_type_group.at(QiType::open) = ui.grp_open;
		bind_type_group.at(QiType::textPad) = ui.grp_textPad;
		bind_type_group.at(QiType::editDialog) = ui.grp_editDialog;
		bind_type_group.at(QiType::volume) = ui.grp_volume;
		bind_type_group.at(QiType::soundPlay) = ui.grp_soundPlay;
	}
	if ("buttons")
	{
		BindSafeIter(bind_type_group, [this](QGroupBox* p, size_t i)
			{
				for (auto& object : p->children())
				{
					if (object->objectName().indexOf("_add_button") != -1) bind_add_button.at(i) = (QPushButton*)object;
					else if (object->objectName().indexOf("_change_button") != -1) bind_chg_button.at(i) = (QPushButton*)object;
					else if (object->objectName().indexOf("_edit_button") != -1) bind_edt_button.at(i) = (QPushButton*)object;
					else if (object->objectName().indexOf("_edit2_button") != -1) bind_edt2_button.at(i) = (QPushButton*)object;
				}
			});
	}
	if ("group of tab index")
	{
		BindSafeIter(bind_type_group, [this](QGroupBox* p, size_t i)
			{
				auto& tab = bind_type_tab[i];
				for (size_t si = 0; si < ui.tab_stacked_widget->count(); si++)
				{
					if (ui.tab_stacked_widget->widget(si)->children().indexOf(p) > -1)
					{
						tab = si;
						break;
					}
				}
			});
	}
	if ("button of tab index")
	{
		bind_tab_button.at(TabIndex::tab_base) = ui.tab_base_button;
		bind_tab_button.at(TabIndex::tab_loop) = ui.tab_loop_button;
		bind_tab_button.at(TabIndex::tab_state) = ui.tab_state_button;
		bind_tab_button.at(TabIndex::tab_time) = ui.tab_time_button;
		bind_tab_button.at(TabIndex::tab_jump) = ui.tab_jump_button;
		bind_tab_button.at(TabIndex::tab_block) = ui.tab_block_button;
		bind_tab_button.at(TabIndex::tab_text) = ui.tab_text_button;
		bind_tab_button.at(TabIndex::tab_image) = ui.tab_image_button;
		bind_tab_button.at(TabIndex::tab_ocr) = ui.tab_ocr_button;
		bind_tab_button.at(TabIndex::tab_dialog) = ui.tab_dialog_button;
		bind_tab_button.at(TabIndex::tab_file) = ui.tab_file_button;
		bind_tab_button.at(TabIndex::tab_var) = ui.tab_var_button;
		bind_tab_button.at(TabIndex::tab_window) = ui.tab_window_button;
	}
}

void EditUi::Event()
{
	Event_Action_Widget();
	Event_Table_Selection();

	if ("title")
	{
		connect(ui.title_close_button, &QPushButton::clicked, this, [this] { Qi::popText->Show("正在保存宏"); Qi::widget.editClose(); });
		connect(ui.title_back_button, &QPushButton::clicked, this, [this] {
			layers.removeLast();
			if (layers.empty())
			{
				Qi::popText->Show("正在保存宏");
				Qi::widget.editClose();
			}
			else
			{
				actions = layers.last().second;
				setWindowTitle(layers.last().first);
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
				Qi::debug = true;
				timeBeginPeriod(1);
				if (ui.action_running_radio->isChecked()) QiThread::StartMacroRun(macro);
				else QiThread::StartMacroEnd(macro);
				testTimer->start(16);
			}
			SetDebugState(debug_run);
			});
		connect(ui.title_var_button, &QPushButton::clicked, this, [this] { Qi::widget.varView->show(); });
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
	if ("tab index button")
	{
		BindSafeIter(bind_tab_button, [this](QPushButton* p, size_t i) { connect(p, &QPushButton::clicked, this, [this, i] { SetTabCurrent(i); }); });
	}
	if ("action table")
	{
		// hotkey
		ui.action_table->installEventFilter(this);
		// drag
		ui.action_table->viewport()->installEventFilter(this);
		connect(ui.action_running_radio, &QRadioButton::toggled, this, [this](bool state) {
			if (state && (layers.size() == 1))
			{
				layers.first().second = actionsRoot = actions = &macro->acRun;
				TableReload();
			}
			});
		connect(ui.action_ending_radio, &QRadioButton::toggled, this, [this](bool state) {
			if (state && (layers.size() == 1))
			{
				layers.first().second = actionsRoot = actions = &macro->acEnd;
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
		// value
		connect(ui.action_table, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) { if (row < 0) return; ItemSet(row); });
		// context menu
		connect(ui.action_table, &QTableWidget::customContextMenuRequested, this, [this] {
			if ("set menu item state")
			{
				muPaste->setEnabled(Qi::clipboard.not_empty());
				if (!tableCurrent.empty())
				{
					muDel->setEnabled(true);
					muCut->setEnabled(true);
					muCopy->setEnabled(true);
					if (tableCurrent.size() == 1)
					{
						const Action& var = actions->at(tableCurrent.front());
						muEdit->setEnabled(bind_edt_button.at(var.index()));
						muEdit2->setEnabled(bind_edt2_button.at(var.index()));
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
					Qi::debug = false;
					Sleep(300);
					SetDebugState(debug_idel);
				}
			}
		);
	}
}
// TODO: new widgets
void EditUi::Event_Action_Widget()
{
	// buttons
	BindSafeIter(bind_add_button, [this](QPushButton* p, size_t i) { connect(bind_add_button[i], &QPushButton::clicked, this, [this, i] { ItemAdd(i); }); });
	BindSafeIter(bind_chg_button, [this](QPushButton* p, size_t i) { connect(bind_chg_button[i], &QPushButton::clicked, this, [this, i] { ItemChange(i); }); });
	BindSafeIter(bind_edt_button, [this](QPushButton* p, size_t i) { connect(bind_edt_button[i], &QPushButton::clicked, this, [this, i] { NextEdit(false); }); });
	BindSafeIter(bind_edt2_button, [this](QPushButton* p, size_t i) { connect(bind_edt2_button[i], &QPushButton::clicked, this, [this, i] { NextEdit(true); }); });
	// check
	connect(ui.tab_lock_check, &QCheckBox::toggled, this, [this](bool state) {
		Qi::set.tabLock = state;
		QiJson::SaveJson();
		});
	connect(ui.tab_hideTip_check, &QCheckBox::toggled, this, [this](bool state) { DisableTip(Qi::set.tabHideTip = state); QiJson::SaveJson(); });
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
			pt = QiFn::P_WRTA({ pt.x, pt.y }, macro->wndInfo.wnd);
		}
		else
		{
			pt = ps.Start();
			pt = QiFn::P_SRTA({ pt.x, pt.y });
		}
		QiMouse mouse(WidgetGetMouse());
		mouse.x = pt.x;
		mouse.y = pt.y;
		WidgetSet(mouse);
		});
	connect(ui.mouse_move_button, &QPushButton::clicked, this, [this] {
		QDistanceSelector ds;
		RECT rect;
		if (macro->wndState)
		{
			if (!macro->wndInfo.update())
			{
				SelectWindow();
			}
			RECT wrect = Window::rect(macro->wndInfo.wnd);
			rect = ds.Start();
		}
		else
		{
			rect = ds.Start();
		}
		QiMouse mouse(WidgetGetMouse());
		mouse.x = rect.right - rect.left;
		mouse.y = rect.bottom - rect.top;
		WidgetSet(mouse);
		});
	connect(ui.mouse_position_radio, &QRadioButton::toggled, this, [this](bool state) {
		if (state)
		{
			ui.mouse_x_edit->setValidator(new QIntValidator(QiMouse::range_pos.first, QiMouse::range_pos.second, this));
			ui.mouse_y_edit->setValidator(new QIntValidator(QiMouse::range_pos.first, QiMouse::range_pos.second, this));
			if (ui.mouse_x_edit->text().toInt() < 0) ui.mouse_x_edit->setText("0");
			if (ui.mouse_y_edit->text().toInt() < 0) ui.mouse_y_edit->setText("0");
			ui.mouse_position_button->setText("+");
			ui.mouse_position_button->setEnabled(true);
			ui.mouse_move_button->setText("×");
			ui.mouse_move_button->setDisabled(true);
		}
		});
	connect(ui.mouse_move_radio, &QRadioButton::toggled, this, [this](bool state) {
		if (state)
		{
			ui.mouse_x_edit->setValidator(new QIntValidator(QiMouse::range_move.first, QiMouse::range_move.second, this));
			ui.mouse_y_edit->setValidator(new QIntValidator(QiMouse::range_move.first, QiMouse::range_move.second, this));
			ui.mouse_position_button->setText("×");
			ui.mouse_position_button->setDisabled(true);
			ui.mouse_move_button->setText("+");
			ui.mouse_move_button->setEnabled(true);
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
			rect = QiFn::R_WRTA(rect, macro->wndInfo.wnd);
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
			rect = QiFn::R_SRTA(rect);
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
		cd.setStyleSheet(QiUi::color_dialog_style);
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
			rect = QiFn::R_WRTA(rect, macro->wndInfo.wnd);
		}
		else
		{
			rect = rs.Start();
			rect = QiFn::R_SRTA(rect);
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
		RECT before = QiFn::R_SATR(image.rect);
		if ((!image.rect.left && !image.rect.top && !image.rect.right && !image.rect.bottom) || rect.left < before.left || rect.top < before.top || rect.right > before.right || rect.bottom > before.bottom) image.rect = QiFn::R_SRTA(rect);
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
			rect = QiFn::R_WRTA(rect, macro->wndInfo.wnd);
		}
		else
		{
			rect = rs.Start();
			rect = QiFn::R_SRTA(rect);
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
		const QiOcr& ocr = WidgetGetOcr();
		std::string text = Qi::ocr->scan(QiFn::R_SATR(ocr.rect), ocr.row);
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
	// textPad
	connect(ui.textPad_button, &QPushButton::clicked, this, [this] {
		QTextDialog td(true);
		ui.textPad_edit->setText(td.Start(ui.textPad_edit->text()));
		});
}
// TODO: new action's preview
void EditUi::Event_Table_Selection()
{
	connect(ui.action_table, &QTableWidget::itemSelectionChanged, this, [this] {
		tableCurrentPrev = tableCurrent;
		tableCurrent.clear();
		// short selection
		QList<QTableWidgetItem*> items = ui.action_table->selectedItems();
		for (auto& i : items) if (i->column() == 0) tableCurrent.append(i->row());
		tableCurrent.sort([](const int& v1, const int& v2) { return v1 < v2; });

		DisableMenus();
		DisableEditButtons(true);
		DisableChangeButtons(tableCurrent.empty());
		widget_pv.hide();
		widget_rv.hide();
		widget_td.hide();

		if (tableCurrent.size() == 1)
		{
			const Action& var = actions->at(tableCurrent.front());

			if (!ui.tab_lock_check->isChecked()) SetTabCurrent(bind_type_tab.at(var.index()));

			auto& edit = bind_edt_button.at(var.index());
			auto& edit2 = bind_edt2_button.at(var.index());
			if (edit) edit->setEnabled(true);
			if (edit2) edit2->setEnabled(true);

			switch (var.index())
			{
			case QiType::mouse:
			{
				const QiMouse& ref = std::get<QiMouse>(var);
				if (!ref.move)
				{
					POINT pt;
					if (macro->wndState)
					{
						if (!macro->wndInfo.update()) SelectWindow();
						POINT rpt = QiFn::P_WATR({ ref.x, ref.y }, macro->wndInfo.wnd);
						pt = Window::pos(macro->wndInfo.wnd);
						pt.x += rpt.x, pt.y += rpt.y;
					}
					else pt = QiFn::P_SATR({ ref.x, ref.y });
					widget_pv.Show(pt);
				}
			} break;
			case QiType::color:
			{
				const QiColor& ref = std::get<QiColor>(var);
				RECT rect;
				if (macro->wndState)
				{
					if (!macro->wndInfo.update()) SelectWindow();
					rect = QiFn::R_WATR(ref.rect, macro->wndInfo.wnd);
					POINT pt = Window::pos(macro->wndInfo.wnd);
					rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
				}
				else rect = QiFn::R_SATR(ref.rect);
				widget_rv.Show(rect);
			} break;
			case QiType::image:
			{
				const QiImage& ref = std::get<QiImage>(var);
				RECT rect;
				if (macro->wndState)
				{
					if (!macro->wndInfo.update()) SelectWindow();
					rect = QiFn::R_WATR(ref.rect, macro->wndInfo.wnd);
					POINT pt = Window::pos(macro->wndInfo.wnd);
					rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
				}
				else rect = QiFn::R_SATR(ref.rect);
				widget_rv.Show(rect);
				WidgetSet(ref);
			} break;
			case QiType::ocr:
			{
				const QiOcr& ref = std::get<QiOcr>(var);
				RECT rect;
				if (macro->wndState)
				{
					if (!macro->wndInfo.update()) SelectWindow();
					rect = QiFn::R_WATR(ref.rect, macro->wndInfo.wnd);
					POINT pt = Window::pos(macro->wndInfo.wnd);
					rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
				}
				else rect = QiFn::R_SATR(ref.rect);
				widget_rv.Show(rect);
			} break;
			case QiType::textPad:
			{
				const QiTextPad& ref = std::get<QiTextPad>(var);
				widget_td.edit()->setPlainText(ref.text);
				widget_td.show();
			} break;
			}
		}
		});
}

// TODO: new widgets
void EditUi::StyleGroup()
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
		ui.window_select_button->setProperty("group", "get_button");
		ui.mouse_position_button->setProperty("group", "get_button");
		ui.mouse_move_button->setProperty("group", "get_button");
		ui.color_rect_button->setProperty("group", "get_button");
		ui.image_rect_button->setProperty("group", "get_button");
		ui.image_shot_button->setProperty("group", "get_button");
		ui.ocr_rect_button->setProperty("group", "get_button");
		ui.textPad_button->setProperty("group", "get_button");
	}
	if ("buttons")
	{

		BindSafeIter(bind_add_button, [this](QPushButton* p, size_t i) { p->setProperty("group", "edit-add_button"); });
		BindSafeIter(bind_chg_button, [this](QPushButton* p, size_t i) { p->setProperty("group", "edit-add_button"); });
		BindSafeIter(bind_edt_button, [this](QPushButton* p, size_t i) { p->setProperty("group", "edit-edit_button"); });
		BindSafeIter(bind_edt2_button, [this](QPushButton* p, size_t i) { p->setProperty("group", "edit-edit_button"); });
		BindSafeIter(bind_tab_button, [this](QPushButton* p, size_t i) { p->setProperty("group", "edit-tab_button"); });
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
		ui.ocr_row_check->setProperty("group", "check");
		ui.editDialog_mult_check->setProperty("group", "check");
		ui.volume_max_check->setProperty("group", "check");
		ui.soundPlay_sync_check->setProperty("group", "check");
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
		ui.soundPlay_play_radio->setProperty("group", "radio");
		ui.soundPlay_pause_radio->setProperty("group", "radio");
		ui.soundPlay_resume_radio->setProperty("group", "radio");
		ui.soundPlay_stop_radio->setProperty("group", "radio");
	}
	if ("line edit")
	{
		ui.window_name_edit->setProperty("group", "line_edit");
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
		ui.open_edit->setProperty("group", "line_edit");
		ui.textPad_edit->setProperty("group", "line_edit");
		ui.editDialog_title_edit->setProperty("group", "line_edit");
		ui.editDialog_text_edit->setProperty("group", "line_edit");
		ui.editDialog_var_edit->setProperty("group", "line_edit");
		ui.volume_edit->setProperty("group", "line_edit");
		ui.volume_time_edit->setProperty("group", "line_edit");
		ui.soundPlay_edit->setProperty("group", "line_edit");
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
		ui.tab_stacked_widget->setProperty("group", "tab_widget");
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
		ui.action_table->setStyleSheet("QTableCornerButton::section,QHeaderView::section,QScrollBar,QScrollBar::sub-line,QScrollBar::add-line{background-color:rgba(0,0,0,0);border:none}QScrollBar::handle{background-color:rgba(128,128,128,0.3);border:none}");
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


void EditUi::DisableTip(bool disable)
{
	BindSafeIter(bind_type_group, [this, disable](QGroupBox* p, size_t i) {p->setToolTipDuration(disable ? 1 : 0); });
}
void EditUi::DisableMenus()
{
	muDel->setDisabled(true);
	muCut->setDisabled(true);
	muCopy->setDisabled(true);
	muEdit->setDisabled(true);
	muEdit2->setDisabled(true);
}
void EditUi::DisableChangeButtons(bool disable)
{
	BindSafeIter(bind_chg_button, [this, disable](QPushButton* p, size_t i) { p->setDisabled(disable); });
}
void EditUi::DisableEditButtons(bool disable)
{
	BindSafeIter(bind_edt_button, [this, disable](QPushButton* p, size_t i) { p->setDisabled(disable); });
	BindSafeIter(bind_edt2_button, [this, disable](QPushButton* p, size_t i) { p->setDisabled(disable); });
	ui.jump_add_button->setDisabled(this);
	ui.blockExec_add_button->setDisabled(this);
}


void EditUi::setWindowTitle(const QString& title)
{
	QDialog::setWindowTitle(title);
	ui.title_label->setText(title);
}
void EditUi::SetTabCurrent(int index)
{
	ui.tab_stacked_widget->setCurrentIndex(index);
	BindSafeIter(bind_tab_button, [this, index](QPushButton* p, size_t i) { index == i ? p->setDisabled(true) : p->setEnabled(true); });
}
void EditUi::SetDebugState(int debugState)
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
void EditUi::SetWindowMode()
{
	ui.window_state_check->setChecked(macro->wndState);
	ui.window_child_check->setChecked(macro->wndInfo.child);
	ui.window_name_edit->setText(macro->wndInfo.wndName);
}

void EditUi::SelectWindow()
{
	QPoint pt = pos();
	move(-1000, -1000);
	macro->wndInfo = QiFn::WindowSelection();
	move(pt);
	SetWindowMode();
}
void EditUi::Reload()
{
	setWindowTitle(layers.last().first);
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
// TODO: new action's next name
void EditUi::NextEdit(bool edit2)
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
		QiOcr& ocr = std::get<QiOcr>(var);
		if (edit2)
		{
			next = &ocr.next2;
			title = "编辑 - 文字识别（失败）";
		}
		else
		{
			next = &ocr.next;
			title = "编辑 - 文字识别（成功）";
		}
	} break;
	case QiType::varCondition:
	{
		QiVarCondition& varCondition = std::get<QiVarCondition>(var);
		if (edit2)
		{
			next = &varCondition.next2;
			title = "编辑 - 变量条件（否）";
		}
		else
		{
			next = &varCondition.next;
			title = "编辑 - 变量条件（是）";
		}
	} break;
	case QiType::volume:
	{
		QiVolume& volume = std::get<QiVolume>(var);
		if (edit2)
		{
			next = &volume.next2;
			title = "编辑 - 音量检测（小于）";
		}
		else
		{
			next = &volume.next;
			title = "编辑 - 音量检测（大于）";
		}
	} break;
	}
	if (next)
	{
		layers.append(Layer(title, next));
		actions = next;
		setWindowTitle(title);
		Reload();
	}
}


// TODO: new action's table item
void EditUi::TableUpdate(int index)
{
	updating = true;
	const Action& var = actions->at(index);
	QString type;
	QString param;
	QColor color(0, 0, 0, 0);
	switch (var.index())
	{
	case QiType::end:
	{
		type = QiUi::Text::acEnd;
	} break;
	case QiType::delay:
	{
		const QiDelay& ref = std::get<QiDelay>(var);
		type = QiUi::Text::acWait;

		QString min = ref.v_min.isEmpty() ? QString::number(ref.min) : ref.v_min;
		QString max = ref.v_max.isEmpty() ? QString::number(ref.max) : ref.v_max;

		if (min == max) param = min;
		else param = min + QString(" ~ ") + max;
	} break;
	case QiType::key:
	{
		const QiKey& ref = std::get<QiKey>(var);
		if (ref.state == QiKey::up) type = QiUi::Text::acUp;
		else if (ref.state == QiKey::down) type = QiUi::Text::acDown;
		else if (ref.state == QiKey::click) type = QiUi::Text::acClick;

		param = QKeyEdit::keyName(ref.vk);
	} break;
	case QiType::mouse:
	{
		const QiMouse& ref = std::get<QiMouse>(var);
		if (ref.move) type = QiUi::Text::acMove;
		else type = QiUi::Text::acPos;

		param = QString::number(ref.x);
		param += " , ";
		param += QString::number(ref.y);
		if (ref.ex)
		{
			param += " | 随机：";
			param += QString::number(ref.ex);
		}
		if (ref.track)
		{
			param += " | 轨迹：";
			param += QString::number(ref.speed);
		}
	} break;
	case QiType::copyText:
	{
		const QiCopyText& ref = std::get<QiCopyText>(var);
		type = QiUi::Text::acCopyText;
		param = QiFn::FoldText(ref.text);
	} break;
	case QiType::color:
	{
		const QiColor& ref = std::get<QiColor>(var);
		type = QiUi::Text::acColor;
		param += QString::number(ref.rect.left);
		param += ",";
		param += QString::number(ref.rect.top);
		param += ",";
		param += QString::number(ref.rect.right);
		param += ",";
		param += QString::number(ref.rect.bottom);
		param += " | ";
		param += QString::number(ref.rgbe.r);
		param += ",";
		param += QString::number(ref.rgbe.g);
		param += ",";
		param += QString::number(ref.rgbe.b);
		param += ",";
		param += QString::number(ref.rgbe.a);
		if (ref.move) param += " 移动";
		color = QColor(ref.rgbe.r, ref.rgbe.g, ref.rgbe.b, 255);
	} break;
	case QiType::loop:
	{
		const QiLoop& ref = std::get<QiLoop>(var);
		type = QiUi::Text::acLoop;

		QString min = ref.v_min.isEmpty() ? QString::number(ref.min) : ref.v_min;
		QString max = ref.v_max.isEmpty() ? QString::number(ref.max) : ref.v_max;

		if (min == '0' && max == '0') param = "无限";
		else if (min == max) param = min;
		else param = min + QString(" ~ ") + max;
	} break;
	case QiType::loopEnd:
	{
		type = QiUi::Text::acEndLoop;
	} break;
	case QiType::keyState:
	{
		const QiKeyState& ref = std::get<QiKeyState>(var);
		type = QiUi::Text::acKeyState;

		param += QKeyEdit::keyName(ref.vk);
	} break;
	case QiType::resetPos:
	{
		type = QiUi::Text::acResetPos;
	} break;
	case QiType::image:
	{
		const QiImage& ref = std::get<QiImage>(var);
		type = QiUi::Text::acImage;

		param += QString::number(ref.rect.left);
		param += ",";
		param += QString::number(ref.rect.top);
		param += ",";
		param += QString::number(ref.rect.right);
		param += ",";
		param += QString::number(ref.rect.bottom);
		param += " | ";
		param += QString::number(ref.map.width());
		param += ",";
		param += QString::number(ref.map.height());
		param += " | ";
		param += QString::number(ref.sim);
		if (ref.move) param += "移动";
	} break;
	case QiType::popText:
	{
		const QiPopText& ref = std::get<QiPopText>(var);
		type = QiUi::Text::acPopText;
		param = QiFn::FoldText(ref.text, 16);
		param += " |　时长：";
		param += QString::number(ref.time);
		if (ref.sync) param += " 等待";
	} break;
	case QiType::savePos:
	{
		type = QiUi::Text::acSavePos;
	} break;
	case QiType::timer:
	{
		const QiTimer& ref = std::get<QiTimer>(var);
		type = QiUi::Text::acTimer;

		QString min = ref.v_min.isEmpty() ? QString::number(ref.min) : ref.v_min;
		QString max = ref.v_max.isEmpty() ? QString::number(ref.max) : ref.v_max;

		if (min == max) param = min;
		else param = min + QString(" ~ ") + max;
	} break;
	case QiType::jump:
	{
		const QiJump& ref = std::get<QiJump>(var);
		type = QiUi::Text::acJump;

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
		type = QiUi::Text::acJumpPoint;

		param = "id：";
		param += QString::number(ref.id);
	} break;
	case QiType::dialog:
	{
		const QiDialog& ref = std::get<QiDialog>(var);
		type = QiUi::Text::acDialog;
		param = QiFn::FoldText(ref.title, 8);
		param += " | ";
		param = QiFn::FoldText(ref.text, 12);
	} break;
	case QiType::block:
	{
		const QiBlock& ref = std::get<QiBlock>(var);
		type = QiUi::Text::acBlock;

		param = "id：";
		param += QString::number(ref.id);
	} break;
	case QiType::blockExec:
	{
		const QiBlockExec& ref = std::get<QiBlockExec>(var);
		type = QiUi::Text::acBlockExec;

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
		type = QiUi::Text::acQuickInput;
		param = QiFn::FoldText(KeyToString(ref.chars));
	} break;
	case QiType::keyBlock:
	{
		const QiKeyBlock& ref = std::get<QiKeyBlock>(var);
		type = QiUi::Text::acKeyBlock;

		if (ref.block) param = "屏蔽：";
		else param = "解除：";
		if (ref.vk) param += QKeyEdit::keyName(ref.vk);
		else param += "鼠标移动";

	} break;
	case QiType::clock:
	{
		const QiClock& ref = std::get<QiClock>(var);
		type = QiUi::Text::acClock;

		param = QString::fromStdString(QiTime::toString(ref.time));
	} break;
	case QiType::ocr:
	{
		const QiOcr& ref = std::get<QiOcr>(var);
		type = QiUi::Text::acOcr;

		param += QString::number(ref.rect.left);
		param += ",";
		param += QString::number(ref.rect.top);
		param += ",";
		param += QString::number(ref.rect.right);
		param += ",";
		param += QString::number(ref.rect.bottom);
		param += "|";
		if (ref.match) param += "匹配：";
		else param += "搜索：";
		param += QiFn::FoldText(ref.text, 12);
	} break;
	case QiType::varOperator:
	{
		const QiVarOperator& ref = std::get<QiVarOperator>(var);
		type = QiUi::Text::acVarOperator;
		param = QiFn::FoldText(ref.script);
	} break;
	case QiType::varCondition:
	{
		const QiVarCondition& ref = std::get<QiVarCondition>(var);
		type = QiUi::Text::acVarCondition;
		param = QiFn::FoldText(ref.script);
	} break;
	case QiType::mouseTrack:
	{
		const QiMouseTrack& ref = std::get<QiMouseTrack>(var);
		type = QiUi::Text::acMouseTrack;

		param = QString::number(ref.s.size());
	} break;
	case QiType::open:
	{
		const QiOpen& ref = std::get<QiOpen>(var);
		type = QiUi::Text::acOpen;
		param = QiFn::FoldText(ref.url, 20, true);
	} break;
	case QiType::textPad:
	{
		const QiTextPad& ref = std::get<QiTextPad>(var);
		type = QiUi::Text::acTextPad;
		param = QiFn::FoldText(ref.text);
	} break;
	case QiType::editDialog:
	{
		const QiEditDialog& ref = std::get<QiEditDialog>(var);
		type = QiUi::Text::acEditDialog;
		param = QiFn::FoldText(ref.title, 4);
		param += " | ";
		param = QiFn::FoldText(ref.text, 8);
		param += " | ";
		param = QiFn::FoldText(ref.var, 8);
	} break;
	case QiType::volume:
	{
		const QiVolume& ref = std::get<QiVolume>(var);
		type = QiUi::Text::acVolume;
		param = ref.max ? "最大：" : "平均：";
		param += QString::number(ref.volume);
		param += ", ";
		param += QString::number(ref.time);
	} break;
	case QiType::soundPlay:
	{
		const QiSoundPlay& ref = std::get<QiSoundPlay>(var);
		type = QiUi::Text::acSoundPlay;
		if (ref.state == QiSoundPlay::play)
		{
			param = QiFn::FoldText(ref.file, 18, true);
			if (ref.sync) param += " 等待";
		}
		else if (ref.state == QiSoundPlay::pause) param = "暂停";
		else if (ref.state == QiSoundPlay::resume) param = "继续";
		else if (ref.state == QiSoundPlay::stop) param = "停止";
	} break;
	}

	if (type.isEmpty())
	{
		ui.action_table->item(index, tableColumn_type)->setText("无效类型");
	}
	else
	{
		if (var.base().debug_entry) ui.action_table->item(index, tableColumn_debug)->setText(QiUi::Symbol::Entry);
		else
		{
			if (var.base().debug_break) ui.action_table->item(index, tableColumn_debug)->setText(QiUi::Symbol::Pause);
			else
			{
				if (var.base().debug_exit) ui.action_table->item(index, tableColumn_debug)->setText(QiUi::Symbol::Exit);
				else ui.action_table->item(index, tableColumn_debug)->setText("");
			}
		}

		if (var.base().disable) ui.action_table->item(index, tableColumn_disable)->setText(QiUi::Symbol::Off);
		else ui.action_table->item(index, tableColumn_disable)->setText(QiUi::Symbol::On);

		ui.action_table->item(index, tableColumn_type)->setText(type);

		{
			QTableWidgetItem* item = new QTableWidgetItem(param);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			if (color.alpha() == 255) item->setTextColor(color);
			ui.action_table->setItem(index, tableColumn_param, item);
		}


		ui.action_table->item(index, tableColumn_mark)->setText(var.base().mark);
	}
	updating = false;
}
void EditUi::TableUpdate(int begin, int end)
{
	for (size_t i = begin; i <= end; i++) TableUpdate(i);
}
void EditUi::TableReload()
{
	tableCurrent.clear();
	tableCurrentPrev.clear();
	DisableMenus();
	DisableEditButtons(true);
	DisableChangeButtons(true);
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
			//ui.action_table->setItem(i, tableColumn_param, new QTableWidgetItem());
			//ui.action_table->item(i, tableColumn_param)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.action_table->setItem(i, tableColumn_mark, new QTableWidgetItem());
			ui.action_table->item(i, tableColumn_mark)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			TableUpdate(i);
			updating = false;
		}
	}
}
void EditUi::TableInsert(int index)
{
	ui.action_table->insertRow(index);
	TableUpdate(index);
}
void EditUi::TableRemove(int index)
{
	ui.action_table->removeRow(index);
}
void EditUi::TableSelection(const QiVector<int> selection)
{
	for (auto& i : selection) for (size_t c = 0; c < ui.action_table->columnCount(); c++) ui.action_table->setItemSelected(ui.action_table->item(i, c), true);
}


// TODO: new action's widget get
Action EditUi::ItemGet(int type)
{
	switch (type)
	{
	case QiType::end: return QiEnd();
	case QiType::delay: return WidgetGetDelay();
	case QiType::key: return WidgetGetKey();
	case QiType::mouse: return WidgetGetMouse();
	case QiType::copyText: return WidgetGetText();
	case QiType::color: return WidgetGetColor();
	case QiType::loop: return WidgetGetLoop();
	case QiType::loopEnd: return QiLoopEnd();
	case QiType::keyState: return WidgetGetKeyState();
	case QiType::resetPos: return QiResetPos();
	case QiType::image: return WidgetGetImage();
	case QiType::popText: return WidgetGetPopText();
	case QiType::savePos: return QiSavePos();
	case QiType::timer: return WidgetGetTimer();
	case QiType::jump: return WidgetGetJump();
	case QiType::jumpPoint: return WidgetGetJumpPoint();
	case QiType::dialog: return WidgetGetDialog();
	case QiType::block: return WidgetGetBlock();
	case QiType::blockExec: return WidgetGetBlockExec();
	case QiType::quickInput: return WidgetGetQuickInput();
	case QiType::keyBlock: return WidgetGetKeyBlock();
	case QiType::clock: return WidgetGetClock();
	case QiType::ocr: return WidgetGetOcr();
	case QiType::varOperator: return WidgetGetVarOperator();
	case QiType::varCondition: return WidgetGetVarCondition();
	case QiType::open: return WidgetGetOpen();
	case QiType::textPad: return WidgetGetTextPad();
	case QiType::editDialog: return WidgetGetEditDialog();
	case QiType::volume: return WidgetGetVolume();
	case QiType::soundPlay: return WidgetGetSoundPlay();
	}
	return Action();
}
// TODO: new action's widget set
void EditUi::ItemSet(int p)
{
	const Action& var = actions->at(p);
	switch (var.index())
	{
	case QiType::delay: WidgetSet(std::get<QiDelay>(var)); break;
	case QiType::key: WidgetSet(std::get<QiKey>(var)); break;
	case QiType::mouse: WidgetSet(std::get<QiMouse>(var)); break;
	case QiType::copyText: WidgetSet(std::get<QiCopyText>(var)); break;
	case QiType::color: WidgetSet(std::get<QiColor>(var)); break;
	case QiType::loop: WidgetSet(std::get<QiLoop>(var)); break;
	case QiType::keyState: WidgetSet(std::get<QiKeyState>(var)); break;
	case QiType::image: WidgetSet(std::get<QiImage>(var)); break;
	case QiType::popText: WidgetSet(std::get<QiPopText>(var)); break;
	case QiType::timer: WidgetSet(std::get<QiTimer>(var)); break;
	case QiType::dialog: WidgetSet(std::get<QiDialog>(var)); break;
	case QiType::quickInput: WidgetSet(std::get<QiQuickInput>(var)); break;
	case QiType::keyBlock: WidgetSet(std::get<QiKeyBlock>(var)); break;
	case QiType::clock: WidgetSet(std::get<QiClock>(var)); break;
	case QiType::ocr: WidgetSet(std::get<QiOcr>(var)); break;
	case QiType::varOperator: WidgetSet(std::get<QiVarOperator>(var)); break;
	case QiType::varCondition: WidgetSet(std::get<QiVarCondition>(var)); break;
	case QiType::open: WidgetSet(std::get<QiOpen>(var)); break;
	case QiType::textPad: WidgetSet(std::get<QiTextPad>(var)); break;
	case QiType::editDialog: WidgetSet(std::get<QiEditDialog>(var)); break;
	case QiType::volume: WidgetSet(std::get<QiVolume>(var)); break;
	case QiType::soundPlay: WidgetSet(std::get<QiSoundPlay>(var)); break;
	}
}
void EditUi::ItemMove(bool up, int len)
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
void EditUi::ItemAdd(int type)
{
	int p = ui.action_table->currentRow();
	if (p < 0) p = actions->size();
	else p++;
	actions->insert(p, ItemGet(type));
	TableReload();
	ui.action_table->setCurrentItem(ui.action_table->item(p, tableColumn_type));
}
void EditUi::ItemChange(int type)
{
	for (auto& i : tableCurrent)
	{
		Action& current = actions->at(i);
		QiBase base_old = current.base();
		current = ItemGet(type);
		QiBase& base_new = current.base();
		base_new.disable = std::move(base_old.disable);
		base_new.mark = std::move(base_old.mark);
		base_new.next = std::move(base_old.next);
		base_new.next2 = std::move(base_old.next2);
	}
	TableReload();
	ui.action_table->clearSelection();
}
void EditUi::ItemDel()
{
	if (tableCurrent.empty()) return;
	std::vector<size_t> positions;
	for (auto& i : tableCurrent) positions.push_back(i);
	actions->remove(positions);
	TableReload();
	ui.action_table->setCurrentItem(0);
}
void EditUi::ItemCut()
{
	ItemCopy();
	ItemDel();
}
void EditUi::ItemCopy()
{
	if (tableCurrent.empty()) return;
	Qi::clipboard.clear();
	for (auto& i : tableCurrent) Qi::clipboard.append(actions->at(i));
}
void EditUi::ItemPaste()
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

// TODO: new action's widget get
QiKey EditUi::WidgetGetKey() {
	QiKey key;
	if (ui.key_down_radio->isChecked()) key.state = QiKey::down;
	else if (ui.key_up_radio->isChecked()) key.state = QiKey::up;
	else if (ui.key_click_radio->isChecked()) key.state = QiKey::click;
	key.vk = ui.key_keyedit->key();
	return key;
}
QiKeyState EditUi::WidgetGetKeyState() {
	QiKeyState keyState;
	keyState.vk = ui.keyState_keyedit->key();
	return keyState;
}
QiMouse EditUi::WidgetGetMouse() {
	QiMouse mouse;
	mouse.move = ui.mouse_move_radio->isChecked();
	mouse.track = ui.mouse_track_check->isChecked();
	if (mouse.move)
	{
		mouse.x = QiRange::Restricted(ui.mouse_x_edit->text().toInt(), QiMouse::range_move);
		mouse.y = QiRange::Restricted(ui.mouse_y_edit->text().toInt(), QiMouse::range_move);
	}
	else
	{
		mouse.x = QiRange::Restricted(ui.mouse_x_edit->text().toInt(), QiMouse::range_pos);
		mouse.y = QiRange::Restricted(ui.mouse_y_edit->text().toInt(), QiMouse::range_pos);
	}
	mouse.ex = QiRange::Restricted(ui.mouse_rand_edit->text().toInt(), QiMouse::range_rand);
	mouse.speed = ui.mouse_speed_edit->text().isEmpty() ? 20 : QiRange::Restricted(ui.mouse_speed_edit->text().toInt(), QiMouse::range_speed);
	return mouse;
}
QiDelay EditUi::WidgetGetDelay() {
	QiDelay delay;
	QString min = ui.delay_min_edit->text();
	QString max = ui.delay_max_edit->text();
	if (min.isEmpty())
	{
		delay.min = 10;
	}
	else
	{
		if (QiVar::isInteger(min.toStdString())) delay.min = QiRange::Restricted(min.toInt(), QiDelay::range_time);
		else delay.v_min = min;
	}
	if (max.isEmpty())
	{
		delay.max = delay.min;
	}
	else
	{
		if (QiVar::isInteger(max.toStdString())) delay.max = QiRange::Restricted(max.toInt(), QiDelay::range_time);
		else delay.v_max = max;
	}
	return delay;
}
QiCopyText EditUi::WidgetGetText() {
	QiCopyText text;
	text.text = ui.copyText_text->toPlainText();
	return text;
}
QiColor EditUi::WidgetGetColor() {
	QiColor color;
	color.move = ui.color_move_check->isChecked();
	color.rect = {
		QiRange::Restricted(ui.color_left_edit->text().toInt(), QiColor::range_rect),
		QiRange::Restricted(ui.color_top_edit->text().toInt(), QiColor::range_rect),
		QiRange::Restricted(ui.color_right_edit->text().toInt(), QiColor::range_rect),
		QiRange::Restricted(ui.color_bottom_edit->text().toInt(), QiColor::range_rect)
	};
	color.rgbe.set(
		QiRange::Restricted(ui.color_red_edit->text().toInt(), QiColor::range_rgb),
		QiRange::Restricted(ui.color_green_edit->text().toInt(), QiColor::range_rgb),
		QiRange::Restricted(ui.color_blue_edit->text().toInt(), QiColor::range_rgb),
		ui.color_sim_edit->text().isEmpty() ? 10 : QiRange::Restricted(ui.color_sim_edit->text().toInt(), QiColor::range_rgb)
	);
	return color;
}
QiLoop EditUi::WidgetGetLoop()
{
	QiLoop loop;
	QString min = ui.loop_min_edit->text();
	QString max = ui.loop_max_edit->text();
	if (min.isEmpty())
	{
		loop.min = 1;
	}
	else
	{
		if (QiVar::isInteger(min.toStdString())) loop.min = QiRange::Restricted(min.toInt(), QiLoop::range_count);
		else loop.v_min = min;
	}
	if (max.isEmpty())
	{
		loop.max = loop.min;
	}
	else
	{
		if (QiVar::isInteger(max.toStdString())) loop.max = QiRange::Restricted(max.toInt(), QiLoop::range_count);
		else loop.v_max = max;
	}
	return loop;
}
QiImage EditUi::WidgetGetImage()
{
	QiImage image;
	image.move = ui.image_move_check->isChecked();
	image.rect = {
		QiRange::Restricted(ui.image_left_edit->text().toInt(), QiImage::range_rect),
		QiRange::Restricted(ui.image_top_edit->text().toInt(), QiImage::range_rect),
		QiRange::Restricted(ui.image_right_edit->text().toInt(), QiImage::range_rect),
		QiRange::Restricted(ui.image_bottom_edit->text().toInt(), QiImage::range_rect)
	};
	image.sim = ui.image_sim_edit->text().isEmpty() ? 80 : QiRange::Restricted(ui.image_sim_edit->text().toInt(), QiImage::range_sim);
	image.map = imageMap;
	return image;
}
QiPopText EditUi::WidgetGetPopText()
{
	QiPopText popText;
	popText.text = ui.popText_text_edit->text();
	popText.time = ui.popText_time_edit->text().isEmpty() ? 1000 : QiRange::Restricted(ui.popText_time_edit->text().toInt(), QiPopText::range_time);
	popText.sync = ui.popText_wait_check->isChecked();
	return popText;
}
QiTimer EditUi::WidgetGetTimer()
{
	QiTimer timer;
	QString min = ui.timer_min_edit->text();
	QString max = ui.timer_max_edit->text();
	if (min.isEmpty())
	{
		timer.min = 1000;
	}
	else
	{
		if (QiVar::isInteger(min.toStdString())) timer.min = QiRange::Restricted(min.toInt(), QiTimer::range_time);
		else timer.v_min = min;
	}
	if (max.isEmpty())
	{
		timer.max = timer.min;
	}
	else
	{
		if (QiVar::isInteger(max.toStdString())) timer.max = QiRange::Restricted(max.toInt(), QiTimer::range_time);
		else timer.v_max = max;
	}
	return timer;
}
QiJump EditUi::WidgetGetJump()
{
	QiJump jump;
	int row = ui.jumpPoint_list->currentRow();
	if (row > -1) jump.id = ui.jumpPoint_list->item(row)->data(DataRole::id).toInt();
	return jump;
}
QiJumpPoint EditUi::WidgetGetJumpPoint()
{
	QiJumpPoint jumpPoint;
	QiVector<int> ids = LoadIds(*actionsRoot, QiType::jumpPoint);
	jumpPoint.id = UniqueId(ids);
	return jumpPoint;
}
QiDialog EditUi::WidgetGetDialog()
{
	QiDialog dialog;
	if (ui.dialog_style_none_radio->isChecked()) dialog.style = QiDialog::none;
	else if (ui.dialog_style_warning_radio->isChecked()) dialog.style = QiDialog::warning;
	else if (ui.dialog_style_error_radio->isChecked()) dialog.style = QiDialog::error;
	dialog.title = ui.dialog_title_edit->text();
	dialog.text = ui.dialog_text_edit->text();
	return dialog;
}
QiBlock EditUi::WidgetGetBlock()
{
	QiBlock block;
	QiVector<int> ids = LoadIds(*actionsRoot, QiType::block);
	block.id = UniqueId(ids);
	return block;
}
QiBlockExec EditUi::WidgetGetBlockExec()
{
	QiBlockExec blockExec;
	int row = ui.block_list->currentRow();
	if (row > -1) blockExec.id = ui.block_list->item(row)->data(DataRole::id).toInt();
	return blockExec;
}
QiQuickInput EditUi::WidgetGetQuickInput()
{
	QiQuickInput quickInput;
	quickInput.chars = StringToKey(ui.quickInput_text_edit->text());
	return quickInput;
}
QiKeyBlock EditUi::WidgetGetKeyBlock()
{
	QiKeyBlock keyBlock;
	if (ui.keyBlock_move_check->isChecked()) keyBlock.vk = 0;
	else keyBlock.vk = ui.keyBlock_keyedit->key();
	keyBlock.block = ui.keyBlock_on_radio->isChecked();
	return keyBlock;
}
QiClock EditUi::WidgetGetClock()
{
	QiClock clock;
	QTime time = ui.clock_time_edit->time();
	clock.time = QiRange::Restricted((int)QiTime::toTimeStamp(time.hour(), time.minute(), time.second()), QiClock::range_time);
	return clock;
}
QiOcr EditUi::WidgetGetOcr()
{
	QiOcr ocr;
	ocr.rect = {
		QiRange::Restricted(ui.ocr_left_edit->text().toInt(), QiOcr::range_rect),
		QiRange::Restricted(ui.ocr_top_edit->text().toInt(), QiOcr::range_rect),
		QiRange::Restricted(ui.ocr_right_edit->text().toInt(), QiOcr::range_rect),
		QiRange::Restricted(ui.ocr_bottom_edit->text().toInt(), QiOcr::range_rect)
	};
	ocr.text = ui.ocr_text_edit->text();
	ocr.var = ui.ocr_var_edit->text();
	ocr.match = ui.ocr_match_check->isChecked();
	ocr.row = ui.ocr_row_check->isChecked();
	return ocr;
}
QiVarOperator EditUi::WidgetGetVarOperator()
{
	QiVarOperator varOperator;
	varOperator.script = ui.varOperator_edit->toPlainText();
	return varOperator;
}
QiVarCondition EditUi::WidgetGetVarCondition()
{
	QiVarCondition varCondition;
	varCondition.script = ui.varCondition_edit->text();
	return varCondition;
}
QiOpen EditUi::WidgetGetOpen()
{
	QiOpen open;
	open.url = ui.open_edit->text().isEmpty() ? ui.open_edit->placeholderText() : ui.open_edit->text();
	return open;
}
QiTextPad EditUi::WidgetGetTextPad()
{
	QiTextPad textPad;
	textPad.text = ui.textPad_edit->text();
	return textPad;
}
QiEditDialog EditUi::WidgetGetEditDialog()
{
	QiEditDialog editDialog;
	editDialog.mult = ui.editDialog_mult_check->isChecked();
	editDialog.title = ui.editDialog_title_edit->text();
	editDialog.text = ui.editDialog_text_edit->text();
	editDialog.var = ui.editDialog_var_edit->text();
	return editDialog;
}
QiVolume EditUi::WidgetGetVolume()
{
	QiVolume volume;
	volume.volume = QiRange::Restricted(ui.volume_edit->value(), QiVolume::range_volume);
	volume.time = QiRange::Restricted(ui.volume_time_edit->value(), QiVolume::range_time);
	volume.max = ui.volume_max_check->isChecked();
	return volume;
}
QiSoundPlay EditUi::WidgetGetSoundPlay()
{
	QiSoundPlay soundPlay;
	soundPlay.file = ui.soundPlay_edit->text().isEmpty() ? ui.soundPlay_edit->placeholderText() : ui.soundPlay_edit->text();
	soundPlay.sync = ui.soundPlay_sync_check->isChecked();
	if (ui.soundPlay_play_radio->isChecked()) soundPlay.state = QiSoundPlay::play;
	else if (ui.soundPlay_pause_radio->isChecked()) soundPlay.state = QiSoundPlay::pause;
	else if (ui.soundPlay_resume_radio->isChecked()) soundPlay.state = QiSoundPlay::resume;
	else if (ui.soundPlay_stop_radio->isChecked()) soundPlay.state = QiSoundPlay::stop;
	return soundPlay;
}

// TODO: new action's widget set
void EditUi::WidgetSet(const QiKey& key)
{
	if (key.state == QiKey::down) ui.key_down_radio->setChecked(true);
	else if (key.state == QiKey::up) ui.key_up_radio->setChecked(true);
	else if (key.state == QiKey::click) ui.key_click_radio->setChecked(true);
	ui.key_keyedit->setKey(key.vk);
}
void EditUi::WidgetSet(const QiKeyState& keyState)
{
	ui.keyState_keyedit->setKey(keyState.vk);
}
void EditUi::WidgetSet(const QiMouse& mouse)
{
	mouse.move ? ui.mouse_move_radio->setChecked(true) : ui.mouse_position_radio->setChecked(true);
	ui.mouse_track_check->setChecked(mouse.track);
	ui.mouse_speed_edit->setText(QString::number(mouse.speed));
	ui.mouse_x_edit->setText(QString::number(mouse.x)); ui.mouse_y_edit->setText(QString::number(mouse.y)); if (mouse.ex > -1) ui.mouse_rand_edit->setText(QString::number(mouse.ex));
}
void EditUi::WidgetSet(const QiDelay& delay)
{
	ui.delay_min_edit->setText(delay.v_min.isEmpty() ? QString::number(delay.min) : delay.v_min);
	ui.delay_max_edit->setText(delay.v_max.isEmpty() ? QString::number(delay.max) : delay.v_max);
}
void EditUi::WidgetSet(const QiCopyText& text)
{
	ui.copyText_text->setPlainText(text.text);
}
void EditUi::WidgetSet(const QiColor& color) {
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
void EditUi::WidgetSet(const QiLoop& loop)
{
	ui.loop_min_edit->setText(loop.v_min.isEmpty() ? QString::number(loop.min) : loop.v_min);
	ui.loop_max_edit->setText(loop.v_max.isEmpty() ? QString::number(loop.max) : loop.v_max);
}
void EditUi::WidgetSet(const QiImage& image) {
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
void EditUi::WidgetSet(const QiPopText& popText) {
	ui.popText_text_edit->setText(popText.text);
	ui.popText_time_edit->setText(QString::number(popText.time));
	ui.popText_wait_check->setChecked(popText.sync);
}
void EditUi::WidgetSet(const QiTimer& timer)
{
	ui.timer_min_edit->setText(timer.v_min.isEmpty() ? QString::number(timer.min) : timer.v_min);
	ui.timer_max_edit->setText(timer.v_max.isEmpty() ? QString::number(timer.max) : timer.v_max);
}
void EditUi::WidgetSet(const QiDialog& dialog)
{
	if (dialog.style == QiDialog::none) ui.dialog_style_none_radio->setChecked(true);
	if (dialog.style == QiDialog::warning) ui.dialog_style_warning_radio->setChecked(true);
	if (dialog.style == QiDialog::error) ui.dialog_style_error_radio->setChecked(true);
	ui.dialog_title_edit->setText(dialog.title);
	ui.dialog_text_edit->setText(dialog.text);
}
void EditUi::WidgetSet(const QiQuickInput& quickInput)
{
	ui.quickInput_text_edit->setText(KeyToString(quickInput.chars));
}
void EditUi::WidgetSet(const QiKeyBlock& keyBlock)
{
	if (keyBlock.vk) ui.keyBlock_keyedit->setKey(keyBlock.vk);
	ui.keyBlock_move_check->setChecked(!keyBlock.vk);
	keyBlock.block ? ui.keyBlock_on_radio->setChecked(true) : ui.keyBlock_off_radio->setChecked(true);
}
void EditUi::WidgetSet(const QiClock& clock)
{
	ui.clock_time_edit->setTime(QTime(QiTime::h(clock.time), QiTime::m(clock.time), QiTime::s(clock.time)));
}
void EditUi::WidgetSet(const QiOcr& ocr)
{
	ui.ocr_left_edit->setText(QString::number(ocr.rect.left));
	ui.ocr_top_edit->setText(QString::number(ocr.rect.top));
	ui.ocr_right_edit->setText(QString::number(ocr.rect.right));
	ui.ocr_bottom_edit->setText(QString::number(ocr.rect.bottom));
	ui.ocr_text_edit->setText(ocr.text);
	ui.ocr_var_edit->setText(ocr.var);
	ui.ocr_match_check->setChecked(ocr.match);
	ui.ocr_row_check->setChecked(ocr.row);
}
void EditUi::WidgetSet(const QiVarOperator& varOperator)
{
	ui.varOperator_edit->setPlainText(varOperator.script);
}
void EditUi::WidgetSet(const QiVarCondition& varCondition)
{
	ui.varCondition_edit->setText(varCondition.script);
}
void EditUi::WidgetSet(const QiOpen& open)
{
	ui.open_edit->setText(open.url);
}
void EditUi::WidgetSet(const QiTextPad& textPad)
{
	ui.textPad_edit->setText(textPad.text);
}
void EditUi::WidgetSet(const QiEditDialog& editDialog)
{
	ui.editDialog_mult_check->setChecked(editDialog.mult);
	ui.editDialog_title_edit->setText(editDialog.title);
	ui.editDialog_text_edit->setText(editDialog.text);
	ui.editDialog_var_edit->setText(editDialog.var);
}
void EditUi::WidgetSet(const QiVolume& volume)
{
	ui.volume_edit->setValue(volume.volume);
	ui.volume_time_edit->setValue(volume.time);
	ui.volume_max_check->setChecked(volume.max);
}
void EditUi::WidgetSet(const QiSoundPlay& soundPlay)
{
	ui.soundPlay_edit->setText(soundPlay.file);
	ui.soundPlay_sync_check->setChecked(soundPlay.sync);
	if (soundPlay.state == QiSoundPlay::play) ui.soundPlay_play_radio->setChecked(true);
	if (soundPlay.state == QiSoundPlay::pause) ui.soundPlay_pause_radio->setChecked(true);
	if (soundPlay.state == QiSoundPlay::resume) ui.soundPlay_resume_radio->setChecked(true);
	if (soundPlay.state == QiSoundPlay::stop) ui.soundPlay_stop_radio->setChecked(true);
}


// iter binds
template<class Ty, class Fn>
void EditUi::BindSafeIter(QiVector<Ty*>& bind, Fn call)
{
	for (size_t i = 0; i < bind.size(); i++) if (bind.at(i)) call(bind.at(i), i);
}

// iter actions
bool EditUi::IterActions(Actions& actions, std::function<bool(Action&)> callBack, int type)
{
	for (auto& i : actions)
	{
		if ((type == QiType::none || i.index() == type) && callBack(i)) return true;
		if (i.base().next.not_empty() && IterActions(i.base().next, callBack, type)) return true;
		if (i.base().next2.not_empty() && IterActions(i.base().next2, callBack, type)) return true;
	}
	return false;
}
bool EditUi::IterActions(const Actions& actions, std::function<bool(const Action&)> callBack, int type)
{
	return IterActions((Actions&)actions, (std::function<bool(Action&)>)callBack, type);
}


// type: jumpPoint, jump, block, blockExec
QiVector<int> EditUi::LoadIds(const Actions& actions, int type)
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
Actions EditUi::LoadType(const Actions& actions, int type)
{
	Actions result;
	IterActions(actions, [&result, type](const Action& action) {
		if (action.index() == type) result.append(action);
		return false;
		}, type);
	return result;
}
bool EditUi::FindId(const QiVector<int>& ids, int id)
{
	for (auto& i : ids) if (i == id) return true; return false;
}
// type: jump, blockExec
void EditUi::InvalidId(Actions& actions, int type)
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
int EditUi::UniqueId(const QiVector<int>& ids)
{
	int id = 1;
	for (auto& i : ids) if (i >= id) id = i + 1;
	return id;
}
// type: jumpPoint, block
void EditUi::UniqueActionsId(QiVector<int> ids, Actions& actions, int type)
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
// id list
void EditUi::ListJumpPointReload()
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
void EditUi::ListBlockReload()
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


// quickinput
QiVector<unsigned char> EditUi::StringToKey(const QString str)
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
QString EditUi::KeyToString(const QiVector<unsigned char >& keys)
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


bool EditUi::event(QEvent* e)
{
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
	{
		QKeyEvent* keyEvent = (QKeyEvent*)e;
		if ((keyEvent->key() == Qt::Key_Escape) || (keyEvent->key() == Qt::Key_Return) || keyEvent->key() == Qt::Key_Enter || (keyEvent->key() == Qt::Key_Space)) return true;
	}
	return QDialog::event(e);
}
bool EditUi::eventFilter(QObject* obj, QEvent* e)
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
void EditUi::showEvent(QShowEvent*)
{
	Qi::application->setStyleSheet(Qi::ui.themes[Qi::set.theme].style);
	SetForegroundWindow((HWND)QWidget::winId());
}
void EditUi::customEvent(QEvent* e)
{
	if (e->type() == EditEvent::close)
	{
		Qi::widget.macroEdited();
		widget_pv.hide();
		widget_rv.hide();
		widget_td.hide();
		close();
	}
	if (e->type() == EditEvent::debug_pause) SetDebugState(debug_pause);
}