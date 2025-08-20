#pragma once
#include <qmainwindow.h>
#include <qdialog.h>
#include <qwidget.h>
#include <qtimer.h>
#include <qevent.h>
#include <qcursor.h>
class QWidgetFrameless : public QWidget
{
	Q_OBJECT
public:
	QWidgetFrameless(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QWidget(parent, f),
		m_window_adjust_timer(new QTimer(this)),
		m_window_adjust_type(0),
		m_window_adjust_move_height(0),
		m_window_adjust_scale_width(8)
	{
		setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
		setMinimumSize(100, 50);
		connect(m_window_adjust_timer, &QTimer::timeout, this, &QWidgetFrameless::updateCursorShape);
	}
	void setWindowMoveArea(int height)
	{
		m_window_adjust_move_height = height;
	}
	void setWindowScaleArea(int width)
	{
		m_window_adjust_scale_width = width;
	}
protected:
	virtual bool event(QEvent* e)
	{
		if (e->type() == QEvent::WindowActivate) m_window_adjust_timer->start(32);
		else if (e->type() == QEvent::WindowDeactivate) m_window_adjust_timer->stop();
		return QWidget::event(e);
	}
	virtual void mousePressEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			m_window_adjust_mouse_pos = e->globalPos();
			m_window_adjust_geometry = geometry();
			if (m_window_adjust_scale_width > 0 && maximumWidth() - minimumWidth() > 0 && maximumHeight() - minimumHeight() > 0)
			{
				bool r = QRect(width() - m_window_adjust_scale_width, 0, m_window_adjust_scale_width, height()).contains(e->pos());
				bool b = QRect(0, height() - m_window_adjust_scale_width, width(), m_window_adjust_scale_width).contains(e->pos());
				bool l = QRect(0, 0, m_window_adjust_scale_width, height()).contains(e->pos());
				bool t = QRect(0, 0, width(), m_window_adjust_scale_width).contains(e->pos());
				if (r && b) m_window_adjust_type = 5;
				else if (l && b) m_window_adjust_type = 6;
				else if (r && t) m_window_adjust_type = 7;
				else if (l && t) m_window_adjust_type = 8;
				else if (r) m_window_adjust_type = 2;
				else if (b) m_window_adjust_type = 3;
				else if (l) m_window_adjust_type = 4;
				else if (t) m_window_adjust_type = 9;
			}
			if (m_window_adjust_type == 0)
			{
				if (m_window_adjust_move_height < 1)
				{
					m_window_adjust_type = 1;
				}
				else
				{
					QRect move_rect(0, 0, width(), m_window_adjust_move_height);
					if (move_rect.contains(e->pos()))
					{
						m_window_adjust_type = 1;
					}
				}
			}
			e->accept();
		}
	}
	virtual void mouseMoveEvent(QMouseEvent* e)
	{
		if (m_window_adjust_type == 0)
		{
		}
		else if (m_window_adjust_type == 1)
		{
			QPoint delta = e->globalPos() - m_window_adjust_mouse_pos;
			move(pos() + delta);
			m_window_adjust_mouse_pos = e->globalPos();
			e->accept();
		}
		else
		{
			QPoint delta = e->globalPos() - m_window_adjust_mouse_pos;
			QRect newGeometry = m_window_adjust_geometry;
			switch (m_window_adjust_type)
			{
			case 2: newGeometry.setWidth(m_window_adjust_geometry.width() + delta.x()); break;
			case 3: newGeometry.setHeight(m_window_adjust_geometry.height() + delta.y()); break;
			case 4: newGeometry.setX(m_window_adjust_geometry.x() + delta.x()); newGeometry.setWidth(m_window_adjust_geometry.width() - delta.x()); break;
			case 5: newGeometry.setWidth(m_window_adjust_geometry.width() + delta.x()); newGeometry.setHeight(m_window_adjust_geometry.height() + delta.y()); break;
			case 6: newGeometry.setX(m_window_adjust_geometry.x() + delta.x()); newGeometry.setWidth(m_window_adjust_geometry.width() - delta.x()); newGeometry.setHeight(m_window_adjust_geometry.height() + delta.y()); break;
			case 7: newGeometry.setWidth(m_window_adjust_geometry.width() + delta.x()); newGeometry.setY(m_window_adjust_geometry.y() + delta.y()); newGeometry.setHeight(m_window_adjust_geometry.height() - delta.y()); break;
			case 8: newGeometry.setX(m_window_adjust_geometry.x() + delta.x()); newGeometry.setWidth(m_window_adjust_geometry.width() - delta.x()); newGeometry.setY(m_window_adjust_geometry.y() + delta.y()); newGeometry.setHeight(m_window_adjust_geometry.height() - delta.y()); break;
			case 9: newGeometry.setY(m_window_adjust_geometry.y() + delta.y()); newGeometry.setHeight(m_window_adjust_geometry.height() - delta.y()); break;
			}
			if (newGeometry.width() < minimumWidth())
			{
				if (m_window_adjust_type == 4 || m_window_adjust_type == 6 || m_window_adjust_type == 8) newGeometry.setX(geometry().x() + geometry().width() - minimumWidth());
				newGeometry.setWidth(minimumWidth());
			}
			if (newGeometry.height() < minimumHeight())
			{
				if (m_window_adjust_type == 7 || m_window_adjust_type == 8 || m_window_adjust_type == 9) newGeometry.setY(geometry().y() + geometry().height() - minimumHeight());
				newGeometry.setHeight(minimumHeight());
			}
			setGeometry(newGeometry);
			e->accept();
		}
	}
	virtual void mouseReleaseEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			m_window_adjust_type = 0;
			e->accept();
		}
	}
