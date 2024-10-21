#pragma execution_character_set("utf-8")
#pragma once
#include <windows.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qevent.h>
#include <qpainter.h>

class QWindowSelection : public QDialog
{
	Q_OBJECT;

	HWND hWnd;
	float scale;
	QTimer* timer;
	QLabel* label;
	QHBoxLayout* layout;

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
		label = new QLabel(this);
		layout = new QHBoxLayout(this);
		layout->addWidget(label);

		label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		label->setStyleSheet("color:white;font-size:36px");
		label->setText("点击回车选择窗口");

		connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
		timer->start(16);
	}

	HWND Start() { exec(); return hWnd; }

private Q_SLOTS:

	void timerEvent()
	{
		hWnd = GetForegroundWindow();
		RECT window_rect; GetWindowRect(hWnd, &window_rect);

		int window_text_length = GetWindowTextLengthW(hWnd);
		wchar_t* window_text = new wchar_t[window_text_length + 1];
		window_text[window_text_length] = 0;

		GetWindowTextW(hWnd, window_text, window_text_length);

		QString title = QString::fromWCharArray(window_text);
		delete[] window_text;

		QRect rect;
		rect.setLeft(window_rect.left / scale);
		rect.setTop(window_rect.top / scale);
		rect.setRight(window_rect.right / scale);
		rect.setBottom(window_rect.bottom / scale);

		setGeometry(rect);

		if (GetAsyncKeyState(VK_RETURN) & 0x8000) close();
	}
};