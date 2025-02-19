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
		tab_image,
		tab_text,
		tab_dialog,
		tab_window
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
	const int tableColumn_type = 0;
	const int tableColumn_disable = 1;
	const int tableColumn_param = 2;
	const int tableColumn_mark = 3;
	Ui::EditUiClass ui;
	// edit
	QList<Layer> layers;
	bool changing = false;
	bool updating = false;
	QList<int> tableCurrent;
	QList<int> tableCurrentPrev;
	// table context menu
	QMenu* menu;
	QAction* muDel;
	QAction* muChange;
	QAction* muCut;
	QAction* muCopy;
	QAction* muPaste;
	QAction* muEdit;
	QAction* muEdit2;
	// test run
	QTimer* testTimer;
	int testCount = 0;
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
		WidgetUpdate();
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
			ui.window_select_button->setProperty("group", "get_button");
		}
		if ("action add button")
		{
			ui.key_add_button->setProperty("group", "edit-add_button");
			ui.mouse_add_button->setProperty("group", "edit-add_button");
			ui.delay_add_button->setProperty("group", "edit-add_button");
			ui.loop_add_button->setProperty("group", "edit-add_button");
			ui.timer_add_button->setProperty("group", "edit-add_button");
			ui.keyState_add_button->setProperty("group", "edit-add_button");
			ui.mousePos_add_button->setProperty("group", "edit-add_button");
			ui.endLoop_add_button->setProperty("group", "edit-add_button");
			ui.end_add_button->setProperty("group", "edit-add_button");
			ui.color_add_button->setProperty("group", "edit-add_button");
			ui.image_add_button->setProperty("group", "edit-add_button");
			ui.copyText_add_button->setProperty("group", "edit-add_button");
			ui.popText_add_button->setProperty("group", "edit-add_button");
			ui.jump_add_button->setProperty("group", "edit-add_button");
			ui.jumpPoint_add_button->setProperty("group", "edit-add_button");
			ui.dialog_add_button->setProperty("group", "edit-add_button");
			ui.block_add_button->setProperty("group", "edit-add_button");
			ui.blockExec_add_button->setProperty("group", "edit-add_button");
			ui.quickInput_add_button->setProperty("group", "edit-add_button");
			ui.keyBlock_add_button->setProperty("group", "edit-add_button");
		}
		if ("action edit button")
		{
			ui.loop_edit_button->setProperty("group", "edit-edit_button");
			ui.timer_edit_button->setProperty("group", "edit-edit_button");
			ui.keyState_edit_button->setProperty("group", "edit-edit_button");
			ui.keyState_edit2_button->setProperty("group", "edit-edit_button");
			ui.color_edit_button->setProperty("group", "edit-edit_button");
			ui.color_edit2_button->setProperty("group", "edit-edit_button");
			ui.image_edit_button->setProperty("group", "edit-edit_button");
			ui.image_edit2_button->setProperty("group", "edit-edit_button");
			ui.dialog_edit_button->setProperty("group", "edit-edit_button");
			ui.dialog_edit2_button->setProperty("group", "edit-edit_button");
			ui.block_edit_button->setProperty("group", "edit-edit_button");
		}
		if ("check box")
		{
			ui.tab_lock_check->setProperty("group", "check");
			ui.mouse_track_check->setProperty("group", "check");
			ui.color_move_check->setProperty("group", "check");
			ui.image_move_check->setProperty("group", "check");
			ui.popText_wait_check->setProperty("group", "check");
			ui.window_state_check->setProperty("group", "check");
			ui.window_child_check->setProperty("group", "check");
			ui.keyBlock_move_check->setProperty("group", "check");
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
			ui.rememberPos_radio->setProperty("group", "check");
			ui.recoverPos_radio->setProperty("group", "check");
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
			ui.window_name_edit->setProperty("group", "line_edit");
		}
		if ("text edit")
		{
			ui.copyText_text->setProperty("group", "text_edit");
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
		if ("action table")
		{
			ui.action_table->setProperty("group", "table");
			ui.action_table->horizontalHeader()->setProperty("group", "table_header");
			ui.action_table->verticalHeader()->setProperty("group", "table_header");
			menu->setProperty("group", "context_menu");
		}
		if ("action list")
		{
			ui.jumpPoint_list->setProperty("group", "table");
			ui.block_list->setProperty("group", "table");
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
	}
	// >>>> new action set here
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
		}
		if ("button")
		{
			if ("text")
			{
				ui.key_add_button->setText(Qi::ui.text.etAdd);
				ui.keyState_add_button->setText(Qi::ui.text.etAdd);
				ui.mouse_add_button->setText(Qi::ui.text.etAdd);
				ui.delay_add_button->setText(Qi::ui.text.etAdd);
				ui.loop_add_button->setText(Qi::ui.text.etAdd);
				ui.copyText_add_button->setText(Qi::ui.text.etAdd);
				ui.color_add_button->setText(Qi::ui.text.etAdd);
				ui.image_add_button->setText(Qi::ui.text.etAdd);
				ui.end_add_button->setText(Qi::ui.text.etAdd);
				ui.endLoop_add_button->setText(Qi::ui.text.etAdd);
				ui.jump_add_button->setText(Qi::ui.text.etAdd);
				ui.jumpPoint_add_button->setText(Qi::ui.text.etAdd);
				ui.loop_edit_button->setText(Qi::ui.text.etEdit);
				ui.timer_edit_button->setText(Qi::ui.text.etEdit);
				ui.block_add_button->setText(Qi::ui.text.etAdd);
				ui.blockExec_add_button->setText(Qi::ui.text.etAdd);
			}
			if ("clear shortcut")
			{
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
				// action add
				ui.key_add_button->installEventFilter(this);
				ui.keyState_add_button->installEventFilter(this);
				ui.mouse_add_button->installEventFilter(this);
				ui.delay_add_button->installEventFilter(this);
				ui.loop_add_button->installEventFilter(this);
				ui.copyText_add_button->installEventFilter(this);
				ui.color_add_button->installEventFilter(this);
				ui.image_add_button->installEventFilter(this);
				ui.end_add_button->installEventFilter(this);
				ui.endLoop_add_button->installEventFilter(this);
				ui.jump_add_button->installEventFilter(this);
				ui.jumpPoint_add_button->installEventFilter(this);
				ui.dialog_add_button->installEventFilter(this);
				ui.block_add_button->installEventFilter(this);
				ui.blockExec_add_button->installEventFilter(this);
				// action edit
				ui.keyState_edit_button->installEventFilter(this);
				ui.loop_edit_button->installEventFilter(this);
				ui.timer_edit_button->installEventFilter(this);
				ui.color_edit_button->installEventFilter(this);
				ui.image_edit_button->installEventFilter(this);
				ui.dialog_edit_button->installEventFilter(this);
				ui.dialog_edit2_button->installEventFilter(this);
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
			QButtonGroup* mousePosRbs = new QButtonGroup(this);
			mousePosRbs->addButton(ui.recoverPos_radio);
			mousePosRbs->addButton(ui.rememberPos_radio);
			ui.recoverPos_radio->setChecked(true);
			QButtonGroup* dialogRbs = new QButtonGroup(this);
			dialogRbs->addButton(ui.dialog_style_none_radio);
			dialogRbs->addButton(ui.dialog_style_warning_radio);
			dialogRbs->addButton(ui.dialog_style_error_radio);
			ui.dialog_style_none_radio->setChecked(true);
			QButtonGroup* keyBlockRbs = new QButtonGroup(this);
			mousePosRbs->addButton(ui.keyBlock_on_radio);
			mousePosRbs->addButton(ui.keyBlock_off_radio);
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
			ui.action_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			ui.action_table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
			ui.action_table->verticalHeader()->setDefaultSectionSize(0);
			ui.action_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Fixed);
			ui.action_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Fixed);
			ui.action_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Interactive);
			ui.action_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeMode::Interactive);
			ui.action_table->setColumnWidth(tableColumn_type, 100);
			ui.action_table->setColumnWidth(tableColumn_disable, 50);
			ui.action_table->setColumnWidth(tableColumn_param, 300);
		}
		if ("key edit")
		{
			ui.key_keyedit->setMode(QKeyEdit::Mode::solid);
			ui.key_keyedit->setMouseEnable(true);
			ui.key_keyedit->setWheelEnable(true);
			ui.key_keyedit->setKey(QKeyEdit::Key(VK_LBUTTON));

			ui.keyState_keyedit->setMode(QKeyEdit::Mode::solid);
			ui.keyState_keyedit->setMouseEnable(true);
			ui.keyState_keyedit->setWheelEnable(true);
			ui.keyState_keyedit->setPadEnable(true);
			ui.keyState_keyedit->setKey(QKeyEdit::Key(VK_LBUTTON));

			ui.keyBlock_keyedit->setMode(QKeyEdit::Mode::solid);
			ui.keyBlock_keyedit->setMouseEnable(true);
			ui.keyBlock_keyedit->setWheelEnable(true);
			ui.keyBlock_keyedit->setKey(QKeyEdit::Key(VK_LBUTTON));
		}
		if ("timer")
		{
			testTimer = new QTimer(this);
		}
		// enable qlable scale
		ui.image_view_label->setScaledContents(true);
		// load Window mode
		if (macro->wndState) macro->wi.update(), SetWindowMode();
	}
	// >>>> new action set here
	void Event()
	{
		if ("title")
		{
			connect(ui.title_close_button, &QPushButton::clicked, this, [this] {
				Qi::popText->Show("正在保存宏");
				QApplication::postEvent(this, new QEvent(QEvent::User));
				});
			connect(ui.title_back_button, &QPushButton::clicked, this, [this] {
				layers.removeLast();
				if (layers.empty())
				{
					Qi::popText->Show("正在保存宏");
					QApplication::postEvent(this, new QEvent(QEvent::User));
				}
				else
				{
					actions = layers.last().actions;
					setWindowTitle(layers.last().title);
					WidgetUpdate();
				}
				});
			connect(ui.title_run_button, &QPushButton::clicked, this, [this] {
				setDisabled(true);
				testCount = macro->count;
				macro->count = 1;
				Qi::run = true;
				timeBeginPeriod(1);
				QiThread::StartMacroRun(macro);
				testTimer->start(16);
				});
			connect(ui.window_select_button, &QPushButton::clicked, this, [this] { SelectWindow(); });
			connect(ui.window_state_check, &QCheckBox::clicked, this, [this](bool state) {
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
			connect(ui.window_child_check, &QCheckBox::clicked, this, [this](bool state) { macro->wi.child = state; });
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
					layers.first().actions = actionsRoot = actions = &macro->acRun;
					TableUpdate();
				}
				});
			connect(ui.action_ending_radio, &QRadioButton::toggled, this, [this](bool state) {
				if (state && (layers.size() == 1))
				{
					layers.first().actions = actionsRoot = actions = &macro->acEnd;
					TableUpdate();
				}
				});
			connect(ui.action_table, &QTableWidget::cellChanged, this, [this](int row, int column) {
				if (updating) return;
				if (row < 0) return;
				if (column != tableColumn_mark) return;
				QiBase& base = actions->at(row).base();
				base.mark = ui.action_table->item(row, tableColumn_mark)->text();
				if ((base.type == QiType::jumpPoint) || (base.type == QiType::block)) TableUpdate();
				});
			// mark edit
			connect(ui.action_table, &QTableWidget::cellClicked, this, [this](int row, int column) {
				if (column == tableColumn_mark) ui.action_table->editItem(ui.action_table->item(row, tableColumn_mark));
				else if (column == tableColumn_disable)
				{
					if (tableCurrentPrev.empty())
					{
						QiBase& base = actions->at(row).base();
						base.disable = !base.disable;
					}
					else
					{
						for (auto i : tableCurrentPrev)
						{
							QiBase& base = actions->at(i).base();
							base.disable = !base.disable;
						}
					}
					TableUpdate();
				}
				});
			// selection
			// >>>> new action set here
			connect(ui.action_table, &QTableWidget::itemSelectionChanged, this, [this]() {
				QList<QTableWidgetItem*> items = ui.action_table->selectedItems();
				tableCurrentPrev = tableCurrent;
				tableCurrent.clear();
				for (auto i : items) if (i->column() == 0) tableCurrent.append(i->row());
				DisableButtons();
				DisableMenus();
				pv.hide();
				rv.hide();
				// selction is solid
				if (tableCurrent.size() == 1)
				{
					bool tabLock = ui.tab_lock_check->isChecked();
					int tab = tab_base;
					const Action& var = actions->at(tableCurrent.first());
					switch (var.index())
					{
					case QiType::end: tab = tab_state; break;
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
								if (!macro->wi.update()) SelectWindow();
								POINT rpt = QiFn::WATR({ mouse.x, mouse.y }, macro->wi.wnd);
								pt = Window::pos(macro->wi.wnd);
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
							if (!macro->wi.update()) SelectWindow();
							rect = QiFn::WATRR(color.rect, macro->wi.wnd);
							POINT pt = Window::pos(macro->wi.wnd);
							rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
						}
						else rect = QiFn::ATRR(color.rect);
						rv.Show(rect);
					} break;
					case QiType::loop: tab = tab_loop;
					{
						ui.loop_edit_button->setEnabled(true);
					} break;
					case QiType::loopEnd: tab = tab_state; break;
					case QiType::keyState: tab = tab_state;
					{
						ui.keyState_edit_button->setEnabled(true);
						ui.keyState_edit2_button->setEnabled(true);
					} break;
					case QiType::recoverPos: tab = tab_state; break;
					case QiType::image: tab = tab_image;
					{
						const QiImage& image = std::get<QiImage>(var);
						ui.image_edit_button->setEnabled(true);
						ui.image_edit2_button->setEnabled(true);
						RECT rect;
						if (macro->wndState)
						{
							if (!macro->wi.update()) SelectWindow();
							rect = QiFn::WATRR(image.rect, macro->wi.wnd);
							POINT pt = Window::pos(macro->wi.wnd);
							rect.left += pt.x, rect.top += pt.y, rect.right += pt.x, rect.bottom += pt.y;
						}
						else rect = QiFn::ATRR(image.rect);
						rv.Show(rect);
						WidgetSet(image);
					} break;
					case QiType::popText: tab = tab_text; break;
					case QiType::rememberPos: tab = tab_state; break;
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
					}
					if (!tabLock) ui.tabWidget->setCurrentIndex(tab);
				}
				});
			// change
			connect(ui.action_table, &QTableWidget::cellDoubleClicked, this, [this] { changing ? SetChangeState(false) : SetChangeState(true); });
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
					muChange->setEnabled(true);
					// selction is solid
					if (tableCurrent.size() == 1)
					{
						if ("have next")
						{
							bool edit = false;
							bool edit2 = false;
							const Action& var = actions->at(tableCurrent.first());
							switch (var.index())
							{
							case QiType::color: edit = edit2 = true; break;
							case QiType::loop: edit = true; break;
							case QiType::keyState: edit = edit2 = true; break;
							case QiType::image: edit = edit2 = true; break;
							case QiType::timer: edit = true; break;
							case QiType::dialog: edit = edit2 = true; break;
							case QiType::block: edit = true; break;
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
			connect(muChange, &QAction::triggered, this, [this] { changing ? SetChangeState(false) : SetChangeState(true); });
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
		// >>>> new action set here
		if ("action widget")
		{
			if ("base tab")
			{
				connect(ui.key_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::key); });
				connect(ui.mouse_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::mouse); });
				connect(ui.delay_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::delay); });
				// move
				connect(ui.mouse_position_button, &QPushButton::clicked, this, [this] {
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
				// quickInput
				connect(ui.quickInput_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::quickInput); });
			}
			if ("state tab")
			{
				connect(ui.keyState_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::keyState); });
				connect(ui.mousePos_add_button, &QPushButton::clicked, this, [this] { bool recover = ui.recoverPos_radio->isChecked(); if (recover) OnAddButton(QiType::recoverPos); else OnAddButton(QiType::rememberPos); });
				connect(ui.end_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::end); });
				connect(ui.endLoop_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::loopEnd); });
				// edit
				connect(ui.keyState_edit_button, &QPushButton::clicked, this, [this] { NextEdit(false); });
				connect(ui.keyState_edit2_button, &QPushButton::clicked, this, [this] { NextEdit(true); });
				// keyBlock
				connect(ui.keyBlock_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::keyBlock); });
			}
			if ("loop tab")
			{
				connect(ui.loop_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::loop); });
				connect(ui.timer_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::timer); });
				// edit
				connect(ui.loop_edit_button, &QPushButton::clicked, this, [this] { NextEdit(false); });
				connect(ui.timer_edit_button, &QPushButton::clicked, this, [this] { NextEdit(false); });
				// loop
				connect(ui.loop_min_edit, &QLineEdit::textChanged, this, [this](const QString& text) { ui.loop_max_edit->setText(text); });
				// timer
				connect(ui.timer_min_edit, &QLineEdit::textChanged, this, [this](const QString& text) { ui.timer_max_edit->setText(text); });
			}
			if ("text tab")
			{
				connect(ui.copyText_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::copyText); });
				connect(ui.popText_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::popText); });
			}
			if ("color tab")
			{
				connect(ui.color_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::color); });
				// edit
				connect(ui.color_edit_button, &QPushButton::clicked, this, [this] { NextEdit(false); });
				connect(ui.color_edit2_button, &QPushButton::clicked, this, [this] { NextEdit(true); });
				// color
				connect(ui.color_rect_button, &QPushButton::clicked, this, [this] {
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
			}
			if ("image tab")
			{
				connect(ui.image_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::image); });
				// edit
				connect(ui.image_edit_button, &QPushButton::clicked, this, [this] { NextEdit(false); });
				connect(ui.image_edit2_button, &QPushButton::clicked, this, [this] { NextEdit(true); });
				// image
				connect(ui.image_rect_button, &QPushButton::clicked, this, [this] {
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
					WidgetSet(image);
					});
				connect(ui.image_shot_button, &QPushButton::clicked, this, [this] {
					QRectSelection rs;
					RECT rect = rs.Start();
					QiImage image(WidgetGetImage());
					Image::ScreenRgbMap(imageMap, rect);
					image.map = imageMap;
					WidgetSet(image);
					});
			}
			if ("jump tab")
			{
				connect(ui.jump_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::jump); });
				connect(ui.jumpPoint_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::jumpPoint); });
			}
			if ("block tab")
			{
				connect(ui.block_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::block); });
				connect(ui.blockExec_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::blockExec); });
				// edit
				connect(ui.block_edit_button, &QPushButton::clicked, this, [this] { NextEdit(false); });
			}
			if ("dialog tab")
			{
				connect(ui.dialog_add_button, &QPushButton::clicked, this, [this] { OnAddButton(QiType::dialog); });
				// edit
				connect(ui.dialog_edit_button, &QPushButton::clicked, this, [this] { NextEdit(false); });
				connect(ui.dialog_edit2_button, &QPushButton::clicked, this, [this] { NextEdit(true); });
			}
		}
		if ("timer")
		{
			connect(testTimer, &QTimer::timeout, this, [this]
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
						Sleep(300);
						setEnabled(true);
					}
				}
			);
		}
	}
	// >>>> new action set here (include edit)
	void DisableButtons()
	{
		ui.keyState_edit_button->setDisabled(true);
		ui.keyState_edit2_button->setDisabled(true);
		ui.loop_edit_button->setDisabled(true);
		ui.timer_edit_button->setDisabled(true);
		ui.color_edit_button->setDisabled(true);
		ui.color_edit2_button->setDisabled(true);
		ui.image_edit_button->setDisabled(true);
		ui.image_edit2_button->setDisabled(true);
		ui.jump_add_button->setDisabled(true);
		ui.dialog_edit_button->setDisabled(true);
		ui.dialog_edit2_button->setDisabled(true);
		ui.block_edit_button->setDisabled(true);
		ui.blockExec_add_button->setDisabled(true);
	}
	void DisableMenus()
	{
		muDel->setDisabled(true);
		muChange->setDisabled(true);
		muCut->setDisabled(true);
		muCopy->setDisabled(true);
		muEdit->setDisabled(true);
		muEdit2->setDisabled(true);
	}
	void WidgetUpdate()
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
		TableUpdate();
	}
	// >>>> new action set here (include edit)
	void NextEdit(bool edit2)
	{
		SetChangeState(false);
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
		}
		if (next)
		{
			Layer layer;
			layer.actions = next;
			layer.title = title;
			layers.append(layer);
			actions = next;
			setWindowTitle(title);
			WidgetUpdate();
		}
	}
	void SetWindowMode()
	{
		ui.window_state_check->setChecked(macro->wndState);
		ui.window_child_check->setChecked(macro->wi.child);
		ui.window_name_edit->setText(macro->wi.wndName);
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
		macro->wi = QiFn::WindowSelection();
		move(pt);
		SetWindowMode();
	}
	// unique ids
	// type: jumpPoint, jump, block, blockExec
	QiVector<int> LoadIds(const Actions& actions, int type)
	{
		QiVector<int> ids;
		for (size_t i = 0; i < actions.size(); i++)
		{
			const Action& var = actions[i];
			if (var.index() == type)
			{
				switch (type)
				{
				case QiType::jumpPoint:
				{
					const QiJumpPoint& ref = std::get<QiJumpPoint>(var);
					ids.append_copy(ref.id);
				} break;
				case QiType::jump:
				{
					const QiJump& ref = std::get<QiJump>(var);
					ids.append_copy(ref.id);
				} break;
				case QiType::block:
				{
					const QiBlock& ref = std::get<QiBlock>(var);
					ids.append_copy(ref.id);
				} break;
				case QiType::blockExec:
				{
					const QiBlockExec& ref = std::get<QiBlockExec>(var);
					ids.append_copy(ref.id);
				} break;
				}
			}
			else
			{
				const QiBase& base = var.base();
				if (!base.next.empty()) ids.append_copy(LoadIds(base.next, type));
				if (!base.next2.empty()) ids.append_copy(LoadIds(base.next2, type));
			}
		}
		return ids;
	}
	bool FindId(const QiVector<int>& ids, int id)
	{
		for (auto i : ids) if (i == id) return true;
		return false;
	}
	// type: jump, blockExec
	void InvalidId(const QiVector<int>& ids, Actions& actions, int type)
	{
		for (size_t i = 0; i < actions.size(); i++)
		{
			Action& var = actions[i];
			if (var.index() == type)
			{
				switch (type)
				{
				case QiType::jump:
				{
					QiJump& jump = std::get<QiJump>(var);
					if (!FindId(ids, jump.id)) jump.id = -1;
				} break;
				case QiType::blockExec:
				{
					QiBlockExec& blockExec = std::get<QiBlockExec>(var);
					if (!FindId(ids, blockExec.id)) blockExec.id = -1;
				} break;
				}
			}
			else
			{
				QiBase& base = var.base();
				if (!base.next.empty()) InvalidId(ids, base.next, type);
				if (!base.next2.empty()) InvalidId(ids, base.next2, type);
			}
		}
	}
	int UniqueId(const QiVector<int>& ids)
	{
		int id = 1;
		for (size_t i = 0; i < ids.size(); i++)
		{
			if (ids[i] >= id) id = ids[i] + 1;
		}
		return id;
	}
	// type: jumpPoint, block
	void UniqueActionsId_CallBack(QiVector<int>& ids, Actions& actions, int type)
	{
		for (size_t i = 0; i < actions.size(); i++)
		{
			Action& var = actions[i];

			if (var.index() == type)
			{
				switch (type)
				{
				case QiType::jumpPoint:
				{
					QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(var);
					if (FindId(ids, jumpPoint.id))
					{
						int id_res = jumpPoint.id;
						jumpPoint.id = UniqueId(ids);
						ids.append_copy(jumpPoint.id);
						for (size_t ix = 0; ix < actions.size(); ix++)
						{
							Action& var = actions[ix];
							if (var.index() == QiType::jump)
							{
								QiJump& jump = std::get<QiJump>(var);
								if (id_res == jump.id) jump.id = jumpPoint.id;
							}
						}
					}
				} break;
				case QiType::block:
				{
					QiBlock& block = std::get<QiBlock>(var);
					if (FindId(ids, block.id))
					{
						int id_res = block.id;
						block.id = UniqueId(ids);
						ids.append_copy(block.id);
						for (size_t ix = 0; ix < actions.size(); ix++)
						{
							Action& var = actions[ix];
							if (var.index() == QiType::blockExec)
							{
								QiBlockExec& blockExec = std::get<QiBlockExec>(var);
								if (id_res == blockExec.id) blockExec.id = block.id;
							}
						}
					}
				} break;
				}
			}
			else
			{
				QiBase& base = var.base();
				if (!base.next.empty()) UniqueActionsId(ids, base.next, type);
				if (!base.next2.empty()) UniqueActionsId(ids, base.next2, type);
			}
		}
	}
	QiVector<int> UniqueActionsId(const QiVector<int>& ids, Actions& actions, int type)
	{
		QiVector<int> ids_result;
		ids_result.copy(ids);
		UniqueActionsId_CallBack(ids_result, actions, type);
		return ids_result;
	}

	Actions LoadType(const Actions& actions, int type)
	{
		Actions result;
		for (auto i : actions) if (i.index() == type) result.append_copy(i);
		return result;
	}
	void ListJumpPointUpdate()
	{
		QiVector<int> ids = LoadIds(*actionsRoot, QiType::jump);
		InvalidId(ids, *actionsRoot, QiType::jump);
		ui.jumpPoint_list->clear();
		Actions jumpPoints = LoadType(*actions, QiType::jumpPoint);
		for (auto i : jumpPoints)
		{
			const QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(i);
			QListWidgetItem* item = new QListWidgetItem(QString::number(jumpPoint.id) + QString::fromUtf8("　　　") + jumpPoint.mark);
			item->setData(DataRole::id, jumpPoint.id);
			ui.jumpPoint_list->addItem(item);
		}
	}
	void ListBlockUpdate()
	{
		QiVector<int> ids = LoadIds(*actionsRoot, QiType::blockExec);
		InvalidId(ids, *actionsRoot, QiType::blockExec);
		ui.block_list->clear();
		Actions blocks = LoadType(*actions, QiType::block);
		for (auto i : blocks)
		{
			const QiBlock& block = std::get<QiBlock>(i);
			QListWidgetItem* item = new QListWidgetItem(QString::number(block.id) + QString::fromUtf8("　　　") + block.mark);
			item->setData(DataRole::id, block.id);
			ui.block_list->addItem(item);
		}
	}
	// table
	// >>>> new action set here
	void TableUpdate()
	{
		tableCurrent.clear();
		tableCurrentPrev.clear();
		DisableButtons();
		DisableMenus();
		ListJumpPointUpdate();
		ListBlockUpdate();

		updating = true;
		ui.action_table->clearMask();
		ui.action_table->setRowCount(actions->size());
		ui.action_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.action_table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.action_table->verticalHeader()->setDefaultSectionSize(0);

		for (size_t i = 0; i < actions->size(); i++)
		{
			const Action& var = actions->at(i);

			QString type;
			QString param;
			QString mark = var.base().mark;
			bool failed = false;

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
			case QiType::recoverPos:
			{
				type = Qi::ui.text.acRecoverPos;
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
			case QiType::rememberPos:
			{
				type = Qi::ui.text.acRememberPos;
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

				for (auto i : ref.chars) param += i;

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
			default: type = "加载失败", failed = true; break;
			}

			QTableWidgetItem* item = new QTableWidgetItem(type);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.action_table->setItem(i, tableColumn_type, item);

			if (!failed)
			{
				if (var.base().disable) item = new QTableWidgetItem(Qi::ui.text.syOff);
				else item = new QTableWidgetItem(Qi::ui.text.syOn);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.action_table->setItem(i, tableColumn_disable, item);

				item = new QTableWidgetItem(param);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.action_table->setItem(i, tableColumn_param, item);

				item = new QTableWidgetItem(mark);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.action_table->setItem(i, tableColumn_mark, item);
			}
		}
		updating = false;
	}
private:
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
		else if (obj == ui.action_table->viewport()) // move drop
		{
			if (e->type() == QEvent::Drop)
			{
				QDropEvent* drop = static_cast<QDropEvent*>(e);
				QTableWidgetItem* item = ui.action_table->itemAt(drop->pos());
				int before = 0, after = 0;
				before = ui.action_table->currentRow();
				if (before < 0) return 0;
				if (item) after = item->row();
				else after = ui.action_table->rowCount() - 1;
				if (after < 0) return 0;
				actions->move(before, after);
				TableUpdate();
				ui.action_table->setCurrentItem(ui.action_table->item(after, tableColumn_type));
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
		close();
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
	// action
	void OnAddButton(int type)
	{
		if (changing) ItemChange(type);
		else ItemAdd(type);
	}
	// >>>> new action set here
	void SetChangeState(bool state)
	{
		changing = state;

		QString text = Qi::ui.text.etAdd;
		if (changing)
		{
			int p = ui.action_table->currentRow();  if (p < 0) return;
			ItemUse(p);

			text = Qi::ui.text.etChange;
		}

		ui.key_add_button->setText(text);
		ui.keyState_add_button->setText(text);
		ui.mouse_add_button->setText(text);
		ui.delay_add_button->setText(text);
		ui.loop_add_button->setText(text);
		ui.timer_add_button->setText(text);
		ui.copyText_add_button->setText(text);
		ui.color_add_button->setText(text);
		ui.end_add_button->setText(text);
		ui.endLoop_add_button->setText(text);
		ui.mousePos_add_button->setText(text);
		ui.image_add_button->setText(text);
		ui.popText_add_button->setText(text);
		ui.mousePos_add_button->setText(text);
		ui.jump_add_button->setText(text);
		ui.jumpPoint_add_button->setText(text);
		ui.dialog_add_button->setText(text);
		ui.block_add_button->setText(text);
		ui.blockExec_add_button->setText(text);
		ui.quickInput_add_button->setText(text);
		ui.keyBlock_add_button->setText(text);
	}
	// >>>> new action set here
	void ItemUse(int p)
	{
		const Action& var = actions->at(p);
		switch (var.index())
		{
		case QiType::delay:
		{
			const QiDelay& ref = std::get<QiDelay>(var);
			WidgetSet(ref);
		} break;
		case QiType::key:
		{
			const QiKey& ref = std::get<QiKey>(var);
			WidgetSet(ref);
		} break;
		case QiType::mouse:
		{
			const QiMouse& ref = std::get<QiMouse>(var);
			WidgetSet(ref);
		} break;
		case QiType::copyText:
		{
			const QiCopyText& ref = std::get<QiCopyText>(var);
			WidgetSet(ref);
		} break;
		case QiType::color:
		{
			const QiColor& ref = std::get<QiColor>(var);
			WidgetSet(ref);
		} break;
		case QiType::loop:
		{
			const QiLoop& ref = std::get<QiLoop>(var);
			WidgetSet(ref);
		} break;
		case QiType::keyState:
		{
			const QiKeyState& ref = std::get<QiKeyState>(var);
			WidgetSet(ref);
		} break;
		case QiType::image:
		{
			const QiImage& ref = std::get<QiImage>(var);
			WidgetSet(ref);
		} break;
		case QiType::popText:
		{
			const QiPopText& ref = std::get<QiPopText>(var);
			WidgetSet(ref);
		} break;
		case QiType::timer:
		{
			const QiTimer& ref = std::get<QiTimer>(var);
			WidgetSet(ref);
		} break;
		case QiType::dialog:
		{
			const QiDialog& ref = std::get<QiDialog>(var);
			WidgetSet(ref);
		} break;
		case QiType::quickInput:
		{
			const QiQuickInput& ref = std::get<QiQuickInput>(var);
			WidgetSet(ref);
		} break;
		case QiType::keyBlock:
		{
			const QiKeyBlock& ref = std::get<QiKeyBlock>(var);
			WidgetSet(ref);
		}
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
		case QiType::quickInput: action = WidgetGetQuickInput(); break;
		case QiType::keyBlock: action = WidgetGetKeyBlock(); break;
		}
		return action;
	}
	void ItemMove(bool up)
	{
		int p = ui.action_table->currentRow();
		if (p < 0) return;

		if (up && ((p - 1) >= 0))
		{
			actions->swap(p, p - 1);
			ui.action_table->setCurrentItem(ui.action_table->item(p - 1, tableColumn_type));
		}
		else if (!up && (p + 1) < ui.action_table->rowCount())
		{
			actions->swap(p, p + 1);
			ui.action_table->setCurrentItem(ui.action_table->item(p + 1, tableColumn_type));
		}
		TableUpdate();
	}
	void ItemAdd(int type)
	{
		int p = ui.action_table->currentRow();
		if (p < 0) p = actions->size();
		else p++;
		actions->insert(p, ItemGet(type));
		TableUpdate();
		ui.action_table->setCurrentItem(ui.action_table->item(p, tableColumn_type));
	}
	void ItemChange(int type)
	{
		for (auto i : tableCurrent)
		{
			Action& current = actions->at(i);
			QiBase base_old(std::move(current.base()));
			current = std::move(ItemGet(type));
			QiBase& base_new = current.base();
			base_new.disable = std::move(base_old.disable);
			base_new.mark = std::move(base_old.mark);
			base_new.next = std::move(base_old.next);
			base_new.next2 = std::move(base_old.next2);
		}
		TableUpdate();
		ui.action_table->setCurrentItem(0);
		SetChangeState(false);
	}
	void ItemDel()
	{
		if (tableCurrent.empty()) return;
		std::vector<size_t> positions;
		for (auto i : tableCurrent) positions.push_back(i);
		actions->remove(positions);
		TableUpdate();
		ui.action_table->setCurrentItem(0);
		SetChangeState(false);
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
		for (auto i : tableCurrent) Qi::clipboard.append_copy(actions->at(i));
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
		actions->insert_copy(p, Qi::clipboard);
		TableUpdate();
	}
	// get params from widget
	QiKey WidgetGetKey() {
		QiKey key;
		if (ui.key_down_radio->isChecked()) key.state = QiKey::down;
		else if (ui.key_up_radio->isChecked()) key.state = QiKey::up;
		else if (ui.key_click_radio->isChecked()) key.state = QiKey::click;
		key.vk = ui.key_keyedit->key().keyCode;
		return key;
	}
	QiKeyState WidgetGetKeyState() {
		QiKeyState keyState;
		keyState.vk = ui.keyState_keyedit->key().keyCode;
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
		QString text = ui.quickInput_text_edit->text();
		for (auto i : text)
		{
			char c = i.toLatin1();
			if (InRange(c, '0', '9', 0) || InRange(c, 'A', 'Z', 0)) quickInput.chars.append_copy(c);
			else if (InRange(c, 'a', 'z', 0)) quickInput.chars.append_copy(tolower(c));
		}
		return quickInput;
	}
	QiKeyBlock WidgetGetKeyBlock()
	{
		QiKeyBlock keyBlock;
		if (ui.keyBlock_move_check->isChecked()) keyBlock.vk = 0;
		else keyBlock.vk = ui.keyBlock_keyedit->key().keyCode;
		keyBlock.block = ui.keyBlock_on_radio->isChecked();
		return keyBlock;
	}
	// load params from widget
	void WidgetSet(const QiKey& key)
	{
		if (key.state == QiKey::down) ui.key_down_radio->setChecked(true);
		else if (key.state == QiKey::up) ui.key_up_radio->setChecked(true);
		else if (key.state == QiKey::click) ui.key_click_radio->setChecked(true);
		ui.key_keyedit->setKey(QKeyEdit::Key(key.vk));
	}
	void WidgetSet(const QiKeyState& keyState)
	{
		ui.keyState_keyedit->setKey(QKeyEdit::Key(keyState.vk));
	}
	void WidgetSet(const QiMouse& mouse)
	{
		if (mouse.move) ui.mouse_move_radio->setChecked(true);
		else ui.mouse_position_radio->setChecked(true);
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
		ui.copyText_text->setText(text.text);
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
		QString text;
		for (auto i : quickInput.chars) text += i;
		ui.quickInput_text_edit->setText(text);
	}
	void WidgetSet(const QiKeyBlock& keyBlock)
	{
		if (keyBlock.vk) ui.keyBlock_keyedit->setKey(keyBlock.vk), ui.keyBlock_move_check->setChecked(false);
		else ui.keyBlock_move_check->setChecked(true);
		ui.keyBlock_on_radio->setChecked(keyBlock.block);
	}
};