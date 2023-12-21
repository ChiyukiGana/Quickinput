#pragma once
#pragma execution_character_set("utf-8")

#include <qcolordialog.h>
#include <qpalette.h>
#include <qevent.h>
#include <qtimer.h>
#include <qbuttongroup.h>
#include "QColorSelection.h"
#include "QPositionSelection.h"
#include "QSelectionBox.h"
#include "ui_EditUi.h"
#include "QuickInputDef.h"

class EditUi : public QDialog
{
	Q_OBJECT
public:

	EditUi(QuickInputStruct* qis = 0, List<Item>* items = 0) : QDialog()
	{
		this->qis = qis;
		this->items = items;
		qis->hdc = GetDC(0);

		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		setAttribute(Qt::WA_TranslucentBackground);

		ui.bnColorEdit->setDisabled(1);
		ui.bnLoopEdit->setDisabled(1);

		ControlInit();
		ControlEvent();
	}

private:

	const int32 posMin = -10000;
	const int32 posMax = 10000;
	const int32 moveRandMax = 9999;
	const int32 delayMax = 999999;
	const int32 delayRandMax = 9999;
	const int32 loopCountMax = 9999;
	const int32 colorMax = 255;

	Ui::EditUiClass ui;
	QuickInputStruct* qis;
	List<Item>* items = 0;
	QPoint msPos;
	bool changing = 0;
	bool editing = 0;
	bool posCtrl = 0;

