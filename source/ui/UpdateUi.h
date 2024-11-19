#pragma execution_character_set("utf-8")
#pragma once
#include <qevent.h>
#include "src/minc.h"
#include "ui_UpdateUi.h"
class UpdateUi : public QDialog
{
	Q_OBJECT;
	Ui::UpdateUiClass ui;

public:
	UpdateUi(const QString& version, const QString& content)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		StyleGroup();

		ui.lb_version->setText(version);
		ui.lb_content->setText(content);

		connect(ui.bnClose, &QPushButton::clicked, this, [this](bool) { close(); });

		Qi::widget.versionActive = true;
		exec();
		Qi::widget.versionActive = false;
	}
	void StyleGroup()
	{
		setProperty("group", "frame");
		ui.titleWidget->setProperty("group", "title");
		ui.clientWidget->setProperty("group", "client");
	}
private:
	QPoint msPos; bool mouseDown = false; void mousePressEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) msPos = e->pos(), mouseDown = true; e->accept(); }void mouseMoveEvent(QMouseEvent* e) { if (mouseDown) move(e->pos() + pos() - msPos); }void mouseReleaseEvent(QMouseEvent* e) { if (e->button() == Qt::LeftButton) mouseDown = false; }
};