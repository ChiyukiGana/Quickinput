#pragma execution_character_set("utf-8")
#include "ui/MainUi.h"
#include "ui/InstallUi.h"
#include "src/minc.h"

QuickInputStruct qis;

void Init();
void Install();

int main(int argc, char* argv[])
{
	// set utf8 for all streams
	std::locale::global(std::locale(".UTF8"));

	// reset work path to exe path
	Process::RunPath();

	// mutex
	std::wstring mutex = Path::toSlash(Process::runPath()); // convert \ to / to support  mutex name
	if (Process::isRunning(mutex.c_str())) { MsgBox::Warning(L"当前文件夹的程序已经运行，若运行更多程序请复制此文件夹", L"提示"); return -1; }
	CreateMutexW(0, 0, mutex.c_str()); // if this Quick input is not running: create mutex

	Init();

	QApplication application(argc, argv);
	application.setFont(QFont("Microsoft YaHei"));

	Install();

	MainUi mainWindow;
	application.exec();

	return 0;
}

void Init()
{
	// default style
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
*[group="table"]::item:selected {})");
		File::TextSaveU(L"style\\style示例.css", R"(* {
    border: none;
    font-size: 12px;
    font-family: "Microsoft YaHei"
}

/* window */
*[group="frame"] {
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
    background-color: #ACF
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

	// config, macro
	QiJson::LoadJson();

	// screen pixel
	qis.screen = System::screenSize();

	// enlarge high resolution screen
	if (!qis.set.zoomBlock && qis.screen.cy > 1200) qputenv("QT_SCALE_FACTOR", QByteArray::number(System::screenZoomRote(), 10, 1));

	// popbox thread
	PopBox::Init();

	// symbol
	{
		if (System::Version().dwMajorVersion >= 10)
		{
			qis.ui.text.syOn = "✅";
			qis.ui.text.syOff = "⛔";
			qis.ui.text.syOk = "⭕";
			qis.ui.text.syYes = "✔️";
			qis.ui.text.syNot = "❌";
			qis.ui.text.syStop = "🛑";
			qis.ui.text.syShow = "🔼";
			qis.ui.text.syHide = "🔽";
			qis.ui.text.syOption = "⚙";
			qis.ui.text.syLink = "🔗";
			qis.ui.text.syEdit = "🔧";
			qis.ui.text.syUp = "⬆️";
			qis.ui.text.syDown = "⬇️";
			qis.ui.text.syTurn = "🔃";
			qis.ui.text.syLeft = "🔙";
			qis.ui.text.syTop = "🔝";
			qis.ui.text.syRight = "🔜";
			qis.ui.text.syMove = "🔛";
			qis.ui.text.syTime = "⏳";
			qis.ui.text.syText = "🅰️";
			qis.ui.text.syLoop = "♾️";
			qis.ui.text.syColor = "🌈";
			qis.ui.text.syImage = "🖼";
		}
		else
		{
			qis.ui.text.syOn = "✔";
			qis.ui.text.syOff = "✘";
			qis.ui.text.syOk = "✔";
			qis.ui.text.syYes = "✔";
			qis.ui.text.syNot = "✘";
			qis.ui.text.syStop = "Ⓢ";
			qis.ui.text.syShow = "▲";
			qis.ui.text.syHide = "▼";
			qis.ui.text.syOption = "✱";
			qis.ui.text.syLink = "※";
			qis.ui.text.syEdit = "▲";
			qis.ui.text.syUp = "↑";
			qis.ui.text.syDown = "↓";
			qis.ui.text.syTurn = "◈";
			qis.ui.text.syLeft = "←";
			qis.ui.text.syTop = "↑";
			qis.ui.text.syRight = "→";
			qis.ui.text.syMove = "↘";
			qis.ui.text.syTime = "☽";
			qis.ui.text.syText = "A";
			qis.ui.text.syLoop = "↩";
			qis.ui.text.syColor = "☀";
			qis.ui.text.syImage = "☀";
		}
	}

	// text
	{
		qis.ui.text.muOn = (QString::fromUtf8("启用") + qis.ui.text.syOn);
		qis.ui.text.muOff = (QString::fromUtf8("禁用") + qis.ui.text.syOff);
		qis.ui.text.muShow = (QString::fromUtf8("显示") + qis.ui.text.syShow);
		qis.ui.text.muHide = (QString::fromUtf8("隐藏") + qis.ui.text.syHide);
		qis.ui.text.muExit = (QString::fromUtf8("退出") + qis.ui.text.syStop);
		qis.ui.text.acDown = (QString::fromUtf8("按下") + qis.ui.text.syDown);
		qis.ui.text.acUp = (QString::fromUtf8("松开") + qis.ui.text.syUp);
		qis.ui.text.acClick = (QString::fromUtf8("点击") + qis.ui.text.syTurn);
		qis.ui.text.acPos = (QString::fromUtf8("位置") + qis.ui.text.syLeft);
		qis.ui.text.acMove = (QString::fromUtf8("移动") + qis.ui.text.syMove);
		qis.ui.text.acWait = (QString::fromUtf8("等待") + qis.ui.text.syTime);
		qis.ui.text.acText = (QString::fromUtf8("复制") + qis.ui.text.syText);
		qis.ui.text.acLoop = (QString::fromUtf8("循环") + qis.ui.text.syLoop);
		qis.ui.text.acColor = (QString::fromUtf8("找色") + qis.ui.text.syColor);
		qis.ui.text.acEnd = (QString::fromUtf8("结束") + qis.ui.text.syStop);
		qis.ui.text.acEndLoop = (QString::fromUtf8("结束循环") + qis.ui.text.syStop);
		qis.ui.text.acKeyState = (QString::fromUtf8("按键状态") + qis.ui.text.syStop);
		qis.ui.text.acRecoverPos = (QString::fromUtf8("恢复位置") + qis.ui.text.syTurn);
		qis.ui.text.acImage = (QString::fromUtf8("找图") + qis.ui.text.syImage);
		qis.ui.text.acPopText = (QString::fromUtf8("弹出") + qis.ui.text.syText);
		qis.ui.text.acRememberPos = (QString::fromUtf8("记录位置") + qis.ui.text.syTurn);
		qis.ui.text.acTimer = (QString::fromUtf8("定时") + qis.ui.text.syLoop);
		qis.ui.text.trOn = (QString::fromUtf8("启用") + qis.ui.text.syOn);
		qis.ui.text.trOff = (QString::fromUtf8("禁用") + qis.ui.text.syOff);
		qis.ui.text.etFunc = (QString::fromUtf8("动作") + qis.ui.text.syOption);
		qis.ui.text.etParam = (QString::fromUtf8("参数") + qis.ui.text.syLink);
		qis.ui.text.etAdd = (QString::fromUtf8("添加") + qis.ui.text.syYes);
		qis.ui.text.etDel = (QString::fromUtf8("删除") + qis.ui.text.syNot);
		qis.ui.text.etChange = (QString::fromUtf8("修改") + qis.ui.text.syOk);
		qis.ui.text.etEdit = (QString::fromUtf8("编辑") + qis.ui.text.syEdit);
		qis.ui.text.rcStart = (QString::fromUtf8("开始") + qis.ui.text.syOk);
		qis.ui.text.rcStop = (QString::fromUtf8("停止") + qis.ui.text.syOk);
		qis.ui.text.rcClose = (QString::fromUtf8("取消") + qis.ui.text.syNot);
	}

	// style
	{
		QiUi::Theme theme;
		theme.name = "浅蓝";
		theme.style = R"(*{border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"]{border:1px solid gray}*[group="title"]{background-color:white;font-size:14px}*[group="client"]{background-color:#CEF}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QToolTip{background-color:white}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#ADE}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#ACF;color:black}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#CEF;color:black}*[group="table"]::item:selected{background-color:#ADF;color:black}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		qis.ui.themes.Add(theme);

		theme.name = "浅红";
		theme.style = R"(*{border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"]{border:1px solid gray}*[group="title"]{background-color:white;font-size:14px}*[group="client"]{background-color:#FDF}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QToolTip{background-color:#FFF5FF}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#FCE}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#FBE;color:black}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#FDF;color:black}*[group="table"]::item:selected{background-color:#FCE;color:black}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		qis.ui.themes.Add(theme);

		theme.name = "橘子";
		theme.style = R"(*{border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"]{border:1px solid gray}*[group="title"]{background-color:white;font-size:14px}*[group="client"]{background-color:#FEC;background-image:url(:/neko.png)}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QToolTip{background-color:#FFFFF5}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#FDA}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#FFD0B0;color:black}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#FEC;color:black}*[group="table"]::item:selected{background-color:#FFD0B0;color:black}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		qis.ui.themes.Add(theme);

		theme.name = "薄荷";
		theme.style = R"(*{border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"]{border:1px solid gray}*[group="title"]{background-color:white;font-size:14px}*[group="client"]{background-color:#AED}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QToolTip{background-color:#F5FFFF}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#9DC}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#8EB;color:black}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#AED;color:black}*[group="table"]::item:selected{background-color:#A5E5D5;color:black}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		qis.ui.themes.Add(theme);

		theme.name = "黑暗";
		theme.style = R"(*{color:white;border:none;font-size:12px;font-family:"Microsoft YaHei"}*[group="frame"]{border:1px solid gray}*[group="title"]{background-color:black;font-size:14px}*[group="client"]{background-color:#333}*[group="get_button"],*[group="check"]::indicator,*[group="radio"]::indicator,*[group="combo"],*[group="context_menu"],*[group="context_tip"],*[group="line_edit"],*[group="text_edit"],*[group="key_edit"],*[group="table"],*[group="table_header"],QToolTip{background-color:#555}*[group="macro-button"],*[group="settings-button"],*[group="edit-add_button"],*[group="edit-edit_button"],*[group="record-button"],*[group="tab_widget_bar"]::tab:first,*[group="tab_widget_bar"]::tab,*[group="tab_widget_bar"]::tab:last,*[group="ltab_widget"]::tab{background-color:#666}*[group="get_button"]:hover,*[group="macro-button"]:hover,*[group="settings-button"]:hover,*[group="edit-add_button"]:hover,*[group="edit-edit_button"]:hover,*[group="record-button"]:hover,*[group="check"]::indicator:hover,*[group="context_menu"]:selected,*[group="radio"]::indicator:hover,*[group="radio"]::item:selected,*[group="tab_widget_bar"]::tab:first:hover,*[group="tab_widget_bar"]::tab:hover,*[group="tab_widget_bar"]::tab:last:hover,*[group="table"]::item:hover{background-color:#777;color:white}*[group="tab_widget_bar"]::tab:first:selected,*[group="tab_widget_bar"]::tab:selected,*[group="tab_widget_bar"]::tab:last:selected{background-color:#333;color:white}*[group="table"]::item:selected{background-color:#888;color:white}*[group="check"]::indicator:checked,*[group="radio"]::indicator:checked{image:url(:/checked.png)}*[group="get_button"]:disabled,*[group="macro-button"]:disabled,*[group="settings-button"]:disabled,*[group="edit-add_button"]:disabled,*[group="edit-edit_button"]:disabled,*[group="record-button"]:disabled,*[group="check"]:disabled,*[group="radio"]:disabled,*[group="combo"]:disabled,*[group="context_menu"]::item:disabled,*[group="line_edit"]:disabled,*[group="text_edit"]:disabled,*[group="key_edit"]:disabled{color:gray}*[group="check"]::indicator,*[group="radio"]::indicator{width:20px;height:20px})";
		qis.ui.themes.Add(theme);
	}

	// style load
	{
		File::FileList fl = File::FindFile(L"style\\*.css");
		for (size_t i = 0; i < fl.size(); i++)
		{
			if (String::Compare(fl.at(i).name, L"style属性说明.css") == 0 || String::Compare(fl.at(i).name, L"style示例.css") == 0) continue;
			QiUi::Theme theme;
			theme.name = QString::fromWCharArray(Path::RemoveExtension(fl.at(i).name).c_str());
			theme.style = QString::fromUtf8(File::TextReadU(Path::Append(L"style", fl.at(i).name)).c_str());
			qis.ui.themes.Add(theme);
		}
	}
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