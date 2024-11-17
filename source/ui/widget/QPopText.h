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
	QPoint _point;
	QString _text;
	QColor _color;
	int _time;
public:
	enum
	{
		pop = QEvent::User + 1,
		show,
		hide,
		setPos
	};
	PopTextEvent(const QPoint& point) : QEvent((QEvent::Type)setPos), _point(point) {}
	PopTextEvent(int type, const QString& text = QString(), const QColor& color = QColor(0xC0, 0xE0, 0xFF), int time = 1000) : QEvent((QEvent::Type)type), _text(text), _color(color), _time(time) { }
	QPoint point() const { return _point; }
	QString text() const { return _text; }
	QColor color() const { return _color; }
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
public:

public:
	QPopText() : QDialog()
	{
		point = QPoint(5000, 0);
		color = QColor(0xC0, 0xE0, 0xFF);
		timer = new QTimer(this);
		left = 0;
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
		QApplication::postEvent(this, new PopTextEvent(QPoint(x, y)));
	}
	void Show(const QString& text, const QColor color = QColor(0xC0, 0xE0, 0xFF)) { QApplication::postEvent(this, new PopTextEvent(PopTextEvent::show, text, color, 0)); }
	void Popup(const QString& text, const QColor color = QColor(0xC0, 0xE0, 0xFF), int time = 1000) { QApplication::postEvent(this, new PopTextEvent(PopTextEvent::pop, text, color, time)); }
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
	}
	void paintEvent(QPaintEvent* e)
	{
		QPainter painter(this);
		QFont font("Microsoft YaHei"); font.setPixelSize(24); font.setBold(true);
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