#pragma once
#include <src/inc_header.h>
#include "ui_InstallUi.h"
class InstallUi : public QDialog
{
	Q_OBJECT;
	using This = InstallUi;
	Ui::InstallUiClass ui;
	QString program;
public:
	InstallUi()
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		Init();
		Event();
	}
	void Init()
	{
		ui.path_edit->setText(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + "/" + "QuickInput");
		ui.desktop_check->setChecked(true);
		ui.start_check->setChecked(true);
	}
	void Event()
	{
		connect(ui.title_close_button, &QPushButton::clicked, this, [this]{ exit(0); });
		connect(ui.path_button, &QPushButton::clicked, this, &This::OnBnPath);
		connect(ui.install_button, &QPushButton::clicked, this, &This::OnBnInstall);
	}
private:
	void showEvent(QShowEvent*)
	{
		SetForegroundWindow((HWND)QWidget::winId());
	}
	// window move
	QPoint mouse_positon;
	bool mouse_down = false;
	void mousePressEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			mouse_positon = e->pos();
			mouse_down = true;
			e->accept();
		}
	}
	void mouseMoveEvent(QMouseEvent* e)
	{
		if (mouse_down)
		{
			if (Distance(mouse_positon.x(), mouse_positon.y(), e->pos().x(), e->pos().y()) < 100)
			{
				move(e->pos() + pos() - mouse_positon);
				e->accept();
			}
		}
	}
	void mouseReleaseEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::LeftButton)
		{
			mouse_down = false;
			e->accept();
		}
	}
private Q_SLOTS:
	void OnBnPath()
	{
		QString path = QFileDialog::getExistingDirectory(this, "安装位置", program);
		if (path.size()) ui.path_edit->setText(path);
	}
	void OnBnInstall()
	{
		QString name = "/Quickinput.exe";
		QString resDir = ui.path_edit->text();
		QString desktopDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
		QString startDir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);

		if (!QDir(resDir).exists() && !QDir(resDir).mkdir(resDir)) MsgBox::Error(L"创建目录失败"), exit(0);

		QString resFile = resDir + name;
		if (QFile::exists(resFile)) QFile::remove(resFile);
		if (!QFile::copy(QApplication::applicationFilePath(), resFile)) MsgBox::Error(L"创建文件失败"), exit(0);

		if (ui.desktop_check->isChecked())
		{
			QString desktopFile = desktopDir + "/" + File::Unique(desktopDir, "Quickinput", ".lnk");
			if (!QFile::link(resFile, desktopFile)) MsgBox::Error(L"创建快捷方式失败"), exit(0);
		}
		if (ui.start_check->isChecked())
		{
			if (!QDir(startDir).exists() && !QDir(startDir).mkdir(startDir)) MsgBox::Error(L"创建目录失败"), exit(0);
			QString startFile = startDir + "/" + File::Unique(startDir, "Quickinput", ".lnk");
			if (!QFile::link(resFile, startFile)) MsgBox::Error(L"创建快捷方式失败"), exit(0);
		}
		ShellExecuteW(NULL, L"open", (LPCWSTR)resFile.utf16(), NULL, NULL, SW_SHOW);
		exit(0);
	}
};