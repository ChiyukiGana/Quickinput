#pragma execution_character_set("utf-8")
#pragma once
#include <qapplication.h>
#include <qdialog.h>
#include <qpainter.h>
#include <qevent.h>
#include <qtimer.h>
#include <qscreen.h>

class PopTextEvent : public QEvent
{
	QString _text;
	QColor _color;
	QPoint _point;
	int _size;
	int _time;
public:
	enum
	{
		pop = QEvent::User + 1,
		show,
		hide,
		setPos,
		setSize,
		setTime
	};
	PopTextEvent(int type, const QString& text, const QColor& color, int time) : QEvent((QEvent::Type)type), _text(text), _color(color), _time(time) {}
	PopTextEvent(int type, const QString& text, const QColor& color) : QEvent((QEvent::Type)type), _text(text), _color(color) {}
	PopTextEvent(int type) : QEvent((QEvent::Type)type) {}
	PopTextEvent(int type, const QPoint& point) : QEvent((QEvent::Type)type), _point(point) {}
	PopTextEvent(int type, int size) : QEvent((QEvent::Type)type), _size(size) {}
	PopTextEvent(int type, int time, bool t) : QEvent((QEvent::Type)type), _time(time) {}
	QString text() const { return _text; }
	QColor color() const { return _color; }
	QPoint point() const { return _point; }
	int size() const { return _size; }
	int time() const { return _time; }
};

class QPopText : public QDialog
{
	Q_OBJECT;
	QPoint point;
	QString text;
	QColor color;
	QTimer* timer;
	int left;
	int size;
	int time;
public:

public:
	QPopText() : QDialog()
	{
		point = QPoint(5000, 0);
		color = QColor(0xC0, 0xE0, 0xFF);
		timer = new QTimer(this);
		left = 0;
		size = 20;
		time = 1000;
		connect(timer, SIGNAL(timeout()), this, SLOT(OnTimer()));

		setAttribute(Qt::WA_TransparentForMouseEvents, true);
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setPosition(5000, 5000);
		setWindowOpacity(0.0);
		setStyleSheet("color:black");
		show();
	}
	void setPosition(int x, int y)
	{
		QApplication::postEvent(this, new PopTextEvent(PopTextEvent::setPos, QPoint(x, y)));
	}
	void setSize(int size)
	{
		QApplication::postEvent(this, new PopTextEvent(PopTextEvent::setSize, size));
	}
	void setTime(int time)
	{
		QApplication::postEvent(this, new PopTextEvent(PopTextEvent::setTime, time, true));
	}
	void Show(const QString& text, const QColor color = QColor(0xC0, 0xE0, 0xFF)) { QApplication::postEvent(this, new PopTextEvent(PopTextEvent::show, text, color)); }
	void Popup(const QString& text, const QColor color = QColor(0xC0, 0xE0, 0xFF)) { QApplication::postEvent(this, new PopTextEvent(PopTextEvent::pop, text, color, time)); }
	void Popup(int time, const QString& text, const QColor color = QColor(0xC0, 0xE0, 0xFF)) { QApplication::postEvent(this, new PopTextEvent(PopTextEvent::pop, text, color, time)); }
	void Hide() { QApplication::postEvent(this, new PopTextEvent(PopTextEvent::hide)); }
private:
	void customEvent(QEvent* e)
	{
		PopTextEvent* popText = (PopTextEvent*)e;
		if (popText->type() == PopTextEvent::pop)
		{
			timer->stop();
			color = popText->color();
			text = popText->text();
			left = popText->time();
			setWindowOpacity(1.0);
			timer->start(32);
			repaint();
		}
		else if (popText->type() == PopTextEvent::show)
		{
			timer->stop();
			color = popText->color();
			text = popText->text();
			setWindowOpacity(1.0);
			repaint();
		}
		else if (popText->type() == PopTextEvent::hide)
		{
			timer->stop();
			setWindowOpacity(0.0);
			repaint();
		}
		else if (popText->type() == PopTextEvent::setPos)
		{
			point = popText->point();
		}
		else if (popText->type() == PopTextEvent::setSize)
		{
			size = popText->size();
			if (size < 10) size = 10;
			else if (size > 72) size = 72;
		}
		else if (popText->type() == PopTextEvent::setTime)
		{
			time = popText->time();
			if (time < 100) time = 100;
			else if (time > 5000) time = 5000;
		}
	}
	void paintEvent(QPaintEvent* e)
	{
		QPainter painter(this);
		QFont font("Microsoft YaHei"); font.setPixelSize(size); font.setBold(true);
		painter.setFont(font);
		QSize screen = QApplication::primaryScreen()->size();
		int pos_x = (int)((float)(point.x()) * ((float)(screen.width()) / 10000.0f));
		int pos_y = (int)((float)(point.y()) * ((float)(screen.height()) / 10000.0f));
		int size_x = painter.fontMetrics().width(text) + font.pixelSize();
		int size_y = painter.fontMetrics().height() + (font.pixelSize() >> 1);
		if (pos_x < (size_x >> 1)) pos_x = (size_x >> 1);
		else if (pos_x > (screen.width() - (size_x >> 1))) pos_x = screen.width() - (size_x >> 1);
		if (pos_y < (size_y >> 1)) pos_y = (size_y >> 1);
		else if (pos_y > (screen.height() - (size_y >> 1))) pos_y = screen.height() - (size_y >> 1);
		setGeometry(pos_x - (size_x >> 1), pos_y - (size_y >> 1), size_x, size_y);
		painter.fillRect(0, 0, width(), height(), color);
		painter.drawText(0, 0, width(), height(), Qt::AlignCenter, text);
	}

private Q_SLOTS:
	void OnTimer()
	{
		if (left < 1000)
		{
			double opacity = ((double)left) / 1000.0;
			setWindowOpacity(opacity);
		}
		if (left > 32) left -= 32;
		else
		{
			left = 0;
			timer->stop();
			setWindowOpacity(0.0);
		}
	}
};