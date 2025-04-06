#pragma once
#include <windows.h>
#include <QtWidgets/qwidget.h>
#include <qevent.h>
#include <qpainter.h>
#include <qscreen.h>
#include <qapplication.h>
#include <qguiapplication.h>
#include <qdesktopwidget.h>

class QPointView : public QWidget
{
	Q_OBJECT;
	QPoint pt;
	QColor color = QColor(255, 0, 0);
	quint32 weight = 2;
	quint32 size = 20;
	float zoom;

public:
	static SIZE screenSize()
	{
		HWND wnd = GetDesktopWindow();
		HMONITOR mt = MonitorFromWindow(wnd, MONITOR_DEFAULTTONEAREST);

		MONITORINFOEXW mti;
		mti.cbSize = sizeof(MONITORINFOEXW);
		GetMonitorInfoW(mt, &mti);

		DEVMODEW dm;
		dm.dmSize = sizeof(DEVMODEW);
		dm.dmDriverExtra = 0;
		EnumDisplaySettingsW(mti.szDevice, ENUM_CURRENT_SETTINGS, &dm);

		return { (LONG)dm.dmPelsWidth, (LONG)dm.dmPelsHeight };
	}
	static RECT screenRect() { SIZE size = screenSize(); return { 0, 0, size.cx - 1, size.cy - 1 }; }

	QPointView()
	{
		setAttribute(Qt::WA_TransparentForMouseEvents);
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setAttribute(Qt::WA_TranslucentBackground);
		zoom = (float)(screenSize().cx) / (float)(QGuiApplication::primaryScreen()->geometry().width());
	}

	void APoint(POINT apt) { pt.setX(apt.x / zoom), pt.setY(apt.y / zoom); }
	void Point(QPoint pt) { this->pt = pt; }
	void Size(quint32 size) { this->size = size; }
	void Weight(quint32 weight) { this->weight = weight; }
	void Color(QColor color) { this->color = color; }
	void Move() { setGeometry(QRect(pt.x() - size / 2, pt.y() - size / 2, size, size)); }

	void Show(POINT apt, const quint32 size = 30, const quint32 weight = 2, QColor color = QColor(255, 0, 0))
	{
		APoint(apt), Size(size), Weight(weight), Color(color);
		Move();
		show();
	}

protected:
	void paintEvent(QPaintEvent*) {
		QPainter ptr(this);
		QPen pen(color); pen.setWidth(weight); ptr.setPen(pen);
		ptr.drawLine(QPoint(0, size / 2), QPoint(size * 2, size / 2));
		ptr.drawLine(QPoint(size / 2, 0), QPoint(size / 2, size * 2));
	}
};