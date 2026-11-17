#pragma once
#include <windows.h>
#include <QtWidgets/qdialog.h>
#include <qlabel.h>
#include <qevent.h>
#include <qpainter.h>
#include <qscreen.h>
#include <qapplication.h>
#include <qguiapplication.h>

class QPointSelector : public QDialog
{
	Q_OBJECT;
	QLabel* m_label;
	POINT m_selectPoint{};
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
	QPointSelector() : m_label(new QLabel(this))
	{
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setAttribute(Qt::WA_TranslucentBackground);
		setCursor(Qt::CrossCursor);
		QFont font("Microsoft YaHei"); font.setPixelSize(16);
		m_label->setFont(font);
		m_label->setAlignment(Qt::AlignCenter);
		m_label->setAttribute(Qt::WA_TransparentForMouseEvents);
		m_label->setStyleSheet("color:white;background-color:black");
	}
	QString styleSheet() { return m_label->styleSheet(); }
	void setStyleSheet(const QString& styleSheet) { m_label->setStyleSheet(styleSheet); }

	void setSubRect(const RECT& subRect)
	{
		m_subRect = subRect;
	}
	RECT subRect() const
	{
		return m_subRect;
	}

	POINT selectPoint() const { return m_selectPoint; }

	virtual int exec() override
	{
		clipCursor(m_subRect);
		setMouseTracking(true);
		return QDialog::exec();
	}

	static POINT SelectPrimaryScreen(const RECT& subRect = toRECT(physPrimaryScreen()))
	{
		auto* selector = new QPointSelector();
		selector->setGeometry(logicPrimaryScreen());
		selector->setSubRect(subRect);
		selector->exec();
		return selector->selectPoint();
	}
	static POINT SelectVirtualScreen(const RECT& subRect = toRECT(physVirtualScreen()))
	{
		auto* selector = new QPointSelector();
		selector->setGeometry(physVirtualScreen());
		selector->setSubRect(subRect);
		selector->exec();
		return selector->selectPoint();
	}

protected:
	void paintEvent(QPaintEvent*)
	{
		QPainter pa(this);
		pa.fillRect(0, 0, width(), height(), QColor(0, 0, 0, 50));
	}
	void mousePressEvent(QMouseEvent* e)
	{
		clipCursor(m_subRect);
	}
	void mouseMoveEvent(QMouseEvent* e)
	{
		const QPoint mouse = e->pos();
		GetCursorPos(&m_selectPoint);
		m_label->setText(QString::number(m_selectPoint.x - m_subRect.left) + QString::fromUtf8(" - ") + QString::number(m_selectPoint.y - m_subRect.top));
		QFontMetrics fc(m_label->font()); int cx = fc.horizontalAdvance(m_label->text()), cy = fc.height();
		QPoint pt(mouse.x() + 25, mouse.y() + 25);
		if ((pt.x() > width() - cx - 25) && (pt.y() > height() - cy - 25)) pt.setX(mouse.x() - cx - 25), pt.setY(mouse.y() - cy - 25);
		else { if (pt.x() > width() - cx - 25) pt.setX(width() - cx - 25); if (pt.y() > height() - cy - 25) pt.setY(height() - cy - 25); }
		m_label->setGeometry(QRect(pt, QSize(cx + 6, cy)));
	}
	void mouseReleaseEvent(QMouseEvent* e)
	{
		GetCursorPos(&m_selectPoint);
		unClipCursor();
		setMouseTracking(false);
		close();
	}
};