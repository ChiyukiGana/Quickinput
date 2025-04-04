#pragma once
#include <src/inc_header.h>
#include "ui_PopsUi.h"
class PopsUi : public QDialog
{
	Q_OBJECT;
	using This = PopsUi;
	Ui::PopsUiClass ui;
	SettingsData* sets = &Qi::set;
public:
	PopsUi() : QDialog()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		Init();
		Event();
		StyleGroup();
	}
private:
	void StyleGroup()
	{
		setProperty("group", "frame");
		ui.title_widget->setProperty("group", "title");
		ui.content_widget->setProperty("group", "client");
		ui.title_close_button->setProperty("group", "title-close_button");
		ui.enable_edit->setProperty("group", "line_edit");
		ui.disable_edit->setProperty("group", "line_edit");
		ui.window_on_edit->setProperty("group", "line_edit");
		ui.window_off_edit->setProperty("group", "line_edit");
		ui.click_on_edit->setProperty("group", "line_edit");
		ui.click_off_edit->setProperty("group", "line_edit");
		ui.switch_on_edit->setProperty("group", "line_edit");
		ui.switch_off_edit->setProperty("group", "line_edit");
		ui.down_on_edit->setProperty("group", "line_edit");
		ui.down_off_edit->setProperty("group", "line_edit");
		ui.up_on_edit->setProperty("group", "line_edit");
		ui.up_off_edit->setProperty("group", "line_edit");
	}
	void Init()
	{
		ui.enable_edit->setAttribute(Qt::WA_Hover, true);
		ui.disable_edit->setAttribute(Qt::WA_Hover, true);
		ui.window_on_edit->setAttribute(Qt::WA_Hover, true);
		ui.window_off_edit->setAttribute(Qt::WA_Hover, true);
		ui.click_on_edit->setAttribute(Qt::WA_Hover, true);
		ui.click_off_edit->setAttribute(Qt::WA_Hover, true);
		ui.switch_on_edit->setAttribute(Qt::WA_Hover, true);
		ui.switch_off_edit->setAttribute(Qt::WA_Hover, true);
		ui.down_on_edit->setAttribute(Qt::WA_Hover, true);
		ui.down_off_edit->setAttribute(Qt::WA_Hover, true);
		ui.up_on_edit->setAttribute(Qt::WA_Hover, true);
		ui.up_off_edit->setAttribute(Qt::WA_Hover, true);
		ui.enable_edit->installEventFilter(this);
		ui.disable_edit->installEventFilter(this);
		ui.window_on_edit->installEventFilter(this);
		ui.window_off_edit->installEventFilter(this);
		ui.click_on_edit->installEventFilter(this);
		ui.click_off_edit->installEventFilter(this);
		ui.switch_on_edit->installEventFilter(this);
		ui.switch_off_edit->installEventFilter(this);
		ui.down_on_edit->installEventFilter(this);
		ui.down_off_edit->installEventFilter(this);
		ui.up_on_edit->installEventFilter(this);
		ui.up_off_edit->installEventFilter(this);
		ui.disply_x_slider->setAttribute(Qt::WA_Hover, true);
		ui.disply_y_slider->setAttribute(Qt::WA_Hover, true);
		ui.disply_x_slider->installEventFilter(this);
		ui.disply_y_slider->installEventFilter(this);
		ui.disply_size_slider->installEventFilter(this);
		ui.disply_time_slider->installEventFilter(this);
		Update();
	}
	void Event()
	{
		connect(ui.title_close_button, &QPushButton::clicked, this, &This::close);
		// line edit
		connect(ui.enable_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.qe.t = text; });
		connect(ui.disable_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.qd.t = text; });
		connect(ui.window_on_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.we.t = text; });
		connect(ui.window_off_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.wd.t = text; });
		connect(ui.click_on_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.qce.t = text; });
		connect(ui.click_off_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.qcd.t = text; });
		connect(ui.switch_on_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.swe.t = text; });
		connect(ui.switch_off_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.swd.t = text; });
		connect(ui.down_on_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.dwe.t = text; });
		connect(ui.down_off_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.dwd.t = text; });
		connect(ui.up_on_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.upe.t = text; });
		connect(ui.up_off_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.upd.t = text; });
		// button
		connect(ui.enable_button, &QPushButton::clicked, this, [this](bool){ SelectColor(Qi::ui.pop.qe); Update(); });
		connect(ui.disable_button, &QPushButton::clicked, this, [this](bool) { SelectColor(Qi::ui.pop.qd); Update(); });
		connect(ui.window_on_button, &QPushButton::clicked, this, [this](bool){ SelectColor(Qi::ui.pop.we); Update(); });
		connect(ui.window_off_button, &QPushButton::clicked, this, [this](bool){ SelectColor(Qi::ui.pop.wd); Update(); });
		connect(ui.click_on_button, &QPushButton::clicked, this, [this](bool){ SelectColor(Qi::ui.pop.qce); Update(); });
		connect(ui.click_off_button, &QPushButton::clicked, this, [this](bool){ SelectColor(Qi::ui.pop.qcd); Update(); });
		connect(ui.switch_on_button, &QPushButton::clicked, this, [this](bool){ SelectColor(Qi::ui.pop.swe); Update(); });
		connect(ui.switch_off_button, &QPushButton::clicked, this, [this](bool){ SelectColor(Qi::ui.pop.swd); Update(); });
		connect(ui.down_on_button, &QPushButton::clicked, this, [this](bool){ SelectColor(Qi::ui.pop.dwe); Update(); });
		connect(ui.down_off_button, &QPushButton::clicked, this, [this](bool){ SelectColor(Qi::ui.pop.dwd); Update(); });
		connect(ui.up_on_button, &QPushButton::clicked, this, [this](bool){ SelectColor(Qi::ui.pop.upe); Update(); });
		connect(ui.up_off_button, &QPushButton::clicked, this, [this](bool){ SelectColor(Qi::ui.pop.upd); Update(); });
		// slider
		connect(ui.disply_x_slider, &QSlider::valueChanged, this, [this](int value){ Qi::ui.pop.p.x = value; Qi::popText->setPosition(Qi::ui.pop.p.x, Qi::ui.pop.p.y); Qi::popText->Show("提示框位置"); });
		connect(ui.disply_y_slider, &QSlider::valueChanged, this, [this](int value) { Qi::ui.pop.p.y = value; Qi::popText->setPosition(Qi::ui.pop.p.x, Qi::ui.pop.p.y); Qi::popText->Show("提示框位置"); });
		connect(ui.disply_size_slider, &QSlider::valueChanged, this, [this](int value) { Qi::ui.pop.size = value; Qi::popText->setSize(Qi::ui.pop.size); Qi::popText->Show("提示框大小"); });
		connect(ui.disply_time_slider, &QSlider::valueChanged, this, [this](int value) { Qi::ui.pop.time = value; Qi::popText->Popup(Qi::ui.pop.time, "提示框时间"); });
	}
	void SetColor(const QColor& color, QPushButton* button)
	{
		button->setStyleSheet(QString("border:1px solid black;background-color:rgb(") + QString::number(color.red()) + "," + QString::number(color.green()) + "," + QString::number(color.blue()) + ")");
	}
	void SelectColor(QiUi::PopTextInfo& p)
	{
		QColorDialog cd(p.c, this);
		cd.setStyleSheet(Qi::ui.dialogStyle);
		cd.exec();
		p.c = cd.currentColor();
	}
	void Update()
	{
		ui.enable_edit->setText(Qi::ui.pop.qe.t);
		ui.disable_edit->setText(Qi::ui.pop.qd.t);
		ui.window_on_edit->setText(Qi::ui.pop.we.t);
		ui.window_off_edit->setText(Qi::ui.pop.wd.t);
		ui.click_on_edit->setText(Qi::ui.pop.qce.t);
		ui.click_off_edit->setText(Qi::ui.pop.qcd.t);
		ui.switch_on_edit->setText(Qi::ui.pop.swe.t);
		ui.switch_off_edit->setText(Qi::ui.pop.swd.t);
		ui.down_on_edit->setText(Qi::ui.pop.dwe.t);
		ui.down_off_edit->setText(Qi::ui.pop.dwd.t);
		ui.up_on_edit->setText(Qi::ui.pop.upe.t);
		ui.up_off_edit->setText(Qi::ui.pop.upd.t);
		SetColor(Qi::ui.pop.qe.c, ui.enable_button);
		SetColor(Qi::ui.pop.qd.c, ui.disable_button);
		SetColor(Qi::ui.pop.we.c, ui.window_on_button);
		SetColor(Qi::ui.pop.wd.c, ui.window_off_button);
		SetColor(Qi::ui.pop.qce.c, ui.click_on_button);
		SetColor(Qi::ui.pop.qcd.c, ui.click_off_button);
		SetColor(Qi::ui.pop.swe.c, ui.switch_on_button);
		SetColor(Qi::ui.pop.swd.c, ui.switch_off_button);
		SetColor(Qi::ui.pop.dwe.c, ui.down_on_button);
		SetColor(Qi::ui.pop.dwd.c, ui.down_off_button);
		SetColor(Qi::ui.pop.upe.c, ui.up_on_button);
		SetColor(Qi::ui.pop.upd.c, ui.up_off_button);
		ui.disply_x_slider->setValue(Qi::ui.pop.p.x);
		ui.disply_y_slider->setValue(Qi::ui.pop.p.y);
		ui.disply_size_slider->setValue(Qi::ui.pop.size);
		ui.disply_time_slider->setValue(Qi::ui.pop.time);
	}
	bool event(QEvent* e)
	{
		if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
		{
			QKeyEvent* keyEvent = (QKeyEvent*)e;
			if ((keyEvent->key() == Qt::Key_Escape) || (keyEvent->key() == Qt::Key_Return) || keyEvent->key() == Qt::Key_Enter || (keyEvent->key() == Qt::Key_Space)) return true;
		}
		return QDialog::event(e);
	}
	bool eventFilter(QObject* obj, QEvent* e)
	{
		if (e->type() == QEvent::HoverEnter)
		{
			QiUi::PopTextInfo* p = nullptr;
			bool qs = false;
			bool ws = false;
			bool qc = false;
			bool mc = false;
			bool pt = false;
			bool sz = false;
			bool tm = false;
			if (obj == ui.enable_edit) p = &Qi::ui.pop.qe, qs = true;
			else if (obj == ui.disable_edit) p = &Qi::ui.pop.qd, qs = true;
			else if (obj == ui.window_on_edit) p = &Qi::ui.pop.we, ws = true;
			else if (obj == ui.window_off_edit) p = &Qi::ui.pop.wd, ws = true;
			else if (obj == ui.click_on_edit) p = &Qi::ui.pop.qce, qc = true;
			else if (obj == ui.click_off_edit) p = &Qi::ui.pop.qcd, qc = true;
			else if (obj == ui.switch_on_edit) p = &Qi::ui.pop.swe, mc = true;
			else if (obj == ui.switch_off_edit) p = &Qi::ui.pop.swd, mc = true;
			else if (obj == ui.down_on_edit) p = &Qi::ui.pop.dwe, mc = true;
			else if (obj == ui.down_off_edit) p = &Qi::ui.pop.dwd, mc = true;
			else if (obj == ui.up_on_edit) p = &Qi::ui.pop.upe, mc = true;
			else if (obj == ui.up_off_edit) p = &Qi::ui.pop.upd, mc = true;
			else if (obj == ui.disply_x_slider) pt = true;
			else if (obj == ui.disply_y_slider) pt = true;
			else if (obj == ui.disply_size_slider) sz = true;
			else if (obj == ui.disply_time_slider) tm = true;
			if (qs) Qi::popText->Show(QiFn::ParseCustom(p->t, "(文件夹)", ""), p->c);
			else if (ws) Qi::popText->Show(QiFn::ParseCustom(p->t, "(窗口名)", ""), p->c);
			else if (qc) Qi::popText->Show(QiFn::ParseCustom(p->t, "(按键)", ""), p->c);
			else if (mc) Qi::popText->Show(QiFn::ParseCustom(p->t, "(宏名称)", "(1)"), p->c);
			else if (pt) Qi::popText->Show("提示框位置");
			else if (sz) Qi::popText->Show("提示框大小");
			else if (tm) Qi::popText->Popup(Qi::ui.pop.time, "提示框时间");
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
};