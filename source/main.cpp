#pragma execution_character_set("utf-8")
#include "ui/MainUi.h"
#include "ui/InstallUi.h"
#include "src/minc.h"

void Init();
void Install();

int main(int argc, char* argv[])
{
	// set utf8 for all streams
	std::locale::global(std::locale(".UTF8"));

	// reset work path to exe path
	Process::RunPath();

	// mutex
	std::wstring mutex = Path::toSlash(Process::runPath());
	if (Process::isRunning(mutex.c_str())) { MsgBox::Warning(L"当前文件夹的程序已经运行，若运行更多程序请复制此文件夹", L"提示"); return -1; }
	CreateMutexW(0, 0, mutex.c_str());

	// json, font, style
	Init();

	QApplication application(argc, argv); 
	Qi::application = &application;
	Install();

	Qi::popText = new QPopText;
	Qi::popText->setPosition(Qi::ui.pop.p.x, Qi::ui.pop.p.y);
	Qi::popText->setSize(Qi::ui.pop.size);

	MainUi mainWindow;
	application.exec();
	return 0;
}

void Init()
{
	if ("sample style")
	{
		std::wstring path = Path::Append(Process::runPath(), L"style");
		if (!File::FolderState(path))
		{
			File::FolderCreate(L"style");
			File::TextSaveU(L"style\\style属性说明.css", R"(/* 选择框图标 */
*[group="check"] {
    image: url(:/checked.png)
}

/* 选择列表图标 */
*[group="combo"] {
    image: url(:/arrow.png);
}

/************window*************/

/* 窗口 */
*[group="frame"] {}

/* 标题区 */
*[group="title"] {}

/* 内容区 */
*[group="client"] {}

/************title button*************/

/* 标题区关闭按钮 */
*[group="title-close_button"] {}

/* 标题区隐藏按钮 */
*[group="title-hide_button"] {}

/* 标题区最小化按钮 */
*[group="title-min_button"] {}

/* 标题区运行按钮 */
*[group="title-run_button"] {}

/************button*************/

/* 获取坐标，范围，窗口按钮 */
*[group="get_button"] {}

/* 宏窗口按钮 */
*[group="macro-button"] {}

/* 设置窗口按钮 */
*[group="settings-button"] {}

/* 编辑窗口添加按钮 */
*[group="edit-add_button"] {}

/* 编辑窗口编辑按钮 */
*[group="edit-edit_button"] {}

/* 录制窗口按钮 */
*[group="record-button"] {}

/************select*************/

/* 选择按钮 */
*[group="check"] {}

/* 选择按钮图标 */
*[group="check"]::indicator {}

/* 单选按钮 */
*[group="radio"] {}

/* 单选按钮图标 */
*[group="radio"]::indicator {}

/************combo*************/

/* 选项列表 */
*[group="combo"] {}

/* 选择列表图标框 */
*[group="combo"]::drop-down {}

/* 选择列表图标 */
*[group="combo"]::down-arrow {}

/************edit*************/

/* 单行编辑框 */
*[group="line_eidt"] {}

/* 文本编辑框 */
*[group="text_edit"] {}

/* 按键编辑框 */
*[group="key_edit"] {}

/* 右键菜单 */
*[group="context_menu"] {}

/* 悬停提示 */
*[group="context_tip"] {}

/************tab_widget*************/

/* 标签Box */
*[group="tab_widget"] {}

/* 标签Box首标签 */
*[group="tab_widget_bar"]::tab:first {}

/* 标签Box中标签 */
*[group="tab_widget_bar"]::tab {}

/* 标签Box尾标签 */
*[group="tab_widget_bar"]::tab:last {}

/************tool*************/

/* 列标签Box */
*[group="ltab_widget"] {}

/* 列标签Box按钮 */
*[group="ltab_widget"]::tab {}

/************table*************/

/* 列表 */
*[group="table"] {}

/* 列表头 */
*[group="table_header"] {}

/* 列表项目 */
*[group="table"]::item {}

/* 列表项目悬停 */
*[group="table"]::item:hover {}

/* 列表项目选中 */
*[group="table"]::item:selected {}

/* 列表项目编辑 */
QTableWidget QLineEdit {})");
			File::TextSaveU(L"style\\style示例.css", R"(* {
    border: none;
    font-size: 12px;
    font-family: "Microsoft YaHei"
}

/* window */
*[group="frame"],
*[group="context_menu"] {
    border:1px solid gray
}
*[group="title"] {
    background-color: white;
    font-size: 14px
}
*[group="client"] {
    background-color: #CEF
}

/* normal */
*[group="get_button"],
*[group="check"]::indicator,
*[group="radio"]::indicator,
*[group="combo"],
*[group="context_menu"],
*[group="context_tip"],
*[group="line_edit"],
*[group="text_edit"],
*[group="key_edit"],
*[group="table"],
*[group="table_header"] {
    background-color: white
}

/* normal */
*[group="macro-button"],
*[group="settings-button"],
*[group="edit-add_button"],
*[group="edit-edit_button"],
*[group="record-button"],
*[group="tab_widget_bar"]::tab:first,
*[group="tab_widget_bar"]::tab,
*[group="tab_widget_bar"]::tab:last,
*[group="ltab_widget"]::tab {
    background-color: #ADE
}

/* hover */
*[group="get_button"]:hover,
*[group="macro-button"]:hover,
*[group="settings-button"]:hover,
*[group="edit-add_button"]:hover,
*[group="edit-edit_button"]:hover,
*[group="record-button"]:hover,
*[group="check"]::indicator:hover,
*[group="radio"]::indicator:hover,
*[group="combo"]::item:selected,
*[group="context_menu"]:selected,
*[group="tab_widget_bar"]::tab:first:hover,
*[group="tab_widget_bar"]::tab:hover,
*[group="tab_widget_bar"]::tab:last:hover,
*[group="table"]::item:hover {
    background-color: #ACF;
	color: black
}

/* selected */
*[group="tab_widget_bar"]::tab:first:selected,
*[group="tab_widget_bar"]::tab:selected,
*[group="tab_widget_bar"]::tab:last:selected {
    background-color: #CEF
}

/* selected */
*[group="table"]::item:selected {
    background-color: #ADF;
    color: black
}

/* checked */
*[group="check"]::indicator:checked,
*[group="radio"]::indicator:checked {
    image: url(:/checked.png)
}

/* disabled */
*[group="get_button"]:disabled,
*[group="macro-button"]:disabled,
*[group="settings-button"]:disabled,
*[group="edit-add_button"]:disabled,
*[group="edit-edit_button"]:disabled,
*[group="record-button"]:disabled,
*[group="check"]:disabled,
*[group="radio"]:disabled,
*[group="combo"]:disabled,
*[group="context_menu"]::item:disabled,
*[group="line_edit"]:disabled,
*[group="text_edit"]:disabled,
*[group="key_edit"]:disabled {
    color:gray
}

/* 控件大小 */
*[group="check"]::indicator,
*[group="radio"]::indicator {
    width: 20px;
    height: 20px
})");
		}
	}

	// config, macro
	QiJson::LoadJson();

	// screen pixel
	Qi::screen = System::screenSize();

	// enlarge high resolution screen
	if (!Qi::set.scaleBlock) qputenv("QT_SCALE_FACTOR", QByteArray::number(System::screenZoomRote(), 10, 1));

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
		}
	}

	if ("text")
	{
		Qi::ui.text.muOn = (QString::fromUtf8("启用") + Qi::ui.text.syOn);
		Qi::ui.text.muOff = (QString::fromUtf8("禁用") + Qi::ui.text.syOff);
		Qi::ui.text.muShow = (QString::fromUtf8("显示") + Qi::ui.text.syShow);
		Qi::ui.text.muHide = (QString::fromUtf8("隐藏") + Qi::ui.text.syHide);
		Qi::ui.text.muExit = (QString::fromUtf8("退出") + Qi::ui.text.syStop);
		
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
		Qi::ui.text.acBlock = (QString::fromUtf8("块") + Qi::ui.text.syText);
		Qi::ui.text.acBlockExec = (QString::fromUtf8("执行") + Qi::ui.text.syText);

		Qi::ui.text.trOn = (QString::fromUtf8("启用") + Qi::ui.text.syOn);
		Qi::ui.text.trOff = (QString::fromUtf8("禁用") + Qi::ui.text.syOff);

		Qi::ui.text.etFunc = (QString::fromUtf8("动作") + Qi::ui.text.syOption);
		Qi::ui.text.etParam = (QString::fromUtf8("参数") + Qi::ui.text.syLink);
		Qi::ui.text.etAdd = (QString::fromUtf8("添加") + Qi::ui.text.syYes);
		Qi::ui.text.etDel = (QString::fromUtf8("删除") + Qi::ui.text.syNot);
		Qi::ui.text.etChange = (QString::fromUtf8("修改") + Qi::ui.text.syOk);
		Qi::ui.text.etEdit = (QString::fromUtf8("编辑") + Qi::ui.text.syEdit);

		Qi::ui.text.rcStart = (QString::fromUtf8("开始") + Qi::ui.text.syOk);
		Qi::ui.text.rcStop = (QString::fromUtf8("停止") + Qi::ui.text.syOk);
		Qi::ui.text.rcClose = (QString::fromUtf8("取消") + Qi::ui.text.syNot);
	}

	if ("style")
	{
		QiUi::Theme theme;
		theme.name = "浅蓝";
		theme.style = R"(*{border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"],*[group="context_menu"]{border:1px solid gray}*[group="title"]{background-color:white;font-size:14px}*[group="client"]{background-color:#CEF}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QToolTip{background-color:white}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#ADE}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#ACF;color:black}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#CEF;color:black}*[group="table"]::item:selected{background-color:#ADF;color:black}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		Qi::ui.themes.Add(theme);

		theme.name = "糖果";
		theme.style = R"(*{border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"],*[group="context_menu"]{border:1px solid gray}*[group="title"]{background-color:white;font-size:14px}*[group="client"]{background-color:#FDF;background-image:url(:/candy.png)}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QToolTip{background-color:#FFF5FF}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#FCE}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#FBE;color:black}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#FDF;color:black}*[group="table"]::item:selected{background-color:#FCE;color:black}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		Qi::ui.themes.Add(theme);

		theme.name = "橘子";
		theme.style = R"(*{border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"],*[group="context_menu"]{border:1px solid gray}*[group="title"]{background-color:white;font-size:14px}*[group="client"]{background-color:#FEC;background-image:url(:/neko.png)}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QToolTip{background-color:#FFFFF5}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#FDA}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#FFD0B0;color:black}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#FEC;color:black}*[group="table"]::item:selected{background-color:#FFD0B0;color:black}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		Qi::ui.themes.Add(theme);

		theme.name = "四叶草";
		theme.style = R"(*{border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"],*[group="context_menu"]{border:1px solid gray}*[group="title"]{background-color:white;font-size:14px}*[group="client"]{background-color:#AED;background-image:url(:/blade.png);image-position:bottom right}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QToolTip{background-color:#F5FFFF}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#9DC}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#8EB;color:black}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#AED;color:black}*[group="table"]::item:selected{background-color:#A5E5D5;color:black}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		Qi::ui.themes.Add(theme);

		theme.name = "黑暗";
		theme.style = R"(*{color:white;border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"],*[group="context_menu"]{border:1px solid gray}*[group="title"]{background-color:black;font-size:14px}*[group="client"]{background-color:#333}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QTableWidget QLineEdit,QToolTip{background-color:#555}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#666}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#777;color:white}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#333;color:white}*[group="table"]::item:selected{background-color:#888;color:white}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		Qi::ui.themes.Add(theme);
	}

	if ("custom style")
	{
		File::FileList fl = File::FindFile(L"style\\*.css");
		for (size_t i = 0; i < fl.size(); i++)
		{
			if (String::Compare(fl.at(i).name, L"style属性说明.css") == 0 || String::Compare(fl.at(i).name, L"style示例.css") == 0) continue;
			QiUi::Theme theme;
			theme.name = QString::fromWCharArray(Path::RemoveExtension(fl.at(i).name).c_str());
			theme.style = QString::fromUtf8(File::TextReadU(Path::Append(L"style", fl.at(i).name)).c_str());
			Qi::ui.themes.Add(theme);
		}
	}

	if (Qi::set.theme >= Qi::ui.themes.size()) Qi::set.theme = 0;
}
void Install()
{
	std::wstring path = Process::runPath();
	if (path.find(L"AppData\\Local\\Temp") != std::wstring::npos)
	{
		if (MsgBox::Message(L"这样将无法保存数据，请解压后使用。\n\n您也可以安装此程序，点击Yes安装程序。", L"您可能在压缩包内打开了程序", MB_ICONINFORMATION | MB_YESNO) == IDYES)
		{
			InstallUi install;
			install.exec();
		}
	}
}