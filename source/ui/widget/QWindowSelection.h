#pragma execution_character_set("utf-8")
#pragma once
#include <windows.h>
#include <qdialog.h>
#include <qtimer.h>
#include <qevent.h>
#include <qpainter.h>

class QWindowSelection : public QDialog
{
	Q_OBJECT;
	QTimer* timer;
	QRect prev;
	HWND hWnd;
	float scale;

public:
	QWindowSelection(QWidget* parent = nullptr) : QDialog(parent)
	{
		setAttribute(Qt::WA_TranslucentBackground, true);
		setAttribute(Qt::WA_TransparentForMouseEvents, true);
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setStyleSheet("background-color:rgba(0,128,255,0.2)");

		hWnd = nullptr;

		QString scale_env = qgetenv("QT_SCALE_FACTOR");
		scale = scale_env.toFloat();

		timer = new QTimer(this);

		connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
		timer->start(16);
	}

	HWND Start() { exec(); return hWnd; }

private:
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
		hWnd = GetForegroundWindow();
		if (hWnd)
		{
			if (GetAsyncKeyState(VK_RETURN) & 0x8000) close();
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