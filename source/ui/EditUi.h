#pragma once
#include <src/inc_header.h>
#include "ui_EditUi.h"
class EditUi : public QDialogFrameless
{
	Q_OBJECT;
	using This = EditUi;
	enum TabIndex
	{
		tab_base,
		tab_loop,
		tab_state,
		tab_time,
		tab_jump,
		tab_block,
		tab_text,
		tab_ocr,
		tab_image,
		tab_dialog,
		tab_file,
		tab_var,
		tab_window,
		tab_count
	};
	enum TableColumn
	{
		tableColumn_debug,
		tableColumn_disable,
		tableColumn_type,
		tableColumn_param,
		tableColumn_mark,
		tableColumn_count
	};
	enum DebugState
	{
		debug_idel,
		debug_run,
		debug_pause
	};

	Ui::EditUiClass ui;
	
	// edit ui param
	struct Layer
	{
		QString title;
		Actions* actions;
		QiVector<int> items;
	};
	QList<Layer> layers;
	bool changing = false;
	bool updating = false;
	QiVector<int> tableCurrent;
	QiVector<int> tableCurrentPrev;

	// debug run
	QTimer* testTimer;
	QTimer* markPointTimer;
	int debugState = debug_idel;
	std::future<void> varop;

	// context menu
	QMenu* menu;
	QAction* muDel;
	QAction* muCut;
	QAction* muCopy;
	QAction* muPaste;
	QAction* muRedo;
	QAction* muUndo;
	QAction* muEdit;
	QAction* muEdit2;

	QMenu* titleMenu;
	QAction* muBack;
	QAction* muSave;
	QAction* muDiscard;

	// widget bind
	QiVector<size_t> bind_type_tab = QiVector<size_t>(static_cast<size_t>(QiType::count), 0);
	QiVector<QGroupBox*> bind_type_group = QiVector<QGroupBox*>(static_cast<size_t>(QiType::count), nullptr);
	QiVector<QPushButton*> bind_tab_button = QiVector<QPushButton*>(TabIndex::tab_count, nullptr);
	QiVector<QPushButton*> bind_add_button = QiVector<QPushButton*>(static_cast<size_t>(QiType::count), nullptr);
	QiVector<QPushButton*> bind_chg_button = QiVector<QPushButton*>(static_cast<size_t>(QiType::count), nullptr);
	QiVector<QPushButton*> bind_edt_button = QiVector<QPushButton*>(static_cast<size_t>(QiType::count), nullptr);
	QiVector<QPushButton*> bind_edt2_button = QiVector<QPushButton*>(static_cast<size_t>(QiType::count), nullptr);

	// custom widget
	QPointView widget_pv;
	QPointView widget_mkpv;
	QRectView widget_rv;
	QRectView range_rv;
	QTextDialog widget_td;

	// for image viewer
	RgbMap imageMap;

	// macro
	Macro* macro = nullptr;
	Actions* actions = nullptr;
	Actions* actionsRoot = nullptr;
	HistoryActions actionsHistory;

	QiIdList jumpIds;
	QiIdList blockIds;

	struct IDChecker
	{
		bool jump;
		bool block;
		QiIdList& jumpIds;
		QiIdList& blockIds;
		IDChecker(QiIdList& jump_ids, QiIdList& block_ids) : jump(false), block(false), jumpIds(jump_ids), blockIds(block_ids) {}
		void check(const Action& action)
		{
			if (jump && block) return;
			if (!jump && action.type() == QiType::jumpPoint) jump = true;
			if (!block && action.type() == QiType::block) block = true;
			if (jump && block) return;
			action.iter([this](const Action& a) {
				if (!jump && a.type() == QiType::jumpPoint) jump = true;
				if (!block && a.type() == QiType::block) block = true;
				return jump && block;
				});
		}
		void check(const Actions& actions)
		{
			if (jump && block) return;
			actions.iter([this](const Action& a) {
				if (!jump && a.index() == static_cast<size_t>(QiType::jumpPoint)) jump = true;
				if (!block && a.index() == static_cast<size_t>(QiType::block)) block = true;
				return jump && block;
				});
		}
		void load(const Actions& actions)
		{
			jumpIds = actions.loadId(QiType::jumpPoint);
			blockIds = actions.loadId(QiType::block);
		}
		bool reset(const Actions& actions)
		{
			bool result = jump || block;
			if (jump) jumpIds = actions.loadId(QiType::jumpPoint);
			if (block) blockIds = actions.loadId(QiType::block);
			jump = false;
			block = false;
			return result;
		}
		bool jump_exist() const
		{
			return jump;
		}
		bool block_exist() const
		{
			return block;
		}
	};