private:
	void updateCursorShape()
	{
		if (m_window_adjust_scale_width > 0 && maximumWidth() - minimumWidth() > 0 && maximumHeight() - minimumHeight() > 0)
		{
			const QPoint pos = mapFromGlobal(QCursor::pos());
			bool r = QRect(width() - m_window_adjust_scale_width, 0, m_window_adjust_scale_width, height()).contains(pos);
			bool b = QRect(0, height() - m_window_adjust_scale_width, width(), m_window_adjust_scale_width).contains(pos);
			bool l = QRect(0, 0, m_window_adjust_scale_width, height()).contains(pos);
			bool t = QRect(0, 0, width(), m_window_adjust_scale_width).contains(pos);
			if (r && b) setCursor(Qt::SizeFDiagCursor);
			else if (l && b) setCursor(Qt::SizeBDiagCursor);
			else if (r && t) setCursor(Qt::SizeBDiagCursor);
			else if (l && t) setCursor(Qt::SizeFDiagCursor);
			else if (r) setCursor(Qt::SizeHorCursor);
			else if (b) setCursor(Qt::SizeVerCursor);
			else if (l) setCursor(Qt::SizeHorCursor);
			else if (t) setCursor(Qt::SizeVerCursor);
			else unsetCursor();
		}
	}
	QTimer* m_window_adjust_timer;
	QRect m_window_adjust_geometry;
	QPoint m_window_adjust_mouse_pos;
	int m_window_adjust_type;
	int m_window_adjust_move_height;
	int m_window_adjust_scale_width;
};
class QDialogFrameless : public QDialog
{
	Q_OBJECT
public:
	QDialogFrameless(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QDialog(parent, f),
		m_window_adjust_timer(new QTimer(this)),
		m_window_adjust_type(0),
		m_window_adjust_move_height(0),
		m_window_adjust_scale_width(8)
	{
		setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
		setMinimumSize(100, 50);
		connect(m_window_adjust_timer, &QTimer::timeout, this, &QDialogFrameless::updateCursorShape);
	}
	void setWindowMoveArea(int height)
	{
		m_window_adjust_move_height = height;
	}
	void setWindowScaleArea(int width)
	{
		m_window_adjust_scale_width = width;
	}
protected:
	virtual bool event(QEvent* e)
	{
		if (e->type() == QEvent::WindowActivate) m_window_adjust_timer->start(32);
		else if (e->type() == QEvent::WindowDeactivate) m_window_adjust_timer->stop();
		return QDialog::event(e);
	}
	virtual void mousePressEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			m_window_adjust_mouse_pos = e->globalPos();
			m_window_adjust_geometry = geometry();
			if (m_window_adjust_scale_width > 0 && maximumWidth() - minimumWidth() > 0 && maximumHeight() - minimumHeight() > 0)
			{
				bool r = QRect(width() - m_window_adjust_scale_width, 0, m_window_adjust_scale_width, height()).contains(e->pos());
				bool b = QRect(0, height() - m_window_adjust_scale_width, width(), m_window_adjust_scale_width).contains(e->pos());
				bool l = QRect(0, 0, m_window_adjust_scale_width, height()).contains(e->pos());
				bool t = QRect(0, 0, width(), m_window_adjust_scale_width).contains(e->pos());
				if (r && b) m_window_adjust_type = 5;
				else if (l && b) m_window_adjust_type = 6;
				else if (r && t) m_window_adjust_type = 7;
				else if (l && t) m_window_adjust_type = 8;
				else if (r) m_window_adjust_type = 2;
				else if (b) m_window_adjust_type = 3;
				else if (l) m_window_adjust_type = 4;
				else if (t) m_window_adjust_type = 9;
			}
			if (m_window_adjust_type == 0)
			{
				if (m_window_adjust_move_height < 1)
				{
					m_window_adjust_type = 1;
				}
				else
				{
					QRect move_rect(0, 0, width(), m_window_adjust_move_height);
					if (move_rect.contains(e->pos()))
					{
						m_window_adjust_type = 1;
					}
				}
			}
			e->accept();
		}
	}
	virtual void mouseMoveEvent(QMouseEvent* e)
	{
		if (m_window_adjust_type == 0)
		{
		}
		else if (m_window_adjust_type == 1)
		{
			QPoint delta = e->globalPos() - m_window_adjust_mouse_pos;
			move(pos() + delta);
			m_window_adjust_mouse_pos = e->globalPos();
			e->accept();
		}
		else
		{
			QPoint delta = e->globalPos() - m_window_adjust_mouse_pos;
			QRect newGeometry = m_window_adjust_geometry;
			switch (m_window_adjust_type)
			{
			case 2: newGeometry.setWidth(m_window_adjust_geometry.width() + delta.x()); break;
			case 3: newGeometry.setHeight(m_window_adjust_geometry.height() + delta.y()); break;
			case 4: newGeometry.setX(m_window_adjust_geometry.x() + delta.x()); newGeometry.setWidth(m_window_adjust_geometry.width() - delta.x()); break;
			case 5: newGeometry.setWidth(m_window_adjust_geometry.width() + delta.x()); newGeometry.setHeight(m_window_adjust_geometry.height() + delta.y()); break;
			case 6: newGeometry.setX(m_window_adjust_geometry.x() + delta.x()); newGeometry.setWidth(m_window_adjust_geometry.width() - delta.x()); newGeometry.setHeight(m_window_adjust_geometry.height() + delta.y()); break;
			case 7: newGeometry.setWidth(m_window_adjust_geometry.width() + delta.x()); newGeometry.setY(m_window_adjust_geometry.y() + delta.y()); newGeometry.setHeight(m_window_adjust_geometry.height() - delta.y()); break;
			case 8: newGeometry.setX(m_window_adjust_geometry.x() + delta.x()); newGeometry.setWidth(m_window_adjust_geometry.width() - delta.x()); newGeometry.setY(m_window_adjust_geometry.y() + delta.y()); newGeometry.setHeight(m_window_adjust_geometry.height() - delta.y()); break;
			case 9: newGeometry.setY(m_window_adjust_geometry.y() + delta.y()); newGeometry.setHeight(m_window_adjust_geometry.height() - delta.y()); break;
			}
			if (newGeometry.width() < minimumWidth())
			{
				if (m_window_adjust_type == 4 || m_window_adjust_type == 6 || m_window_adjust_type == 8) newGeometry.setX(geometry().x() + geometry().width() - minimumWidth());
				newGeometry.setWidth(minimumWidth());
			}
			if (newGeometry.height() < minimumHeight())
			{
				if (m_window_adjust_type == 7 || m_window_adjust_type == 8 || m_window_adjust_type == 9) newGeometry.setY(geometry().y() + geometry().height() - minimumHeight());
				newGeometry.setHeight(minimumHeight());
			}
			setGeometry(newGeometry);
			e->accept();
		}
	}
	virtual void mouseReleaseEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			m_window_adjust_type = 0;
			e->accept();
		}
	}
