#pragma once
#include <windows.h>
#include <QtWidgets/qwidget.h>
#include <qevent.h>
#include <qpainter.h>
#include <qscreen.h>
#include <qapplication.h>
#include <qguiapplication.h>

class QRectView : public QWidget
{
	Q_OBJECT;
	QRect m_rect;
	QColor m_color = QColor(255, 0, 0);
	int m_weight = 2;

public:
	QRectView()
	{
		setAttribute(Qt::WA_TransparentForMouseEvents);
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setAttribute(Qt::WA_TranslucentBackground);
		move();
	}

	void setRect(const RECT& arect) { m_rect.setLeft(arect.left), m_rect.setTop(arect.top), m_rect.setRight(arect.right), m_rect.setBottom(arect.bottom); }
	void setRect(const QRect& rect) { m_rect = rect; }
	void setWeight(int weight) { m_weight = weight; }
	void setColor(const QColor& color) { m_color = color; }
	void move()
	{
		setGeometry(m_rect);
		SetWindowPos(reinterpret_cast<HWND>(winId()), 0, m_rect.left(), m_rect.top(), m_rect.width(), m_rect.height(), SWP_NOZORDER);
		update();
	}

	void show(const RECT& rect, int weight = 2, const QColor& color = QColor(255, 0, 0))
	{
		setRect(rect), setWeight(weight), setColor(color);
		QWidget::show();
		move();
	}
	void show(const QRect& rect, int weight = 2, const QColor& color = QColor(255, 0, 0))
	{
		setRect(rect), setWeight(weight), setColor(color);
		QWidget::show();
		move();
	}

protected:
	void paintEvent(QPaintEvent*)
	{
		QPainter ptr(this);
		QPen pen(m_color); pen.setWidth(m_weight); ptr.setPen(pen);
		ptr.drawRect(QRect(1, 1, m_rect.width() - 2, m_rect.height() - 2));
	}
};