	void ControlInit()
	{
		ui.hkKey->Mode(0);
		ui.hkKey->VirtualKey(VK_LBUTTON);
		ui.bnColorValue->setStyleSheet("background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #FFF,stop:0.5 #F8F,stop:1 #F00);border:none;width:20px;height:20px");
		ui.bnLoopEdit->setText(UI::etEdit);
		ui.bnColorEdit->setText(UI::etEdit);
		ui.bnDel->setText(UI::etDel);

		//RadioButton
		{
			QButtonGroup* keyRbs = new QButtonGroup(this);
			QButtonGroup* moveRbs = new QButtonGroup(this);
			QButtonGroup* colorRbs = new QButtonGroup(this);
			keyRbs->addButton(ui.rbDown);
			keyRbs->addButton(ui.rbUp);
			keyRbs->addButton(ui.rbClick);
			moveRbs->addButton(ui.rbPos);
			moveRbs->addButton(ui.rbMove);
			colorRbs->addButton(ui.rbColorGet);
			colorRbs->addButton(ui.rbColorNot);
			ui.rbDown->setChecked(1);
			ui.rbPos->setChecked(1);
			ui.rbColorGet->setChecked(1);
		}

		//Value
		{
			ui.etX->setValidator(new QIntValidator(0, posMax, this));
			ui.etY->setValidator(new QIntValidator(0, posMax, this));
			ui.etMoveRand->setValidator(new QIntValidator(0, moveRandMax, this));
			ui.etTime->setValidator(new QIntValidator(0, delayMax, this));
			ui.etDelayRand->setValidator(new QIntValidator(0, delayRandMax, this));
			ui.etCount->setValidator(new QIntValidator(0, loopCountMax, this));
			ui.etR->setValidator(new QIntValidator(0, colorMax, this));
			ui.etG->setValidator(new QIntValidator(0, colorMax, this));
			ui.etB->setValidator(new QIntValidator(0, colorMax, this));
			ui.etCX->setValidator(new QIntValidator(0, colorMax, this));
			ui.etColorL->setValidator(new QIntValidator(0, posMax, this));
			ui.etColorT->setValidator(new QIntValidator(0, posMax, this));
			ui.etColorR->setValidator(new QIntValidator(0, posMax, this));
			ui.etColorB->setValidator(new QIntValidator(0, posMax, this));
		}

		//Table
		{
			ui.tbItem->setColumnCount(3);
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"功能"));
				ui.tbItem->setHorizontalHeaderItem(0, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"参数"));
				ui.tbItem->setHorizontalHeaderItem(1, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"备注"));
				ui.tbItem->setHorizontalHeaderItem(2, tbi);
			}
			ui.tbItem->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			ui.tbItem->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
			ui.tbItem->verticalHeader()->setDefaultSectionSize(0);
			ui.tbItem->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Fixed);
			ui.tbItem->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Interactive);
			ui.tbItem->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Stretch);
			ui.tbItem->setColumnWidth(0, 100);
			ui.tbItem->setColumnWidth(1, 300);
		}

	}

	void ControlEvent()
	{
		ui.tbItem->installEventFilter(this);
		ui.tbItem->viewport()->installEventFilter(this);

		connect(ui.tbItem, SIGNAL(cellClicked(int, int)), this, SLOT(OnTbClicked(int, int)));
		connect(ui.tbItem, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(OnTbDoubleClick(int, int)));
		connect(ui.tbItem, SIGNAL(cellChanged(int, int)), this, SLOT(OnTbChanged(int, int)));
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
		connect(ui.bnKeyAdd, SIGNAL(clicked()), this, SLOT(OnBnKeyAdd()));
		connect(ui.bnMoveAdd, SIGNAL(clicked()), this, SLOT(OnBnMoveAdd()));
		connect(ui.bnDelayAdd, SIGNAL(clicked()), this, SLOT(OnBnDelayAdd()));
		connect(ui.bnLoopAdd, SIGNAL(clicked()), this, SLOT(OnBnLoopAdd()));
		connect(ui.bnLoopEdit, SIGNAL(clicked()), this, SLOT(OnBnLoopEdit()));
		connect(ui.bnColorAdd, SIGNAL(clicked()), this, SLOT(OnBnColorAdd()));
		connect(ui.bnColorEdit, SIGNAL(clicked()), this, SLOT(OnColorEdit()));
		connect(ui.bnTextAdd, SIGNAL(clicked()), this, SLOT(OnBnTextAdd()));
		connect(ui.bnDel, SIGNAL(clicked()), this, SLOT(OnBnDel()));
		connect(ui.bnPos, SIGNAL(clicked()), this, SLOT(OnBnPos()));
		connect(ui.bnColorRect, SIGNAL(clicked()), this, SLOT(OnBnColorRect()));
		connect(ui.bnColorValue, SIGNAL(clicked()), this, SLOT(OnBnColorValue()));
		connect(ui.bnEndAdd, SIGNAL(clicked()), this, SLOT(OnBnEndAdd()));
		connect(ui.bnEndLoopAdd, SIGNAL(clicked()), this, SLOT(OnBnEndLoopAdd()));
		connect(ui.rbPos, SIGNAL(toggled(bool)), this, SLOT(OnRbPos(bool)));
		connect(ui.rbMove, SIGNAL(toggled(bool)), this, SLOT(OnRbMove(bool)));
		connect(ui.rbColorGet, SIGNAL(toggled(bool)), this, SLOT(OnRbColor(bool)));
	}

	//------------------------Control Data

	void SetHkKey(BYTE key)
	{
		ui.hkKey->VirtualKey(key);
	}
	void SetEtPos(RECT pt)
	{
		ui.etX->setText(QString::number(pt.left));
		ui.etY->setText(QString::number(pt.top));
		if (pt.right > -1) ui.etMoveRand->setText(QString::number(pt.right));
	}
	void SetEtDelay(POINT pt)
	{
		ui.etTime->setText(QString::number(pt.x));
		ui.etDelayRand->setText(QString::number(pt.y));
	}
	void SetRbColorMode(bool mode) { if (mode) ui.rbColorGet->setChecked(1); else ui.rbColorNot->setChecked(1); }
	void SetEtColorRect(RECT rect)
	{
		ui.etColorL->setText(QString::number(rect.left));
		ui.etColorT->setText(QString::number(rect.top));
		ui.etColorR->setText(QString::number(rect.right));
		ui.etColorB->setText(QString::number(rect.bottom));
	}
	void SetEtColorValue(QColor color)
	{
		ui.etR->setText(QString::number(color.red()));
		ui.etG->setText(QString::number(color.green()));
		ui.etB->setText(QString::number(color.blue()));
		ui.etCX->setText(QString::number(color.alpha()));
	}
	void SetColor(QColor color) {
		ui.etR->setText(QString::number(color.red()));
		ui.etG->setText(QString::number(color.green()));
		ui.etB->setText(QString::number(color.blue()));
	}
	void SetEtLoopCount(UINT count) { ui.etCount->setText(QString::number(count)); }

	BYTE GetHkKey() { return ui.hkKey->virtualKey(); }
	RECT GetEtPos() {
		int l = ui.etX->text().toInt();
		int t = ui.etY->text().toInt();
		int r = ui.etMoveRand->text().toInt();
		if (l > posMax) l = posMax;
		if (t > posMax) t = posMax;
		if (r > posMax) r = posMax;
		if (l < posMin) l = posMin;
		if (t < posMin) t = posMin;
		return { l, t, r, 0 };
	}
	POINT GetEtDelay() {
		int x = ui.etTime->text().toInt();
		int y = ui.etDelayRand->text().toInt();
		if (x > delayMax) x = delayMax;
		if (y > delayMax) y = delayRandMax;
		return { x, y };
	}
	bool GetRbColorMode() { return ui.rbColorGet->isChecked(); }
	RECT GetEtColorRect() {
		int l = ui.etColorL->text().toInt();
		int t = ui.etColorT->text().toInt();
		int r = ui.etColorR->text().toInt();
		int b = ui.etColorB->text().toInt();
		if (l > posMax) l = posMax;
		if (t > posMax) t = posMax;
		if (r > posMax) r = posMax;
		if (b > posMax) b = posMax;
		return { l,t,r,b };
	}
	QColor GetEtColorValue() {
		int r = ui.etR->text().toInt();
		int g = ui.etG->text().toInt();
		int b = ui.etB->text().toInt();
		int x = 0;
		if (ui.etCX->text() == "") x = 5;
		else x = ui.etCX->text().toInt();
		if (r > colorMax) r = colorMax;
		if (g > colorMax) g = colorMax;
		if (b > colorMax) b = colorMax;
		if (x > colorMax) x = colorMax;
		QColor color;
		color.setRgba(qRgba(r, g, b, x));
		return color;
	}
	UINT GetEtLoopCount() {
		int i = 0;
		if (ui.etCount->text() == "") i = 1;
		else i = ui.etCount->text().toInt();
		if (i > loopCountMax) i = loopCountMax;
		return i;
	}

	void LoadHkKey(Item* item) { SetHkKey(item[0].a); }
	void LoadEtPos(Item* item) { SetEtPos({ item[0].b, item[0].c, item[0].a }); }
	void LoadEtDelay(Item* item) { SetEtDelay({ item->b, item->c }); }
	void LoadEtColorRect(Item* item)
	{
		RECT rect;
		rect.left = (DWORD)item[0].b >> 16;
		rect.top = (WORD)item[0].b;
		rect.right = (DWORD)item[0].c >> 16;
		rect.bottom = (WORD)item[0].c;
		SetEtColorRect(rect);
		SetRbColorMode(item[0].d);
	}
	void LoadEtColorValue(Item* item)
	{
		QColor color;
		color.setRgba(item[0].a);
		SetEtColorValue(color);
		SetRbColorMode(item[0].d);
	}
	void LoadEtLoopCount(Item* item)
	{
		SetEtLoopCount(item->b);
	}

	void SaveHkKey(Item* item) { item[0].a = GetHkKey(); }
	void SaveEtPos(Item* item)
	{
		RECT pt = GetEtPos();
		item[0].b = pt.left;
		item[0].c = pt.top;
		item[0].a = pt.right;
	}
	void SaveEtDelay(Item* item)
	{
		POINT pt = GetEtDelay();
		item[0].b = pt.x;
		item[0].c = pt.y;
	}
	void SaveEtColorRect(Item* item)
	{
		RECT rect = GetEtColorRect();
		item[0].b = rect.left << 16 | rect.top;
		item[0].c = rect.right << 16 | rect.bottom;
		item[0].d = GetRbColorMode();
	}
	void SaveEtColorValue(Item* item)
	{
		QColor color = GetEtColorValue();
		item[0].a = color.rgba();
		item[0].d = GetRbColorMode();
	}
	void SaveEtLoopCount(Item* item)
	{
		item[0].b = GetEtLoopCount();
	}

	//=================Control Data

	void SetChanging()
	{
		if (changing)
		{
			ui.bnKeyAdd->setText(UI::etChange);
			ui.bnMoveAdd->setText(UI::etChange);
			ui.bnDelayAdd->setText(UI::etChange);
			ui.bnLoopAdd->setText(UI::etChange);
			ui.bnTextAdd->setText(UI::etChange);
			ui.bnColorAdd->setText(UI::etChange);
			ui.bnEndAdd->setText(UI::etChange);
			ui.bnEndLoopAdd->setText(UI::etChange);
		}
		else
		{
			ui.bnKeyAdd->setText(UI::etAdd);
			ui.bnMoveAdd->setText(UI::etAdd);
			ui.bnDelayAdd->setText(UI::etAdd);
			ui.bnLoopAdd->setText(UI::etAdd);
			ui.bnTextAdd->setText(UI::etAdd);
			ui.bnColorAdd->setText(UI::etAdd);
			ui.bnEndAdd->setText(UI::etAdd);
			ui.bnEndLoopAdd->setText(UI::etAdd);
		}
	}

	void TbUpdate()
	{
		ui.tbItem->clearMask();
		ui.tbItem->setRowCount(items[0].len());
		ui.tbItem->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.tbItem->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tbItem->verticalHeader()->setDefaultSectionSize(0);

		for (uint32 u = 0; u < items[0].len(); u++)
		{
			QString ps;
			switch (items[0][u].type)
			{
			case Item::endLoop:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiEndLoop));
				break;
			case Item::end:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiEnd));
				break;
			case Item::delay:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiWait));
				ps = QString::number(items[0][u].b);
				ps += "ㅤㅤ";
				ps += QString::number(items[0][u].c);
				break;
			case Item::down:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiDown));
				ps = QString::fromWCharArray(Input::Name(items[0][u].a));
				break;
			case Item::up:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiUp));
				ps = QString::fromWCharArray(Input::Name(items[0][u].a));
				break;
			case Item::click:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiClick));
				ps = QString::fromWCharArray(Input::Name(items[0][u].a));
				break;
			case Item::pos:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiPos));
				ps = QString::number(items[0][u].b);
				ps += " - ";
				ps += QString::number(items[0][u].c);
				ps += "ㅤㅤ";
				ps += QString::number(items[0][u].a);
				break;
			case Item::move:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiMove));
				ps = QString::number(items[0][u].b);
				ps += "ㅤㅤ";
				ps += QString::number(items[0][u].c);
				ps += "ㅤㅤ";
				ps += QString::number(items[0][u].a);
				break;
			case Item::text:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiText));
				ps = QString::fromWCharArray(items[0][u].s.substr(0, 32).c_str());
				if (items[0][u].s.length() > 31) ps += "...";
				break;
			case Item::color:
			{
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiColor));
				QColor color;
				color.setRgba(items[0][u].a);
				ps = "(";
				ps += QString::number(HIWORD(items[0][u].b));
				ps += ",";
				ps += QString::number(LOWORD(items[0][u].b));
				ps += ",";
				ps += QString::number(HIWORD(items[0][u].c));
				ps += ",";
				ps += QString::number(LOWORD(items[0][u].c));
				ps += ")　(";
				ps += QString::number(color.red());
				ps += ",";
				ps += QString::number(color.green());
				ps += ",";
				ps += QString::number(color.blue());
				ps += ",";
				ps += QString::number(color.alpha());
				if (items[0][u].d) ps += ")　找到";
				else ps += ")　未找到";
				break;
			}
			case Item::loop:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiLoop));
				if (items[0][u].b) ps = QString::number(items[0][u].b);
				else ps = "无限";
				break;
			}
			ui.tbItem->setItem(u, 1, new QTableWidgetItem(ps));
			if (items[0][u].type == Item::text) ui.tbItem->setItem(u, 2, new QTableWidgetItem(""));
			else ui.tbItem->setItem(u, 2, new QTableWidgetItem(QString::fromWCharArray(items[0][u].s.c_str())));
			ui.tbItem->item(u, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbItem->item(u, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbItem->item(u, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
	}

	void AddItem(int32 type)
	{
		Item item;
		int pos = ui.tbItem->currentRow();

		if (pos < 0) pos = items[0].len();
		else if ((pos + 1) <= items[0].len()) pos++;

		switch (type)
		{
		case Item::endLoop:
			item.type = Item::endLoop;
			break;
		case Item::end:
			item.type = Item::end;
			break;
		case Item::delay:
			item.type = Item::delay;
			SaveEtDelay(&item);
			break;
		case Item::down:
			item.type = Item::down;
			SaveHkKey(&item);
			break;
		case Item::up:
			item.type = Item::up;
			SaveHkKey(&item);
			break;
		case Item::click:
			item.type = Item::click;
			SaveHkKey(&item);
			break;
		case Item::pos:
			item.type = Item::pos;
			SaveEtPos(&item);
			break;
		case Item::move:
			item.type = Item::move;
			SaveEtPos(&item);
			break;
		case Item::text:
			item.type = Item::text;
			item.s = (LPCWSTR)ui.etText->toPlainText().utf16();
			break;
		case Item::color:
			item.type = Item::color;
			SaveEtColorRect(&item);
			SaveEtColorValue(&item);
			break;
		case Item::loop:
			item.type = Item::loop;
			SaveEtLoopCount(&item);
		}

		items[0].Ins(pos, item);
		TbUpdate();
		ui.tbItem->setCurrentItem(ui.tbItem->item(pos, 0));
	}

	void ChangeItem(int32 type)
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		switch (type)
		{
		case Item::endLoop:
			items[0][pos].type = Item::endLoop;
			break;
		case Item::end:
			items[0][pos].type = Item::end;
			break;
		case Item::delay:
			items[0][pos].type = Item::delay;
			items[0][pos].b = ui.etTime->text().toInt();
			items[0][pos].c = ui.etDelayRand->text().toInt();
			SaveEtDelay(&items[0][pos]);
			break;
		case Item::down:
			items[0][pos].type = Item::down;
			SaveHkKey(&items[0][pos]);
			break;
		case Item::up:
			items[0][pos].type = Item::up;
			SaveHkKey(&items[0][pos]);
			break;
		case Item::click:
			items[0][pos].type = Item::click;
			SaveHkKey(&items[0][pos]);
			break;
		case Item::pos:
			items[0][pos].type = Item::pos;
			SaveEtPos(&items[0][pos]);
			break;
		case Item::move:
			items[0][pos].type = Item::move;
			SaveEtPos(&items[0][pos]);
			break;
		case Item::text:
			items[0][pos].type = Item::text;
			items[0][pos].s = (LPCWSTR)ui.etText->toPlainText().utf16();
			break;
		case Item::color:
			items[0][pos].type = Item::color;
			SaveEtColorRect(&items[0][pos]);
			SaveEtColorValue(&items[0][pos]);
			break;
		case Item::loop:
			items[0][pos].type = Item::loop;
			SaveEtLoopCount(&items[0][pos]);
			break;
		}
		TbUpdate();
		ui.tbItem->setCurrentItem(0);
		changing = 0;
		SetChanging();
	}

	void MoveItem(bool up)
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		if (up && ((pos - 1) >= 0))
		{
			items[0].Swp(pos, pos - 1);
			ui.tbItem->setCurrentItem(ui.tbItem->item(pos - 1, 0));
		}
		else if (!up && (pos + 1) < ui.tbItem->rowCount())
		{
			items[0].Swp(pos, pos + 1);
			ui.tbItem->setCurrentItem(ui.tbItem->item(pos + 1, 0));
		}
		TbUpdate();
	}

	void showEvent(QShowEvent*)
	{
		ui.lbTitle->setText(windowTitle());
		TbUpdate();
	}

	void mousePressEvent(QMouseEvent* et)
	{
		if (et->buttons() & Qt::LeftButton) msPos = et->pos();
	}

	void mouseMoveEvent(QMouseEvent* et)
	{
		if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - msPos);
	}

