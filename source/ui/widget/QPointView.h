#pragma once
#include <windows.h>
#include <QtWidgets/qwidget.h>
#include <qevent.h>
#include <qpainter.h>
#include <qscreen.h>
#include <qapplication.h>
#include <qguiapplication.h>

class QPointView : public QWidget
{
	Q_OBJECT;
	QPoint m_point;
	QColor m_color = QColor(255, 0, 0);
	int m_weight = 2;
	int m_size = 20;

public:
	QPointView()
	{
		setAttribute(Qt::WA_TransparentForMouseEvents);
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setAttribute(Qt::WA_TranslucentBackground);
		move();
	}

	void setPoint(const POINT& apt) { m_point.setX(apt.x), m_point.setY(apt.y); }
	void setPoint(const QPoint& pt) { m_point = pt; }
	void setSize(int size) { m_size = size; }
	void setWeight(int weight) { m_weight = weight; }
	void setColor(const QColor& color) { m_color = color; }
	void move()
	{
		setGeometry(QRect(0, 0, m_size, m_size));
		SetWindowPos(reinterpret_cast<HWND>(winId()), 0, m_point.x() - m_size / 2, m_point.y() - m_size / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		update();
	}

	void show(const POINT& apt, const int size = 30, const int weight = 2, const QColor& color = QColor(255, 0, 0))
	{
		setPoint(apt), setSize(size), setWeight(weight), setColor(color);
		QWidget::show();
		move();
	}

	void show(const QPoint& pt, const int size = 30, const int weight = 2, const QColor& color = QColor(255, 0, 0))
	{
		setPoint(pt), setSize(size), setWeight(weight), setColor(color);
		QWidget::show();
		move();
	}

protected:
	void paintEvent(QPaintEvent*)
	{
		QPainter ptr(this);
		QPen pen(m_color); pen.setWidth(m_weight); ptr.setPen(pen);
		ptr.drawLine(QPoint(0, m_size / 2), QPoint(m_size * 2, m_size / 2));
		ptr.drawLine(QPoint(m_size / 2, 0), QPoint(m_size / 2, m_size * 2));
	}
};