private:
	void updateCursorShape()
	{
		if (m_window_adjust_scale_width > 0 && maximumWidth() - minimumWidth() > 0 && maximumHeight() - minimumHeight() > 0)
		{
			const QPoint pos = mapFromGlobal(QCursor::pos());
			bool r = QRect(width() - m_window_adjust_scale_width, 0, m_window_adjust_scale_width, height()).contains(pos);
			bool b = QRect(0, height() - m_window_adjust_scale_width, width(), m_window_adjust_scale_width).contains(pos);
			bool l = QRect(0, 0, m_window_adjust_scale_width, height()).contains(pos);
			bool t = QRect(0, 0, width(), m_window_adjust_scale_width).contains(pos);
			if (r && b) setCursor(Qt::SizeFDiagCursor);
			else if (l && b) setCursor(Qt::SizeBDiagCursor);
			else if (r && t) setCursor(Qt::SizeBDiagCursor);
			else if (l && t) setCursor(Qt::SizeFDiagCursor);
			else if (r) setCursor(Qt::SizeHorCursor);
			else if (b) setCursor(Qt::SizeVerCursor);
			else if (l) setCursor(Qt::SizeHorCursor);
			else if (t) setCursor(Qt::SizeVerCursor);
			else unsetCursor();
		}
	}
	QTimer* m_window_adjust_timer;
	QRect m_window_adjust_geometry;
	QPoint m_window_adjust_mouse_pos;
	int m_window_adjust_type;
	int m_window_adjust_move_height;
	int m_window_adjust_scale_width;
};
class QMainWindowFrameless : public QMainWindow
{
	Q_OBJECT
public:
	QMainWindowFrameless(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QMainWindow(parent, f),
		m_window_adjust_timer(new QTimer(this)),
		m_window_adjust_type(0),
		m_window_adjust_move_height(0),
		m_window_adjust_scale_width(8)
	{
		setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
		setMinimumSize(100, 50);
		connect(m_window_adjust_timer, &QTimer::timeout, this, &QMainWindowFrameless::updateCursorShape);
	}
	void setWindowMoveArea(int height)
	{
		m_window_adjust_move_height = height;
	}
	void setWindowScaleArea(int width)
	{
		m_window_adjust_scale_width = width;
	}
protected:
	virtual bool event(QEvent* e)
	{
		if (e->type() == QEvent::WindowActivate) m_window_adjust_timer->start(32);
		else if (e->type() == QEvent::WindowDeactivate) m_window_adjust_timer->stop();
		return QMainWindow::event(e);
	}
	virtual void mousePressEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			m_window_adjust_mouse_pos = e->globalPos();
			m_window_adjust_geometry = geometry();
			if (m_window_adjust_scale_width > 0 && maximumWidth() - minimumWidth() > 0 && maximumHeight() - minimumHeight() > 0)
			{
				bool r = QRect(width() - m_window_adjust_scale_width, 0, m_window_adjust_scale_width, height()).contains(e->pos());
				bool b = QRect(0, height() - m_window_adjust_scale_width, width(), m_window_adjust_scale_width).contains(e->pos());
				bool l = QRect(0, 0, m_window_adjust_scale_width, height()).contains(e->pos());
				bool t = QRect(0, 0, width(), m_window_adjust_scale_width).contains(e->pos());
				if (r && b) m_window_adjust_type = 5;
				else if (l && b) m_window_adjust_type = 6;
				else if (r && t) m_window_adjust_type = 7;
				else if (l && t) m_window_adjust_type = 8;
				else if (r) m_window_adjust_type = 2;
				else if (b) m_window_adjust_type = 3;
				else if (l) m_window_adjust_type = 4;
				else if (t) m_window_adjust_type = 9;
			}
			if (m_window_adjust_type == 0)
			{
				if (m_window_adjust_move_height < 1)
				{
					m_window_adjust_type = 1;
				}
				else
				{
					QRect move_rect(0, 0, width(), m_window_adjust_move_height);
					if (move_rect.contains(e->pos()))
					{
						m_window_adjust_type = 1;
					}
				}
			}
			e->accept();
		}
	}
	virtual void mouseMoveEvent(QMouseEvent* e)
	{
		if (m_window_adjust_type == 0)
		{
		}
		else if (m_window_adjust_type == 1)
		{
			QPoint delta = e->globalPos() - m_window_adjust_mouse_pos;
			move(pos() + delta);
			m_window_adjust_mouse_pos = e->globalPos();
			e->accept();
		}
		else
		{
			QPoint delta = e->globalPos() - m_window_adjust_mouse_pos;
			QRect newGeometry = m_window_adjust_geometry;
			switch (m_window_adjust_type)
			{
			case 2: newGeometry.setWidth(m_window_adjust_geometry.width() + delta.x()); break;
			case 3: newGeometry.setHeight(m_window_adjust_geometry.height() + delta.y()); break;
			case 4: newGeometry.setX(m_window_adjust_geometry.x() + delta.x()); newGeometry.setWidth(m_window_adjust_geometry.width() - delta.x()); break;
			case 5: newGeometry.setWidth(m_window_adjust_geometry.width() + delta.x()); newGeometry.setHeight(m_window_adjust_geometry.height() + delta.y()); break;
			case 6: newGeometry.setX(m_window_adjust_geometry.x() + delta.x()); newGeometry.setWidth(m_window_adjust_geometry.width() - delta.x()); newGeometry.setHeight(m_window_adjust_geometry.height() + delta.y()); break;
			case 7: newGeometry.setWidth(m_window_adjust_geometry.width() + delta.x()); newGeometry.setY(m_window_adjust_geometry.y() + delta.y()); newGeometry.setHeight(m_window_adjust_geometry.height() - delta.y()); break;
			case 8: newGeometry.setX(m_window_adjust_geometry.x() + delta.x()); newGeometry.setWidth(m_window_adjust_geometry.width() - delta.x()); newGeometry.setY(m_window_adjust_geometry.y() + delta.y()); newGeometry.setHeight(m_window_adjust_geometry.height() - delta.y()); break;
			case 9: newGeometry.setY(m_window_adjust_geometry.y() + delta.y()); newGeometry.setHeight(m_window_adjust_geometry.height() - delta.y()); break;
			}
			if (newGeometry.width() < minimumWidth())
			{
				if (m_window_adjust_type == 4 || m_window_adjust_type == 6 || m_window_adjust_type == 8) newGeometry.setX(geometry().x() + geometry().width() - minimumWidth());
				newGeometry.setWidth(minimumWidth());
			}
			if (newGeometry.height() < minimumHeight())
			{
				if (m_window_adjust_type == 7 || m_window_adjust_type == 8 || m_window_adjust_type == 9) newGeometry.setY(geometry().y() + geometry().height() - minimumHeight());
				newGeometry.setHeight(minimumHeight());
			}
			setGeometry(newGeometry);
			e->accept();
		}
	}
	virtual void mouseReleaseEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			m_window_adjust_type = 0;
			e->accept();
		}
	}
