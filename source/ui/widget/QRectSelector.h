#pragma once
#include <windows.h>
#include <QtWidgets/qdialog.h>
#include <qevent.h>
#include <qpainter.h>
#include <qscreen.h>
#include <qapplication.h>
#include <qguiapplication.h>

class QRectSelector : public QDialog
{
	Q_OBJECT;
	QWidget* m_widget;
	QPoint m_mouseEnter;
	POINT m_mouseEnter_p{};
	RECT m_selectRect{};
	RECT m_subRect{};

public:
	static QRect physPrimaryScreen()
	{
		return QRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	}
	static QRect physVirtualScreen()
	{
		return QRect(GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN), GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
	}
	static QRect logicPrimaryScreen()
	{
		return QGuiApplication::primaryScreen()->geometry();
	}
	static constexpr RECT makeRect(const POINT& p1, const POINT& p2)
	{
		return { qMin(p1.x, p2.x), qMin(p1.y, p2.y), qMax(p1.x, p2.x), qMax(p1.y, p2.y) };
	}
	static constexpr RECT toRECT(const QRect& rect)
	{
		return { rect.left(), rect.top(), rect.right(), rect.bottom() };
	}
	static void clipCursor(const RECT& rect)
	{
		RECT r = rect;
		r.right += 1;
		r.bottom += 1;
		ClipCursor(&r);
	}
	static void unClipCursor()
	{
		ClipCursor(nullptr);
	}

public:
	QRectSelector() : m_widget(new QWidget(this))
	{
		setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
		setAttribute(Qt::WA_TranslucentBackground);
		setCursor(Qt::CrossCursor);
		m_widget->hide();
		m_widget->setAttribute(Qt::WA_TransparentForMouseEvents);
		m_widget->setStyleSheet("background-color:rgba(128,144,224,0.5);border:1px solid #08F;");
	}

	QString selectionStyleSheet() const { return m_widget->styleSheet(); }
	void setSelectionStyleSheet(const QString& styleSheet) { m_widget->setStyleSheet(styleSheet); }

	void setSubRect(const RECT& subRect)
	{
		m_subRect = subRect;
	}
	RECT subRect() const
	{
		return m_subRect;
	}

	RECT selectRect() const { return m_selectRect; }

	virtual int exec() override
	{
		clipCursor(m_subRect);
		return QDialog::exec();
	}

	static RECT SelectPrimaryScreen(const RECT& subRect = toRECT(physPrimaryScreen()))
	{
		auto* selector = new QRectSelector();
		selector->setGeometry(logicPrimaryScreen());
		selector->setSubRect(subRect);
		selector->exec();
		return selector->selectRect();
	}
	static RECT SelectVirtualScreen(const RECT& subRect = toRECT(physVirtualScreen()))
	{
		auto* selector = new QRectSelector();
		selector->setGeometry(physVirtualScreen());
		selector->setSubRect(subRect);
		selector->exec();
		return selector->selectRect();
	}

protected:
	void paintEvent(QPaintEvent*)
	{
		QPainter pa(this);
		pa.fillRect(0, 0, width(), height(), QColor(0, 0, 0, 50));
	}
	void mousePressEvent(QMouseEvent* e)
	{
		m_mouseEnter = e->pos();
		GetCursorPos(&m_mouseEnter_p);
		clipCursor(m_subRect);
	}
	void mouseMoveEvent(QMouseEvent* e)
	{
		m_widget->setGeometry(QRect(m_mouseEnter, e->pos()).normalized());
		if (!m_widget->isVisible()) m_widget->show();
	}
	void mouseReleaseEvent(QMouseEvent* e)
	{
		POINT mouseLeave_p;
		GetCursorPos(&mouseLeave_p);
		m_selectRect = makeRect(m_mouseEnter_p, mouseLeave_p);
		unClipCursor();
		close();
	}
};