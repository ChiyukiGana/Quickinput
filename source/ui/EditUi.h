#pragma once
#include <src/inc_header.h>
#include "ui_EditUi.h"
class EditUi : public QDialog
{
	Q_OBJECT;
	DecWindowMove;
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

	// context menu
	QMenu* menu;
	QAction* muDel;
	QAction* muCut;
	QAction* muCopy;
	QAction* muPaste;
	QAction* muEdit;
	QAction* muEdit2;

	QMenu* titleMenu;
	QAction* muBack;
	QAction* muSave;
	QAction* muDiscard;

	// widget bind
	QiVector<size_t> bind_type_tab = QiVector<size_t>(QiType::count, 0);
	QiVector<QGroupBox*> bind_type_group = QiVector<QGroupBox*>(QiType::count, nullptr);
	QiVector<QPushButton*> bind_tab_button = QiVector<QPushButton*>(TabIndex::tab_count, nullptr);
	QiVector<QPushButton*> bind_add_button = QiVector<QPushButton*>(QiType::count, nullptr);
	QiVector<QPushButton*> bind_chg_button = QiVector<QPushButton*>(QiType::count, nullptr);
	QiVector<QPushButton*> bind_edt_button = QiVector<QPushButton*>(QiType::count, nullptr);
	QiVector<QPushButton*> bind_edt2_button = QiVector<QPushButton*>(QiType::count, nullptr);

	// custom widget
	QPointView widget_pv;
	QPointView widget_mkpv;
	QRectView widget_rv;
	QTextDialog widget_td;

	// for image viewer
	RgbMap imageMap;

	// macro
	Macro* macro = nullptr;
	Actions* actions = nullptr;
	Actions* actionsRoot = nullptr;

public:
	EditUi(Macro* macro, Actions* actions);

private:
	void Init();
	void Init_Bind();

	void Event();
	void Event_Action_Widget();
	void Event_Table_Selection();

	void StyleGroup();


	void DisableTip(bool disable);
	void DisableMenus();
	void DisableChangeButtons(bool disable);


	void setWindowTitle(const QString& title);
	void SetTabCurrent(int index);
	void SetGroupCurrent(int type = QiType::none);
	void SetEditCurrent(int type = QiType::none);
	void SetChangeCurrent(int type = QiType::none);
	void SetDebugState(int debugState);
	void SetWindowMode();

	void SelectWindow();
	void Reload();
	void NextEdit(bool edit2);


	void TableUpdate(int index);
	void TableUpdate(int begin, int end);
	void TableReload();
	void TableInsert(int index);
	void TableRemove(int index);
	void TableSelection(const QiVector<int> selection);


	void ItemSet(int p);
	Action ItemGet(int type);
	void ItemMove(bool up, int len);
	void ItemAdd(int type);
	void ItemChange(int type);
	void ItemDel();
	void ItemCut();
	void ItemCopy();
	void ItemPaste();

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

	// load params to widget
	void WidgetSet(const QiKey& key);
	void WidgetSet(const QiKeyState& keyState);
	void WidgetSet(const QiMouse& mouse);
	void WidgetSet(const QiDelay& delay);
	void WidgetSet(const QiCopyText& text);
	void WidgetSet(const QiColor& color);
	void WidgetSet(const QiLoop& loop);
	void WidgetSet(const QiImage& image);
	void WidgetSet(const QiPopText& popText);
	void WidgetSet(const QiTimer& timer);
	void WidgetSet(const QiDialog& dialog);
	void WidgetSet(const QiQuickInput& quickInput);
	void WidgetSet(const QiKeyBlock& keyBlock);
	void WidgetSet(const QiClock& clock);
	void WidgetSet(const QiOcr& ocr);
	void WidgetSet(const QiVarOperator& varOperator);
	void WidgetSet(const QiVarCondition& varCondition);
	void WidgetSet(const QiOpen& open);
	void WidgetSet(const QiTextPad& textPad);
	void WidgetSet(const QiEditDialog& editDialog);
	void WidgetSet(const QiVolume& volume);
	void WidgetSet(const QiSoundPlay& soundPlay);


	// iter binds
	template<class Ty, class Fn>
	void BindSafeIter(QiVector<Ty*>& bind, Fn call);

	// iter actions
	bool IterActions(Actions& actions, std::function<bool(Action&)> callBack, int type = QiType::none);
	bool IterActions(const Actions& actions, std::function<bool(const Action&)> callBack, int type = QiType::none);


	// type: jumpPoint, jump, block, blockExec
	QiVector<int> LoadIds(const Actions& actions, int type);
	Actions LoadType(const Actions& actions, int type);
	bool FindId(const QiVector<int>& ids, int id);
	// type: jump, blockExec
	void InvalidId(Actions& actions, int type);
	int UniqueId(const QiVector<int>& ids);
	// type: jumpPoint, block
	void UniqueActionsId(QiVector<int> ids, Actions& actions, int type);
	// id list
	void ListJumpPointReload();
	void ListBlockReload();


	// quickinput
	QiVector<unsigned char> StringToKey(const QString str);
	QString KeyToString(const QiVector<unsigned char >& keys);

	void Forward(const QString& title, Actions* next);
	void Back();
	void Exit(bool save = true);

	bool event(QEvent*);
	bool eventFilter(QObject*, QEvent*);
	void showEvent(QShowEvent*);
	void customEvent(QEvent*);
};