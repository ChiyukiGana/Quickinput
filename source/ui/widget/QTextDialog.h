#pragma once
#include <qdialog.h>
#include <qplaintextedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <windows.h>
class QTextDialog : public QDialog
{
	Q_OBJECT;
	const bool editable = false;
	QPlainTextEdit* textEdit;
	QPushButton* saveButton;
	QVBoxLayout* layout;
	QHBoxLayout* buttonLayout;
	QString text;
	bool save = false;
public:
	QTextDialog(bool editable = false) : editable(editable)
	{
		setWindowFlags(Qt::WindowCloseButtonHint);
		setWindowIcon(QIcon(":/icon.png"));
		setGeometry(QRect(100, 100, 500, 380));
		setStyleSheet("color:black");

		textEdit = new QPlainTextEdit(this);
		textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		saveButton = new QPushButton(this);
		saveButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		saveButton->setMinimumSize(QSize(60, 24));
		saveButton->setMaximumSize(QSize(60, 24));
		saveButton->setText("保存");
		saveButton->setStyleSheet("background-color:white;border:1px solid gray;border-radius:4px");

		layout = new QVBoxLayout(this);
		layout->setContentsMargins(8, 8, 8, 8);

		buttonLayout = new QHBoxLayout();
		buttonLayout->setContentsMargins(0, 0, 0, 0);

		buttonLayout->addItem(new QSpacerItem(2, 2, QSizePolicy::Expanding, QSizePolicy::Minimum));
		buttonLayout->addWidget(saveButton);

		layout->addWidget(textEdit);
		layout->addLayout(buttonLayout);

		if (editable)
		{
			setWindowTitle("QuickInput - TextEdit");
			textEdit->setReadOnly(false);
			saveButton->setHidden(false);
		}
		else
		{
			setWindowTitle("QuickInput - TextView");
			textEdit->setReadOnly(true);
			saveButton->setHidden(true);
		}
	}
	QPlainTextEdit* edit()
	{
		return textEdit;
	}
	QString Start(const QString& text)
	{
		this->text = text;
		textEdit->setPlainText(text);
		textEdit->setFocus();
		textEdit->selectAll();
		connect(saveButton, &QPushButton::clicked, this, [this] { save = true; close(); });
		exec();
		if (save) return textEdit->toPlainText();
		return text;
	}
	void closeEvent(QCloseEvent*)
	{
		if (editable && !save && text != textEdit->toPlainText() && MessageBoxW(nullptr, L"是否保存？", L"TextEdit", MB_YESNO) == IDYES) save = true;
	}
};