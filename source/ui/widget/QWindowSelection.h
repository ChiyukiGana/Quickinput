#pragma execution_character_set("utf-8")
#pragma once
#include <cg/cg.h>
#include <windows.h>
#include <qdialog.h>
#include <qtimer.h>
#include <qevent.h>
#include <qpainter.h>
#include <qapplication.h>

class QWindowSelection : public QDialog
{
	Q_OBJECT;
	using This = QWindowSelection;

	enum Event
	{
		_begin = QEvent::User
	};

	QTimer* timer;
	QRect prev;
	HWND hWnd;
	float scale;
	bool active;

	DWORD current;
	DWORD call;

public:
	QWindowSelection()
	{
		setAttribute(Qt::WA_TranslucentBackground, true);
		setAttribute(Qt::WA_TransparentForMouseEvents, true);
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setStyleSheet("background-color:rgba(0,128,255,0.2)");

		QString scale_env = qgetenv("QT_SCALE_FACTOR");
		scale = scale_env.toFloat();

		timer = new QTimer(this);
		connect(timer, &QTimer::timeout, this, &This::timerEvent);

		current = GetCurrentThreadId();

		hide();
	}

	HWND Start() {
		call = GetCurrentThreadId();
		hWnd = nullptr;
		active = true;
		if (current == call)
		{
			timer->start(32);
			exec();
		}
		else
		{
			QApplication::postEvent(this, new QEvent((QEvent::Type)_begin));
			while (active) Sleep(32);
		}
		return hWnd;
	}

private:
	void customEvent(QEvent* e)
	{
		if (e->type() == _begin)
		{
			timer->start(32);
			show();
		}
	}
	void paintEvent(QPaintEvent* e)
	{
		QPainter painter(this);
		int font_size = width() >> 3; if (font_size > 72) font_size = 72; else if (font_size < 10) font_size = 10;
		QFont font;
		font.setPixelSize(font_size);
		painter.setFont(font);
		painter.setPen(QColor(255, 255, 255));
		painter.fillRect(0, 0, width(), height(), QColor(0, 128, 255, 64));
		painter.drawText(0, 0, width(), height(), Qt::AlignCenter, "点击回车选择");
	}

private Q_SLOTS:

	void timerEvent()
	{
		if (!active) return;

		hWnd = GetForegroundWindow();
		if (hWnd)
		{
			if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			{
				active = false;
				timer->stop();
				if (current == call) close();
				else hide();
			}
			else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			{
				active = false;
				hWnd = nullptr;
				timer->stop();
				if (current == call) close();
				else hide();
			}
			RECT window_rect = {};
			if (GetWindowRect(hWnd, &window_rect))
			{
				QRect rect;
				rect.setLeft(window_rect.left / scale);
				rect.setTop(window_rect.top / scale);
				rect.setRight(window_rect.right / scale);
				rect.setBottom(window_rect.bottom / scale);
				
				if (rect != prev)
				{
					prev = rect;
					setGeometry(rect);
					repaint();
				}
			}
		}
	}
};