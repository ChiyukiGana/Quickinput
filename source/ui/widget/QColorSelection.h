#pragma once
#include <windows.h>
#include <QtWidgets/qdialog.h>
#include <qevent.h>
#include <qpainter.h>
#include <qscreen.h>
#include <qapplication.h>
#include <qguiapplication.h>
#include <qdesktopwidget.h>

class QColorBox : public QWidget
{
	Q_OBJECT
public:
	QColorBox(QWidget* parent = 0) : QWidget(parent) {}
	void setColor(QColor color) { this->color = color; repaint(); }
private:
	QColor color;
protected:
	void paintEvent(QPaintEvent*) { QPainter pa(this); pa.fillRect(this->rect(), color); }
};

class QColorSelection : public QDialog
{
	Q_OBJECT;
	QColorBox colorBox;
	QColor color;
	QRect vrect;
	HDC hdc = 0;
	bool end = 0;

public:

	QColorSelection() : QDialog()
	{
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setAttribute(Qt::WA_TranslucentBackground);
		setCursor(Qt::CrossCursor);
		colorBox.setParent(this);
		colorBox.setMinimumWidth(20);
		colorBox.setMinimumHeight(20);
		colorBox.setMaximumWidth(20);
		colorBox.setMaximumHeight(20);
	}

	QColor Start()
	{
		hdc = GetDC(0);
		end = 0;
		vrect = QGuiApplication::primaryScreen()->geometry();
		setGeometry(vrect);
		grabMouse(), setMouseTracking(1);
		exec();
		ReleaseDC(0, hdc);
		return color;
	}

private:
	void SetColor(QMouseEvent* e)
	{
		QPoint ms = e->pos();
		QPoint pt(ms.x() + 25, ms.y() + 25);
		POINT ams; GetCursorPos(&ams);
		COLORREF rgb = GetPixel(hdc, ams.x, ams.y);
		if ((pt.x() > vrect.width() - 25) && (pt.y() > vrect.height() - 25)) pt.setX(ms.x() - 45), pt.setY(ms.y() - 45);
		else { if (pt.x() > vrect.width() - 25) pt.setX(vrect.width() - 25); if (pt.y() > vrect.height() - 25) pt.setY(vrect.height() - 25); }
		color = qRgb(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
		colorBox.setColor(color);
		colorBox.move(pt);
	}

protected:
	void paintEvent(QPaintEvent*) { if (!end) { QPainter pa(this); pa.fillRect(this->rect(), QColor(127, 127, 127, 1)); } }
	void mousePressEvent(QMouseEvent* e) { SetColor(e); }
	void mouseReleaseEvent(QMouseEvent* e) { end = 1; repaint(); SetColor(e); releaseMouse(); close(); }
	void mouseMoveEvent(QMouseEvent* e) { SetColor(e); }
};