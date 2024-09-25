#pragma execution_character_set("utf-8")
#pragma once
#include <qcolordialog.h>
#include <qevent.h>
#include "../src/minc.h"
#include "ui_PopsUi.h"

class PopsUi : public QDialog
{
	Q_OBJECT;
	Ui::PopsUiClass ui;
	SettingsData* sets = &qis.set;
public:
	PopsUi() : QDialog()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
	}
	void SetStyleGroup()
	{
		setProperty("group", QVariant(QString::fromUtf8("frame")));
		ui.titleWidget->setProperty("group", QVariant(QString::fromUtf8("title")));
		ui.clientWidget->setProperty("group", QVariant(QString::fromUtf8("client")));
		ui.bnClose->setProperty("group", QVariant(QString::fromUtf8("title-close_button")));
		ui.etQE->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.etQD->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.etWE->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.etWD->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.etQcE->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.etQcD->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.etSwE->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.etSwD->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.etDwE->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.etDwD->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.etUpE->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
		ui.etUpD->setProperty("group", QVariant(QString::fromUtf8("line_edit")));
	}
	void ReStyle()
	{
		setStyleSheet("");
		setStyleSheet(qis.ui.themes[qis.set.theme].style);
	}
private:
	void WidInit()
	{
		ui.etQE->setAttribute(Qt::WA_Hover, true);
		ui.etQD->setAttribute(Qt::WA_Hover, true);
		ui.etWE->setAttribute(Qt::WA_Hover, true);
		ui.etWD->setAttribute(Qt::WA_Hover, true);
		ui.etQcE->setAttribute(Qt::WA_Hover, true);
		ui.etQcD->setAttribute(Qt::WA_Hover, true);
		ui.etSwE->setAttribute(Qt::WA_Hover, true);
		ui.etSwD->setAttribute(Qt::WA_Hover, true);
		ui.etDwE->setAttribute(Qt::WA_Hover, true);
		ui.etDwD->setAttribute(Qt::WA_Hover, true);
		ui.etUpE->setAttribute(Qt::WA_Hover, true);
		ui.etUpD->setAttribute(Qt::WA_Hover, true);

		ui.sdLR->setAttribute(Qt::WA_Hover, true);
		ui.sdTB->setAttribute(Qt::WA_Hover, true);

		ui.etQE->installEventFilter(this);
		ui.etQD->installEventFilter(this);
		ui.etWE->installEventFilter(this);
		ui.etWD->installEventFilter(this);
		ui.etQcE->installEventFilter(this);
		ui.etQcD->installEventFilter(this);
		ui.etSwE->installEventFilter(this);
		ui.etSwD->installEventFilter(this);
		ui.etDwE->installEventFilter(this);
		ui.etDwD->installEventFilter(this);
		ui.etUpE->installEventFilter(this);
		ui.etUpD->installEventFilter(this);

		ui.sdLR->installEventFilter(this);
		ui.sdTB->installEventFilter(this);

		SetStyleGroup();
		Update();
	}
	void WidEvent()
	{
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));

		connect(ui.etQE, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtQE(const QString&)));
		connect(ui.etQD, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtQD(const QString&)));
		connect(ui.etWE, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtWE(const QString&)));
		connect(ui.etWD, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtWD(const QString&)));
		connect(ui.etQcE, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtQcE(const QString&)));
		connect(ui.etQcD, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtQcD(const QString&)));
		connect(ui.etSwE, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtSwE(const QString&)));
		connect(ui.etSwD, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtSwD(const QString&)));
		connect(ui.etDwE, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtDwE(const QString&)));
		connect(ui.etDwD, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtDwD(const QString&)));
		connect(ui.etUpE, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtUpE(const QString&)));
		connect(ui.etUpD, SIGNAL(textChanged(const QString&)), this, SLOT(OnEtUpD(const QString&)));

		connect(ui.bnQE, SIGNAL(clicked()), this, SLOT(OnBnQE()));
		connect(ui.bnQD, SIGNAL(clicked()), this, SLOT(OnBnQD()));
		connect(ui.bnWE, SIGNAL(clicked()), this, SLOT(OnBnWE()));
		connect(ui.bnWD, SIGNAL(clicked()), this, SLOT(OnBnWD()));
		connect(ui.bnQcE, SIGNAL(clicked()), this, SLOT(OnBnQcE()));
		connect(ui.bnQcD, SIGNAL(clicked()), this, SLOT(OnBnQcD()));
		connect(ui.bnSwE, SIGNAL(clicked()), this, SLOT(OnBnSwE()));
		connect(ui.bnSwD, SIGNAL(clicked()), this, SLOT(OnBnSwD()));
		connect(ui.bnDwE, SIGNAL(clicked()), this, SLOT(OnBnDwE()));
		connect(ui.bnDwD, SIGNAL(clicked()), this, SLOT(OnBnDwD()));
		connect(ui.bnUpE, SIGNAL(clicked()), this, SLOT(OnBnUpE()));
		connect(ui.bnUpD, SIGNAL(clicked()), this, SLOT(OnBnUpD()));

		connect(ui.sdLR, SIGNAL(valueChanged(int)), this, SLOT(OnSdLR(int)));
		connect(ui.sdTB, SIGNAL(valueChanged(int)), this, SLOT(OnSdTB(int)));
	}
	void SetColor(COLORREF color, QPushButton* button)
	{
		button->setStyleSheet(QString("border:1px solid black;background-color:rgb(") + QString::number(GetRValue(color)) + "," + QString::number(GetGValue(color)) + "," + QString::number(GetBValue(color)) + ")");
	}
	void SelectColor(QiUi::PopBoxBase& pb)
	{
		QColorDialog cd(QColor(GetRValue(pb.c), GetGValue(pb.c), GetBValue(pb.c)));
		cd.exec();
		QColor color = cd.currentColor();
		pb.c = RGB(color.red(), color.green(), color.blue());
	}
	void Update()
	{
		ui.etQE->setText(QString::fromWCharArray(qis.ui.pop.qe.t.c_str()));
		ui.etQD->setText(QString::fromWCharArray(qis.ui.pop.qd.t.c_str()));
		ui.etWE->setText(QString::fromWCharArray(qis.ui.pop.we.t.c_str()));
		ui.etWD->setText(QString::fromWCharArray(qis.ui.pop.wd.t.c_str()));
		ui.etQcE->setText(QString::fromWCharArray(qis.ui.pop.qce.t.c_str()));
		ui.etQcD->setText(QString::fromWCharArray(qis.ui.pop.qcd.t.c_str()));
		ui.etSwE->setText(QString::fromWCharArray(qis.ui.pop.swe.t.c_str()));
		ui.etSwD->setText(QString::fromWCharArray(qis.ui.pop.swd.t.c_str()));
		ui.etDwE->setText(QString::fromWCharArray(qis.ui.pop.dwe.t.c_str()));
		ui.etDwD->setText(QString::fromWCharArray(qis.ui.pop.dwd.t.c_str()));
		ui.etUpE->setText(QString::fromWCharArray(qis.ui.pop.upe.t.c_str()));
		ui.etUpD->setText(QString::fromWCharArray(qis.ui.pop.upd.t.c_str()));

		SetColor(qis.ui.pop.qe.c, ui.bnQE);
		SetColor(qis.ui.pop.qd.c, ui.bnQD);
		SetColor(qis.ui.pop.we.c, ui.bnWE);
		SetColor(qis.ui.pop.wd.c, ui.bnWD);
		SetColor(qis.ui.pop.qce.c, ui.bnQcE);
		SetColor(qis.ui.pop.qcd.c, ui.bnQcD);
		SetColor(qis.ui.pop.swe.c, ui.bnSwE);
		SetColor(qis.ui.pop.swd.c, ui.bnSwD);
		SetColor(qis.ui.pop.dwe.c, ui.bnDwE);
		SetColor(qis.ui.pop.dwd.c, ui.bnDwD);
		SetColor(qis.ui.pop.upe.c, ui.bnUpE);
		SetColor(qis.ui.pop.upd.c, ui.bnUpD);

		ui.sdLR->setValue(qis.ui.pop.p.x);
		ui.sdTB->setValue(qis.ui.pop.p.y);
	}
	bool eventFilter(QObject* obj, QEvent* et)
	{
		if (et->type() == QEvent::HoverEnter)
		{
			QiUi::PopBoxBase* pb = nullptr;
			bool qs = false;
			bool ws = false;
			if (obj == ui.etQE) pb = &qis.ui.pop.qe, qs = true;
			else if (obj == ui.etQD) pb = &qis.ui.pop.qd, qs = true;
			else if (obj == ui.etWE) pb = &qis.ui.pop.we, ws = true;
			else if (obj == ui.etWD) pb = &qis.ui.pop.wd, ws = true;
			else if (obj == ui.etQcE) pb = &qis.ui.pop.qce;
			else if (obj == ui.etQcD) pb = &qis.ui.pop.qcd;
			else if (obj == ui.etSwE) pb = &qis.ui.pop.swe;
			else if (obj == ui.etSwD) pb = &qis.ui.pop.swd;
			else if (obj == ui.etDwE) pb = &qis.ui.pop.dwe;
			else if (obj == ui.etDwD) pb = &qis.ui.pop.dwd;
			else if (obj == ui.etUpE) pb = &qis.ui.pop.upe;
			else if (obj == ui.etUpD) pb = &qis.ui.pop.upd;
			if (!pb) PopBox::Show(L"示例窗口");
			else if (qs) PopBox::Show(QiFn::ParseQPop(pb->t), pb->c);
			else if (ws) PopBox::Show(QiFn::ParseWPop(pb->t), pb->c);
			else PopBox::Show(QiFn::ParseMacroPop(pb->t), pb->c);
		}
		else if (et->type() == QEvent::HoverLeave)
		{
			PopBox::Hide();
		}
		return QDialog::eventFilter(obj, et);
	}
	void showEvent(QShowEvent*)
	{
		ReStyle();
		SetForegroundWindow((HWND)QWidget::winId());
	}
	QPoint msPos; bool mouseDown = false; void mousePressEvent(QMouseEvent* et) { if (et->button() == Qt::LeftButton) msPos = et->pos(), mouseDown = true; et->accept(); }void mouseMoveEvent(QMouseEvent* et) { if (mouseDown) move(et->pos() + pos() - msPos); }void mouseReleaseEvent(QMouseEvent* et) { if (et->button() == Qt::LeftButton) mouseDown = false; }