	IDChecker idChecker;

public:
	EditUi(Macro* macro);

private:
	void Init();
	void Init_Bind();

	void Event();
	void Event_Action_Widget();
	void Event_Table_Selection();

	void StyleGroup();


	void Disable(bool disable);
	void DisableTip(bool disable);
	void DisableMenus();
	void DisableChangeButtons(bool disable);


	void setWindowTitle(const QString& title);
	void SetTabCurrent(int index);
	void SetGroupCurrent(QiType type = QiType::none);
	void SetEditCurrent(QiType type = QiType::none);
	void SetDebugState(int debugState);
	void SetWindowMode();

	void SelectWindow();
	void Reload();
	void WidgetReload();
	void NextEdit(bool edit2);


	void TableUpdate(int index);
	void TableUpdate(int begin, int end);
	void TableReload();
	void TableInsert(int index);
	void TableRemove(int index);
	void TableSelection(const QiVector<int> selection);


	void ItemSet(int p);
	Action ItemGet(QiType type);
	void ItemMove(bool up, int len);
	void ItemAdd(QiType type);
	void ItemChange(QiType type);
	void ItemDel();
	void ItemCut();
	void ItemCopy();
	void ItemPaste();
	void Redo();
	void Undo();

	// get params from widget
	QiKey WidgetGetKey();
	QiKeyState WidgetGetKeyState();
	QiMouse WidgetGetMouse();
	QiDelay WidgetGetDelay();
	QiCopyText WidgetGetText();
	QiColor WidgetGetColor();
	QiLoop WidgetGetLoop();
	QiImage WidgetGetImage();
	QiPopText WidgetGetPopText();
	QiTimer WidgetGetTimer();
	QiJump WidgetGetJump();
	QiJumpPoint WidgetGetJumpPoint();
	QiDialog WidgetGetDialog();
	QiBlock WidgetGetBlock();
	QiBlockExec WidgetGetBlockExec();
	QiQuickInput WidgetGetQuickInput();
	QiKeyBlock WidgetGetKeyBlock();
	QiClock WidgetGetClock();
	QiOcr WidgetGetOcr();
	QiVarOperator WidgetGetVarOperator();
	QiVarCondition WidgetGetVarCondition();
	QiOpen WidgetGetOpen();
	QiTextPad WidgetGetTextPad();
	QiEditDialog WidgetGetEditDialog();
	QiVolume WidgetGetVolume();
	QiSoundPlay WidgetGetSoundPlay();
	QiMsgView WidgetGetMsgView();
	QiRangeSet WidgetGetRange();

	// load params to widget
	void WidgetSet(const QiKey&);
	void WidgetSet(const QiKeyState&);
	void WidgetSet(const QiMouse&);
	void WidgetSet(const QiDelay&);
	void WidgetSet(const QiCopyText&);
	void WidgetSet(const QiColor&);
	void WidgetSet(const QiLoop&);
	void WidgetSet(const QiImage&);
	void WidgetSet(const QiPopText&);
	void WidgetSet(const QiTimer&);
	void WidgetSet(const QiDialog&);
	void WidgetSet(const QiQuickInput&);
	void WidgetSet(const QiKeyBlock&);
	void WidgetSet(const QiClock&);
	void WidgetSet(const QiOcr&);
	void WidgetSet(const QiVarOperator&);
	void WidgetSet(const QiVarCondition&);
	void WidgetSet(const QiOpen&);
	void WidgetSet(const QiTextPad&);
	void WidgetSet(const QiEditDialog&);
	void WidgetSet(const QiVolume&);
	void WidgetSet(const QiSoundPlay&);
	void WidgetSet(const QiMsgView&);
	void WidgetSet(const QiRangeSet&);


	// iter binds
	template<class Ty, class Fn>
	void BindSafeIter(QiVector<Ty*>& bind, Fn call);

	// type: jumpPoint, block
	void UniqueActionsId(QiIdList ids, Actions& actions, QiType type, QiType type2);
	// id list
	void ListJumpPointReload();
	void ListBlockReload();


	void Forward(const QString& title, Actions* next);
	void Back();
	void Exit(bool save = true);

	bool event(QEvent*);
	bool eventFilter(QObject*, QEvent*);
	void showEvent(QShowEvent*);
	void resizeEvent(QResizeEvent*);
	void customEvent(QEvent*);
};