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
	QRect rect;
	QColor color = QColor(255, 0, 0);
	quint32 weight = 2;
	RECT arect = { 0 };
	float scale = 1.0f;

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

	QRectView()
	{
		setAttribute(Qt::WA_TransparentForMouseEvents);
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setAttribute(Qt::WA_TranslucentBackground);
		scale = (float)(screenSize().cx) / (float)(QGuiApplication::primaryScreen()->geometry().width());
	}

	void ARect(RECT arect) { this->arect = arect; rect.setLeft(arect.left / scale), rect.setTop(arect.top / scale), rect.setRight(arect.right / scale), rect.setBottom(arect.bottom / scale); }
	void Rect(QRect rect) { this->rect = rect; }
	void Weight(quint32 weight) { this->weight = weight; }
	void Color(QColor color) { this->color = color; }
	void Move() { setGeometry(rect); }

	void Show(RECT arect, quint32 weight = 2, QColor color = QColor(255, 0, 0))
	{
		ARect(arect), Weight(weight), Color(color);
		Move();
		show();
	}

protected:
	void paintEvent(QPaintEvent*) {
		QPainter ptr(this);
		QPen pen(color); pen.setWidth(weight); ptr.setPen(pen);
		ptr.drawRect(QRect(1, 1, rect.width() - 2, rect.height() - 2));
	}
};