#pragma once
#include <windows.h>
#include <QtWidgets/qdialog.h>
#include <qlabel.h>
#include <qevent.h>
#include <qpainter.h>
#include <qscreen.h>
#include <qapplication.h>
#include <qguiapplication.h>
#include <qdesktopwidget.h>

class QPointSelection : public QDialog
{
	Q_OBJECT;
	QLabel lb;
	QPoint ms;
	QRect vrect;
	POINT ams = { 0 };
	RECT rect = { 0 };
	RECT mrect = { 0 };

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

	QPointSelection() : lb(QLabel(this))
	{
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setAttribute(Qt::WA_TranslucentBackground);
		setCursor(Qt::CrossCursor);
		QFont font("Microsoft YaHei"); font.setPixelSize(16);
		lb.setFont(font);
		lb.setAlignment(Qt::AlignCenter);
		lb.setStyleSheet("color:white;background-color:black");
	}

	POINT Start(RECT rect = screenRect())
	{
		this->rect = rect;
		mrect.left = rect.left, mrect.top = rect.top, mrect.right = rect.right + 1, mrect.bottom = rect.bottom + 1;
		float zoom = (float)(screenSize().cx) / (float)(QGuiApplication::primaryScreen()->geometry().width());
		vrect.setLeft((float)rect.left / zoom), vrect.setTop((float)rect.top / zoom), vrect.setRight((float)rect.right / zoom), vrect.setBottom((float)rect.bottom / zoom);
		setGeometry(vrect), grabMouse(), setMouseTracking(1);
		ClipCursor(&rect);
		exec();
		return relPos();
	}

	void setStyleSheet(const QString& styleSheet) { lb.setStyleSheet(styleSheet); }
	POINT relPos() const { return { ams.x - rect.left, ams.y - rect.top }; }
	POINT absPos() const { return ams; }

protected:
	void updateLabel()
	{
		GetCursorPos(&ams);
		lb.setText(QString::number(ams.x - rect.left) + QString::fromUtf8(" - ") + QString::number(ams.y - rect.top));
		QFontMetrics fc(lb.font()); int cx = fc.width(lb.text()), cy = fc.height();
		QPoint pt(ms.x() + 25, ms.y() + 25);
		if ((pt.x() > vrect.width() - cx - 25) && (pt.y() > vrect.height() - cy - 25)) pt.setX(ms.x() - cx - 25), pt.setY(ms.y() - cy - 25);
		else { if (pt.x() > vrect.width() - cx - 25) pt.setX(vrect.width() - cx - 25); if (pt.y() > vrect.height() - cy - 25) pt.setY(vrect.height() - cy - 25); }
		lb.setGeometry(QRect(pt, QSize(cx + 6, cy)));
	}
	void paintEvent(QPaintEvent*) { QPainter pa(this); pa.fillRect(0, 0, vrect.width(), vrect.height(), QColor(0, 0, 0, 50)); }
	void mouseMoveEvent(QMouseEvent* e) { ClipCursor(&mrect); ms = e->pos(); updateLabel(); }
	void mousePressEvent(QMouseEvent* e) { ClipCursor(&mrect); ms = e->pos(); updateLabel(); }
	void mouseReleaseEvent(QMouseEvent* e) { GetCursorPos(&ams); ClipCursor(0); releaseMouse(); close(); }
};