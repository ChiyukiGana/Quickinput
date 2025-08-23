#include "PopsUi.h"
PopsUi::PopsUi()
{
	ui.setupUi(this);
	Init();
	Event();
	StyleGroup();
}

void PopsUi::Init()
{
	bind_text_edit.append(ui.enable_edit);
	bind_text_edit.append(ui.disable_edit);
	bind_text_edit.append(ui.enable_edit);
	bind_text_edit.append(ui.disable_edit);
	bind_text_edit.append(ui.window_on_edit);
	bind_text_edit.append(ui.window_off_edit);
	bind_text_edit.append(ui.click_on_edit);
	bind_text_edit.append(ui.click_off_edit);
	bind_text_edit.append(ui.switch_on_edit);
	bind_text_edit.append(ui.switch_off_edit);
	bind_text_edit.append(ui.down_on_edit);
	bind_text_edit.append(ui.down_off_edit);
	bind_text_edit.append(ui.up_on_edit);
	bind_text_edit.append(ui.up_off_edit);

	bind_sound_edit.append(ui.enable_sound_edit);
	bind_sound_edit.append(ui.disable_sound_edit);
	bind_sound_edit.append(ui.window_on_sound_edit);
	bind_sound_edit.append(ui.window_off_sound_edit);
	bind_sound_edit.append(ui.click_on_sound_edit);
	bind_sound_edit.append(ui.click_off_sound_edit);
	bind_sound_edit.append(ui.switch_on_sound_edit);
	bind_sound_edit.append(ui.switch_off_sound_edit);
	bind_sound_edit.append(ui.down_on_sound_edit);
	bind_sound_edit.append(ui.down_off_sound_edit);
	bind_sound_edit.append(ui.up_on_sound_edit);
	bind_sound_edit.append(ui.up_off_sound_edit);

	for (auto& i : bind_text_edit) {
		i->setAttribute(Qt::WA_Hover, true);
		i->installEventFilter(this);
	}
	bind_text_edit.resize(0);

	for (auto& i : bind_sound_edit) {
		i->setAttribute(Qt::WA_Hover, true);
		i->installEventFilter(this);
		i->setDialog("选择音频", "音频文件(*.mp3 *.wav)", "C:/Windows/Media");
		i->setFiles(Qi::ui.sounds);
	}
	bind_sound_edit.resize(0);

	ui.disply_x_slider->setAttribute(Qt::WA_Hover, true);
	ui.disply_y_slider->setAttribute(Qt::WA_Hover, true);
	ui.disply_size_slider->setAttribute(Qt::WA_Hover, true);
	ui.disply_time_slider->setAttribute(Qt::WA_Hover, true);

	ui.disply_x_slider->installEventFilter(this);
	ui.disply_y_slider->installEventFilter(this);
	ui.disply_size_slider->installEventFilter(this);
	ui.disply_time_slider->installEventFilter(this);

	Update();
}
void PopsUi::Event()
{
	connect(ui.title_close_button, &QPushButton::clicked, this, &This::close);
	// text line edit
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

	// sound line edit
	connect(ui.enable_sound_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.qe.s = text; });
	connect(ui.disable_sound_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.qd.s = text; });
	connect(ui.window_on_sound_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.we.s = text; });
	connect(ui.window_off_sound_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.wd.s = text; });
	connect(ui.click_on_sound_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.qce.s = text; });
	connect(ui.click_off_sound_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.qcd.s = text; });
	connect(ui.switch_on_sound_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.swe.s = text; });
	connect(ui.switch_off_sound_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.swd.s = text; });
	connect(ui.down_on_sound_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.dwe.s = text; });
	connect(ui.down_off_sound_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.dwd.s = text; });
	connect(ui.up_on_sound_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.upe.s = text; });
	connect(ui.up_off_sound_edit, &QLineEdit::textChanged, this, [](const QString& text) { Qi::ui.pop.upd.s = text; });

	// button
	connect(ui.enable_button, &QPushButton::clicked, this, [this](bool) { SelectColor(Qi::ui.pop.qe); Update(); });
	connect(ui.disable_button, &QPushButton::clicked, this, [this](bool) { SelectColor(Qi::ui.pop.qd); Update(); });
	connect(ui.window_on_button, &QPushButton::clicked, this, [this](bool) { SelectColor(Qi::ui.pop.we); Update(); });
	connect(ui.window_off_button, &QPushButton::clicked, this, [this](bool) { SelectColor(Qi::ui.pop.wd); Update(); });
	connect(ui.click_on_button, &QPushButton::clicked, this, [this](bool) { SelectColor(Qi::ui.pop.qce); Update(); });
	connect(ui.click_off_button, &QPushButton::clicked, this, [this](bool) { SelectColor(Qi::ui.pop.qcd); Update(); });
	connect(ui.switch_on_button, &QPushButton::clicked, this, [this](bool) { SelectColor(Qi::ui.pop.swe); Update(); });
	connect(ui.switch_off_button, &QPushButton::clicked, this, [this](bool) { SelectColor(Qi::ui.pop.swd); Update(); });
	connect(ui.down_on_button, &QPushButton::clicked, this, [this](bool) { SelectColor(Qi::ui.pop.dwe); Update(); });
	connect(ui.down_off_button, &QPushButton::clicked, this, [this](bool) { SelectColor(Qi::ui.pop.dwd); Update(); });
	connect(ui.up_on_button, &QPushButton::clicked, this, [this](bool) { SelectColor(Qi::ui.pop.upe); Update(); });
	connect(ui.up_off_button, &QPushButton::clicked, this, [this](bool) { SelectColor(Qi::ui.pop.upd); Update(); });
	// slider
	connect(ui.disply_x_slider, &QSlider::valueChanged, this, [this](int value) { Qi::ui.pop.p.x = value; Qi::popText->setPosition(Qi::ui.pop.p.x, Qi::ui.pop.p.y); Qi::popText->Show("提示框位置"); });
	connect(ui.disply_y_slider, &QSlider::valueChanged, this, [this](int value) { Qi::ui.pop.p.y = value; Qi::popText->setPosition(Qi::ui.pop.p.x, Qi::ui.pop.p.y); Qi::popText->Show("提示框位置"); });
	connect(ui.disply_size_slider, &QSlider::valueChanged, this, [this](int value) { Qi::ui.pop.size = value; Qi::popText->setSize(Qi::ui.pop.size); Qi::popText->Show("提示框大小"); });
	connect(ui.disply_time_slider, &QSlider::valueChanged, this, [this](int value) { Qi::ui.pop.time = value; Qi::popText->Popup(Qi::ui.pop.time, "提示框时间"); });
}
void PopsUi::StyleGroup()
{
	setProperty("group", "frame");
	ui.title_widget->setProperty("group", "title");
	ui.content_widget->setProperty("group", "client");
	ui.title_close_button->setProperty("group", "title-close_button");

	// text
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

	// sound
	ui.enable_sound_edit->setProperty("group", "line_edit");
	ui.disable_sound_edit->setProperty("group", "line_edit");
	ui.window_on_sound_edit->setProperty("group", "line_edit");
	ui.window_off_sound_edit->setProperty("group", "line_edit");
	ui.click_on_sound_edit->setProperty("group", "line_edit");
	ui.click_off_sound_edit->setProperty("group", "line_edit");
	ui.switch_on_sound_edit->setProperty("group", "line_edit");
	ui.switch_off_sound_edit->setProperty("group", "line_edit");
	ui.down_on_sound_edit->setProperty("group", "line_edit");
	ui.down_off_sound_edit->setProperty("group", "line_edit");
	ui.up_on_sound_edit->setProperty("group", "line_edit");
	ui.up_off_sound_edit->setProperty("group", "line_edit");
}

