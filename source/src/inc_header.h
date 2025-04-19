#pragma once
#include "inc_project.h"
#include "type.h"

#include "json.h"
#include "func.h"
#include "thread.h"
#include "interpreter.h"

#define DecWindowMove QPoint m_window_move_pos;bool m_window_move_press;void mousePressEvent(QMouseEvent*);void mouseMoveEvent(QMouseEvent*);void mouseReleaseEvent(QMouseEvent*);
#define DefWindowMove(className) void className::mousePressEvent(QMouseEvent*e){if(e->button()==Qt::LeftButton){m_window_move_pos=e->pos();m_window_move_press=true;e->accept();}}void className::mouseMoveEvent(QMouseEvent*e){if(m_window_move_press){if (Distance(m_window_move_pos.x(),m_window_move_pos.y(),e->pos().x(),e->pos().y())<100){move(e->pos()+pos()-m_window_move_pos);e->accept();}}}void className::mouseReleaseEvent(QMouseEvent*e){if(e->button()==Qt::LeftButton){m_window_move_press=false;e->accept();}}