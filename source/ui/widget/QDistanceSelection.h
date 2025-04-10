#pragma once
#include <windows.h>
#include <QtWidgets/qdialog.h>
#include <qevent.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qscreen.h>
#include <qapplication.h>
#include <qguiapplication.h>
#include <qdesktopwidget.h>

class QDistanceSelector : public QDialog
{
    Q_OBJECT;
    QLabel lb;
    QPoint msAfter;
    QPoint msBefore;
    POINT amsAfter = { 0 };
    POINT amsBefore = { 0 };
    int m_lineWidth = 2;
    QColor m_lineColor = Qt::red;
    bool m_shiftPressed = false;

public:
    QDistanceSelector() : lb(QLabel(this))
    {
        setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setCursor(Qt::CrossCursor);
        QFont font("Microsoft YaHei"); font.setPixelSize(16);
        lb.setFont(font);
        lb.setAlignment(Qt::AlignCenter);
        lb.setStyleSheet("color:white;background-color:black");
        lb.hide();
    }
    void setStyleSheet(const QString& styleSheet) { lb.setStyleSheet(styleSheet); }

    static RECT RectAbs(RECT rect) { if (rect.left > rect.right) std::swap(rect.left, rect.right); if (rect.top > rect.bottom) std::swap(rect.top, rect.bottom); return rect; }
    static double GetDistance(int x, int y, int x2, int y2)
    {
        int dx = x2 - x;
        int dy = y2 - y;
        return sqrt(dx * dx + dy * dy);
    }

    RECT Start(bool abs_rect = false)
    {
        setGeometry(QGuiApplication::primaryScreen()->geometry()), grabMouse();
        exec();
        return Selection(abs_rect);
    }
    RECT Selection(bool abs_rect = false)
    {
        if (abs_rect) return RectAbs({ amsBefore.x, amsBefore.y, amsAfter.x, amsAfter.y });
        return { amsBefore.x, amsBefore.y, amsAfter.x, amsAfter.y };
    }
protected:
    void updateLabel()
    {
        lb.setText(QString::number(amsAfter.x - amsBefore.x) + QString::fromUtf8(" , ") + QString::number(amsAfter.y - amsBefore.y));
        QFontMetrics fc(lb.font()); int cx = fc.width(lb.text()), cy = fc.height();
        QPoint pt(msAfter.x() + 25, msAfter.y() + 25);
        lb.setGeometry(QRect(pt, QSize(cx + 6, cy)));
    }
    void paintEvent(QPaintEvent*)
    {
        QPainter pa(this);
        pa.fillRect(0, 0, width(), height(), QColor(0, 0, 0, 50));
        if (lb.isHidden()) return;

        pa.setPen(QPen(m_lineColor, m_lineWidth));

        QPoint endPoint = msAfter;
        if (m_shiftPressed)
        {
            int dx = abs(msAfter.x() - msBefore.x());
            int dy = abs(msAfter.y() - msBefore.y());

            if (dx > dy) endPoint.setY(msBefore.y());
            else endPoint.setX(msBefore.x());
        }

        pa.drawLine(QPoint(msBefore.x(), msBefore.y()), endPoint);
        
        pa.setBrush(m_lineColor);
        pa.drawEllipse(QPoint(msBefore.x(), msBefore.y()), m_lineWidth, m_lineWidth);
        pa.drawEllipse(endPoint, m_lineWidth, m_lineWidth);

        updateLabel();
    }
    void keyPressEvent(QKeyEvent* e)
    {
        if (e->key() == Qt::Key_Shift)
        {
            m_shiftPressed = true;
            update();
        }
        QDialog::keyPressEvent(e);
    }
    void keyReleaseEvent(QKeyEvent* e)
    {
        if (e->key() == Qt::Key_Shift)
        {
            m_shiftPressed = false;
            update();
        }
        QDialog::keyReleaseEvent(e);
    }
    void mousePressEvent(QMouseEvent* e)
    {
        GetCursorPos(&amsAfter);
        amsBefore = amsAfter;
        msBefore = msAfter = e->pos();
        lb.show();
        update();
    }
    void mouseReleaseEvent(QMouseEvent* e)
    {
        ClipCursor(0);
        releaseMouse();
        close();
    }
    void mouseMoveEvent(QMouseEvent* e)
    {
        GetCursorPos(&amsAfter);
        msAfter = e->pos();

        if (m_shiftPressed)
        {
            int dx = abs(msAfter.x() - msBefore.x());
            int dy = abs(msAfter.y() - msBefore.y());

            if (dx > dy)
            {
                SetCursorPos(amsAfter.x, amsBefore.y);
                msAfter.setY(msBefore.y());
            }
            else
            {
                SetCursorPos(amsBefore.x, amsAfter.y);
                msAfter.setX(msBefore.x());
            }
        }

        update();
    }
};