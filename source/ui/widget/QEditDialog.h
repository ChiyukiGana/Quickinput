#pragma once
#include <qdialog.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtimer.h>
class QEditDialog : public QDialog
{
	Q_OBJECT;
	QLineEdit* lineEdit;
public:
	QEditDialog(QWidget* parent = nullptr) : QDialog(parent)
	{
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
		setFocusPolicy(Qt::StrongFocus);
		lineEdit = new QLineEdit(this);
		lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		lineEdit->setAlignment(Qt::AlignCenter);
		lineEdit->setFocusPolicy(Qt::StrongFocus);
		QHBoxLayout* layout = new QHBoxLayout(this);
		layout->addWidget(lineEdit);
		layout->setContentsMargins(0, 0, 0, 0);
		connect(lineEdit, &QLineEdit::returnPressed, this, [this]() { close(); });
	}
	QLineEdit* edit()
	{
		return lineEdit;
	}
	QString Start(const QString& text)
	{
		lineEdit->setText(text);
		lineEdit->selectAll();
		QTimer::singleShot(50, this, [this]() {
			lineEdit->setFocus();
			activateWindow();
		});
		exec();
		return lineEdit->text();
	}
};