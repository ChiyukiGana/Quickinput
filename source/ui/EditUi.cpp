#include <EditUi.h>
#include <RecordUi.h>
EditUi::EditUi(Macro* macro) : macro(macro), actionsRoot(&macro->acRun), actionsHistory(30), idChecker(jumpIds, blockIds)
{
	ui.setupUi(this);

	QRect screen = QGuiApplication::primaryScreen()->geometry();
	setMaximumSize(screen.size());
	if (Qi::set.editSize.width() >= minimumWidth() && Qi::set.editSize.height() >= minimumHeight())
	{
		Qi::set.editSize.setWidth(std::clamp(Qi::set.editSize.width(), minimumWidth(), screen.width()));
		Qi::set.editSize.setHeight(std::clamp(Qi::set.editSize.height(), minimumHeight(), screen.height()));
		QRect centeredRect(QPoint(0, 0), Qi::set.editSize);
		centeredRect.moveCenter(screen.center());
		setGeometry(centeredRect);
	}

	Forward(QString::fromUtf8("编辑 - ") + macro->name, actionsRoot);
	Init();
	Event();
	StyleGroup();
	Reload();
}

void EditUi::Init()
{
	Init_Bind();

	if ("title")
	{
		ui.title_icon->installEventFilter(this);
		ui.title_label->installEventFilter(this);
	}
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
		ui.action_table->setHorizontalHeaderItem(tableColumn_disable, new QTableWidgetItem("禁用"));
		ui.action_table->setHorizontalHeaderItem(tableColumn_type, new QTableWidgetItem("功能"));
		ui.action_table->setHorizontalHeaderItem(tableColumn_param, new QTableWidgetItem("参数"));
		ui.action_table->setHorizontalHeaderItem(tableColumn_mark, new QTableWidgetItem("备注"));
		ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_debug, QHeaderView::ResizeMode::Fixed);
		ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_disable, QHeaderView::ResizeMode::Fixed);
		ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_type, QHeaderView::ResizeMode::Fixed);
		ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_param, QHeaderView::ResizeMode::Fixed);
		ui.action_table->horizontalHeader()->setSectionResizeMode(tableColumn_mark, QHeaderView::ResizeMode::Stretch);
		ui.action_table->setColumnWidth(tableColumn_debug, 35);
		ui.action_table->setColumnWidth(tableColumn_disable, 50);
		ui.action_table->setColumnWidth(tableColumn_type, 100);
		ui.action_table->setColumnWidth(tableColumn_param, 300);
	}
	if ("context menu")
	{
		menu = new QMenu(this);
		muDel = menu->addAction("删除");
		muCut = menu->addAction("剪切");
		muCopy = menu->addAction("复制");
		muPaste = menu->addAction("粘贴");
		muEdit = menu->addAction("编辑");
		muEdit2 = menu->addAction("编辑2");
		muRedo = menu->addAction("恢复");
		muUndo = menu->addAction("撤销");

		titleMenu = new QMenu(this);
		muBack = titleMenu->addAction("返回");
		muSave = titleMenu->addAction("保存更改");
		muDiscard = titleMenu->addAction("放弃更改");
	}
	if ("button")
	{
		if ("clear shortcut")
		{
			// action add
			BindSafeIter(bind_add_button, [this](QPushButton* p, size_t) { p->installEventFilter(this); });
			BindSafeIter(bind_chg_button, [this](QPushButton* p, size_t) { p->installEventFilter(this); });
			BindSafeIter(bind_edt_button, [this](QPushButton* p, size_t) { p->installEventFilter(this); });
			BindSafeIter(bind_edt2_button, [this](QPushButton* p, size_t) { p->installEventFilter(this); });

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

		QButtonGroup* msgViewRbs = new QButtonGroup(this);
		msgViewRbs->addButton(ui.msgView_type_set_radio);
		msgViewRbs->addButton(ui.msgView_type_add_radio);
		msgViewRbs->addButton(ui.msgView_type_clear_radio);
		msgViewRbs->addButton(ui.msgView_type_show_radio);
		msgViewRbs->addButton(ui.msgView_type_hide_radio);
		ui.msgView_type_set_radio->setChecked(true);

		QButtonGroup* msgViewRbs2 = new QButtonGroup(this);
		msgViewRbs2->addButton(ui.msgView_level_msg_radio);
		msgViewRbs2->addButton(ui.msgView_level_war_radio);
		msgViewRbs2->addButton(ui.msgView_level_err_radio);
		ui.msgView_level_msg_radio->setChecked(true);
	}
	if ("file edit")
	{
		ui.soundPlay_edit->setDialog("选择音频", "音频文件(*.mp3 *.wav)", "C:/Windows/Media");
		ui.soundPlay_edit->setFiles(Qi::ui.sounds);
	}
	if ("line edit range")
	{
		ui.mouse_speed_edit->setValidator(new QIntValidator(QiMouse::range_speed.first, QiMouse::range_speed.second, this));
		ui.mouse_rand_edit->setValidator(new QIntValidator(QiMouse::range_rand.first, QiMouse::range_rand.second, this));
		ui.color_red_edit->setValidator(new QIntValidator(QiColor::range_rgb.first, QiColor::range_rgb.second, this));
		ui.color_green_edit->setValidator(new QIntValidator(QiColor::range_rgb.first, QiColor::range_rgb.second, this));
		ui.color_blue_edit->setValidator(new QIntValidator(QiColor::range_rgb.first, QiColor::range_rgb.second, this));
		ui.color_sim_edit->setValidator(new QIntValidator(QiColor::range_rgb.first, QiColor::range_rgb.second, this));
		ui.image_sim_edit->setValidator(new QIntValidator(QiImage::range_sim.first, QiImage::range_sim.second, this));
		ui.popText_time_edit->setValidator(new QIntValidator(QiPopText::range_time.first, QiPopText::range_time.second, this));
	}
	if ("text edit")
	{
		ui.copyText_textedit->setTabStopDistance(QFontMetrics(QFont("Microsoft YaHei")).horizontalAdvance("    "));
		ui.varOperator_textedit->setTabStopDistance(QFontMetrics(QFont("Microsoft YaHei")).horizontalAdvance("    "));
		ui.msgView_textedit->setTabStopDistance(QFontMetrics(QFont("Microsoft YaHei")).horizontalAdvance("    "));
	}
	if ("key edit")
	{
		ui.key_keyedit->setCombinationMode(false);
		ui.key_keyedit->setDeviceEnabled(true, true, true);
		ui.key_keyedit->setKey(VK_LBUTTON);

		ui.keyState_keyedit->setCombinationMode(false);
		ui.keyState_keyedit->setDeviceEnabled(true, true, true);
#ifdef Q_KEYEDIT_PAD_ENABLED
		ui.keyState_keyedit->setPadEnabled(Qi::set.pad);
#endif
		ui.keyState_keyedit->setKey(VK_LBUTTON);

		ui.keyBlock_keyedit->setCombinationMode(false);
		ui.keyBlock_keyedit->setDeviceEnabled(true, true, true);
		ui.keyBlock_keyedit->setKey(VK_LBUTTON);
	}
	if ("timer")
	{
		testTimer = new QTimer(this);
		markPointTimer = new QTimer(this);
		testTimer->setInterval(32);
		markPointTimer->setInterval(32);
		if (Qi::set.markPoint) markPointTimer->start();
	}
	if ("check")
	{
		ui.tab_lock_check->setChecked(Qi::set.tabLock);
		ui.tab_hideTip_check->setChecked(Qi::set.tabHideTip);
		ui.tab_markPoint_check->setChecked(Qi::set.markPoint);
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
		bind_type_group.at(static_cast<size_t>(QiType::none)) = nullptr;
		bind_type_group.at(static_cast<size_t>(QiType::end)) = ui.grp_end;
		bind_type_group.at(static_cast<size_t>(QiType::delay)) = ui.grp_delay;
		bind_type_group.at(static_cast<size_t>(QiType::key)) = ui.grp_key;
		bind_type_group.at(static_cast<size_t>(QiType::mouse)) = ui.grp_mouse;
		bind_type_group.at(static_cast<size_t>(QiType::copyText)) = ui.grp_copyText;
		bind_type_group.at(static_cast<size_t>(QiType::color)) = ui.grp_color;
		bind_type_group.at(static_cast<size_t>(QiType::loop)) = ui.grp_loop;
		bind_type_group.at(static_cast<size_t>(QiType::loopEnd)) = ui.grp_loopEnd;
		bind_type_group.at(static_cast<size_t>(QiType::keyState)) = ui.grp_keyState;
		bind_type_group.at(static_cast<size_t>(QiType::resetPos)) = ui.grp_resetPos;
		bind_type_group.at(static_cast<size_t>(QiType::image)) = ui.grp_image;
		bind_type_group.at(static_cast<size_t>(QiType::popText)) = ui.grp_popText;
		bind_type_group.at(static_cast<size_t>(QiType::savePos)) = ui.grp_savePos;
		bind_type_group.at(static_cast<size_t>(QiType::timer)) = ui.grp_timer;
		bind_type_group.at(static_cast<size_t>(QiType::jump)) = ui.grp_jump;
		bind_type_group.at(static_cast<size_t>(QiType::jumpPoint)) = ui.grp_jumpPoint;
		bind_type_group.at(static_cast<size_t>(QiType::dialog)) = ui.grp_dialog;
		bind_type_group.at(static_cast<size_t>(QiType::block)) = ui.grp_block;
		bind_type_group.at(static_cast<size_t>(QiType::blockExec)) = ui.grp_blockExec;
		bind_type_group.at(static_cast<size_t>(QiType::quickInput)) = ui.grp_quickInput;
		bind_type_group.at(static_cast<size_t>(QiType::keyBlock)) = ui.grp_keyBlock;
		bind_type_group.at(static_cast<size_t>(QiType::clock)) = ui.grp_clock;
		bind_type_group.at(static_cast<size_t>(QiType::ocr)) = ui.grp_ocr;
		bind_type_group.at(static_cast<size_t>(QiType::varOperator)) = ui.grp_varOperator;
		bind_type_group.at(static_cast<size_t>(QiType::varCondition)) = ui.grp_varCondition;
		bind_type_group.at(static_cast<size_t>(QiType::mouseTrack)) = nullptr;
		bind_type_group.at(static_cast<size_t>(QiType::open)) = ui.grp_open;
		bind_type_group.at(static_cast<size_t>(QiType::textPad)) = ui.grp_textPad;
		bind_type_group.at(static_cast<size_t>(QiType::editDialog)) = ui.grp_editDialog;
		bind_type_group.at(static_cast<size_t>(QiType::volume)) = ui.grp_volume;
		bind_type_group.at(static_cast<size_t>(QiType::soundPlay)) = ui.grp_soundPlay;
		bind_type_group.at(static_cast<size_t>(QiType::msgView)) = ui.grp_msgView;
		bind_type_group.at(static_cast<size_t>(QiType::range)) = ui.grp_range;
	}
	if ("operation button")
	{
		BindSafeIter(bind_type_group, [this](QGroupBox* p, size_t i) {
			for (auto& object : p->children())
			{
				const QString name = object->objectName();
				if (name.indexOf("_add_button") != -1) bind_add_button.at(i) = reinterpret_cast<QPushButton*>(object);
				else if (name.indexOf("_change_button") != -1) bind_chg_button.at(i) = reinterpret_cast<QPushButton*>(object);
				else if (name.indexOf("_edit_button") != -1) bind_edt_button.at(i) = reinterpret_cast<QPushButton*>(object);
				else if (name.indexOf("_edit2_button") != -1) bind_edt2_button.at(i) = reinterpret_cast<QPushButton*>(object);
			}
			});
	}
	if ("group of tab index")
	{
		BindSafeIter(bind_type_group, [this](QGroupBox* p, size_t i) {
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
		connect(ui.title_close_button, &QPushButton::clicked, this, [this] { Qi::popText->Show("正在保存宏"); QTimer::singleShot(32, [] { Qi::widget.editClose(); }); });
		connect(ui.title_back_button, &QPushButton::clicked, this, [this] { Back(); });
		connect(ui.title_run_button, &QPushButton::clicked, this, [this] {
			if (macro->thread.active())
			{
				if (debugState == debug_run)
				{
					macro->thread.stop();
				}
				macro->interpreter->DebugContinue();
			}
			else
			{
				Qi::debug = true;
				timeBeginPeriod(1);
				ui.action_running_radio->isChecked() ? macro->thread.run_start(macro) : macro->thread.end_start(macro);
				testTimer->start();
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
	if ("record")
	{
		connect(ui.rec_button, &QPushButton::clicked, this, [this] {
			Qi::widget.dialogActive = true;
			hide();
			RecordUi rw(nullptr);
			Macro macro = rw.Start();
			if (macro.acRun) actions->append(std::move(macro.acRun));
			show();
			Qi::widget.dialogActive = false;
			Reload();
			});
		connect(ui.rec_window_button, &QPushButton::clicked, this, [this] {
			Qi::widget.dialogActive = true;
			hide();
			WndInfo wndInfo = QiFn::WindowSelection();
			if (wndInfo.wnd)
			{
				RecordUi rw(&wndInfo);
				Macro macro = rw.Start();
				if (macro.acRun) actions->append(std::move(macro.acRun));
			}
			else Qi::popText->Popup(2000, "窗口已失效");
			show();
			Qi::widget.dialogActive = false;
			Reload();
			});
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
		// actions
		connect(ui.action_running_radio, &QRadioButton::toggled, this, [this](bool state) {
			if (state && (layers.size() == 1))
			{
				layers.first().actions = actionsRoot = actions = &macro->acRun;
				Reload();
			}
			});
		connect(ui.action_ending_radio, &QRadioButton::toggled, this, [this](bool state) {
			if (state && (layers.size() == 1))
			{
				layers.first().actions = actionsRoot = actions = &macro->acEnd;
				Reload();
			}
			});
		// mark
		connect(ui.action_table, &QTableWidget::cellChanged, this, [this](int row, int column) {
			if (updating) return;
			if (row < 0) return;
			if (column != tableColumn_mark) return;
			QiBase& base = actions->at(row).base();
			base.mark = ui.action_table->item(row, tableColumn_mark)->text();
			if (base.type == QiType::jumpPoint) ListJumpPointReload();
			else if (base.type == QiType::block) ListBlockReload();
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
					actionsRoot->iter([](Action& action) { if (action.base().debug_entry) action.base().debug_entry = false; return false; });
					base.debug_entry = true;
					base.debug_break = base.debug_exit = false;
				}
				else if (Input::state(VK_SHIFT))
				{
					actionsRoot->iter([](Action& action) { if (action.base().debug_exit) action.base().debug_exit = false; return false; });
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
		connect(ui.action_table, &QTableWidget::cellDoubleClicked, this, [this](int row, int) { if (row < 0) return; ItemSet(row); });
		// context menu
		connect(ui.action_table, &QTableWidget::customContextMenuRequested, this, [this] {
			if ("set menu item state")
			{
				muPaste->setEnabled(Qi::clipboard.not_empty());
				muRedo->setEnabled(actionsHistory.canRedo());
				muUndo->setEnabled(actionsHistory.canUndo());
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
	if ("context menu")
	{
		connect(muDel, &QAction::triggered, this, [this] { ItemDel(); });
		connect(muCut, &QAction::triggered, this, [this] { ItemCut(); });
		connect(muCopy, &QAction::triggered, this, [this] { ItemCopy(); });
		connect(muPaste, &QAction::triggered, this, [this] { ItemPaste(); });
		connect(muEdit, &QAction::triggered, this, [this] { NextEdit(false); });
		connect(muEdit2, &QAction::triggered, this, [this] { NextEdit(true); });
		connect(muRedo, &QAction::triggered, this, [this] { Redo(); });
		connect(muUndo, &QAction::triggered, this, [this] { Undo(); });

		connect(muBack, &QAction::triggered, this, [this]() { Back(); });
		connect(muSave, &QAction::triggered, this, [this]() { Qi::popText->Show("正在保存宏"); QTimer::singleShot(32, [] { Qi::widget.editClose(); }); });
		connect(muDiscard, &QAction::triggered, this, [this]() { Exit(false); });
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
		connect(testTimer, &QTimer::timeout, this, [this] {
			if (macro->thread.active())
			{
				if (Input::state(VK_F10))
				{
					if (Input::state(VK_SHIFT)) macro->thread.stop();
					macro->interpreter->DebugContinue();
					SetDebugState(debug_run);
					Input::Loop(VK_F10, 1);
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
			});
		connect(markPointTimer, &QTimer::timeout, this, [this] {
			if (isHidden()) return;
			static bool isPress;
			if (GetAsyncKeyState(VK_SHIFT))
			{
				if (!isPress)
				{
					isPress = true;
					POINT cur; GetCursorPos(&cur);
					widget_mkpv.Show(cur, 30, 2, QColor(0, 160, 0));
				}
			}
			else if (isPress) isPress = false;
			});
	}
}
// TODO: new widgets
void EditUi::Event_Action_Widget()
{
	// buttons
	BindSafeIter(bind_add_button, [this](QPushButton*, size_t i) { connect(bind_add_button[i], &QPushButton::clicked, this, [this, i] { ItemAdd(static_cast<QiType>(i)); }); });
	BindSafeIter(bind_chg_button, [this](QPushButton*, size_t i) { connect(bind_chg_button[i], &QPushButton::clicked, this, [this, i] { ItemChange(static_cast<QiType>(i)); }); });
	BindSafeIter(bind_edt_button, [this](QPushButton*, size_t i) { connect(bind_edt_button[i], &QPushButton::clicked, this, [this, i] { NextEdit(false); }); });
	BindSafeIter(bind_edt2_button, [this](QPushButton*, size_t i) { connect(bind_edt2_button[i], &QPushButton::clicked, this, [this, i] { NextEdit(true); }); });
	// check
	connect(ui.tab_lock_check, &QCheckBox::toggled, this, [this](bool state) {
		Qi::set.tabLock = state;
		QiJson::SaveJson();
		});
	connect(ui.tab_hideTip_check, &QCheckBox::toggled, this, [this](bool state) { DisableTip(Qi::set.tabHideTip = state); QiJson::SaveJson(); });
	connect(ui.tab_markPoint_check, &QCheckBox::toggled, this, [this](bool state) {
		if (Qi::set.markPoint = state)
		{
			markPointTimer->start();
		}
		else
		{
			markPointTimer->stop();
			widget_mkpv.hide();
		}
		QiJson::SaveJson();
		});
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
			pt = QiCvt::WP_RtA({ pt.x, pt.y }, macro->wndInfo.wnd, macro->range);
		}
		else
		{
			pt = ps.Start();
			pt = QiCvt::SP_RtA({ pt.x, pt.y }, macro->range);
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
	connect(ui.delay_min_edit, &QLineEdit::textEdited, this, [this](const QString& text) { ui.delay_max_edit->setText(text); });
	// loop
	connect(ui.loop_min_edit, &QLineEdit::textEdited, this, [this](const QString& text) { ui.loop_max_edit->setText(text); });
	// timer
	connect(ui.timer_min_edit, &QLineEdit::textEdited, this, [this](const QString& text) { ui.timer_max_edit->setText(text); });
	// keyBlock
	connect(ui.keyBlock_move_check, &QCheckBox::toggled, this, [this](bool check) { if (check)ui.keyBlock_all_check->setChecked(false); });
	connect(ui.keyBlock_all_check, &QCheckBox::toggled, this, [this](bool check) { if (check)ui.keyBlock_move_check->setChecked(false); });
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
			rect = QiCvt::WR_RtA(rect, macro->wndInfo.wnd, macro->range);
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
			rect = QiCvt::SR_RtA(rect, macro->range);
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
		QColor c = cs.Start();
		QColorDialog cd(c, this);
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
			rect = QiCvt::WR_RtA(rect, macro->wndInfo.wnd, macro->range);
		}
		else
		{
			rect = rs.Start();
			rect = QiCvt::SR_RtA(rect, macro->range);
		}
		QiImage image(WidgetGetImage());
		image.rect = rect;
		WidgetSet(image);
		});
	connect(ui.image_shot_button, &QPushButton::clicked, this, [this] {
		QRectSelection rs;
		RECT rect = rs.Start();
		if (rect.left && rect.top && rect.right && rect.bottom)
		{
			QiImage image(WidgetGetImage());
			Image::ScreenRgbMap(imageMap, rect);
			image.map = imageMap;
			WidgetSet(image);
		}
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
			rect = QiCvt::WR_RtA(rect, macro->wndInfo.wnd, macro->range);
		}
		else
		{
			rect = rs.Start();
			rect = QiCvt::SR_RtA(rect, macro->range);
		}
		QiOcr ocr(WidgetGetOcr());
		ocr.rect = rect;
		WidgetSet(ocr);
		});
	connect(ui.ocr_test_button, &QPushButton::clicked, this, [this] {
		if (!Qi::ocr.valid())
		{
			MsgBox::Warning(L"没有安装OCR组件，无法使用文字识别功能");
			return;
		}
		const QiOcr& ocr = WidgetGetOcr();
		std::string text = Qi::ocr.scan(QiCvt::SR_AtR(ocr.rect, macro->range), ocr.row);
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
		td.button()->setStyleSheet("background-color:white;border:1px solid gray;border-radius:4px");
		ui.textPad_edit->setText(td.Start(ui.textPad_edit->text()));
		});
	// varOperator
	connect(ui.varOperator_test_button, &QPushButton::clicked, this, [this] {
		setDisabled(true);
		varop = std::async([code = ui.varOperator_textedit->toPlainText().toStdString(), pMacro = macro]() {
			try
			{
				pMacro->script_interpreter.interpretAll(code);
			}
			catch (std::exception e)
			{
				QiScriptInterpreter::showError(e.what());
			}
			Qi::widget.editVaropStop();
			});
		});
	// msgWindow
	connect(ui.msgView_level_msg_radio, &QRadioButton::clicked, [this] { ui.msgView_type_add_radio->setChecked(true); });
	connect(ui.msgView_level_war_radio, &QRadioButton::clicked, [this] { ui.msgView_type_add_radio->setChecked(true); });
	connect(ui.msgView_level_err_radio, &QRadioButton::clicked, [this] { ui.msgView_type_add_radio->setChecked(true); });
	// range
	connect(ui.range_rect_button, &QPushButton::clicked, this, [this] {
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
			rect = QiCvt::WR_RtA(rect, macro->wndInfo.wnd);
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
			rect = QiCvt::SR_RtA(rect);
		}
		QiRangeSet range;
		range.rect = rect;
		WidgetSet(range);
		});
	connect(ui.range_wnd_button, &QPushButton::clicked, this, [this] {
		WndInfo w = QiFn::WindowSelection();
		if (w.wnd)
		{
			QiRangeSet range;
			range.rect = QiCvt::SR_RtA(Window::rect(w.wnd));
			range.wnd = w.wndName;
			range.var = ui.range_id_edit->text();
			WidgetSet(range);
		}
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
		DisableChangeButtons(tableCurrent.empty());
		widget_pv.hide();
		widget_rv.hide();
		widget_td.hide();
		if (!ui.tab_range_check->isChecked()) range_rv.hide();

		if (tableCurrent.size() == 1)
		{
			const Action& var = actions->at(tableCurrent.front());

			if (!ui.tab_lock_check->isChecked()) SetTabCurrent(bind_type_tab.at(var.index()));
			SetGroupCurrent(var.type());
			SetEditCurrent(var.type());

			switch (var.type())
			{
			case QiType::mouse:
			{
				const QiMouse& ref = var.to<QiMouse>();
				if (!ref.move)
				{
					POINT pt;
					if (macro->wndState)
					{
						if (!macro->wndInfo.update()) SelectWindow();
						POINT rpt = QiCvt::WP_AtR({ ref.x, ref.y }, macro->wndInfo.wnd, macro->range);
						pt = Window::pos(macro->wndInfo.wnd);
						pt.x += rpt.x, pt.y += rpt.y;
					}
					else pt = QiCvt::SP_AtR({ ref.x, ref.y }, macro->range);
					widget_pv.Show(pt);
				}
			} break;
			case QiType::color:
			{
				const QiColor& ref = var.to<QiColor>();
				RECT rect;
				if (macro->wndState)
				{
					if (!macro->wndInfo.update()) SelectWindow();
					rect = QiCvt::WR_AtR(ref.rect, macro->wndInfo.wnd, macro->range);
					POINT pt = Window::pos(macro->wndInfo.wnd);
					rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
				}
				else rect = QiCvt::SR_AtR(ref.rect, macro->range);
				widget_rv.Show(rect);
			} break;
			case QiType::image:
			{
				const QiImage& ref = var.to<QiImage>();
				RECT rect;
				if (macro->wndState)
				{
					if (!macro->wndInfo.update()) SelectWindow();
					rect = QiCvt::WR_AtR(ref.rect, macro->wndInfo.wnd, macro->range);
					POINT pt = Window::pos(macro->wndInfo.wnd);
					rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
				}
				else rect = QiCvt::SR_AtR(ref.rect, macro->range);
				widget_rv.Show(rect);
				WidgetSet(ref);
			} break;
			case QiType::ocr:
			{
				const QiOcr& ref = var.to<QiOcr>();
				RECT rect;
				if (macro->wndState)
				{
					if (!macro->wndInfo.update()) SelectWindow();
					rect = QiCvt::WR_AtR(ref.rect, macro->wndInfo.wnd, macro->range);
					POINT pt = Window::pos(macro->wndInfo.wnd);
					rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
				}
				else rect = QiCvt::SR_AtR(ref.rect, macro->range);
				widget_rv.Show(rect);
			} break;
			case QiType::textPad:
			{
				const QiTextPad& ref = var.to<QiTextPad>();
				widget_td.edit()->setPlainText(ref.text);
				widget_td.show();
			} break;
			case QiType::range:
			{
				const QiRangeSet& ref = var.to<QiRangeSet>();
				macro->range = ref.rect;
				RECT rect;
				if (macro->wndState)
				{
					if (!macro->wndInfo.update()) SelectWindow();
					rect = QiCvt::WR_AtR(ref.rect, macro->wndInfo.wnd);
					POINT pt = Window::pos(macro->wndInfo.wnd);
					rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
				}
				else rect = QiCvt::SR_AtR(ref.rect);
				range_rv.Show(rect, 2, QColor(0, 127, 192));
			} break;
			}
		}
		else
		{
			SetGroupCurrent();
			SetEditCurrent();
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
		ui.content_widget->setProperty("group", "edit-client");
		ui.title_run_button->setProperty("group", "title-title_run_button");
		ui.title_back_button->setProperty("group", "title-back_button");
		ui.title_close_button->setProperty("group", "title-close_button");
	}
	if ("tool button")
	{
		ui.window_select_button->setProperty("group", "get_button");
	}
	if ("operation button")
	{

		BindSafeIter(bind_add_button, [this](QPushButton* p, size_t) { p->setProperty("group", "edit-add_button"); });
		BindSafeIter(bind_chg_button, [this](QPushButton* p, size_t) { p->setProperty("group", "edit-add_button"); });
		BindSafeIter(bind_edt_button, [this](QPushButton* p, size_t) { p->setProperty("group", "edit-edit_button"); });
		BindSafeIter(bind_edt2_button, [this](QPushButton* p, size_t) { p->setProperty("group", "edit-edit_button"); });
		BindSafeIter(bind_tab_button, [this](QPushButton* p, size_t) { p->setProperty("group", "edit-tab_button"); });
		ui.rec_button->setProperty("group", "edit-add_button");
		ui.rec_window_button->setProperty("group", "edit-add_button");
		ui.ocr_test_button->setProperty("group", "edit-add_button");
		ui.varOperator_test_button->setProperty("group", "edit-add_button");
		ui.range_wnd_button->setProperty("group", "edit-add_button");
	}
	if ("check box")
	{
		ui.tab_lock_check->setProperty("group", "check");
		ui.tab_hideTip_check->setProperty("group", "check");
		ui.tab_markPoint_check->setProperty("group", "check");
		ui.tab_range_check->setProperty("group", "check");

		ui.window_state_check->setProperty("group", "check");
		ui.window_child_check->setProperty("group", "check");
	}
	if ("radio button")
	{
		ui.action_running_radio->setProperty("group", "radio");
		ui.action_ending_radio->setProperty("group", "radio");
	}
	if ("tab widget")
	{
		ui.tab_stacked_widget->setProperty("group", "tab_widget");
	}
	if ("list")
	{
		ui.jumpPoint_list->setProperty("group", "action_table_header");
		ui.block_list->setProperty("group", "action_table_header");
	}
	if ("table")
	{
		ui.action_table->setProperty("group", "action_table");
		ui.action_table->horizontalHeader()->setProperty("group", "action_table_header");
		ui.action_table->verticalHeader()->setProperty("group", "action_table_header");
		ui.action_table->setStyleSheet("QTableCornerButton::section,QHeaderView::section,QScrollBar,QScrollBar::sub-line,QScrollBar::add-line{background-color:rgba(0,0,0,0);border:none}QScrollBar::handle{background-color:rgba(128,128,128,0.3);border:none}");
	}
	if ("menu")
	{
		menu->setProperty("group", "context_menu");
		titleMenu->setProperty("group", "context_menu");
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
				box->setContentsMargins(0, 0, 0, 0);
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

	if ("all other widget in GroupBox")
	{
		BindSafeIter(bind_type_group, [this](QGroupBox* p, size_t i) {
			for (auto& object : p->children())
			{
				if (!object->property("group").isNull()) continue;
				const QString name = object->objectName();
				if (name.indexOf("_check") != -1) object->setProperty("group", "check");
				else if (name.indexOf("_radio") != -1) object->setProperty("group", "radio");
				else if (name.indexOf("_edit") != -1) object->setProperty("group", "line_edit");
				else if (name.indexOf("_textedit") != -1) object->setProperty("group", "text_edit");
				else if (name.indexOf("_keyedit") != -1) object->setProperty("group", "key_edit");
				else if (name.indexOf("_button") != -1) object->setProperty("group", "get_button");
			}
			});
	}
}


void EditUi::Disable(bool disable)
{
	ui.action_running_radio->setDisabled(disable);
	ui.action_ending_radio->setDisabled(disable);
	ui.title_close_button->setDisabled(disable);
	ui.title_back_button->setDisabled(disable);
	ui.content_widget->setDisabled(disable);
}
void EditUi::DisableTip(bool disable)
{
	BindSafeIter(bind_type_group, [this, disable](QGroupBox* p, size_t) {p->setToolTipDuration(disable ? 1 : 0); });
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
	BindSafeIter(bind_chg_button, [this, disable](QPushButton* p, size_t) { p->setDisabled(disable); });
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
void EditUi::SetGroupCurrent(QiType type)
{
	BindSafeIter(bind_type_group, [this, type](QGroupBox* p, size_t i) {
		if (i == static_cast<size_t>(type)) p->setStyleSheet("QGroupBox{border:1px solid red;border-radius:5px}");
		else p->setStyleSheet("QGroupBox{border:1px solid gray;border-radius:5px}");
		});
}
void EditUi::SetEditCurrent(QiType type)
{
	BindSafeIter(bind_edt_button, [this, type](QPushButton* p, size_t i) {
		if (i == static_cast<size_t>(type)) p->setEnabled(true);
		else p->setDisabled(true);
		});
	BindSafeIter(bind_edt2_button, [this, type](QPushButton* p, size_t i) {
		if (i == static_cast<size_t>(type)) p->setEnabled(true);
		else p->setDisabled(true);
		});
}
void EditUi::SetDebugState(int debugState)
{
	this->debugState = debugState;
	if (debugState == debug_idel)
	{
		ui.title_run_label->setText("运行");
		ui.title_run_button->setStyleSheet("QPushButton{background-color:#0E0;border-radius:10px}QPushButton:hover{background-color:#0C0}");
		Disable(false);
	}
	else if (debugState == debug_run)
	{
		ui.title_run_label->setText("停止(Shift F10)");
		ui.title_run_button->setStyleSheet("QPushButton{background-color:#E00;border-radius:10px}QPushButton:hover{background-color:#C00}");
		Disable(true);
	}
	else if (debugState == debug_pause)
	{
		ui.title_run_label->setText("继续(F10) 停止(Shift F10)");
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
	idChecker.load(*actionsRoot);
	TableReload();
	WidgetReload();
}
void EditUi::WidgetReload()
{
	tableCurrent.clear();
	tableCurrentPrev.clear();
	DisableMenus();
	DisableChangeButtons(true);
	SetGroupCurrent();
	SetEditCurrent();
	ListJumpPointReload();
	ListBlockReload();
}
// TODO: new action's next name
void EditUi::NextEdit(bool edit2)
{
	int p = ui.action_table->currentRow(); if (p < 0) return;
	QString title;
	Actions* next = nullptr;
	Action& var = actions->at(p);
	switch (var.type())
	{
	case QiType::color:
	{
		QiColor& color = var.to<QiColor>();
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
		QiLoop& loop = var.to<QiLoop>();
		next = &loop.next;
		title = "编辑 - 循环";
	} break;
	case QiType::keyState:
	{
		QiKeyState& keyState = var.to<QiKeyState>();
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
		QiImage& image = var.to<QiImage>();
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
		QiTimer& timer = var.to<QiTimer>();
		if (edit2)
		{
			next = &timer.next2;
			title = "编辑 - 定时（超时）";
		}
		else
		{
			next = &timer.next;
			title = "编辑 - 定时（循环）";
		}
	} break;
	case QiType::dialog:
	{
		QiDialog& dialog = var.to<QiDialog>();
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
		QiBlock& block = var.to<QiBlock>();
		next = &block.next;
		title = "编辑 - 块";
	} break;
	case QiType::clock:
	{
		QiClock& clock = var.to<QiClock>();
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
		QiOcr& ocr = var.to<QiOcr>();
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
		QiVarCondition& varCondition = var.to<QiVarCondition>();
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
		QiVolume& volume = var.to<QiVolume>();
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
	if (next) Forward(title, next);
}


// TODO: new action's table item
void EditUi::TableUpdate(int index)
{
	updating = true;
	const Action& var = actions->at(index);
	QString type = var.base().name();
	QString param;
	QColor color(0, 0, 0, 0);
	switch (var.type())
	{
	case QiType::end:
	{
		type += QiUi::Symbol::Stop;
	} break;
	case QiType::delay:
	{
		const QiDelay& ref = var.to<QiDelay>();
		type += QiUi::Symbol::Time;
		QString min = ref.v_min.isEmpty() ? QString::number(ref.min) : ref.v_min;
		QString max = ref.v_max.isEmpty() ? QString::number(ref.max) : ref.v_max;
		if (min == max) param = min;
		else param = min + QString(" ~ ") + max;
	} break;
	case QiType::key:
	{
		const QiKey& ref = var.to<QiKey>();
		if (ref.state == QiKey::up) type += QiUi::Symbol::Up;
		else if (ref.state == QiKey::down) type += QiUi::Symbol::Down;
		else if (ref.state == QiKey::click) type += QiUi::Symbol::Turn;
		param = QKeyEdit::keyName(ref.vk);
	} break;
	case QiType::mouse:
	{
		const QiMouse& ref = var.to<QiMouse>();
		if (ref.move) type += QiUi::Symbol::Move;
		else type += QiUi::Symbol::Left;
		param = (ref.v_x.isEmpty() ? QString::number(ref.x) : ref.v_x)
			+ " , " + (ref.v_y.isEmpty() ? QString::number(ref.y) : ref.v_y);
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
		const QiCopyText& ref = var.to<QiCopyText>();
		type += QiUi::Symbol::Text;
		param = QiFn::FoldText(ref.text);
	} break;
	case QiType::color:
	{
		const QiColor& ref = var.to<QiColor>();
		type += QiUi::Symbol::Color;
		param = (ref.v_left.isEmpty() ? QString::number(ref.rect.left) : ref.v_left)
			+ "," + (ref.v_top.isEmpty() ? QString::number(ref.rect.top) : ref.v_top)
			+ "," + (ref.v_right.isEmpty() ? QString::number(ref.rect.right) : ref.v_right)
			+ "," + (ref.v_bottom.isEmpty() ? QString::number(ref.rect.bottom) : ref.v_bottom)
			+ " | " + QString::number(ref.rgbe.r)
			+ "," + QString::number(ref.rgbe.g)
			+ "," + QString::number(ref.rgbe.b)
			+ "," + QString::number(ref.rgbe.a)
			+ (ref.move ? " 移动" : "");
		color = QColor(ref.rgbe.r, ref.rgbe.g, ref.rgbe.b, 255);
	} break;
	case QiType::loop:
	{
		const QiLoop& ref = var.to<QiLoop>();
		type += QiUi::Symbol::Loop;

		QString min = ref.v_min.isEmpty() ? QString::number(ref.min) : ref.v_min;
		QString max = ref.v_max.isEmpty() ? QString::number(ref.max) : ref.v_max;

		if (min == '0' && max == '0') param = "无限";
		else if (min == max) param = min;
		else param = min + QString(" ~ ") + max;
	} break;
	case QiType::loopEnd:
	{
		type += QiUi::Symbol::Stop;
	} break;
	case QiType::keyState:
	{
		const QiKeyState& ref = var.to<QiKeyState>();
		type += QiUi::Symbol::Stop;

		param += QKeyEdit::keyName(ref.vk);
	} break;
	case QiType::resetPos:
	{
		type += QiUi::Symbol::Turn;
	} break;
	case QiType::image:
	{
		const QiImage& ref = var.to<QiImage>();
		type += QiUi::Symbol::Image;
		param = (ref.v_left.isEmpty() ? QString::number(ref.rect.left) : ref.v_left)
			+ "," + (ref.v_top.isEmpty() ? QString::number(ref.rect.top) : ref.v_top)
			+ "," + (ref.v_right.isEmpty() ? QString::number(ref.rect.right) : ref.v_right)
			+ "," + (ref.v_bottom.isEmpty() ? QString::number(ref.rect.bottom) : ref.v_bottom)
			+ " | " + QString::number(ref.map.width())
			+ "," + QString::number(ref.map.height())
			+ " | " + QString::number(ref.sim)
			+ (ref.move ? " 移动" : "");
	} break;
	case QiType::popText:
	{
		const QiPopText& ref = var.to<QiPopText>();
		type += QiUi::Symbol::Text;
		param = QiFn::FoldText(ref.text, 16);
		param += " |　时长：";
		param += QString::number(ref.time);
		if (ref.sync) param += " 等待";
	} break;
	case QiType::savePos:
	{
		type += QiUi::Symbol::Turn;
	} break;
	case QiType::timer:
	{
		const QiTimer& ref = var.to<QiTimer>();
		type += QiUi::Symbol::Loop;

		QString min = ref.v_min.isEmpty() ? QString::number(ref.min) : ref.v_min;
		QString max = ref.v_max.isEmpty() ? QString::number(ref.max) : ref.v_max;

		if (min == max) param = min;
		else param = min + QString(" ~ ") + max;
	} break;
	case QiType::jump:
	{
		const QiJump& ref = var.to<QiJump>();
		type += QiUi::Symbol::Jump;

		if (ref.id < 1)
		{
			param = "无效锚点id";
		}
		else
		{
			param = "id：";
			param += QString::number(ref.id);

			if (jumpIds.exist(ref.id))
			{
				const Action* ac = actionsRoot->find(QiType::jumpPoint, ref.id);
				if (ac && !ac->base().mark.isEmpty()) param += QString(" ") + ac->base().mark;
			}
			else
			{
				param += " (无效)";
			}
		}
	} break;
	case QiType::jumpPoint:
	{
		const QiJumpPoint& ref = var.to<QiJumpPoint>();
		type += QiUi::Symbol::Point;

		param = "id：";
		param += QString::number(ref.id);
	} break;
	case QiType::dialog:
	{
		const QiDialog& ref = var.to<QiDialog>();
		type += QiUi::Symbol::Text;
		param = QiFn::FoldText(ref.title, 8);
		param += " | ";
		param += QiFn::FoldText(ref.text, 12);
	} break;
	case QiType::block:
	{
		const QiBlock& ref = var.to<QiBlock>();
		type += QiUi::Symbol::Block;

		param = "id：";
		param += QString::number(ref.id);
	} break;
	case QiType::blockExec:
	{
		const QiBlockExec& ref = var.to<QiBlockExec>();
		type += QiUi::Symbol::BlockExec;

		if (ref.id < 1)
		{
			param = "无效块id";
		}
		else
		{
			param = "id：";
			param += QString::number(ref.id);

			if (blockIds.exist(ref.id))
			{
				const Action* ac = actionsRoot->find(QiType::block, ref.id);
				if (ac && !ac->base().mark.isEmpty()) param += QString(" ") + ac->base().mark;
			}
			else
			{
				param += " (无效)";
			}
		}
	} break;
	case QiType::quickInput:
	{
		const QiQuickInput& ref = var.to<QiQuickInput>();
		type += QiUi::Symbol::Text;
		param = QiFn::FoldText(ref.text);
	} break;
	case QiType::keyBlock:
	{
		const QiKeyBlock& ref = var.to<QiKeyBlock>();
		type += QiUi::Symbol::Stop;

		if (ref.block) param = "屏蔽：";
		else param = "解除：";
		if (ref.vk == QiKeyBlock::all) param += "全部输入";
		else if (ref.vk == QiKeyBlock::move) param += "鼠标移动";
		else param += QKeyEdit::keyName(ref.vk);

	} break;
	case QiType::clock:
	{
		const QiClock& ref = var.to<QiClock>();
		type += QiUi::Symbol::Time;

		param = QString::fromStdString(QiTime::toString(ref.time));
	} break;
	case QiType::ocr:
	{
		const QiOcr& ref = var.to<QiOcr>();
		type += QiUi::Symbol::Text;
		param = (ref.v_left.isEmpty() ? QString::number(ref.rect.left) : ref.v_left)
			+ "," + (ref.v_top.isEmpty() ? QString::number(ref.rect.top) : ref.v_top)
			+ "," + (ref.v_right.isEmpty() ? QString::number(ref.rect.right) : ref.v_right)
			+ "," + (ref.v_bottom.isEmpty() ? QString::number(ref.rect.bottom) : ref.v_bottom);
		if (!ref.text.isEmpty()) param += (ref.match ? " 匹配：" : " 搜索：") + QiFn::FoldText(ref.text, 12);
	} break;
	case QiType::varOperator:
	{
		const QiVarOperator& ref = var.to<QiVarOperator>();
		type += QiUi::Symbol::Equal;
		param = QiFn::FoldText(ref.script);
	} break;
	case QiType::varCondition:
	{
		const QiVarCondition& ref = var.to<QiVarCondition>();
		type += QiUi::Symbol::Var;
		param = QiFn::FoldText(ref.script);
	} break;
	case QiType::mouseTrack:
	{
		const QiMouseTrack& ref = var.to<QiMouseTrack>();
		type += QiUi::Symbol::Track;

		param = QString::number(ref.s.size());
	} break;
	case QiType::open:
	{
		const QiOpen& ref = var.to<QiOpen>();
		type += QiUi::Symbol::Link;
		param = QiFn::FoldText(ref.url, 20, true);
	} break;
	case QiType::textPad:
	{
		const QiTextPad& ref = var.to<QiTextPad>();
		type += QiUi::Symbol::Text;
		param = QiFn::FoldText(ref.text);
	} break;
	case QiType::editDialog:
	{
		const QiEditDialog& ref = var.to<QiEditDialog>();
		type += QiUi::Symbol::Text;
		param = QiFn::FoldText(ref.title, 4);
		param += " | ";
		param += QiFn::FoldText(ref.text, 8);
		param += " | ";
		param += QiFn::FoldText(ref.var, 8);
	} break;
	case QiType::volume:
	{
		const QiVolume& ref = var.to<QiVolume>();
		type += QiUi::Symbol::Speaker;
		param = ref.max ? "最大：" : "平均：";
		param += QString::number(ref.volume);
		param += ", ";
		param += QString::number(ref.time);
	} break;
	case QiType::soundPlay:
	{
		const QiSoundPlay& ref = var.to<QiSoundPlay>();
		type += QiUi::Symbol::Speaker;
		if (ref.state == QiSoundPlay::play)
		{
			param = QiFn::FoldText(ref.file, 18, true);
			if (ref.sync) param += " 等待";
		}
		else if (ref.state == QiSoundPlay::pause) param = "暂停";
		else if (ref.state == QiSoundPlay::resume) param = "继续";
		else if (ref.state == QiSoundPlay::stop) param = "停止";
	} break;
	case QiType::msgView:
	{
		const QiMsgView& ref = var.to<QiMsgView>();
		type += QiUi::Symbol::Text;
		if (ref.option == QiMsgView::set) param = QString("设置：") + QiFn::FoldText(ref.text, 16, true);
		else if (ref.option == QiMsgView::add) param = QString("添加：") + QiFn::FoldText(ref.text, 16, true);
		else if (ref.option == QiMsgView::clear) param = "清空";
		else if (ref.option == QiMsgView::show) param = "显示";
		else if (ref.option == QiMsgView::hide) param = "隐藏";
	} break;
	case QiType::range:
	{
		const QiRangeSet& ref = var.to<QiRangeSet>();
		type += QiUi::Symbol::Range;
		if (!ref.var.isEmpty()) param = ref.var;
		else if (!ref.wnd.isEmpty()) param = ref.wnd;
		else param = QString::number(ref.rect.left)
			+ "," + QString::number(ref.rect.top)
			+ "," + QString::number(ref.rect.right)
			+ "," + QString::number(ref.rect.bottom);
	} break;
	}

	for (size_t i = 0; i < tableColumn_count; i++)
	{
		QTableWidgetItem* item = ui.action_table->item(index, i);
		if (!item)
		{
			item = new QTableWidgetItem();
			ui.action_table->setItem(index, i, item);
		}
		item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
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

		ui.action_table->item(index, tableColumn_param)->setText(param);
		if (color.alpha() == 255) ui.action_table->item(index, tableColumn_param)->setForeground(color);

		ui.action_table->item(index, tableColumn_mark)->setText(var.base().mark);
	}
	updating = false;
}
void EditUi::TableUpdate(int begin, int end)
{
	int b = qMin(begin, end);
	int e = qMax(begin, end);
	for (size_t i = b; i <= e; i++) TableUpdate(i);
}
void EditUi::TableReload()
{
	ui.action_table->setRowCount(actions->size());
	for (size_t i = 0; i < ui.action_table->rowCount(); i++) TableUpdate(i);
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
	if (selection.empty()) return;
	if (selection.size() == 1)
	{
		ui.action_table->setCurrentCell(selection.front(), 0);
		return;
	}
	QItemSelectionModel* selectionModel = ui.action_table->selectionModel();
	QItemSelection itemSelection;
	for (auto& row : selection)
	{
		if (row >= 0 && row < ui.action_table->rowCount())
		{
			for (int col = 0; col < ui.action_table->columnCount(); col++)
			{
				QModelIndex index = ui.action_table->model()->index(row, col);
				itemSelection.select(index, index);
			}
		}
	}
	selectionModel->select(itemSelection, QItemSelectionModel::Select);
}


// TODO: new action's widget get
Action EditUi::ItemGet(QiType type)
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
	case QiType::msgView: return WidgetGetMsgView();
	case QiType::range: return WidgetGetRange();
	}
	return Action();
}
// TODO: new action's widget set
void EditUi::ItemSet(int p)
{
	const Action& var = actions->at(p);
	switch (var.type())
	{
	case QiType::delay: WidgetSet(var.to<QiDelay>()); break;
	case QiType::key: WidgetSet(var.to<QiKey>()); break;
	case QiType::mouse: WidgetSet(var.to<QiMouse>()); break;
	case QiType::copyText: WidgetSet(var.to<QiCopyText>()); break;
	case QiType::color: WidgetSet(var.to<QiColor>()); break;
	case QiType::loop: WidgetSet(var.to<QiLoop>()); break;
	case QiType::keyState: WidgetSet(var.to<QiKeyState>()); break;
	case QiType::image: WidgetSet(var.to<QiImage>()); break;
	case QiType::popText: WidgetSet(var.to<QiPopText>()); break;
	case QiType::timer: WidgetSet(var.to<QiTimer>()); break;
	case QiType::dialog: WidgetSet(var.to<QiDialog>()); break;
	case QiType::quickInput: WidgetSet(var.to<QiQuickInput>()); break;
	case QiType::keyBlock: WidgetSet(var.to<QiKeyBlock>()); break;
	case QiType::clock: WidgetSet(var.to<QiClock>()); break;
	case QiType::ocr: WidgetSet(var.to<QiOcr>()); break;
	case QiType::varOperator: WidgetSet(var.to<QiVarOperator>()); break;
	case QiType::varCondition: WidgetSet(var.to<QiVarCondition>()); break;
	case QiType::open: WidgetSet(var.to<QiOpen>()); break;
	case QiType::textPad: WidgetSet(var.to<QiTextPad>()); break;
	case QiType::editDialog: WidgetSet(var.to<QiEditDialog>()); break;
	case QiType::volume: WidgetSet(var.to<QiVolume>()); break;
	case QiType::soundPlay: WidgetSet(var.to<QiSoundPlay>()); break;
	case QiType::msgView: WidgetSet(var.to<QiMsgView>()); break;
	case QiType::range: WidgetSet(var.to<QiRangeSet>()); break;
	}
}
void EditUi::ItemMove(bool up, int len)
{
	int count = ui.action_table->rowCount();
	if (count < 1) return;
	if (tableCurrent.not_empty())
	{
		int old_min = tableCurrent.front();
		int old_max = tableCurrent.back();

		QiVector<int> after;
		int new_min = INT_MAX;
		int new_max = INT_MIN;

		for (int i = 0, p = 0, t = 0, over = 0; i < tableCurrent.size(); i++)
		{
			if (up)
			{
				over = 0 - (tableCurrent.front() - len);
				if (over < 0) over = 0;
				p = tableCurrent[i];
				t = p - len + over;

			}
			else
			{
				over = count - (tableCurrent.back() + len + 1);
				if (over > 0) over = 0;
				p = tableCurrent[tableCurrent.size() - i - 1];
				t = p + len + over;
			}
			if (t < new_min) new_min = t;
			if (t > new_max) new_max = t;

			after.append(t);
			actions->move(p, t);
			actionsHistory.move(p, t);
		}

		int begin = qMin(old_min, new_min);
		int end = qMax(old_max, new_max);

		TableUpdate(begin, end);
		WidgetReload();
		ui.action_table->clearSelection();
		TableSelection(after);
	}
}
void EditUi::ItemAdd(QiType type)
{
	int p = ui.action_table->currentRow();
	if (p < 0) p = actions->size();
	else p++;

	Action action = ItemGet(type);
	actions->insert(p, action);
	actionsHistory.insert(p, action);

	idChecker.check(action);
	if (idChecker.reset(*actionsRoot)) TableReload();
	else TableInsert(p);

	WidgetReload();
	ui.action_table->setCurrentItem(ui.action_table->item(p, tableColumn_type));
}
void EditUi::ItemChange(QiType type)
{
	for (int i : tableCurrent)
	{
		if (actions->valid(i))
		{
			Action& action = actions->at(i);

			idChecker.check(action);

			QiBase base_old = action.base();
			action = ItemGet(type);
			QiBase& base_new = action.base();
			base_new.disable = std::move(base_old.disable);
			base_new.mark = std::move(base_old.mark);
			base_new.next = std::move(base_old.next);
			base_new.next2 = std::move(base_old.next2);
			actionsHistory.set(i, action);

			idChecker.check(action);

			TableUpdate(i);
		}
	}
	if (idChecker.reset(*actionsRoot)) TableReload();
	WidgetReload();
	ui.action_table->clearSelection();
}
void EditUi::ItemDel()
{
	if (tableCurrent.empty()) return;

	while (tableCurrent.not_empty())
	{
		int i = tableCurrent.back();
		tableCurrent.pop_back();
		if (actions->valid(i))
		{
			idChecker.check(actions->at(i));
			TableRemove(i);
			actions->remove(i);
			actionsHistory.remove(i);
		}
	}
	if (idChecker.reset(*actionsRoot)) TableReload();
	WidgetReload();
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
	if (Qi::clipboard.empty()) return;
	int p = ui.action_table->currentRow();
	if (p < 0) p = actions->size();
	else p++;

	// reset jump id
	UniqueActionsId(actionsRoot->loadId(QiType::jumpPoint), Qi::clipboard, QiType::jumpPoint, QiType::jump);
	// reset block id
	UniqueActionsId(actionsRoot->loadId(QiType::block), Qi::clipboard, QiType::block, QiType::blockExec);

	actions->insert(p, Qi::clipboard);
	actionsHistory.insert(p, Qi::clipboard);

	idChecker.check(Qi::clipboard);
	idChecker.reset(*actionsRoot);

	if (idChecker.reset(*actionsRoot)) TableReload();
	else for (size_t i = 0; i < Qi::clipboard.size(); i++) TableInsert(p + i);
	WidgetReload();
}
void EditUi::Redo()
{
	if (actionsHistory.canRedo())
	{
		idChecker.check(*actions);

		actionsHistory.redo();
		*actions = actionsHistory.toVector();

		idChecker.check(*actions);
		idChecker.reset(*actionsRoot);

		TableReload();
		WidgetReload();
	}
}
void EditUi::Undo()
{
	if (actionsHistory.canUndo())
	{
		idChecker.check(*actions);

		actionsHistory.undo();
		*actions = actionsHistory.toVector();

		idChecker.check(*actions);
		idChecker.reset(*actionsRoot);

		TableReload();
		WidgetReload();
	}
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
	mouse.ex = QiRange::Restricted(ui.mouse_rand_edit->text().toInt(), QiMouse::range_rand);
	mouse.speed = ui.mouse_speed_edit->text().isEmpty() ? 20 : QiRange::Restricted(ui.mouse_speed_edit->text().toInt(), QiMouse::range_speed);
	{
		QString v = ui.mouse_x_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) mouse.x = QiRange::Restricted(v.toInt(), mouse.move ? QiMouse::range_move : QiMouse::range_pos);
		else mouse.v_x = v;
	}
	{
		QString v = ui.mouse_y_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) mouse.y = QiRange::Restricted(v.toInt(), mouse.move ? QiMouse::range_move : QiMouse::range_pos);
		else mouse.v_y = v;
	}
	return mouse;
}
QiDelay EditUi::WidgetGetDelay() {
	QiDelay delay;
	{
		QString v = ui.delay_min_edit->text();
		if (v.isEmpty()) delay.min = 10;
		else if (QiVar::isInteger(v.toStdString())) delay.min = QiRange::Restricted(v.toInt(), QiDelay::range_time);
		else delay.v_min = v;
	}
	{
		QString v = ui.delay_max_edit->text();
		if (v.isEmpty()) delay.max = delay.min, delay.v_max = delay.v_min;
		else if (QiVar::isInteger(v.toStdString())) delay.max = QiRange::Restricted(v.toInt(), QiDelay::range_time);
		else delay.v_max = v;
	}
	return delay;
}
QiCopyText EditUi::WidgetGetText() {
	QiCopyText text;
	text.text = ui.copyText_textedit->toPlainText();
	return text;
}
QiColor EditUi::WidgetGetColor() {
	QiColor color;
	{
		QString v = ui.color_left_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) color.rect.left = QiRange::Restricted(v.toInt(), QiColor::range_rect);
		else color.v_left = v;
	}
	{
		QString v = ui.color_top_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) color.rect.top = QiRange::Restricted(v.toInt(), QiColor::range_rect);
		else color.v_top = v;
	}
	{
		QString v = ui.color_right_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) color.rect.right = QiRange::Restricted(v.toInt(), QiColor::range_rect);
		else color.v_right = v;
	}
	{
		QString v = ui.color_bottom_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) color.rect.bottom = QiRange::Restricted(v.toInt(), QiColor::range_rect);
		else color.v_bottom = v;
	}
	color.rgbe.set(
		QiRange::Restricted(ui.color_red_edit->text().toInt(), QiColor::range_rgb),
		QiRange::Restricted(ui.color_green_edit->text().toInt(), QiColor::range_rgb),
		QiRange::Restricted(ui.color_blue_edit->text().toInt(), QiColor::range_rgb),
		ui.color_sim_edit->text().isEmpty() ? 10 : QiRange::Restricted(ui.color_sim_edit->text().toInt(), QiColor::range_rgb)
	);
	color.move = ui.color_move_check->isChecked();
	return color;
}
QiLoop EditUi::WidgetGetLoop()
{
	QiLoop loop;
	{
		QString v = ui.loop_min_edit->text();
		if (v.isEmpty()) loop.min = 1;
		else if (QiVar::isInteger(v.toStdString())) loop.min = QiRange::Restricted(v.toInt(), QiLoop::range_count);
		else loop.v_min = v;
	}
	{
		QString v = ui.loop_max_edit->text();
		if (v.isEmpty()) loop.max = loop.min, loop.v_max = loop.v_min;
		else if (QiVar::isInteger(v.toStdString())) loop.max = QiRange::Restricted(v.toInt(), QiLoop::range_count);
		else loop.v_max = v;
	}
	return loop;
}
QiImage EditUi::WidgetGetImage()
{
	QiImage image;
	{
		QString v = ui.image_left_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) image.rect.left = QiRange::Restricted(v.toInt(), QiImage::range_rect);
		else image.v_left = v;
	}
	{
		QString v = ui.image_top_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) image.rect.top = QiRange::Restricted(v.toInt(), QiImage::range_rect);
		else image.v_top = v;
	}
	{
		QString v = ui.image_right_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) image.rect.right = QiRange::Restricted(v.toInt(), QiImage::range_rect);
		else image.v_right = v;
	}
	{
		QString v = ui.image_bottom_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) image.rect.bottom = QiRange::Restricted(v.toInt(), QiImage::range_rect);
		else image.v_bottom = v;
	}
	image.sim = ui.image_sim_edit->text().isEmpty() ? 80 : QiRange::Restricted(ui.image_sim_edit->text().toInt(), QiImage::range_sim);
	image.map = imageMap;
	image.move = ui.image_move_check->isChecked();
	image.mult = ui.image_mult_check->isChecked();
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
	{
		QString v = ui.timer_min_edit->text();
		if (v.isEmpty()) timer.min = 1000;
		else if (QiVar::isInteger(v.toStdString())) timer.min = QiRange::Restricted(v.toInt(), QiTimer::range_time);
		else timer.v_min = v;
	}
	{
		QString v = ui.timer_max_edit->text();
		if (v.isEmpty()) timer.max = timer.min, timer.v_max = timer.v_min;
		else if (QiVar::isInteger(v.toStdString())) timer.max = QiRange::Restricted(v.toInt(), QiTimer::range_time);
		else timer.v_max = v;
	}
	return timer;
}
QiJump EditUi::WidgetGetJump()
{
	QiJump jump;
	int row = ui.jumpPoint_list->currentRow();
	if (row > -1) jump.id = ui.jumpPoint_list->item(row)->data(static_cast<int>(DataRole::id)).toInt();
	return jump;
}
QiJumpPoint EditUi::WidgetGetJumpPoint()
{
	QiJumpPoint jumpPoint;
	jumpPoint.id = actionsRoot->loadId(QiType::jumpPoint).unique();
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
	block.id = actionsRoot->loadId(QiType::block).unique();
	return block;
}
QiBlockExec EditUi::WidgetGetBlockExec()
{
	QiBlockExec blockExec;
	int row = ui.block_list->currentRow();
	if (row > -1) blockExec.id = ui.block_list->item(row)->data(static_cast<int>(DataRole::id)).toInt();
	return blockExec;
}
QiQuickInput EditUi::WidgetGetQuickInput()
{
	QiQuickInput quickInput;
	quickInput.text = ui.quickInput_text_edit->text();
	return quickInput;
}
QiKeyBlock EditUi::WidgetGetKeyBlock()
{
	QiKeyBlock keyBlock;
	if (ui.keyBlock_all_check->isChecked()) keyBlock.vk = QiKeyBlock::all;
	else if (ui.keyBlock_move_check->isChecked()) keyBlock.vk = QiKeyBlock::move;
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
	{
		QString v = ui.ocr_left_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) ocr.rect.left = QiRange::Restricted(v.toInt(), QiOcr::range_rect);
		else ocr.v_left = v;
	}
	{
		QString v = ui.ocr_top_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) ocr.rect.top = QiRange::Restricted(v.toInt(), QiOcr::range_rect);
		else ocr.v_top = v;
	}
	{
		QString v = ui.ocr_right_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) ocr.rect.right = QiRange::Restricted(v.toInt(), QiOcr::range_rect);
		else ocr.v_right = v;
	}
	{
		QString v = ui.ocr_bottom_edit->text();
		if (v.isEmpty());
		else if (QiVar::isInteger(v.toStdString())) ocr.rect.bottom = QiRange::Restricted(v.toInt(), QiOcr::range_rect);
		else ocr.v_bottom = v;
	}
	ocr.text = ui.ocr_text_edit->text();
	ocr.var = ui.ocr_var_edit->text();
	ocr.row = ui.ocr_row_check->isChecked();
	ocr.match = ui.ocr_match_check->isChecked();
	ocr.move = ui.ocr_move_check->isChecked();
	ocr.mult = ui.ocr_mult_check->isChecked();
	return ocr;
}
QiVarOperator EditUi::WidgetGetVarOperator()
{
	QiVarOperator varOperator;
	varOperator.script = ui.varOperator_textedit->toPlainText();
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
QiMsgView EditUi::WidgetGetMsgView()
{
	QiMsgView msgView;
	msgView.text = ui.msgView_textedit->toPlainText();
	if (ui.msgView_type_set_radio->isChecked()) msgView.option = QiMsgView::set;
	else if (ui.msgView_type_add_radio->isChecked()) msgView.option = QiMsgView::add;
	else if (ui.msgView_type_clear_radio->isChecked()) msgView.option = QiMsgView::clear;
	else if (ui.msgView_type_show_radio->isChecked()) msgView.option = QiMsgView::show;
	else if (ui.msgView_type_hide_radio->isChecked()) msgView.option = QiMsgView::hide;
	if (ui.msgView_level_msg_radio->isChecked()) msgView.level = MsgViewInfo::_msg;
	else if (ui.msgView_level_war_radio->isChecked()) msgView.level = MsgViewInfo::_war;
	else if (ui.msgView_level_err_radio->isChecked()) msgView.level = MsgViewInfo::_err;
	return msgView;
}
QiRangeSet EditUi::WidgetGetRange()
{
	QiRangeSet range;
	range.rect.left = QiRange::Restricted(ui.range_left_edit->text().toInt(), QiRangeSet::range_rect);
	range.rect.top = QiRange::Restricted(ui.range_top_edit->text().toInt(), QiRangeSet::range_rect);
	range.rect.right = QiRange::Restricted(ui.range_right_edit->text().toInt(), QiRangeSet::range_rect);
	range.rect.bottom = QiRange::Restricted(ui.range_bottom_edit->text().toInt(), QiRangeSet::range_rect);
	range.wnd = ui.range_name_edit->text();
	range.var = ui.range_id_edit->text();
	return range;
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
	ui.mouse_x_edit->setText(mouse.v_x.isEmpty() ? QString::number(mouse.x) : mouse.v_x);
	ui.mouse_y_edit->setText(mouse.v_y.isEmpty() ? QString::number(mouse.y) : mouse.v_y);
	if (mouse.ex > -1) ui.mouse_rand_edit->setText(QString::number(mouse.ex));
}
void EditUi::WidgetSet(const QiDelay& delay)
{
	ui.delay_min_edit->setText(delay.v_min.isEmpty() ? QString::number(delay.min) : delay.v_min);
	ui.delay_max_edit->setText(delay.v_max.isEmpty() ? QString::number(delay.max) : delay.v_max);
}
void EditUi::WidgetSet(const QiCopyText& text)
{
	ui.copyText_textedit->setPlainText(text.text);
}
void EditUi::WidgetSet(const QiColor& color) {
	ui.color_move_check->setChecked(color.move);
	ui.color_left_edit->setText(color.v_left.isEmpty() ? QString::number(color.rect.left) : color.v_left);
	ui.color_top_edit->setText(color.v_top.isEmpty() ? QString::number(color.rect.top) : color.v_top);
	ui.color_right_edit->setText(color.v_right.isEmpty() ? QString::number(color.rect.right) : color.v_right);
	ui.color_bottom_edit->setText(color.v_bottom.isEmpty() ? QString::number(color.rect.bottom) : color.v_bottom);
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
	ui.image_left_edit->setText(image.v_left.isEmpty() ? QString::number(image.rect.left) : image.v_left);
	ui.image_top_edit->setText(image.v_top.isEmpty() ? QString::number(image.rect.top) : image.v_top);
	ui.image_right_edit->setText(image.v_right.isEmpty() ? QString::number(image.rect.right) : image.v_right);
	ui.image_bottom_edit->setText(image.v_bottom.isEmpty() ? QString::number(image.rect.bottom) : image.v_bottom);
	ui.image_sim_edit->setText(QString::number(image.sim));
	ui.image_move_check->setChecked(image.move);
	ui.image_mult_check->setChecked(image.mult);
	imageMap = image.map;
	HBITMAP hbmp = Image::toBmp32(imageMap);
	if (hbmp) ui.image_view_label->setPixmap(QPixmap::fromImage(QImage::fromHBITMAP(hbmp)));
	DeleteObject(hbmp);
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
	ui.quickInput_text_edit->setText(quickInput.text);
}
void EditUi::WidgetSet(const QiKeyBlock& keyBlock)
{
	ui.keyBlock_all_check->setChecked(keyBlock.vk == QiKeyBlock::all);
	ui.keyBlock_move_check->setChecked(keyBlock.vk == QiKeyBlock::move);
	if (keyBlock.vk > QiKeyBlock::move) ui.keyBlock_keyedit->setKey(keyBlock.vk);
	else ui.keyBlock_keyedit->clear();
	keyBlock.block ? ui.keyBlock_on_radio->setChecked(true) : ui.keyBlock_off_radio->setChecked(true);
}
void EditUi::WidgetSet(const QiClock& clock)
{
	ui.clock_time_edit->setTime(QTime(QiTime::h(clock.time), QiTime::m(clock.time), QiTime::s(clock.time)));
}
void EditUi::WidgetSet(const QiOcr& ocr)
{
	ui.ocr_left_edit->setText(ocr.v_left.isEmpty() ? QString::number(ocr.rect.left) : ocr.v_left);
	ui.ocr_top_edit->setText(ocr.v_top.isEmpty() ? QString::number(ocr.rect.top) : ocr.v_top);
	ui.ocr_right_edit->setText(ocr.v_right.isEmpty() ? QString::number(ocr.rect.right) : ocr.v_right);
	ui.ocr_bottom_edit->setText(ocr.v_bottom.isEmpty() ? QString::number(ocr.rect.bottom) : ocr.v_bottom);
	ui.ocr_text_edit->setText(ocr.text);
	ui.ocr_var_edit->setText(ocr.var);
	ui.ocr_row_check->setChecked(ocr.row);
	ui.ocr_match_check->setChecked(ocr.match);
	ui.ocr_move_check->setChecked(ocr.move);
	ui.ocr_mult_check->setChecked(ocr.mult);
}
void EditUi::WidgetSet(const QiVarOperator& varOperator)
{
	ui.varOperator_textedit->setPlainText(varOperator.script);
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
void EditUi::WidgetSet(const QiMsgView& msgView)
{
	ui.msgView_textedit->setPlainText(msgView.text);
	if (msgView.option == QiMsgView::set) ui.msgView_type_set_radio->setChecked(true);
	else if (msgView.option == QiMsgView::add) ui.msgView_type_add_radio->setChecked(true);
	else if (msgView.option == QiMsgView::clear) ui.msgView_type_clear_radio->setChecked(true);
	else if (msgView.option == QiMsgView::show) ui.msgView_type_show_radio->setChecked(true);
	else if (msgView.option == QiMsgView::hide) ui.msgView_type_hide_radio->setChecked(true);
	if (msgView.level == MsgViewInfo::_msg) ui.msgView_level_msg_radio->setChecked(true);
	else if (msgView.level == MsgViewInfo::_war) ui.msgView_level_war_radio->setChecked(true);
	else if (msgView.level == MsgViewInfo::_err) ui.msgView_level_err_radio->setChecked(true);
}
void EditUi::WidgetSet(const QiRangeSet& range)
{
	ui.range_left_edit->setText(QString::number(range.rect.left));
	ui.range_top_edit->setText(QString::number(range.rect.top));
	ui.range_right_edit->setText(QString::number(range.rect.right));
	ui.range_bottom_edit->setText(QString::number(range.rect.bottom));
	ui.range_name_edit->setText(range.wnd);
	ui.range_id_edit->setText(range.var);
	macro->range = range.rect;
}


// iter binds
template<class Ty, class Fn>
void EditUi::BindSafeIter(QiVector<Ty*>& bind, Fn call)
{
	for (size_t i = 0; i < bind.size(); i++) if (bind.at(i)) call(bind.at(i), i);
}

// type: jumpPoint, block
void EditUi::UniqueActionsId(QiIdList ids, Actions& actions, QiType type, QiType type2)
{
	actions.iter([this, &ids, &actions, type2](Action& action) {
		QiBase base = action.base();
		if (ids.exist(base.id))
		{
			int id_res = base.id;
			base.id = ids.unique();
			ids.append(base.id);

			if (static_cast<size_t>(type2) > 0)
			{
				actions.iter([id_res, id_new = base.id](Action& action) {
					QiBase base2 = action.base();
					if (id_res == base2.id) base2.id = id_new;
					return false;
					}, type2);
			}
		}
		return false;
		}, type);
}
// id list
void EditUi::ListJumpPointReload()
{
	ui.jumpPoint_list->clear();
	for (int i : jumpIds)
	{
		const Action* action = actionsRoot->find(QiType::jumpPoint, i);
		if (action)
		{
			QListWidgetItem* item = new QListWidgetItem(QString::number(action->base().id) + QString::fromUtf8("　　　") + action->base().mark);
			item->setData(static_cast<int>(DataRole::id), action->base().id);
			ui.jumpPoint_list->addItem(item);
		}
	}
	ui.jump_add_button->setDisabled(true);
}
void EditUi::ListBlockReload()
{
	ui.block_list->clear();
	for (int i : blockIds)
	{
		const Action* action = actionsRoot->find(QiType::block, i);
		if (action)
		{
			QListWidgetItem* item = new QListWidgetItem(QString::number(action->base().id) + QString::fromUtf8("　　　") + action->base().mark);
			item->setData(static_cast<int>(DataRole::id), action->base().id);
			ui.block_list->addItem(item);
		}
	}
	ui.blockExec_add_button->setDisabled(true);
}


void EditUi::Back()
{
	layers.removeLast();
	if (layers.empty())
	{
		Qi::popText->Show("正在保存宏");
		QTimer::singleShot(32, [] { Qi::widget.editClose(); });
	}
	else
	{
		actions = layers.last().actions;
		actionsHistory = *actions;
		setWindowTitle(layers.last().title);
		Reload();
		TableSelection(layers.last().items);
	}
}
void EditUi::Forward(const QString& title, Actions* next)
{
	actions = next;
	actionsHistory = *actions;
	if (layers.isEmpty()) layers.append({ title, next });
	else
	{
		layers.last().items = tableCurrent;
		layers.append({ title, next });
		Reload();
	}
}
void EditUi::Exit(bool save)
{
	Qi::widget.macroEdited(save);
	QiJson::SaveJson();
	close();
}


bool EditUi::event(QEvent* e)
{
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
	{
		QKeyEvent* keyEvent = (QKeyEvent*)e;
		if ((keyEvent->key() == Qt::Key_Escape) || (keyEvent->key() == Qt::Key_Return) || keyEvent->key() == Qt::Key_Enter || (keyEvent->key() == Qt::Key_Space)) return true;
	}
	return QDialogFrameless::event(e);
}
bool EditUi::eventFilter(QObject* obj, QEvent* e)
{
	if (obj == ui.action_table)
	{
		if (e->type() == QEvent::KeyPress)
		{
			QKeyEvent* key = (QKeyEvent*)e;
			if (key->modifiers() == Qt::ControlModifier)
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
				else if (key->key() == Qt::Key_R)
				{
					Redo();
					return true;
				}
				else if (key->key() == Qt::Key_Z)
				{
					Undo();
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
				else if (key->key() == Qt::Key_Backspace)
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
		}
	}
	else if (obj == ui.action_table->viewport()) // move drop
	{
		if (e->type() == QEvent::Drop)
		{
			QDropEvent* drop = reinterpret_cast<QDropEvent*>(e);
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
		if (obj == ui.title_icon || obj == ui.title_label)
		{
			if (e->type() == QEvent::MouseButtonRelease)
			{
				QMouseEvent* mouse = (QMouseEvent*)e;
				if (mouse->button() == Qt::LeftButton) titleMenu->exec(QCursor::pos());
			}
		}
		else if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease)) return true;
	}
	return QDialog::eventFilter(obj, e);
}
void EditUi::showEvent(QShowEvent*)
{
	Qi::application->setStyleSheet(Qi::ui.themes[Qi::set.theme].style);
	SetForegroundWindow((HWND)QWidget::winId());
}
void EditUi::resizeEvent(QResizeEvent* e)
{
	Qi::set.editSize = e->size();
}
void EditUi::customEvent(QEvent* e)
{
	if (e->type() == static_cast<int>(QiEvent::wid_close)) Exit();
	else if (e->type() == static_cast<int>(QiEvent::edt_debug_pause)) SetDebugState(debug_pause);
	else if (e->type() == static_cast<int>(QiEvent::edt_varop_stop)) setDisabled(false);
}