private slots:
	void OnBnClose()
	{
		close();
	}
	void OnEtQE(const QString& text) { qis.ui.pop.qe.t = (wchar_t*)(text.utf16()); }
	void OnEtQD(const QString& text) { qis.ui.pop.qd.t = (wchar_t*)(text.utf16()); }
	void OnEtWE(const QString& text) { qis.ui.pop.we.t = (wchar_t*)(text.utf16()); }
	void OnEtWD(const QString& text) { qis.ui.pop.wd.t = (wchar_t*)(text.utf16()); }
	void OnEtQcE(const QString& text) { qis.ui.pop.qce.t = (wchar_t*)(text.utf16()); }
	void OnEtQcD(const QString& text) { qis.ui.pop.qcd.t = (wchar_t*)(text.utf16()); }
	void OnEtSwE(const QString& text) { qis.ui.pop.swe.t = (wchar_t*)(text.utf16()); }
	void OnEtSwD(const QString& text) { qis.ui.pop.swd.t = (wchar_t*)(text.utf16()); }
	void OnEtDwE(const QString& text) { qis.ui.pop.dwe.t = (wchar_t*)(text.utf16()); }
	void OnEtDwD(const QString& text) { qis.ui.pop.dwd.t = (wchar_t*)(text.utf16()); }
	void OnEtUpE(const QString& text) { qis.ui.pop.upe.t = (wchar_t*)(text.utf16()); }
	void OnEtUpD(const QString& text) { qis.ui.pop.upd.t = (wchar_t*)(text.utf16()); }
	void OnBnQE() { SelectColor(qis.ui.pop.qe); Update(); }
	void OnBnQD() { SelectColor(qis.ui.pop.qd); Update(); }
	void OnBnWE() { SelectColor(qis.ui.pop.we); Update(); }
	void OnBnWD() { SelectColor(qis.ui.pop.wd); Update(); }
	void OnBnQcE() { SelectColor(qis.ui.pop.qce); Update(); }
	void OnBnQcD() { SelectColor(qis.ui.pop.qcd); Update(); }
	void OnBnSwE() { SelectColor(qis.ui.pop.swe); Update(); }
	void OnBnSwD() { SelectColor(qis.ui.pop.swd); Update(); }
	void OnBnDwE() { SelectColor(qis.ui.pop.dwe); Update(); }
	void OnBnDwD() { SelectColor(qis.ui.pop.dwd); Update(); }
	void OnBnUpE() { SelectColor(qis.ui.pop.upe); Update(); }
	void OnBnUpD() { SelectColor(qis.ui.pop.upd); Update(); }
	void OnSdLR(int value) { qis.ui.pop.p.x = value; PopBox::Show(L"示例窗口"); }
	void OnSdTB(int value) { qis.ui.pop.p.y = value; PopBox::Show(L"示例窗口"); }
};