void PopsUi::SetColor(const QColor& color, QPushButton* button)
{
	button->setStyleSheet(QString("border:1px solid black;background-color:rgb(") + QString::number(color.red()) + "," + QString::number(color.green()) + "," + QString::number(color.blue()) + ")");
}
void PopsUi::SelectColor(QiUi::PopTextInfo& p)
{
	QColorDialog cd(p.c, this);
	cd.setStyleSheet(QiUi::color_dialog_style);
	cd.exec();
	p.c = cd.currentColor();
}
void PopsUi::Update()
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

	ui.enable_sound_edit->setText(Qi::ui.pop.qe.s);
	ui.disable_sound_edit->setText(Qi::ui.pop.qd.s);
	ui.window_on_sound_edit->setText(Qi::ui.pop.we.s);
	ui.window_off_sound_edit->setText(Qi::ui.pop.wd.s);
	ui.click_on_sound_edit->setText(Qi::ui.pop.qce.s);
	ui.click_off_sound_edit->setText(Qi::ui.pop.qcd.s);
	ui.switch_on_sound_edit->setText(Qi::ui.pop.swe.s);
	ui.switch_off_sound_edit->setText(Qi::ui.pop.swd.s);
	ui.down_on_sound_edit->setText(Qi::ui.pop.dwe.s);
	ui.down_off_sound_edit->setText(Qi::ui.pop.dwd.s);
	ui.up_on_sound_edit->setText(Qi::ui.pop.upe.s);
	ui.up_off_sound_edit->setText(Qi::ui.pop.upd.s);

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

