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

	EditUi(QWidget* main = 0, QuickInputStruct* qis = 0, List<Item>* items = 0) : QDialog(main)
	{
		this->main = main;
		this->qis = qis;
		this->items = items;
		qis->hdc = GetDC(0);

		ui.setupUi(this);
		setParent(0);
		setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

		ui.bnColorEdit->setDisabled(1);
		ui.bnLoopEdit->setDisabled(1);

		ControlInit();
		ControlEvent();
		FontResize();
		TbUpdate();
	}

private:

	Ui::EditUiClass ui;
	QWidget* main;
	QuickInputStruct* qis;
	List<Item>* items = 0;
	bool changing = 0;
	bool editing = 0;
	bool posCtrl = 0;

	void FontResize()
	{
		ui.tbItem->setFont(UI::font1);
		{
			ui.lbKey->setFont(UI::font1);
			ui.hkKey->setFont(UI::font1);
			ui.rbDown->setFont(UI::font1);
			ui.rbUp->setFont(UI::font1);
			ui.rbClick->setFont(UI::font1);
			ui.bnKeyAdd->setFont(UI::font1);

			ui.bnKeyAdd->setText(UI::etAdd);
		}
		{
			ui.lbMove->setFont(UI::font1);
			ui.rbPos->setFont(UI::font1);
			ui.bnPos->setFont(UI::font1);
			ui.rbMove->setFont(UI::font1);
			ui.lbX->setFont(UI::font1);
			ui.etX->setFont(UI::font1);
			ui.lbY->setFont(UI::font1);
			ui.etY->setFont(UI::font1);
			ui.lbMoveRand->setFont(UI::font1);
			ui.etMoveRand->setFont(UI::font1);
			ui.bnMoveAdd->setFont(UI::font1);

			ui.bnMoveAdd->setText(UI::etAdd);
		}
		{
			ui.lbDelay->setFont(UI::font1);
			ui.lbTime->setFont(UI::font1);
			ui.lbDelayRand->setFont(UI::font1);
			ui.etTime->setFont(UI::font1);
			ui.etDelayRand->setFont(UI::font1);
			ui.bnDelayAdd->setFont(UI::font1);

			ui.bnDelayAdd->setText(UI::etAdd);
		}
		{
			ui.lbLoop->setFont(UI::font1);
			ui.lbCount->setFont(UI::font1);
			ui.etCount->setFont(UI::font1);
			ui.bnLoopEdit->setFont(UI::font1);
			ui.bnLoopAdd->setFont(UI::font1);

			ui.bnLoopEdit->setText(UI::etEdit);
			ui.bnLoopAdd->setText(UI::etAdd);
		}
		{
			ui.lbText->setFont(UI::font1);
			ui.etText->setFont(UI::font1);
			ui.bnTextAdd->setFont(UI::font1);

			ui.bnTextAdd->setText(UI::etAdd);
		}
		{
			ui.lbFindColor->setFont(UI::font1);
			ui.rbColorGet->setFont(UI::font1);
			ui.rbColorNot->setFont(UI::font1);
			ui.lbRange->setFont(UI::font1);
			ui.etColorL->setFont(UI::font1);
			ui.etColorT->setFont(UI::font1);
			ui.etColorR->setFont(UI::font1);
			ui.etColorB->setFont(UI::font1);
			ui.bnColorRect->setFont(UI::font1);

			ui.lbR->setFont(UI::font1);
			ui.etR->setFont(UI::font1);
			ui.lbG->setFont(UI::font1);
			ui.etG->setFont(UI::font1);
			ui.lbB->setFont(UI::font1);
			ui.etB->setFont(UI::font1);
			ui.lbExtend->setFont(UI::font1);
			ui.etCX->setFont(UI::font1);
			ui.bnColorValue->setFont(UI::font1);

			ui.bnColorEdit->setFont(UI::font1);
			ui.bnColorAdd->setFont(UI::font1);

			ui.bnColorEdit->setText(UI::etEdit);
			ui.bnColorAdd->setText(UI::etAdd);
		}
		{
			ui.lbEnd->setFont(UI::font1);
			ui.lbEndLoop->setFont(UI::font1);
			ui.bnEndAdd->setFont(UI::font1);
			ui.bnEndLoopAdd->setFont(UI::font1);
		}
		{
			ui.lbDelete->setFont(UI::font1);
			ui.bnDel->setFont(UI::font1);
			ui.bnDel->setText(UI::etDel);
			ui.lbTips->setFont(UI::font1);
		}
	}

	void ControlInit()
	{
		ui.hkKey->Mode(0);
		ui.hkKey->VirtualKey(VK_LBUTTON);
		ui.bnColorValue->setStyleSheet("background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #FFF,stop:0.5 #F8F,stop:1 #F00);border:none;");

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
			ui.etX->setValidator(new QIntValidator(0, 10000, this));
			ui.etY->setValidator(new QIntValidator(0, 10000, this));
			ui.etMoveRand->setValidator(new QIntValidator(0, 1000, this));
			ui.etTime->setValidator(new QIntValidator(0, 100000, this));
			ui.etDelayRand->setValidator(new QIntValidator(0, 1000, this));
			ui.etCount->setValidator(new QIntValidator(-1, 1000, this));
			ui.etR->setValidator(new QIntValidator(0, 255, this));
			ui.etG->setValidator(new QIntValidator(0, 255, this));
			ui.etB->setValidator(new QIntValidator(0, 255, this));
			ui.etCX->setValidator(new QIntValidator(0, 255, this));
			ui.etColorL->setValidator(new QIntValidator(0, 10000, this));
			ui.etColorT->setValidator(new QIntValidator(0, 10000, this));
			ui.etColorR->setValidator(new QIntValidator(0, 10000, this));
			ui.etColorB->setValidator(new QIntValidator(0, 10000, this));
		}

		//Table
		{
			ui.tbItem->setColumnCount(2);
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"功能"));
				tbi->setFont(UI::font2);
				ui.tbItem->setHorizontalHeaderItem(0, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"参数"));
				tbi->setFont(UI::font2);
				ui.tbItem->setHorizontalHeaderItem(1, tbi);
			}
			ui.tbItem->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			ui.tbItem->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			ui.tbItem->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
			ui.tbItem->setColumnWidth(0, 100);
		}

	}

	void ControlEvent()
	{
		ui.tbItem->installEventFilter(this);

		connect(ui.tbItem, SIGNAL(cellClicked(int, int)), this, SLOT(OnTbSelected(int, int)));
		connect(ui.tbItem, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(OnTbItemsDbClick(int, int)));
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
	void SetRbColorMode(bool mode) { if (!mode) ui.rbColorGet->setChecked(1); else ui.rbColorNot->setChecked(1); }
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
	void SetEtLoopCount(UINT count)
	{
		ui.etCount->setText(QString::number(count));
	}

	BYTE GetHkKey() { return ui.hkKey->virtualKey(); }
	RECT GetEtPos() { return { ui.etX->text().toInt(), ui.etY->text().toInt(), ui.etMoveRand->text().toInt() }; }
	POINT GetEtDelay() { return { ui.etTime->text().toInt(), ui.etDelayRand->text().toInt() }; }
	bool GetRbColorMode() { return ui.rbColorGet->isChecked(); }
	RECT GetEtColorRect() { return { ui.etColorL->text().toInt(), ui.etColorT->text().toInt(), ui.etColorR->text().toInt(), ui.etColorB->text().toInt(), }; }
	QColor GetEtColorValue() {
		int r = ui.etR->text().toInt();
		int g = ui.etG->text().toInt();
		int b = ui.etB->text().toInt();
		int x = 0;
		if (ui.etCX->text() == "") x = 5;
		else x = ui.etCX->text().toInt();
		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;
		if (x > 255) x = 255;
		QColor color;
		color.setRgba(qRgba(r, g, b, x));
		return color;
	}
	UINT GetEtLoopCount() { return ui.etCount->text().toInt(); }

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
		if (item[0].text == L"N") SetRbColorMode(1);
		else SetRbColorMode(0);
	}
	void LoadEtColorValue(Item* item)
	{
		QColor color;
		color.setRgba(item[0].a);
		SetEtColorValue(color);
		if (item[0].text == L"N") SetRbColorMode(1);
		else SetRbColorMode(0);
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
		if (GetRbColorMode()) item[0].text = L"Y";
		else item[0].text = L"N";
	}
	void SaveEtColorValue(Item* item)
	{
		QColor color = GetEtColorValue();
		item[0].a = color.rgba();
		if (GetRbColorMode()) item[0].text = L"Y";
		else item[0].text = L"N";
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
		ui.tbItem->verticalHeader()->setDefaultSectionSize(0);

		for (uint32 u = 0; u < items[0].len(); u++)
		{
			QString ps;
			switch (items[0][u].type)
			{
			case -2:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiEndLoop));
				break;
			case -1:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiEnd));
				break;
			case 0:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiWait));
				ps = QString::number(items[0][u].b);
				ps += "ㅤㅤ";
				ps += QString::number(items[0][u].c);
				break;
			case 1:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiDown));
				ps = QString::fromWCharArray(Input::Name(items[0][u].a));
				break;
			case 2:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiUp));
				ps = QString::fromWCharArray(Input::Name(items[0][u].a));
				break;
			case 3:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiClick));
				ps = QString::fromWCharArray(Input::Name(items[0][u].a));
				break;
			case 4:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiMoveTo));
				ps = QString::number(items[0][u].b);
				ps += " - ";
				ps += QString::number(items[0][u].c);
				ps += "ㅤㅤ";
				ps += QString::number(items[0][u].a);
				break;
			case 5:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiMove));
				ps = QString::number(items[0][u].b);
				ps += "ㅤㅤ";
				ps += QString::number(items[0][u].c);
				ps += "ㅤㅤ";
				ps += QString::number(items[0][u].a);
				break;
			case 6:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiText));
				ps = QString::fromWCharArray(items[0][u].text.substr(0, 32).c_str());
				if (items[0][u].text.length() > 31) ps += "...";
				break;
			case 7:
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
				if (items[0][u].text == L"N") ps += ")　未找到";
				else ps += ")　找到";
				break;
			}
			case 8:
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiLoop));
				if (items[0][u].b == -1) ps = "无限";
				else ps = QString::number(items[0][u].b);
				break;
			}
			ui.tbItem->setItem(u, 1, new QTableWidgetItem(ps));
			ui.tbItem->item(u, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbItem->item(u, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
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
		case -2:
			item.type = -2;
			break;
		case -1:
			item.type = -1;
			break;
		case 0:
			item.type = 0;
			SaveEtDelay(&item);
			break;
		case 1:
			item.type = 1;
			SaveHkKey(&item);
			break;
		case 2:
			item.type = 2;
			SaveHkKey(&item);
			break;
		case 3:
			item.type = 3;
			SaveHkKey(&item);
			break;
		case 4:
			item.type = 4;
			SaveEtPos(&item);
			break;
		case 5:
			item.type = 5;
			SaveEtPos(&item);
			break;
		case 6:
			item.type = 6;
			item.text = (LPCWSTR)ui.etText->toPlainText().utf16();
			break;
		case 7:
			item.type = 7;
			SaveEtColorRect(&item);
			SaveEtColorValue(&item);
			break;
		case 8:
			item.type = 8;
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
		case -2:
			items[0][pos].type = -2;
			break;
		case -1:
			items[0][pos].type = -1;
			break;
		case 0:
			items[0][pos].type = 0;
			items[0][pos].b = ui.etTime->text().toInt();
			items[0][pos].c = ui.etDelayRand->text().toInt();
			SaveEtDelay(&items[0][pos]);
			break;
		case 1:
			items[0][pos].type = 1;
			SaveHkKey(&items[0][pos]);
			break;
		case 2:
			items[0][pos].type = 2;
			SaveHkKey(&items[0][pos]);
			break;
		case 3:
			items[0][pos].type = 3;
			SaveHkKey(&items[0][pos]);
			break;
		case 4:
			items[0][pos].type = 4;
			SaveEtPos(&items[0][pos]);
			break;
		case 5:
			items[0][pos].type = 5;
			SaveEtPos(&items[0][pos]);
			break;
		case 6:
			items[0][pos].type = 6;
			items[0][pos].text = (LPCWSTR)ui.etText->toPlainText().utf16();
			break;
		case 7:
			items[0][pos].type = 7;
			SaveEtColorRect(&items[0][pos]);
			SaveEtColorValue(&items[0][pos]);
			break;
		case 8:
			items[0][pos].type = 8;
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

private slots:

	bool eventFilter(QObject* obj, QEvent* et)
	{
		if (obj == ui.tbItem && et->type() == QEvent::KeyPress)
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
		return 0;
	}

	void OnTbItemsDbClick(int row, int column)
	{
		if (row < 0) return;

		switch (items[0][row].type)
		{
		case 0:
			LoadEtDelay(&items[0][row]);
			break;
		case 1:
			ui.rbDown->setChecked(1);
			ui.rbUp->setChecked(0);
			ui.rbClick->setChecked(0);
			LoadHkKey(&items[0][row]);
			break;
		case 2:
			ui.rbDown->setChecked(0);
			ui.rbUp->setChecked(1);
			ui.rbClick->setChecked(0);
			LoadHkKey(&items[0][row]);
			break;
		case 3:
			ui.rbDown->setChecked(0);
			ui.rbUp->setChecked(0);
			ui.rbClick->setChecked(1);
			LoadHkKey(&items[0][row]);
			break;
		case 4:
			ui.rbPos->setChecked(1);
			OnRbPos(1);
			LoadEtPos(&items[0][row]);
			break;
		case 5:
			ui.rbMove->setChecked(1);
			OnRbMove(1);
			LoadEtPos(&items[0][row]);
			break;
		case 6:
			ui.etText->setText(QString::fromWCharArray(items[0][row].text.c_str()));
			break;
		case 7:
			LoadEtColorRect(&items[0][row]);
			LoadEtColorValue(&items[0][row]);
			break;
		case 8:
			LoadEtLoopCount(&items[0][row]);
			break;
		}
		changing = 1;
		SetChanging();
	}

	void OnTbSelected(int row, int column)
	{
		ui.bnColorEdit->setDisabled(1);
		ui.bnLoopEdit->setDisabled(1);

		if (row < 0) return;
		if (items[0][row].type == 7) ui.bnColorEdit->setDisabled(0);
		else if (items[0][row].type == 8) ui.bnLoopEdit->setDisabled(0);
	}

	void OnBnKeyAdd()
	{
		if (ui.rbDown->isChecked())
		{
			if (changing) ChangeItem(1);
			else AddItem(1);
		}
		else if (ui.rbUp->isChecked())
		{
			if (changing) ChangeItem(2);
			else AddItem(2);
		}
		else if (ui.rbClick->isChecked())
		{
			if (changing) ChangeItem(3);
			else AddItem(3);
		}
	}

	void OnBnMoveAdd()
	{
		if (ui.rbMove->isChecked())
		{
			if (changing) ChangeItem(5);
			else AddItem(5);
		}
		else
		{
			if (changing) ChangeItem(4);
			else AddItem(4);
		}
	}

	void OnBnDelayAdd()
	{
		if (changing) ChangeItem(0);
		else AddItem(0);
	}

	void OnBnLoopAdd()
	{
		if (changing) ChangeItem(8);
		else AddItem(8);
	}

	void OnBnLoopEdit()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		if (items[0][pos].type != 8) return;
		EditUi edit(this, qis, &items[0][pos].next);
		edit.setWindowTitle("编辑 - 循环");
		edit.exec();
	}

	void OnBnTextAdd()
	{
		List<Item> item = *this->items;

		if (changing) ChangeItem(6);
		else AddItem(6);
	}

	void OnBnColorAdd()
	{
		if (changing) ChangeItem(7);
		else AddItem(7);
	}

	void OnColorEdit()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		if (items[0][pos].type != 7) return;
		EditUi edit(this, qis, &items[0][pos].next);
		edit.setWindowTitle("编辑 - 查找颜色");
		edit.exec();
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
	}

	void OnBnEndAdd()
	{
		if (changing) ChangeItem(-1);
		else AddItem(-1);
	}

	void OnBnEndLoopAdd()
	{
		if (changing) ChangeItem(-2);
		else AddItem(-2);
	}

	void OnRbPos(bool state)
	{
		if (state)
		{
			ui.etX->setValidator(new QIntValidator(0, 10000, this));
			ui.etY->setValidator(new QIntValidator(0, 10000, this));
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
			ui.etX->setValidator(new QIntValidator(-10000, 10000, this));
			ui.etY->setValidator(new QIntValidator(-10000, 10000, this));
			ui.bnPos->setText("×");
			ui.bnPos->setDisabled(1);
		}
	}

	void OnRbColor(bool state) {
		if (state)
		{

		}
	}
};