private:
	void updateCursorShape()
	{
		if (m_window_adjust_scale_width > 0 && maximumWidth() - minimumWidth() > 0 && maximumHeight() - minimumHeight() > 0)
		{
			const QPoint pos = mapFromGlobal(QCursor::pos());
			bool r = QRect(width() - m_window_adjust_scale_width, 0, m_window_adjust_scale_width, height()).contains(pos);
			bool b = QRect(0, height() - m_window_adjust_scale_width, width(), m_window_adjust_scale_width).contains(pos);
			bool l = QRect(0, 0, m_window_adjust_scale_width, height()).contains(pos);
			bool t = QRect(0, 0, width(), m_window_adjust_scale_width).contains(pos);
			if (r && b) setCursor(Qt::SizeFDiagCursor);
			else if (l && b) setCursor(Qt::SizeBDiagCursor);
			else if (r && t) setCursor(Qt::SizeBDiagCursor);
			else if (l && t) setCursor(Qt::SizeFDiagCursor);
			else if (r) setCursor(Qt::SizeHorCursor);
			else if (b) setCursor(Qt::SizeVerCursor);
			else if (l) setCursor(Qt::SizeHorCursor);
			else if (t) setCursor(Qt::SizeVerCursor);
			else unsetCursor();
		}
	}
	QTimer* m_window_adjust_timer;
	QRect m_window_adjust_geometry;
	QPoint m_window_adjust_mouse_pos;
	int m_window_adjust_type;
	int m_window_adjust_move_height;
	int m_window_adjust_scale_width;
};