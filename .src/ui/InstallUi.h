#pragma once
#include <qevent.h>
#include <qfiledialog.h>
#include "ui_InstallUi.h"
#include "header.h"

class InstallUi : public QDialog
{
	Q_OBJECT;
	Ui::InstallUiClass ui;

	QPoint msPos;
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
		ui.etPath->setText(program + u8"\\QuickInput");
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
	void mousePressEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) msPos = et->pos(); }
	void mouseMoveEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - msPos); }

private slots:
	void OnBnClose()
	{
		exit(0);
	}

	void OnBnPath()
	{
		QString path = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, u8"安装位置", program));
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
			std::wstring desktopName = File::FileNameNrep(L"Quick input", desktopPath + L"\\*.lnk");
			File::CreateShortcut(Path::Append(desktopPath, (desktopName + L".lnk")), procPath);
		}
		if (ui.ckStart->isChecked())
		{
			std::wstring startName = File::FileNameNrep(L"Quick input", startPath + L"\\*.lnk");
			File::FolderCreate(startPath.c_str());
			File::CreateShortcut(Path::Append(startPath, (startName + L".lnk")), procPath);
		}

		MsgBox::Message(std::wstring(L"程序安装在：\n") + procPath, L"安装完成");
		Process::Start(procPath);
		exit(0);
	}
};