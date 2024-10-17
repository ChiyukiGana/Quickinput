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
	}
	void SetColor(const QColor& color, QPushButton* button)
	{
		button->setStyleSheet(QString("border:1px solid black;background-color:rgb(") + QString::number(color.red()) + "," + QString::number(color.green()) + "," + QString::number(color.blue()) + ")");
	}
	void SelectColor(PopTextInfo& p)
	{
		QColorDialog cd(p.c, this);
		cd.setStyleSheet(qis.ui.dialogStyle);
		cd.exec();
		p.c = cd.currentColor();
	}
	void Update()
	{
		ui.etQE->setText(qis.ui.pop.qe.t);
		ui.etQD->setText(qis.ui.pop.qd.t);
		ui.etWE->setText(qis.ui.pop.we.t);
		ui.etWD->setText(qis.ui.pop.wd.t);
		ui.etQcE->setText(qis.ui.pop.qce.t);
		ui.etQcD->setText(qis.ui.pop.qcd.t);
		ui.etSwE->setText(qis.ui.pop.swe.t);
		ui.etSwD->setText(qis.ui.pop.swd.t);
		ui.etDwE->setText(qis.ui.pop.dwe.t);
		ui.etDwD->setText(qis.ui.pop.dwd.t);
		ui.etUpE->setText(qis.ui.pop.upe.t);
		ui.etUpD->setText(qis.ui.pop.upd.t);

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
	bool eventFilter(QObject* obj, QEvent* e)
	{
		if (e->type() == QEvent::HoverEnter)
		{
			PopTextInfo* p = nullptr;
			bool qs = false;
			bool ws = false;
			bool qc = false;
			if (obj == ui.etQE) p = &qis.ui.pop.qe, qs = true;
			else if (obj == ui.etQD) p = &qis.ui.pop.qd, qs = true;
			else if (obj == ui.etWE) p = &qis.ui.pop.we, ws = true;
			else if (obj == ui.etWD) p = &qis.ui.pop.wd, ws = true;
			else if (obj == ui.etQcE) p = &qis.ui.pop.qce, qc = true;
			else if (obj == ui.etQcD) p = &qis.ui.pop.qcd, qc = true;
			else if (obj == ui.etSwE) p = &qis.ui.pop.swe;
			else if (obj == ui.etSwD) p = &qis.ui.pop.swd;
			else if (obj == ui.etDwE) p = &qis.ui.pop.dwe;
			else if (obj == ui.etDwD) p = &qis.ui.pop.dwd;
			else if (obj == ui.etUpE) p = &qis.ui.pop.upe;
			else if (obj == ui.etUpD) p = &qis.ui.pop.upd;
			if (!p) qis.popText->Show("提示框位置");
			else if (qs) qis.popText->Show(QiFn::ParseCustom(p->t, "(文件夹)", ""), p->c);
			else if (ws) qis.popText->Show(QiFn::ParseCustom(p->t, "(窗口名)", ""), p->c);
			else if (qc) qis.popText->Show(QiFn::ParseCustom(p->t, "(按键)", ""), p->c);
			else qis.popText->Show(QiFn::ParseCustom(p->t, "(宏名称)", "(1)"), p->c);
		}
		else if (e->type() == QEvent::HoverLeave)
		{
			qis.popText->Hide();
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
	void OnEtQE(const QString& text) { qis.ui.pop.qe.t = text; }
	void OnEtQD(const QString& text) { qis.ui.pop.qd.t = text; }
	void OnEtWE(const QString& text) { qis.ui.pop.we.t = text; }
	void OnEtWD(const QString& text) { qis.ui.pop.wd.t = text; }
	void OnEtQcE(const QString& text) { qis.ui.pop.qce.t = text; }
	void OnEtQcD(const QString& text) { qis.ui.pop.qcd.t = text; }
	void OnEtSwE(const QString& text) { qis.ui.pop.swe.t = text; }
	void OnEtSwD(const QString& text) { qis.ui.pop.swd.t = text; }
	void OnEtDwE(const QString& text) { qis.ui.pop.dwe.t = text; }
	void OnEtDwD(const QString& text) { qis.ui.pop.dwd.t = text; }
	void OnEtUpE(const QString& text) { qis.ui.pop.upe.t = text; }
	void OnEtUpD(const QString& text) { qis.ui.pop.upd.t = text; }
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
	void OnSdLR(int value) { qis.ui.pop.p.x = value; qis.popText->setPosition(qis.ui.pop.p.x, qis.ui.pop.p.y); qis.popText->Show("提示框位置"); }
	void OnSdTB(int value) { qis.ui.pop.p.y = value; qis.popText->setPosition(qis.ui.pop.p.x, qis.ui.pop.p.y); qis.popText->Show("提示框位置"); }
};