private slots:

	bool eventFilter(QObject* obj, QEvent* et)
	{
		if (obj == ui.tbItem)
		{
			if (et->type() == QEvent::KeyPress)
			{
				QKeyEvent* key = static_cast<QKeyEvent*>(et);
				if (key->modifiers() == Qt::ShiftModifier)
				{
					if (key->key() == Qt::Key_Backspace)
					{
						OnBnDel();
						return 1;
					}
					else if (key->key() == Qt::Key_Up)
					{
						MoveItem(1);
						return 1;
					}
					else if (key->key() == Qt::Key_Down)
					{
						MoveItem(0);
						return 1;
					}
				}
			}
		}
		if (obj == ui.tbItem->viewport())
		{
			if (et->type() == QEvent::Drop)
			{
				QDropEvent* drop = static_cast<QDropEvent*>(et);
				QTableWidgetItem* item = ui.tbItem->itemAt(drop->pos());
				int before = 0, after = 0;
				before = ui.tbItem->currentRow();
				if (before < 0) return 0;

				if (item) after = item->row();
				else after = ui.tbItem->rowCount() - 1;

				if (after < 0) return 0;

				items[0].Mov(before, after);
				TbUpdate();
				ui.tbItem->setCurrentItem(ui.tbItem->item(after, 0));
				return 1;
			}
		}
		return 0;
	}

	void OnTbClicked(int row, int column)
	{
		ui.bnColorEdit->setDisabled(1);
		ui.bnLoopEdit->setDisabled(1);

		if (row < 0) return;
		if (column == 2)
		{
			ui.tbItem->editItem(ui.tbItem->item(row, column));
		}
		else
		{
			if (items[0][row].type == Item::color) ui.bnColorEdit->setDisabled(0);
			else if (items[0][row].type == Item::loop) ui.bnLoopEdit->setDisabled(0);
		}
	}

	void OnTbDoubleClick(int row, int column)
	{
		if (row < 0) return;

		switch (items[0][row].type)
		{
		case Item::delay:
			LoadEtDelay(&items[0][row]);
			break;
		case Item::down:
			ui.rbDown->setChecked(1);
			ui.rbUp->setChecked(0);
			ui.rbClick->setChecked(0);
			LoadHkKey(&items[0][row]);
			break;
		case Item::up:
			ui.rbDown->setChecked(0);
			ui.rbUp->setChecked(1);
			ui.rbClick->setChecked(0);
			LoadHkKey(&items[0][row]);
			break;
		case Item::click:
			ui.rbDown->setChecked(0);
			ui.rbUp->setChecked(0);
			ui.rbClick->setChecked(1);
			LoadHkKey(&items[0][row]);
			break;
		case Item::pos:
			ui.rbPos->setChecked(1);
			OnRbPos(1);
			LoadEtPos(&items[0][row]);
			break;
		case Item::move:
			ui.rbMove->setChecked(1);
			OnRbMove(1);
			LoadEtPos(&items[0][row]);
			break;
		case Item::text:
			ui.etText->setText(QString::fromWCharArray(items[0][row].s.c_str()));
			break;
		case Item::color:
			LoadEtColorRect(&items[0][row]);
			LoadEtColorValue(&items[0][row]);
			break;
		case Item::loop:
			LoadEtLoopCount(&items[0][row]);
			break;
		}
		changing = 1;
		SetChanging();
	}

	void OnTbChanged(int row, int column)
	{
		if (row < 0) return;
		if (column != 2) return;
		if (items[0][row].type == Item::text) return;
		items[0][row].s = ui.tbItem->item(row, 2)->text().toStdWString();
	}

	void OnBnClose()
	{
		hide();
	}

	void OnBnKeyAdd()
	{
		if (ui.rbDown->isChecked())
		{
			if (changing) ChangeItem(Item::down);
			else AddItem(Item::down);
		}
		else if (ui.rbUp->isChecked())
		{
			if (changing) ChangeItem(Item::up);
			else AddItem(Item::up);
		}
		else if (ui.rbClick->isChecked())
		{
			if (changing) ChangeItem(Item::click);
			else AddItem(Item::click);
		}
	}

	void OnBnMoveAdd()
	{
		if (ui.rbMove->isChecked())
		{
			if (changing) ChangeItem(Item::move);
			else AddItem(Item::move);
		}
		else
		{
			if (changing) ChangeItem(Item::pos);
			else AddItem(Item::pos);
		}
	}

	void OnBnDelayAdd()
	{
		if (changing) ChangeItem(Item::delay);
		else AddItem(Item::delay);
	}

	void OnBnLoopAdd()
	{
		if (changing) ChangeItem(Item::loop);
		else AddItem(Item::loop);
	}

	void OnBnLoopEdit()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		if (items[0][pos].type != Item::loop) return;
		EditUi edit(qis, &items[0][pos].next);
		edit.setWindowTitle("编辑 - 循环");
		edit.exec();
	}

	void OnBnTextAdd()
	{
		List<Item> item = *this->items;

		if (changing) ChangeItem(Item::text);
		else AddItem(Item::text);
	}

	void OnBnColorAdd()
	{
		if (changing) ChangeItem(Item::color);
		else AddItem(Item::color);
	}

	void OnColorEdit()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		if (items[0][pos].type != Item::color) return;
		EditUi edit(qis, &items[0][pos].next);
		edit.setWindowTitle("编辑 - 查找颜色");
		edit.exec();
	}

	void OnBnEndAdd()
	{
		if (changing) ChangeItem(Item::end);
		else AddItem(Item::end);
	}

	void OnBnEndLoopAdd()
	{
		if (changing) ChangeItem(Item::endLoop);
		else AddItem(Item::endLoop);
	}

	void OnBnDel()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;
		items->Del(pos);
		TbUpdate();
		if ((pos - 1) >= 0) ui.tbItem->setCurrentItem(ui.tbItem->item(pos - 1, 0));

		changing = 0;
		SetChanging();
	}

	void OnBnPos()
	{
		QPositionSelection ps;
		POINT pt;
		ps.Start();
		pt = ps.absPos();
		pt = RelToAbs({ pt.x, pt.y });
		SetEtPos({ pt.x, pt.y, -1 });
	}

	void OnBnColorRect()
	{
		QSelectionBox sb;
		sb.setBoxStyleSheet("background-color:rgba(240,192,224,0.5);border:1px solid #F00;");
		sb.Start();
		RECT rect = sb.absRect();
		POINT lt = RelToAbs({ rect.left, rect.top });
		POINT rb = RelToAbs({ rect.right, rect.bottom });
		RECT r = { lt.x, lt.y, rb.x, rb.y };
		SetEtColorRect({ r.left, r.top, r.right, r.bottom });
	}

	void OnBnColorValue()
	{
		QColorSelection cs;
		QColorDialog cd(cs.Start(), this);
		cd.exec();
		SetColor(cd.currentColor());
		QString style = "background-color:rgb(";
		style += QString::number(cd.currentColor().red());
		style += ",";
		style += QString::number(cd.currentColor().green());
		style += ",";
		style += QString::number(cd.currentColor().blue());
		style += ");border:none;width:20px;height:20px";
		ui.bnColorValue->setStyleSheet(style);
	}

	void OnRbPos(bool state)
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

	void OnRbMove(bool state)
	{
		if (state)
		{
			ui.etX->setValidator(new QIntValidator(posMin, posMax, this));
			ui.etY->setValidator(new QIntValidator(posMin, posMax, this));
			ui.bnPos->setText("×");
			ui.bnPos->setDisabled(1);
		}
	}

	void OnRbColor(bool state) {
	}
};