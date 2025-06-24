#pragma once
#include <qdialog.h>
#include <qlineedit.h>
#include <qlayout.h>
class QEditDialog : public QDialog
{
	Q_OBJECT;
	QLineEdit* lineEdit;
public:
	QEditDialog(QWidget* parent = nullptr) : QDialog(parent)
	{
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
		lineEdit = new QLineEdit(this);
		lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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
		lineEdit->setFocus();
		lineEdit->selectAll();
		exec();
		return lineEdit->text();
	}
};