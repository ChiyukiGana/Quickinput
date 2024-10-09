#pragma execution_character_set("utf-8")
#pragma once
#include <qevent.h>
#include <qfiledialog.h>
#include "../src/minc.h"
#include "ui_InstallUi.h"

class InstallUi : public QDialog
{
	Q_OBJECT;
	Ui::InstallUiClass ui;
	QString program;
public:
	InstallUi() : QDialog()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
	}
	void WidInit()
	{
		program = QString::fromWCharArray(Path::ExpandEnvironment(EnvProgram).c_str());
		ui.etPath->setText(program + "\\QuickInput");
		ui.ckDesktop->setChecked(true);
		ui.ckStart->setChecked(true);
	}
	void WidEvent()
	{
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
		connect(ui.bnPath, SIGNAL(clicked()), this, SLOT(OnBnPath()));
		connect(ui.bnInstall, SIGNAL(clicked()), this, SLOT(OnBnInstall()));
	}
private:
	void showEvent(QShowEvent*)
	{
		SetForegroundWindow((HWND)QWidget::winId());
	}
	QPoint msPos; bool mouseDown = false; void mousePressEvent(QMouseEvent* et) { if (et->button() == Qt::LeftButton) msPos = et->pos(), mouseDown = true; et->accept(); }void mouseMoveEvent(QMouseEvent* et) { if (mouseDown) move(et->pos() + pos() - msPos); }void mouseReleaseEvent(QMouseEvent* et) { if (et->button() == Qt::LeftButton) mouseDown = false; }
private Q_SLOTS:
	void OnBnClose()
	{
		exit(0);
	}
	void OnBnPath()
	{
		QString path = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, "安装位置", program));
		if (path.size())
		{
			if (Path::PathState((wchar_t*)(path.utf16()))) path += L"\\QuickInput";
			ui.etPath->setText(path);
		}
	}
	void OnBnInstall()
	{
		std::wstring path = (wchar_t*)(ui.etPath->text().utf16());
		std::wstring procPath = Path::Append(path, L"QuickInput.exe");
		std::wstring desktopPath = System::folderPath(CSIDL_DESKTOP);
		std::wstring startPath = Path::Append(System::folderPath(CSIDL_STARTMENU), L"Programs\\QuickInput");

		File::FolderCreate(path.c_str());
		CopyFileW(Process::exePath().c_str(), procPath.c_str(), 0);

		if (ui.ckDesktop->isChecked())
		{
			std::wstring desktopFile = File::FileUnique(Path::Append(desktopPath, L"Quick input.lnk"));
			File::CreateShortcut(desktopFile, procPath);
		}
		if (ui.ckStart->isChecked())
		{
			std::wstring startFile = File::FileUnique(Path::Append(startPath, L"Quick input.lnk"));
			File::FolderCreate(startPath.c_str());
			File::CreateShortcut(startFile, procPath);
		}

		Process::Start(procPath);
		exit(0);
	}
};