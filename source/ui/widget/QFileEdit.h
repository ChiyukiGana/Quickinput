#pragma once
#include <qlist.h>
#include <qstring.h>
#include <qdir.h>
#include <qevent.h>
#include <qaction.h>
#include <qmenu.h>
#include <qlineedit.h>
#include <qfiledialog.h>

class QFileEdit : public QLineEdit
{
	Q_OBJECT;
	QMenu m_menu;
	QAction m_system;
	QList<QAction*> m_actions;
	QList<QString> m_files;
	QString m_title;
	QString m_type;
	QString m_dir;

public:
	QFileEdit(QWidget* parent = nullptr) : QLineEdit(parent), m_menu(QMenu(this)), m_system("选择文件"), m_title("选择文件"), m_type("*.*"), m_dir("C:/Windows")
	{
		setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
		m_menu.addAction(&m_system);
		m_menu.setProperty("group", "context_menu");
		connect(&m_system, &QAction::triggered, this, [this] { QString file = QFileDialog::getOpenFileName(this, m_title, m_dir, m_type); if (!file.isEmpty()) QLineEdit::setText(QDir::toNativeSeparators(file)); });
	}

	void setDialog(const QString& title, const QString type, const QString dir)
	{
		m_system.setText(title);
		m_title = title;
		m_type = type;
		m_dir = dir;
	}

	void setFiles(const QList<QString>& sounds)
	{
		m_files = sounds;
		m_actions.clear();
		m_menu.clear();
		m_menu.addAction(&m_system);
		for (auto& i : m_files)
		{
			QAction* action = new QAction(i);
			m_actions.append(action);
			m_menu.addAction(action);
			connect(action, &QAction::triggered, this, [this, i] { if (!i.isEmpty()) QLineEdit::setText(i); });
		}
	}
	QList<QString> files() const
	{
		return m_files;
	}

protected:
	bool event(QEvent* event)
	{
		if (isEnabled() && (event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::MouseButtonDblClick))
		{
			QMouseEvent* mouseEvent = (QMouseEvent*)event;
			if ((event->type() == QEvent::MouseButtonRelease && mouseEvent->button() == Qt::MouseButton::RightButton) ||
				(event->type() == QEvent::MouseButtonDblClick && mouseEvent->button() == Qt::MouseButton::LeftButton))
			{
				m_menu.exec(QCursor::pos());
				return true;
			}
		}
		return QLineEdit::event(event);
	}
};