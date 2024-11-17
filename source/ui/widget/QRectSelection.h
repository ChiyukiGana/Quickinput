#pragma once
#include <windows.h>
#include <QtWidgets/qdialog.h>
#include <qevent.h>
#include <qpainter.h>
#include <qscreen.h>
#include <qapplication.h>
#include <qguiapplication.h>
#include <qdesktopwidget.h>

class QRectSelection : public QDialog
{
	Q_OBJECT;
	QWidget wid;
	QPoint msBefore;
	QRect vrect;
	POINT ams = { 0 };
	POINT amsBefore = { 0 };
	RECT rect = { 0 };
	RECT mrect = { 0 };

public:
	QRectSelection(QWidget* parent = 0) : QDialog(parent)
	{
		setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
		setAttribute(Qt::WA_TranslucentBackground);
		setCursor(Qt::CrossCursor);
		wid.setParent(this);
		wid.setStyleSheet("background-color:rgba(128,144,224,0.5);border:1px solid #08F;");
		wid.hide();
	}

	static SIZE screenSize()
	{
		HWND wnd = GetDesktopWindow();
		HMONITOR mt = MonitorFromWindow(wnd, MONITOR_DEFAULTTONEAREST);

		MONITORINFOEX mti;
		mti.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfoW(mt, &mti);

		DEVMODEW dm;
		dm.dmSize = sizeof(DEVMODEW);
		dm.dmDriverExtra = 0;
		EnumDisplaySettingsW(mti.szDevice, ENUM_CURRENT_SETTINGS, &dm);

		return { (LONG)dm.dmPelsWidth, (LONG)dm.dmPelsHeight };
	}
	static RECT screenRect() { SIZE size = screenSize(); return { 0, 0, size.cx - 1, size.cy - 1 }; }
	static RECT RectAbs(RECT rect) { if (rect.left > rect.right) std::swap(rect.left, rect.right); if (rect.top > rect.bottom) std::swap(rect.top, rect.bottom); return rect; }

	RECT Start(RECT rect = screenRect())
	{
		this->rect = rect;
		mrect.left = rect.left, mrect.top = rect.top, mrect.right = rect.right + 1, mrect.bottom = rect.bottom + 1;
		float zoom = (float)(screenSize().cx) / (float)(QGuiApplication::primaryScreen()->geometry().width());
		vrect.setLeft((float)rect.left / zoom), vrect.setTop((float)rect.top / zoom), vrect.setRight((float)rect.right / zoom), vrect.setBottom((float)rect.bottom / zoom);
		setGeometry(vrect), grabMouse();
		ClipCursor(&mrect);
		exec();
		return relRect();
	}

	void setBoxStyleSheet(const QString& styleSheet) { wid.setStyleSheet(styleSheet); }
	RECT relRect() { return RectAbs( { amsBefore.x - rect.left, amsBefore.y - rect.top, ams.x - rect.left, ams.y - rect.top }); }
	RECT absRect() { return RectAbs({ amsBefore.x, amsBefore.y, ams.x, ams.y }); }

protected:
	void paintEvent(QPaintEvent*) { QPainter pa(this); pa.fillRect(0, 0, vrect.width(), vrect.height(), QColor(0, 0, 0, 50)); }
	void mousePressEvent(QMouseEvent* e) { ClipCursor(&mrect); GetCursorPos(&amsBefore); msBefore = e->pos(); }
	void mouseReleaseEvent(QMouseEvent* e) { ClipCursor(0); releaseMouse(); close(); }
	void mouseMoveEvent(QMouseEvent* e)
	{
		ClipCursor(&mrect);
		GetCursorPos(&ams);
		QRect box;
		QPoint msAfter = e->pos();
		if (msAfter.x() < msBefore.x()) box.setLeft(msAfter.x()), box.setRight(msBefore.x());
		else box.setLeft(msBefore.x()), box.setRight(msAfter.x());
		if (msAfter.y() < msBefore.y()) box.setTop(msAfter.y()), box.setBottom(msBefore.y());
		else box.setTop(msBefore.y()), box.setBottom(msAfter.y());
		wid.setGeometry(box);
		wid.show();
	}
};