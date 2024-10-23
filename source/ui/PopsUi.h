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
	SettingsData* sets = &Qi::set;
public:
	PopsUi() : QDialog()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		Init();
		Event();
	}
	void StyleGroup()
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

private:
	void Init()
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
		ui.sdSize->installEventFilter(this);
		ui.sdTime->installEventFilter(this);

		StyleGroup();
		Update();
	}
	void Event()
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
		connect(ui.sdSize, SIGNAL(valueChanged(int)), this, SLOT(OnSdSize(int)));
		connect(ui.sdTime, SIGNAL(valueChanged(int)), this, SLOT(OnSdTime(int)));
	}
	void SetColor(const QColor& color, QPushButton* button)
	{
		button->setStyleSheet(QString("border:1px solid black;background-color:rgb(") + QString::number(color.red()) + "," + QString::number(color.green()) + "," + QString::number(color.blue()) + ")");
	}
	void SelectColor(PopTextInfo& p)
	{
		QColorDialog cd(p.c, this);
		cd.setStyleSheet(Qi::ui.dialogStyle);
		cd.exec();
		p.c = cd.currentColor();
	}
	void Update()
	{
		ui.etQE->setText(Qi::ui.pop.qe.t);
		ui.etQD->setText(Qi::ui.pop.qd.t);
		ui.etWE->setText(Qi::ui.pop.we.t);
		ui.etWD->setText(Qi::ui.pop.wd.t);
		ui.etQcE->setText(Qi::ui.pop.qce.t);
		ui.etQcD->setText(Qi::ui.pop.qcd.t);
		ui.etSwE->setText(Qi::ui.pop.swe.t);
		ui.etSwD->setText(Qi::ui.pop.swd.t);
		ui.etDwE->setText(Qi::ui.pop.dwe.t);
		ui.etDwD->setText(Qi::ui.pop.dwd.t);
		ui.etUpE->setText(Qi::ui.pop.upe.t);
		ui.etUpD->setText(Qi::ui.pop.upd.t);

		SetColor(Qi::ui.pop.qe.c, ui.bnQE);
		SetColor(Qi::ui.pop.qd.c, ui.bnQD);
		SetColor(Qi::ui.pop.we.c, ui.bnWE);
		SetColor(Qi::ui.pop.wd.c, ui.bnWD);
		SetColor(Qi::ui.pop.qce.c, ui.bnQcE);
		SetColor(Qi::ui.pop.qcd.c, ui.bnQcD);
		SetColor(Qi::ui.pop.swe.c, ui.bnSwE);
		SetColor(Qi::ui.pop.swd.c, ui.bnSwD);
		SetColor(Qi::ui.pop.dwe.c, ui.bnDwE);
		SetColor(Qi::ui.pop.dwd.c, ui.bnDwD);
		SetColor(Qi::ui.pop.upe.c, ui.bnUpE);
		SetColor(Qi::ui.pop.upd.c, ui.bnUpD);

		ui.sdLR->setValue(Qi::ui.pop.p.x);
		ui.sdTB->setValue(Qi::ui.pop.p.y);
		ui.sdSize->setValue(Qi::ui.pop.size);
		ui.sdTime->setValue(Qi::ui.pop.time);
	}
	bool eventFilter(QObject* obj, QEvent* e)
	{
		if (e->type() == QEvent::HoverEnter)
		{
			PopTextInfo* p = nullptr;
			bool qs = false;
			bool ws = false;
			bool qc = false;
			bool mc = false;
			bool pt = false;
			bool sz = false;
			bool tm = false;

			if (obj == ui.etQE) p = &Qi::ui.pop.qe, qs = true;
			else if (obj == ui.etQD) p = &Qi::ui.pop.qd, qs = true;
			else if (obj == ui.etWE) p = &Qi::ui.pop.we, ws = true;
			else if (obj == ui.etWD) p = &Qi::ui.pop.wd, ws = true;
			else if (obj == ui.etQcE) p = &Qi::ui.pop.qce, qc = true;
			else if (obj == ui.etQcD) p = &Qi::ui.pop.qcd, qc = true;
			else if (obj == ui.etSwE) p = &Qi::ui.pop.swe, mc = true;
			else if (obj == ui.etSwD) p = &Qi::ui.pop.swd, mc = true;
			else if (obj == ui.etDwE) p = &Qi::ui.pop.dwe, mc = true;
			else if (obj == ui.etDwD) p = &Qi::ui.pop.dwd, mc = true;
			else if (obj == ui.etUpE) p = &Qi::ui.pop.upe, mc = true;
			else if (obj == ui.etUpD) p = &Qi::ui.pop.upd, mc = true;
			else if (obj == ui.sdLR) pt = true;
			else if (obj == ui.sdTB) pt = true;
			else if (obj == ui.sdSize) sz = true;
			else if (obj == ui.sdTime) tm = true;

			if (qs) Qi::popText->Show(QiFn::ParseCustom(p->t, "(文件夹)", ""), p->c);
			else if (ws) Qi::popText->Show(QiFn::ParseCustom(p->t, "(窗口名)", ""), p->c);
			else if (qc) Qi::popText->Show(QiFn::ParseCustom(p->t, "(按键)", ""), p->c);
			else if (mc) Qi::popText->Show(QiFn::ParseCustom(p->t, "(宏名称)", "(1)"), p->c);
			else if (pt) Qi::popText->Show("提示框位置");
			else if (sz) Qi::popText->Show("提示框大小");
			else if (tm) Qi::popText->Popup("提示框时间");
		}
		else if (e->type() == QEvent::HoverLeave)
		{
			Qi::popText->Hide();
		}
		return QDialog::eventFilter(obj, e);
	}
	void showEvent(QShowEvent*)
	{
		SetForegroundWindow((HWND)QWidget::winId());
	}
	QPoint msPos; bool mouseDown = false; void mousePressEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) msPos = e->pos(), mouseDown = true; e->accept(); }void mouseMoveEvent(QMouseEvent* e) { if (mouseDown) move(e->pos() + pos() - msPos); }void mouseReleaseEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) mouseDown = false; }
