#pragma once
#include "UpdateUi.h"
#include "ui_AboutUi.h"
#include <src/inc_header.h>
#include <src/update.h>
class AboutUi : public QWidget
{
	Q_OBJECT;
	Ui::AboutUiClass ui;
	QiUpdate update;
	std::string version, content;
public:
	AboutUi(QWidget* parent) : QWidget(parent), update(this, 20241130, 1)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		ui.url_label->installEventFilter(this);
		ui.license_label->installEventFilter(this);
		StyleGroup();
		if (!update.good()) return;
		update.getlatest();
	}
	void StyleGroup()
	{
		ui.content_widget->setProperty("group", "client");
	}
private:
	bool eventFilter(QObject* obj, QEvent* e)
	{
		if (e->type() == QEvent::MouseButtonRelease)
		{
			if (obj == ui.url_label) QDesktopServices::openUrl(QUrl("http://qinput.cyk.moe"));
			else if (obj == ui.version_label)
			{
				Qi::widget.dialogActive = true;
				UpdateUi u(version.c_str(), content.c_str());
				Qi::widget.dialogActive = false;
			}
			else if (obj == ui.license_label)
			{
				Qi::widget.dialogActive = true;
				MessageBoxW((HWND)Qi::widget.main->winId(), LR"(软件授权许可证

软件：Quickinput 键盘鼠标宏

版权所属© 2023-2024 ChiyukiGana


以下内容中将 “Quickinput 键盘鼠标宏” 称为 "此软件"

----------------------------------------

1：非商用
允许：引用代码，分发
必要：包含作者信息，GPL3.0许可证（项目：文件），本许可证（软件：文件/软件内部）
禁止：商业用途，违法犯罪用途
遵循：GPL3.0许可证

用户可以自由使用/分发/传播此软件本体和附属文件（配置文件，宏，样式表），
如果使用源代码则需要遵循GPL3.0许可证将项目开源。

----------------------------------------

2：商用
允许：引用代码，修改，分发
必要：包含作者信息，本许可证（软件：文件/软件内部）
禁止：违法犯罪用途
遵循：《中华人民共和国著作权法》

无论是通过直接或间接获取到本软件副本，
并进行分发/销售此软件本体或附属文件（配置文件，宏，样式表），
需要取得作者授权，并遵守《中华人民共和国著作权法》，
购买得到的二次分发/销售的软件副本作为商用时仍然需要取得授权。

----------------------------------------

若违反非商用或商用许可条款所规定的事项，作者将根据《中华人民共和国著作权法》追其究法律责任)", L"许可证", MB_TOPMOST);
				Qi::widget.dialogActive = false;
			}
			e->accept();
		}
		return QWidget::eventFilter(obj, e);
	}
	void customEvent(QEvent* e)
	{
		if (update.check(version, content))
		{
			ui.version_label->setText(ui.version_label->text() + "（有新版本）");
			ui.version_label->setCursor(QCursor(Qt::CursorShape::WhatsThisCursor));
			ui.version_label->installEventFilter(this);
		}
	}
};