bool PopsUi::event(QEvent* e)
{
	if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease))
	{
		QKeyEvent* keyEvent = (QKeyEvent*)e;
		if ((keyEvent->key() == Qt::Key_Escape) || (keyEvent->key() == Qt::Key_Return) || keyEvent->key() == Qt::Key_Enter || (keyEvent->key() == Qt::Key_Space)) return true;
	}
	return QDialogFrameless::event(e);
}
bool PopsUi::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::HoverEnter)
	{
		QiUi::PopTextInfo* p = nullptr;
		bool sound = false;
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

		else if (obj == ui.enable_sound_edit) p = &Qi::ui.pop.qe, sound = true;
		else if (obj == ui.disable_sound_edit) p = &Qi::ui.pop.qd, sound = true;
		else if (obj == ui.window_on_sound_edit) p = &Qi::ui.pop.we, sound = true;
		else if (obj == ui.window_off_sound_edit) p = &Qi::ui.pop.wd, sound = true;
		else if (obj == ui.click_on_sound_edit) p = &Qi::ui.pop.qce, sound = true;
		else if (obj == ui.click_off_sound_edit) p = &Qi::ui.pop.qcd, sound = true;
		else if (obj == ui.switch_on_sound_edit) p = &Qi::ui.pop.swe, sound = true;
		else if (obj == ui.switch_off_sound_edit) p = &Qi::ui.pop.swd, sound = true;
		else if (obj == ui.down_on_sound_edit) p = &Qi::ui.pop.dwe, sound = true;
		else if (obj == ui.down_off_sound_edit) p = &Qi::ui.pop.dwd, sound = true;
		else if (obj == ui.up_on_sound_edit) p = &Qi::ui.pop.upe, sound = true;
		else if (obj == ui.up_off_sound_edit) p = &Qi::ui.pop.upd, sound = true;

		if (sound)
		{
			QiFn::SoundPlay(p->s, false);
			Sleep(10);
		}
		else
		{
			if (qs) Qi::popText->Show(QiFn::ParseCustom(p->t, "(文件夹)", ""), p->c);
			else if (ws) Qi::popText->Show(QiFn::ParseCustom(p->t, "(窗口名)", ""), p->c);
			else if (qc) Qi::popText->Show(QiFn::ParseCustom(p->t, "(按键)", ""), p->c);
			else if (mc) Qi::popText->Show(QiFn::ParseCustom(p->t, "(宏名称)", "(1)"), p->c);

			else if (pt) Qi::popText->Show("提示框位置");
			else if (sz) Qi::popText->Show("提示框大小");
			else if (tm) Qi::popText->Popup(Qi::ui.pop.time, "提示框时间");
		}
	}
	else if (e->type() == QEvent::HoverLeave)
	{
		Qi::popText->Hide();
	}
	return QDialogFrameless::eventFilter(obj, e);
}
void PopsUi::showEvent(QShowEvent*)
{
	SetForegroundWindow((HWND)QWidget::winId());
}