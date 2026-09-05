#pragma once
#include <qapplication.h>
#include <qdialog.h>
#include <qplaintextedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <windows.h>
#include <src/lang.h>
#include "QCodeEditor.h"
class QCodeDialog : public QDialog
{
	Q_OBJECT;
	const bool editable = false;
	QCodeEditor* textEdit;
	QPushButton* saveButton;
	QVBoxLayout* layout;
	QHBoxLayout* buttonLayout;
	QString text;
	bool save = false;
public:
	QCodeDialog(bool editable = false, const QString& title = QString(), const QIcon& icon = QIcon()) : editable(editable)
	{
		setWindowFlags(Qt::WindowCloseButtonHint);
		setWindowIcon(icon.isNull() ? QApplication::windowIcon() : icon);
		setGeometry(QRect(100, 100, 500, 380));
		setStyleSheet("background-color:#888;color:white");

		textEdit = new QCodeEditor(this);
		textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		textEdit->setTabStopDistance(QFontMetrics(QFont("Microsoft YaHei")).horizontalAdvance("    "));

		saveButton = new QPushButton(this);
		saveButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		saveButton->setMinimumSize(QSize(60, 24));
		saveButton->setMaximumSize(QSize(60, 24));
		saveButton->setStyleSheet("background-color:#333;color:white;border:1px solid #AAA;border-radius:4px");
		saveButton->setText(lang_trans("保存"));

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
			setWindowTitle(title.isEmpty() ? QString("CodeEdit") : title);
			textEdit->setReadOnly(false);
			saveButton->setHidden(false);
		}
		else
		{
			setWindowTitle(title.isEmpty() ? QString("CodeView") : title);
			textEdit->setReadOnly(true);
			saveButton->setHidden(true);
		}
	}
	QPlainTextEdit* edit()
	{
		return textEdit;
	}
	QPushButton* button()
	{
		return saveButton;
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
		if (editable && !save && text != textEdit->toPlainText() && MessageBoxW(nullptr, lang_trans("是否保存？").toStdWString().c_str(), L"CodeEdit", MB_YESNO) == IDYES) save = true;
	}
};