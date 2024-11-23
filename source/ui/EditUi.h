#pragma once
#include <src/inc_header.h>
#include "ui_EditUi.h"
class EditUi : public QDialog
{
	Q_OBJECT;
	using This = EditUi;
	using JumpPoints = QiVector<QiJumpPoint>;
	using Blocks = QiVector<QiBlock>;
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
	Ui::EditUiClass ui;
	// edit
	QList<Layer> layers;
	bool changing = false;
	bool updating = false;
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
			ui.rememberPos_add_button->setProperty("group", "edit-add_button");
			ui.recoverPos_add_button->setProperty("group", "edit-add_button");
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
			ui.mouse_track_check->setProperty("group", "check");
			ui.color_move_check->setProperty("group", "check");
			ui.image_move_check->setProperty("group", "check");
			ui.popText_wait_check->setProperty("group", "check");
			ui.window_state_check->setProperty("group", "check");
			ui.window_child_check->setProperty("group", "check");
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
		}
		if ("line edit")
		{
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
		if ("set radio group")
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
			ui.action_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Interactive);
			ui.action_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Interactive);
			ui.action_table->setColumnWidth(0, 100);
			ui.action_table->setColumnWidth(1, 300);
			ui.action_table->setColumnWidth(2, 180);
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
			ui.action_table->installEventFilter(this);
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
				if (column != 2) return;
				QiBase& base = actions->at(row).base();
				base.mark = ui.action_table->item(row, 2)->text();
				if ((base.type == QiType::jumpPoint) || (base.type == QiType::block)) TableUpdate();
				});
			connect(ui.action_table, &QTableWidget::itemSelectionChanged, this, [this] {
				QList<QTableWidgetItem*> items = ui.action_table->selectedItems();
				bool solid = items.size() == ui.action_table->columnCount();
				int row = ui.action_table->currentRow();
				DisableButtons();
				DisableMenus();
				pv.hide();
				rv.hide();
				if (items.empty()) return;
				if (solid && ui.action_table->currentColumn() == 2) ui.action_table->editItem(ui.action_table->item(row, 2));
				else
				{
					muDel->setEnabled(true);
					muCut->setEnabled(true);
					muCopy->setEnabled(true);
					// solid current
					if (solid)
					{
						bool edit = false;
						bool edit2 = false;
						const Action& var = actions->at(row);
						switch (var.index())
						{
						case QiType::end:
						{
							ui.tabWidget->setCurrentIndex(tab_state);
						} break;
						case QiType::delay:
						{
							ui.tabWidget->setCurrentIndex(tab_base);
						} break;
						case QiType::key:
						{
							ui.tabWidget->setCurrentIndex(tab_base);
						} break;
						case QiType::mouse:
						{
							const QiMouse& mouse = std::get<QiMouse>(var);
							ui.tabWidget->setCurrentIndex(tab_base);
							if (!mouse.move)
							{
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
						} break;
						case QiType::copyText:
						{
							ui.tabWidget->setCurrentIndex(tab_text);
						} break;
						case QiType::color:
						{
							const QiColor& color = std::get<QiColor>(var);
							ui.tabWidget->setCurrentIndex(tab_color);
							ui.color_edit_button->setEnabled(true);
							ui.color_edit2_button->setEnabled(true);
							edit = edit2 = true;
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
						} break;
						case QiType::loop:
						{
							ui.tabWidget->setCurrentIndex(tab_loop);
							ui.loop_edit_button->setEnabled(true);
							edit = true;
						} break;
						case QiType::loopEnd:
						{
							ui.tabWidget->setCurrentIndex(tab_state);
						} break;
						case QiType::keyState:
						{
							ui.tabWidget->setCurrentIndex(tab_state);
							ui.keyState_edit_button->setEnabled(true);
							ui.keyState_edit2_button->setEnabled(true);
							edit = edit2 = true;
						} break;
						case QiType::recoverPos:
						{
							ui.tabWidget->setCurrentIndex(tab_state);
						} break;
						case QiType::image:
						{
							const QiImage& image = std::get<QiImage>(var);
							ui.tabWidget->setCurrentIndex(tab_image);
							ui.image_edit_button->setEnabled(true);
							ui.image_edit2_button->setEnabled(true);
							edit = edit2 = true;
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
						} break;
						case QiType::popText:
						{
							ui.tabWidget->setCurrentIndex(tab_text);
						} break;
						case QiType::rememberPos:
						{
							ui.tabWidget->setCurrentIndex(tab_state);
						} break;
						case QiType::timer:
						{
							ui.tabWidget->setCurrentIndex(tab_loop);
							edit = true;
						} break;
						case QiType::jump:
						{
							ui.tabWidget->setCurrentIndex(tab_jump);
						} break;
						case QiType::jumpPoint:
						{
							ui.tabWidget->setCurrentIndex(tab_jump);
						} break;
						case QiType::dialog:
						{
							ui.tabWidget->setCurrentIndex(tab_dialog);
							ui.dialog_edit_button->setEnabled(true);
							ui.dialog_edit2_button->setEnabled(true);
							edit = edit2 = true;
						} break;
						case QiType::block:
						{
							ui.tabWidget->setCurrentIndex(tab_block);
							ui.block_edit_button->setEnabled(true);
							edit = true;
						} break;
						case QiType::blockExec:
						{
							ui.tabWidget->setCurrentIndex(tab_block);
						} break;
						}
						muChange->setEnabled(true);
						if (edit) muEdit->setEnabled(true);
						if (edit2) muEdit2->setEnabled(true);
					}
				}
				});
			connect(ui.action_table, &QTableWidget::cellDoubleClicked, this, [this] { changing ? SetChangeState(false) : SetChangeState(true); });
			connect(ui.action_table, &QTableWidget::customContextMenuRequested, this, [this] { menu->exec(QCursor::pos()); });
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
		if ("action widget")
		{
			if ("base tab")
			{
				connect(ui.key_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::key); else ItemAdd(QiType::key); });
				connect(ui.mouse_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::mouse); else ItemAdd(QiType::mouse); });
				connect(ui.delay_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::delay); else ItemAdd(QiType::delay); });
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
					WidgetSetMouse(mouse);
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
			}
			if ("state tab")
			{
				connect(ui.keyState_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::keyState); else ItemAdd(QiType::keyState); });
				connect(ui.rememberPos_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::rememberPos); else ItemAdd(QiType::rememberPos); });
				connect(ui.recoverPos_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::recoverPos); else ItemAdd(QiType::recoverPos); });
				connect(ui.end_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::end); else ItemAdd(QiType::end); });
				connect(ui.endLoop_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::loopEnd); else ItemAdd(QiType::loopEnd); });
				// edit
				connect(ui.keyState_edit_button, &QPushButton::clicked, this, [this] { NextEdit(false); });
				connect(ui.keyState_edit2_button, &QPushButton::clicked, this, [this] { NextEdit(true); });
			}
			if ("loop tab")
			{
				connect(ui.loop_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::loop); else ItemAdd(QiType::loop); });
				connect(ui.timer_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::timer); else ItemAdd(QiType::timer); });
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
				connect(ui.copyText_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::copyText); else ItemAdd(QiType::copyText); });
				connect(ui.popText_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::popText); else ItemAdd(QiType::popText); });
			}
			if ("color tab")
			{
				connect(ui.color_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::color); else ItemAdd(QiType::color); });
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
					WidgetSetColor(color);
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
					WidgetSetColor(color);
					});
			}
			if ("image tab")
			{
				connect(ui.image_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::image); else ItemAdd(QiType::image); });
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
					WidgetSetImage(image);
					});
				connect(ui.image_shot_button, &QPushButton::clicked, this, [this] {
					QRectSelection rs;
					RECT rect = rs.Start();
					QiImage image(WidgetGetImage());
					Image::ScreenRgbMap(imageMap, rect);
					image.map = imageMap;
					WidgetSetImage(image);
					});
			}
			if ("jump tab")
			{
				connect(ui.jump_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::jump); else ItemAdd(QiType::jump); });
				connect(ui.jumpPoint_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::jumpPoint); else ItemAdd(QiType::jumpPoint); });
			}
			if ("block tab")
			{
				connect(ui.block_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::block); else ItemAdd(QiType::block); });
				connect(ui.blockExec_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::blockExec); else ItemAdd(QiType::blockExec); });
				// edit
				connect(ui.block_edit_button, &QPushButton::clicked, this, [this] { NextEdit(false); });
			}
			if ("dialog tab")
			{
				connect(ui.dialog_add_button, &QPushButton::clicked, this, [this] { if (changing) ItemChange(QiType::dialog); else ItemAdd(QiType::dialog); });
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
	// jump
	void LoadJumpPoints(JumpPoints& jumpPoints, const Actions& actions)
	{
		for (size_t i = 0; i < actions.size(); i++)
		{
			const Action& var = actions[i];
			if (var.index() == QiType::jumpPoint)
			{
				const QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(var);
				jumpPoints.append_copy(jumpPoint);
			}
			else
			{
				const QiBase& base = var.base();
				if (!base.next.empty()) LoadJumpPoints(jumpPoints, base.next);
				if (!base.next2.empty()) LoadJumpPoints(jumpPoints, base.next2);
			}
		}
	}
	bool FindJumpPoint(const JumpPoints& jumpPoints, int id)
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
				QiJump& jump = std::get<QiJump>(var);
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
	int JumpPointUnique(const JumpPoints& jumpPoints)
	{
		int id = 1;
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
	// block
	void LoadBlocks(Blocks& blocks, const Actions& actions)
	{
		for (size_t i = 0; i < actions.size(); i++)
		{
			const Action& var = actions[i];
			if (var.index() == QiType::block)
			{
				const QiBlock& block = std::get<QiBlock>(var);
				blocks.append_copy(block);
			}
			else
			{
				const QiBase& base = var.base();
				if (!base.next.empty()) LoadBlocks(blocks, base.next);
				if (!base.next2.empty()) LoadBlocks(blocks, base.next2);
			}
		}
	}
	bool FindBlock(const Blocks& blocks, int id)
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
	int BlockUnique(const Blocks& blocks)
	{
		int id = 1;
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
	// list
	void ListJumpPointUpdate()
	{
		JumpPoints jumpPoints;
		LoadJumpPoints(jumpPoints, *actionsRoot);

		CheckJumpInvalid(jumpPoints, *actionsRoot);

		ui.jumpPoint_list->clear();

		for (size_t i = 0; i < jumpPoints.size(); i++)
		{
			const QiJumpPoint& jumpPoint = jumpPoints[i];
			QListWidgetItem* item = new QListWidgetItem(QString::number(jumpPoint.id) + QString::fromUtf8("　　　") + jumpPoint.mark);
			item->setData(DataRole::id, jumpPoint.id);
			ui.jumpPoint_list->addItem(item);
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
	// table
	void TableUpdate()
	{
		DisableButtons();
		DisableMenus();
		muPaste->setEnabled(Qi::clipboard.not_empty());
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
			case QiType::copyText:
			{
				const QiCopyText& text = std::get<QiCopyText>(var);
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
			ui.action_table->setItem(i, 0, item);

			item = new QTableWidgetItem(param);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.action_table->setItem(i, 1, item);

			item = new QTableWidgetItem(mark);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.action_table->setItem(i, 2, item);
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
				ui.action_table->setCurrentItem(ui.action_table->item(after, 0));
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
	void SetChangeState(bool state)
	{
		if (state)
		{
			int p = ui.action_table->currentRow();  if (p < 0) return;
			ItemUse(p);
			changing = state;
			{
				ui.key_add_button->setText(Qi::ui.text.etChange);
				ui.keyState_add_button->setText(Qi::ui.text.etChange);
				ui.mouse_add_button->setText(Qi::ui.text.etChange);
				ui.delay_add_button->setText(Qi::ui.text.etChange);
				ui.loop_add_button->setText(Qi::ui.text.etChange);
				ui.timer_add_button->setText(Qi::ui.text.etChange);
				ui.copyText_add_button->setText(Qi::ui.text.etChange);
				ui.color_add_button->setText(Qi::ui.text.etChange);
				ui.end_add_button->setText(Qi::ui.text.etChange);
				ui.endLoop_add_button->setText(Qi::ui.text.etChange);
				ui.recoverPos_add_button->setText(Qi::ui.text.etChange);
				ui.image_add_button->setText(Qi::ui.text.etChange);
				ui.popText_add_button->setText(Qi::ui.text.etChange);
				ui.rememberPos_add_button->setText(Qi::ui.text.etChange);
				ui.jump_add_button->setText(Qi::ui.text.etChange);
				ui.jumpPoint_add_button->setText(Qi::ui.text.etChange);
				ui.dialog_add_button->setText(Qi::ui.text.etChange);
				ui.block_add_button->setText(Qi::ui.text.etChange);
				ui.blockExec_add_button->setText(Qi::ui.text.etChange);
			}
		}
		else
		{
			changing = state;
			{
				ui.key_add_button->setText(Qi::ui.text.etAdd);
				ui.keyState_add_button->setText(Qi::ui.text.etAdd);
				ui.mouse_add_button->setText(Qi::ui.text.etAdd);
				ui.delay_add_button->setText(Qi::ui.text.etAdd);
				ui.loop_add_button->setText(Qi::ui.text.etAdd);
				ui.timer_add_button->setText(Qi::ui.text.etAdd);
				ui.copyText_add_button->setText(Qi::ui.text.etAdd);
				ui.color_add_button->setText(Qi::ui.text.etAdd);
				ui.end_add_button->setText(Qi::ui.text.etAdd);
				ui.endLoop_add_button->setText(Qi::ui.text.etAdd);
				ui.recoverPos_add_button->setText(Qi::ui.text.etAdd);
				ui.image_add_button->setText(Qi::ui.text.etAdd);
				ui.popText_add_button->setText(Qi::ui.text.etAdd);
				ui.rememberPos_add_button->setText(Qi::ui.text.etAdd);
				ui.jump_add_button->setText(Qi::ui.text.etAdd);
				ui.jumpPoint_add_button->setText(Qi::ui.text.etAdd);
				ui.dialog_add_button->setText(Qi::ui.text.etAdd);
				ui.block_add_button->setText(Qi::ui.text.etAdd);
				ui.blockExec_add_button->setText(Qi::ui.text.etAdd);
			}
		}
	}
	void ItemUse(int p)
	{
		const Action& var = actions->at(p);
		switch (var.index())
		{
		case QiType::delay:
		{
			const QiDelay& delay = std::get<QiDelay>(var);
			WidgetSetDelay(delay);
		} break;
		case QiType::key:
		{
			const QiKey& key = std::get<QiKey>(var);
			WidgetSetKey(key);
		} break;
		case QiType::mouse:
		{
			const QiMouse& mouse = std::get<QiMouse>(var);
			WidgetSetMouse(mouse);
		} break;
		case QiType::copyText:
		{
			const QiCopyText& text = std::get<QiCopyText>(var);
			WidgetSetCopyText(text);
		} break;
		case QiType::color:
		{
			const QiColor& color = std::get<QiColor>(var);
			WidgetSetColor(color);
		} break;
		case QiType::loop:
		{
			const QiLoop& loop = std::get<QiLoop>(var);
			WidgetSetLoop(loop);
		} break;
		case QiType::keyState:
		{
			const QiKeyState& keyState = std::get<QiKeyState>(var);
			WidgetSetKeyState(keyState);
		} break;
		case QiType::image:
		{
			const QiImage& image = std::get<QiImage>(var);
			WidgetSetImage(image);
		} break;
		case QiType::popText:
		{
			const QiPopText& popText = std::get<QiPopText>(var);
			WidgetSetPopText(popText);
		} break;
		case QiType::timer:
		{
			const QiTimer& timer = std::get<QiTimer>(var);
			WidgetSetTimer(timer);
		} break;
		case QiType::dialog:
		{
			const QiDialog& dialog = std::get<QiDialog>(var);
			WidgetSetDialog(dialog);
		} break;
		}
	}
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
			ui.action_table->setCurrentItem(ui.action_table->item(p - 1, 0));
		}
		else if (!up && (p + 1) < ui.action_table->rowCount())
		{
			actions->swap(p, p + 1);
			ui.action_table->setCurrentItem(ui.action_table->item(p + 1, 0));
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
		ui.action_table->setCurrentItem(ui.action_table->item(p, 0));
	}
	void ItemChange(int type)
	{
		int p = ui.action_table->currentRow(); if (p < 0) return;
		Action& current = actions->at(p);
		QiBase base_old(std::move(current.base()));
		current = std::move(ItemGet(type));
		QiBase& base_new = current.base();
		base_new.mark = std::move(base_old.mark);
		base_new.next = std::move(base_old.next);
		base_new.next2 = std::move(base_old.next2);
		TableUpdate();
		ui.action_table->setCurrentItem(0);
		SetChangeState(false);
	}
	void ItemDel()
	{
		QList<QTableWidgetItem*> items = ui.action_table->selectedItems();
		if (items.empty()) return;
		std::vector<size_t> positions;
		for (int i = 0; i < items.size(); i++) if (items[i]->column() == 0) positions.push_back(items[i]->row());
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
		QList<QTableWidgetItem*> items = ui.action_table->selectedItems();
		if (items.empty()) return;
		Qi::clipboard.clear();
		for (size_t i = 0; i < items.size(); i++) if (items[i]->column() == 0) Qi::clipboard.append_copy(actions->at(items[i]->row()));
		muPaste->setEnabled(Qi::clipboard.not_empty());
	}
	void ItemPaste()
	{
		int p = ui.action_table->currentRow();
		if (p < 0) p = actions->size();
		else p++;
		// reset jump id
		JumpPoints jumpPoints;
		LoadJumpPoints(jumpPoints, *actionsRoot);
		JumpPointUniqueActions(jumpPoints, Qi::clipboard);
		// reset block id
		Blocks blocks;
		LoadBlocks(blocks, *actionsRoot);
		BlockUniqueActions(blocks, Qi::clipboard);
		for (size_t i = Qi::clipboard.size(); i > 0; i--)
		{
			actions->insert_copy(p, Qi::clipboard[i - 1]);
			if (i < 1) break;
		}
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
	// load params from widget
	void WidgetSetKey(const QiKey& key)
	{
		if (key.state == QiKey::down) ui.key_down_radio->setChecked(true);
		else if (key.state == QiKey::up) ui.key_up_radio->setChecked(true);
		else if (key.state == QiKey::click) ui.key_click_radio->setChecked(true);
		ui.key_keyedit->setKey(QKeyEdit::Key(key.vk));
	}
	void WidgetSetKeyState(const QiKeyState& keyState)
	{
		ui.keyState_keyedit->setKey(QKeyEdit::Key(keyState.vk));
	}
	void WidgetSetMouse(const QiMouse& mouse)
	{
		if (mouse.move) ui.mouse_move_radio->setChecked(true);
		else ui.mouse_position_radio->setChecked(true);
		ui.mouse_track_check->setChecked(mouse.track);
		ui.mouse_speed_edit->setText(QString::number(mouse.speed));
		ui.mouse_x_edit->setText(QString::number(mouse.x)); ui.mouse_y_edit->setText(QString::number(mouse.y)); if (mouse.ex > -1) ui.mouse_rand_edit->setText(QString::number(mouse.ex));
	}
	void WidgetSetDelay(const QiDelay& delay)
	{
		ui.delay_min_edit->setText(QString::number(delay.min));
		ui.delay_max_edit->setText(QString::number(delay.max));
	}
	void WidgetSetCopyText(const QiCopyText& text)
	{
		ui.copyText_text->setText(text.text);
	}
	void WidgetSetColor(const QiColor& color) {
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
	void WidgetSetLoop(const QiLoop& loop)
	{
		ui.loop_min_edit->setText(QString::number(loop.min));
		ui.loop_max_edit->setText(QString::number(loop.max));
	}
	void WidgetSetImage(const QiImage& image) {
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
	void WidgetSetPopText(const QiPopText& popText) {
		ui.popText_text_edit->setText(popText.text);
		ui.popText_time_edit->setText(QString::number(popText.time));
		ui.popText_wait_check->setChecked(popText.sync);
	}
	void WidgetSetTimer(const QiTimer& timer)
	{
		ui.timer_min_edit->setText(QString::number(timer.min));
		ui.timer_max_edit->setText(QString::number(timer.max));
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