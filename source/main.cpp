#include <ui/MainUi.h>
#include <src/inc_header.h>
void Init();
int main(int argc, char* argv[])
{
	std::locale::global(std::locale(".UTF8")); // set utf8 for all std streams
	Process::RunPath(); // reset work path to exe path
	std::wstring mutex = Path::toSlash(Process::runPath()); // mutex name, the current directory is only running one
	if (Process::isRunning(mutex.c_str())) // if running
	{
		MsgBox::Warning(L"当前文件夹的程序已经运行，若运行更多程序请复制此文件夹", L"提示");
		return -1;
	}
	CreateMutexW(0, 0, mutex.c_str()); // create mutex
	Init(); // json, font, style
	QApplication application(argc, argv); // qt event loop
	Qi::application = &application; // save to global
	Qi::popText = new QPopText; // popup text on screen of global widget
	Qi::popText->setPosition(Qi::ui.pop.p.x, Qi::ui.pop.p.y);
	Qi::popText->setSize(Qi::ui.pop.size);
	Qi::windowSelection = new QWindowSelection; // select a window of global widget
	MainUi mainWindow;
	application.exec();
	return 0;
}
void Init()
{
	QiJson::LoadJson(); // config, macro
	Qi::screen = System::screenSize(); // screen pixel size
	qputenv("QT_SCALE_FACTOR", QByteArray::number(System::screenScaleRote(), 10, 1)); // enlarge high resolution screen
	if ("symbol")
	{
		if (System::Version().dwMajorVersion >= 10)
		{
			Qi::ui.text.syOn = "✅";
			Qi::ui.text.syOff = "⛔";
			Qi::ui.text.syOk = "⭕";
			Qi::ui.text.syYes = "✔️";
			Qi::ui.text.syNot = "❌";
			Qi::ui.text.syStop = "🛑";
			Qi::ui.text.syShow = "🔼";
			Qi::ui.text.syHide = "🔽";
			Qi::ui.text.syOption = "⚙";
			Qi::ui.text.syLink = "🔗";
			Qi::ui.text.syEdit = "🔧";
			Qi::ui.text.syUp = "⬆️";
			Qi::ui.text.syDown = "⬇️";
			Qi::ui.text.syTurn = "🔃";
			Qi::ui.text.syLeft = "🔙";
			Qi::ui.text.syTop = "🔝";
			Qi::ui.text.syRight = "🔜";
			Qi::ui.text.syMove = "🔛";
			Qi::ui.text.syTime = "⏳";
			Qi::ui.text.syText = "🅰️";
			Qi::ui.text.syLoop = "♾️";
			Qi::ui.text.syColor = "🌈";
			Qi::ui.text.syImage = "🖼";
			Qi::ui.text.syPoint = "🪂";
			Qi::ui.text.syJump = "🛩";
			Qi::ui.text.syBlock = "🪂";
			Qi::ui.text.syBlockExec = "🛩";
		}
		else
		{
			Qi::ui.text.syOn = "✔";
			Qi::ui.text.syOff = "✘";
			Qi::ui.text.syOk = "✔";
			Qi::ui.text.syYes = "✔";
			Qi::ui.text.syNot = "✘";
			Qi::ui.text.syStop = "Ⓢ";
			Qi::ui.text.syShow = "▲";
			Qi::ui.text.syHide = "▼";
			Qi::ui.text.syOption = "✱";
			Qi::ui.text.syLink = "※";
			Qi::ui.text.syEdit = "▲";
			Qi::ui.text.syUp = "↑";
			Qi::ui.text.syDown = "↓";
			Qi::ui.text.syTurn = "◈";
			Qi::ui.text.syLeft = "←";
			Qi::ui.text.syTop = "↑";
			Qi::ui.text.syRight = "→";
			Qi::ui.text.syMove = "↘";
			Qi::ui.text.syTime = "☽";
			Qi::ui.text.syText = "A";
			Qi::ui.text.syLoop = "↩";
			Qi::ui.text.syColor = "☀";
			Qi::ui.text.syImage = "☀";
			Qi::ui.text.syPoint = "☟";
			Qi::ui.text.syJump = "✈";
			Qi::ui.text.syBlock = "☟";
			Qi::ui.text.syBlockExec = "✈";
		}
	}
	if ("text")
	{
		// menu
		Qi::ui.text.muOn = (QString::fromUtf8("启用") + Qi::ui.text.syOn); 
		Qi::ui.text.muOff = (QString::fromUtf8("禁用") + Qi::ui.text.syOff);
		Qi::ui.text.muShow = (QString::fromUtf8("显示") + Qi::ui.text.syShow);
		Qi::ui.text.muHide = (QString::fromUtf8("隐藏") + Qi::ui.text.syHide);
		Qi::ui.text.muExit = (QString::fromUtf8("退出") + Qi::ui.text.syStop);
		// action
		Qi::ui.text.acDown = (QString::fromUtf8("按下") + Qi::ui.text.syDown);
		Qi::ui.text.acUp = (QString::fromUtf8("松开") + Qi::ui.text.syUp);
		Qi::ui.text.acClick = (QString::fromUtf8("点击") + Qi::ui.text.syTurn);
		Qi::ui.text.acPos = (QString::fromUtf8("位置") + Qi::ui.text.syLeft);
		Qi::ui.text.acMove = (QString::fromUtf8("移动") + Qi::ui.text.syMove);
		Qi::ui.text.acWait = (QString::fromUtf8("等待") + Qi::ui.text.syTime);
		Qi::ui.text.acText = (QString::fromUtf8("复制") + Qi::ui.text.syText);
		Qi::ui.text.acLoop = (QString::fromUtf8("循环") + Qi::ui.text.syLoop);
		Qi::ui.text.acColor = (QString::fromUtf8("找色") + Qi::ui.text.syColor);
		Qi::ui.text.acEnd = (QString::fromUtf8("结束") + Qi::ui.text.syStop);
		Qi::ui.text.acEndLoop = (QString::fromUtf8("结束循环") + Qi::ui.text.syStop);
		Qi::ui.text.acKeyState = (QString::fromUtf8("按键状态") + Qi::ui.text.syStop);
		Qi::ui.text.acRecoverPos = (QString::fromUtf8("恢复位置") + Qi::ui.text.syTurn);
		Qi::ui.text.acImage = (QString::fromUtf8("找图") + Qi::ui.text.syImage);
		Qi::ui.text.acPopText = (QString::fromUtf8("弹出") + Qi::ui.text.syText);
		Qi::ui.text.acRememberPos = (QString::fromUtf8("记录位置") + Qi::ui.text.syTurn);
		Qi::ui.text.acTimer = (QString::fromUtf8("定时") + Qi::ui.text.syLoop);
		Qi::ui.text.acJump = (QString::fromUtf8("跳转") + Qi::ui.text.syJump);
		Qi::ui.text.acJumpPoint = (QString::fromUtf8("锚点") + Qi::ui.text.syPoint);
		Qi::ui.text.acDialog= (QString::fromUtf8("对话框") + Qi::ui.text.syText);
		Qi::ui.text.acBlock = (QString::fromUtf8("块") + Qi::ui.text.syBlock);
		Qi::ui.text.acBlockExec = (QString::fromUtf8("执行") + Qi::ui.text.syBlockExec);
		Qi::ui.text.acQuickInput= (QString::fromUtf8("输入字符") + Qi::ui.text.syText);
		Qi::ui.text.acKeyBlock= (QString::fromUtf8("屏蔽按键") + Qi::ui.text.syStop);
		// state
		Qi::ui.text.trOn = (QString::fromUtf8("启用") + Qi::ui.text.syOn);
		Qi::ui.text.trOff = (QString::fromUtf8("禁用") + Qi::ui.text.syOff);
		// edit
		Qi::ui.text.etFunc = (QString::fromUtf8("动作") + Qi::ui.text.syOption);
		Qi::ui.text.etParam = (QString::fromUtf8("参数") + Qi::ui.text.syLink);
		Qi::ui.text.etAdd = (QString::fromUtf8("添加") + Qi::ui.text.syYes);
		Qi::ui.text.etDel = (QString::fromUtf8("删除") + Qi::ui.text.syNot);
		Qi::ui.text.etChange = (QString::fromUtf8("修改") + Qi::ui.text.syOk);
		Qi::ui.text.etEdit = (QString::fromUtf8("编辑") + Qi::ui.text.syEdit);
		// record
		Qi::ui.text.rcStart = (QString::fromUtf8("开始") + Qi::ui.text.syOk);
		Qi::ui.text.rcStop = (QString::fromUtf8("停止") + Qi::ui.text.syOk);
		Qi::ui.text.rcClose = (QString::fromUtf8("取消") + Qi::ui.text.syNot);
	}
	if ("style")
	{
		QiUi::Theme theme;
		theme.name = "浅蓝";
		theme.style = R"(*{border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"],*[group="context_menu"]{border:1px solid gray}*[group="title"]{background-color:white;font-size:14px}*[group="client"]{background-color:#CEF}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QToolTip{background-color:white}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#ADE}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#ACF;color:black}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#CEF;color:black}*[group="table"]::item:selected{background-color:#ADF;color:black}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		Qi::ui.themes.append(theme);
		theme.name = "糖果";
		theme.style = R"(*{border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"],*[group="context_menu"]{border:1px solid gray}*[group="title"]{background-color:white;font-size:14px}*[group="client"]{background-color:#FDF;background-image:url(:/candy.png)}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QToolTip{background-color:#FFF5FF}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#FCE}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#FBE;color:black}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#FDF;color:black}*[group="table"]::item:selected{background-color:#FCE;color:black}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		Qi::ui.themes.append(theme);
		theme.name = "橘子";
		theme.style = R"(*{border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"],*[group="context_menu"]{border:1px solid gray}*[group="title"]{background-color:white;font-size:14px}*[group="client"]{background-color:#FEC;background-image:url(:/neko.png)}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QToolTip{background-color:#FFFFF5}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#FDA}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#FFD0B0;color:black}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#FEC;color:black}*[group="table"]::item:selected{background-color:#FFD0B0;color:black}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		Qi::ui.themes.append(theme);
		theme.name = "四叶草";
		theme.style = R"(*{border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"],*[group="context_menu"]{border:1px solid gray}*[group="title"]{background-color:white;font-size:14px}*[group="client"]{background-color:#AED;background-image:url(:/blade.png);image-position:bottom right}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QToolTip{background-color:#F5FFFF}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#9DC}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#8EB;color:black}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#AED;color:black}*[group="table"]::item:selected{background-color:#A5E5D5;color:black}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		Qi::ui.themes.append(theme);
		theme.name = "黑暗";
		theme.style = R"(*{color:white;border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"],*[group="context_menu"]{border:1px solid gray}*[group="title"]{background-color:black;font-size:14px}*[group="client"]{background-color:#333}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QTableWidget QLineEdit,QToolTip{background-color:#555}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#666}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#777;color:white}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#333;color:white}*[group="table"]::item:selected{background-color:#888;color:white}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		Qi::ui.themes.append(theme);
	}
	if ("custom style")
	{
		QFileInfoList files = File::Find("style", "*.css");
		for (const QFileInfo& file : files)
		{
			QByteArray text;
			File::LoadText(file.filePath(), text);
			if (!text.isEmpty())
			{
				QiUi::Theme theme;
				theme.name = file.baseName();
				theme.style = text;
				Qi::ui.themes.append(theme);
			}
		}
	}
	if (Qi::set.theme >= Qi::ui.themes.size()) Qi::set.theme = 0;
}