private Q_SLOTS:
	void OnBnClose()
	{
		close();
	}
	void OnEtQE(const QString& text) { Qi::ui.pop.qe.t = text; }
	void OnEtQD(const QString& text) { Qi::ui.pop.qd.t = text; }
	void OnEtWE(const QString& text) { Qi::ui.pop.we.t = text; }
	void OnEtWD(const QString& text) { Qi::ui.pop.wd.t = text; }
	void OnEtQcE(const QString& text) { Qi::ui.pop.qce.t = text; }
	void OnEtQcD(const QString& text) { Qi::ui.pop.qcd.t = text; }
	void OnEtSwE(const QString& text) { Qi::ui.pop.swe.t = text; }
	void OnEtSwD(const QString& text) { Qi::ui.pop.swd.t = text; }
	void OnEtDwE(const QString& text) { Qi::ui.pop.dwe.t = text; }
	void OnEtDwD(const QString& text) { Qi::ui.pop.dwd.t = text; }
	void OnEtUpE(const QString& text) { Qi::ui.pop.upe.t = text; }
	void OnEtUpD(const QString& text) { Qi::ui.pop.upd.t = text; }
	void OnBnQE() { SelectColor(Qi::ui.pop.qe); Update(); }
	void OnBnQD() { SelectColor(Qi::ui.pop.qd); Update(); }
	void OnBnWE() { SelectColor(Qi::ui.pop.we); Update(); }
	void OnBnWD() { SelectColor(Qi::ui.pop.wd); Update(); }
	void OnBnQcE() { SelectColor(Qi::ui.pop.qce); Update(); }
	void OnBnQcD() { SelectColor(Qi::ui.pop.qcd); Update(); }
	void OnBnSwE() { SelectColor(Qi::ui.pop.swe); Update(); }
	void OnBnSwD() { SelectColor(Qi::ui.pop.swd); Update(); }
	void OnBnDwE() { SelectColor(Qi::ui.pop.dwe); Update(); }
	void OnBnDwD() { SelectColor(Qi::ui.pop.dwd); Update(); }
	void OnBnUpE() { SelectColor(Qi::ui.pop.upe); Update(); }
	void OnBnUpD() { SelectColor(Qi::ui.pop.upd); Update(); }
	void OnSdLR(int value) { Qi::ui.pop.p.x = value; Qi::popText->setPosition(Qi::ui.pop.p.x, Qi::ui.pop.p.y); Qi::popText->Show("提示框位置"); }
	void OnSdTB(int value) { Qi::ui.pop.p.y = value; Qi::popText->setPosition(Qi::ui.pop.p.x, Qi::ui.pop.p.y); Qi::popText->Show("提示框位置"); }
	void OnSdSize(int value) { Qi::ui.pop.size = value; Qi::popText->setSize(Qi::ui.pop.size); Qi::popText->Show("提示框大小"); }
	void OnSdTime(int value) { Qi::ui.pop.time = value; Qi::popText->setTime(Qi::ui.pop.time); Qi::popText->Popup("提示框时间"); }
};