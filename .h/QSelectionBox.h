#pragma once

#include <windows.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <QtWidgets/qdialog.h>
#include <qevent.h>
#include <qpainter.h>

class QSelectionBox : public QDialog
{
	Q_OBJECT

public:

	QSelectionBox(QWidget* parent = 0) : QDialog(parent)
	{
		setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
		setAttribute(Qt::WA_TranslucentBackground);
		setCursor(Qt::CrossCursor);
		box.setParent(this);
		box.hide();
	}

	void Start()
	{
		QRect screen = QApplication::desktop()->screenGeometry();
		setMinimumWidth(screen.width());
		setMinimumHeight(screen.height());
		setMaximumWidth(screen.width());
		setMaximumHeight(screen.height());
		move(0, 0);

		box.hide();
		grabMouse();
		exec();
	}

	QRect relRect()
	{
		return boxRect;
	}

	RECT absRect()
	{
		return boxRectRel;
	}

	void paintEvent(QPaintEvent*) { QPainter pa(this); pa.fillRect(this->rect(), QColor(0, 0, 0, 1)); }

	void setBoxStyleSheet(const QString& styleSheet) { box.setStyleSheet(styleSheet); }

	void setBoxStyle(QStyle* style) { box.setStyle(style); }

private:
	QWidget box;
	QPoint msBefore;
	POINT msBeforeRel;
	QRect boxRect;
	RECT boxRectRel;

protected:

	void mousePressEvent(QMouseEvent* et) { msBefore = et->pos(); GetCursorPos(&msBeforeRel); }

	void mouseReleaseEvent(QMouseEvent* et) { mouseMoveEvent(et); releaseMouse(); close(); }

	void mouseMoveEvent(QMouseEvent* et)
	{
		QPoint msAfter = et->pos();
		POINT msAfterRel = { 0, 0 };
		GetCursorPos(&msAfterRel);

		if (msAfter.x() < msBefore.x())
		{
			boxRect.setLeft(msAfter.x()), boxRect.setRight(msBefore.x());
			boxRectRel.left = msAfterRel.x, boxRectRel.right = msBeforeRel.x;
		}
		else
		{
			boxRect.setLeft(msBefore.x()), boxRect.setRight(msAfter.x());
			boxRectRel.left = msBeforeRel.x, boxRectRel.right = msAfterRel.x;
		}
		if (msAfter.y() < msBefore.y())
		{
			boxRect.setTop(msAfter.y()), boxRect.setBottom(msBefore.y());
			boxRectRel.top = msAfterRel.y, boxRectRel.bottom = msBeforeRel.y;
		}
		else
		{
			boxRect.setTop(msBefore.y()), boxRect.setBottom(msAfter.y());
			boxRectRel.top = msBeforeRel.y, boxRectRel.bottom = msAfterRel.y;
		}

		box.setGeometry(boxRect);
		box.show();
	}
};