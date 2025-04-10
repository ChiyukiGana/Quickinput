#pragma once
#include "UpdateUi.h"
#include "ui_AboutUi.h"
#include <src/inc_header.h>
#include <src/update.h>
class AboutUi : public QWidget
{
	Q_OBJECT;
	Ui::AboutUiClass ui;
	QiUpdate* update;
	QString version_res;
	std::string version, content;
public:
	AboutUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		version_res = ui.version_label->text();
		update = new QiUpdate(this, version_res);
		ui.url_label->installEventFilter(this);
		ui.license_label->installEventFilter(this);
		if (update->good()) update->getlatest();
	}
	QString Version() const
	{
		return version_res;
	}
private:
	bool eventFilter(QObject* obj, QEvent* e)
	{
		if (e->type() == QEvent::MouseButtonRelease)
		{
			if (obj == ui.url_label) ShellExecuteW(nullptr, L"open", L"http://qinput.cyk.moe", nullptr, nullptr, SW_HIDE);
			else if (obj == ui.version_label)
			{
				Qi::widget.dialogActive = true;
				UpdateUi u(version.c_str(), content.c_str());
				Qi::widget.dialogActive = false;
			}
			else if (obj == ui.license_label)
			{
				Qi::widget.dialogActive = true;
				MessageBoxW((HWND)Qi::widget.main->winId(), LR"(软件许可证

软件：Quickinput 键盘鼠标宏
版权所属：©2023-2025 ChiyukiGana

以下内容中将 "Quickinput键盘鼠标宏" 称为 "此软件"

--------------------

【非商用】
允许：
	1. 引用代码
	2. 修改软件
	3. 分发软件

必要：
	1. 软件页面版权信息：©2023-2025 ChiyukiGana
	2. 本许可证或软件关于页面的许可证
	3. 若使用了源代码需要包含GPL3.0许可证
	4. 若修改了此软件则需要保持开源

禁止：
	1. 违法犯罪
	2. 商业用途

遵循：
	1. 本许可证
	2. GPL3.0许可证

用户可以自由使用/分发/传播此软件本体和附属文件（配置文件，宏，样式表），若使用了源代码或修改了软件需要保持开源。

--------------------

【商用】
允许：
	1. 引用代码
	2. 修改软件
	3. 分发软件

必要：
	1. 软件页面版权信息：©2023-2025 ChiyukiGana
	2. 本许可证或软件关于页面的许可证

禁止：
	1. 违法犯罪

遵循：
	1. 本许可证
	2. 《中华人民共和国著作权法》

无论是通过直接或间接获取到本软件副本，并进行分发/销售此软件本体或附属文件（配置文件，宏，样式表）的商业行为需要取得作者授权，并遵守《中华人民共和国著作权法》，购买得到的二次分发/销售的软件副本作为商用时仍然需要取得授权。

--------------------

若违反非商用或商用许可条款所规定的事项，作者将根据《中华人民共和国著作权法》追其究法律责任。)", L"许可证", MB_TOPMOST);
				Qi::widget.dialogActive = false;
			}
			e->accept();
		}
		return QWidget::eventFilter(obj, e);
	}
	void customEvent(QEvent* e)
	{
		if (update->check(version, content))
		{
			ui.version_label->setText(ui.version_label->text() + "（有新版本）");
			ui.version_label->setCursor(QCursor(Qt::CursorShape::WhatsThisCursor));
			ui.version_label->installEventFilter(this);
		}
	}
};