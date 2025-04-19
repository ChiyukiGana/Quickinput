#pragma optimize("",off)
static const char QUICKINPUT_COPYRIGHT_STRING[] = "QUICKINPUT_(C)CHIYUKIGANA";
#pragma optimize("",on)
#include <ui/MainUi.h>
#include <src/inc_header.h>

#define INTEGRITY_VERIFY
#ifdef INTEGRITY_VERIFY
#include <src/integrity_verify.h>
#endif

void Init()
{
	QiJson::LoadJson(); // config, macro
	Qi::screen = System::screenSize(); // screen pixel size
	qputenv("QT_SCALE_FACTOR", QByteArray::number(System::screenScaleRote(), 10, 1)); // enlarge high resolution screen
	if ("style")
	{
		QiUi::Theme theme;
		if (true)
		{
			theme.name = "浅蓝";
			theme.style = R"(
* {
	color: black;
    font-size: 12px;
    font-family: "Microsoft YaHei";
    border: none
}

*[group="frame"],
*[group="context_menu"] {
    border: 1px solid gray
}

*[group="title"] {
    background-color: white;
    font-size: 14px
}

*[group="client"],
*[group="edit-client"] {
    background-color: #CEF
}

*[group="line_edit"],
*[group="text_edit"],
*[group="key_edit"] {
    background-color: white
}

*[group="check"]::indicator,
*[group="radio"]::indicator,
*[group="get_button"] {
    background-color: white;
    width: 20px;
    height: 20px
}

*[group="check"]::indicator:hover,
*[group="radio"]::indicator:hover,
*[group="get_button"]:hover {
    background-color: #ACF
}

*[group="check"]::indicator:checked,
*[group="radio"]::indicator:checked {
    image: url(:/checked.png)
}

QToolTip,
*[group="combo"],
*[group="combo_body"],
*[group="context_menu"] {
    background-color: white
}

*[group="combo"]::item:hover,
*[group="combo_body"]::item:hover,
*[group="context_menu"]:selected {
    background-color: white;
    color: black
}

*[group="macro-button"],
*[group="settings-button"],
*[group="edit-add_button"],
*[group="edit-edit_button"],
*[group="record-button"] {
    background-color: #ADE
}

*[group="macro-button"]:hover,
*[group="settings-button"]:hover,
*[group="edit-add_button"]:hover,
*[group="edit-edit_button"]:hover,
*[group="record-button"]:hover {
    background-color: #ACF
}

*[group="tab_widget_bar"]::tab:first,
*[group="tab_widget_bar"]::tab,
*[group="tab_widget_bar"]::tab:last,
*[group="ltab_widget"]::tab,
*[group="edit-tab_button"] {
    background-color: #ADE
}

*[group="tab_widget_bar"]::tab:first:hover,
*[group="tab_widget_bar"]::tab:hover,
*[group="tab_widget_bar"]::tab:last:hover,
*[group="ltab_widget"]::tab:hover,
*[group="edit-tab_button"]:hover {
    background-color: #ADE
}

*[group="tab_widget_bar"]::tab:first:selected,
*[group="tab_widget_bar"]::tab:selected,
*[group="tab_widget_bar"]::tab:last:selected,
*[group="ltab_widget"]::tab:selected,
*[group="edit-tab_button"]:disabled {
    background-color: #CEF;
    color: black
}

*[group="group_table"],
*[group="macro_table"],
*[group="action_table"],
*[group="group_table_header"],
*[group="macro_table_header"],
*[group="action_table_header"] {
    background-color: white
}

*[group="macro_table_header"]
{
	background-color: #F5F5F5
}

*[group="macro_table"]::item:hover,
*[group="action_table"]::item:hover {
    background-color: #ACF;
    color: black
}

*[group="macro_table"]::item:selected,
*[group="action_table"]::item:selected {
    background-color: #ADF;
    color: black
}

*[group="frame"]:disabled,
*[group="context_menu"]:disabled,
*[group="title"]:disabled,
*[group="client"]:disabled,
*[group="line_edit"]:disabled,
*[group="text_edit"]:disabled,
*[group="key_edit"]:disabled,
*[group="get_button"]:disabled,
*[group="check"]:disabled,
*[group="radio"]:disabled,
*[group="combo"]:disabled,
*[group="combo_body"]:disabled,
*[group="context_menu"]:disabled,
*[group="macro-button"]:disabled,
*[group="settings-button"]:disabled,
*[group="edit-add_button"]:disabled,
*[group="edit-edit_button"]:disabled,
*[group="record-button"]:disabled,
*[group="tab_widget_bar"]::tab:first:disabled,
*[group="tab_widget_bar"]::tab:disabled,
*[group="tab_widget_bar"]::tab:last:disabled,
*[group="ltab_widget"]::tab:disabled,
*[group="group_table"]:disabled,
*[group="macro_table"]:disabled,
*[group="action_table"]:disabled,
*[group="group_table_header"]:disabled,
*[group="macro_table_header"]:disabled,
*[group="action_table_header"]:disabled {
    color: gray
}
)";
			Qi::ui.themes.append(theme);
		}
		if (true)
		{
			theme.name = "糖果";
			theme.style = R"(
* {
	color: black;
    font-size: 12px;
    font-family: "Microsoft YaHei";
    border: none
}

*[group="frame"],
*[group="context_menu"] {
    border: 1px solid gray
}

*[group="title"] {
    background-color: white;
    font-size: 14px
}

*[group="client"],
*[group="edit-client"] {
    background-color: #FDF;
    background-image: url(:/candy.png)
}

*[group="line_edit"],
*[group="text_edit"],
*[group="key_edit"] {
    background-color: #FFF5FF
}

*[group="check"]::indicator,
*[group="radio"]::indicator,
*[group="get_button"] {
    background-color: #FFF5FF;
    width: 20px;
    height: 20px
}

*[group="check"]::indicator:hover,
*[group="radio"]::indicator:hover,
*[group="get_button"]:hover {
    background-color: #FBE
}

*[group="check"]::indicator:checked,
*[group="radio"]::indicator:checked {
    image: url(:/checked.png)
}

QToolTip,
*[group="combo"],
*[group="combo_body"],
*[group="context_menu"] {
    background-color: #FFF5FF
}

*[group="combo"]::item:hover,
*[group="combo_body"]::item:hover,
*[group="context_menu"]:selected {
    background-color: #FBE;
    color: black
}

*[group="macro-button"],
*[group="settings-button"],
*[group="edit-add_button"],
*[group="edit-edit_button"],
*[group="record-button"] {
    background-color: #FCE
}

*[group="macro-button"]:hover,
*[group="settings-button"]:hover,
*[group="edit-add_button"]:hover,
*[group="edit-edit_button"]:hover,
*[group="record-button"]:hover {
    background-color: #FBE
}

*[group="tab_widget_bar"]::tab:first,
*[group="tab_widget_bar"]::tab,
*[group="tab_widget_bar"]::tab:last,
*[group="ltab_widget"]::tab,
*[group="edit-tab_button"] {
    background-color: #FCE
}

*[group="tab_widget_bar"]::tab:first:hover,
*[group="tab_widget_bar"]::tab:hover,
*[group="tab_widget_bar"]::tab:last:hover,
*[group="ltab_widget"]::tab:hover,
*[group="edit-tab_button"]:hover {
    background-color: #FBE
}

*[group="tab_widget_bar"]::tab:first:selected,
*[group="tab_widget_bar"]::tab:selected,
*[group="tab_widget_bar"]::tab:last:selected,
*[group="ltab_widget"]::tab:selected,
*[group="edit-tab_button"]:disabled {
    background-color: #FDF;
    color: black
}

*[group="group_table"],
*[group="macro_table"],
*[group="action_table"],
*[group="group_table_header"],
*[group="macro_table_header"],
*[group="action_table_header"] {
    background-color: white
}

*[group="macro_table_header"]
{
	background-color: #FFF0FF
}

*[group="macro_table"]::item:hover,
*[group="action_table"]::item:hover {
    background-color: #FBE;
    color: black
}

*[group="macro_table"]::item:selected,
*[group="action_table"]::item:selected {
    background-color: #FCE;
    color: black
}

*[group="frame"]:disabled,
*[group="context_menu"]:disabled,
*[group="title"]:disabled,
*[group="client"]:disabled,
*[group="line_edit"]:disabled,
*[group="text_edit"]:disabled,
*[group="key_edit"]:disabled,
*[group="get_button"]:disabled,
*[group="check"]:disabled,
*[group="radio"]:disabled,
*[group="combo"]:disabled,
*[group="combo_body"]:disabled,
*[group="context_menu"]:disabled,
*[group="macro-button"]:disabled,
*[group="settings-button"]:disabled,
*[group="edit-add_button"]:disabled,
*[group="edit-edit_button"]:disabled,
*[group="record-button"]:disabled,
*[group="tab_widget_bar"]::tab:first:disabled,
*[group="tab_widget_bar"]::tab:disabled,
*[group="tab_widget_bar"]::tab:last:disabled,
*[group="ltab_widget"]::tab:disabled,
*[group="group_table"]:disabled,
*[group="macro_table"]:disabled,
*[group="action_table"]:disabled,
*[group="group_table_header"]:disabled,
*[group="macro_table_header"]:disabled,
*[group="action_table_header"]:disabled {
    color: gray
}
)";
			Qi::ui.themes.append(theme);
		}
		if (true)
		{
			theme.name = "橘子";
			theme.style = R"(
* {
	color: black;
    font-size: 12px;
    font-family: "Microsoft YaHei";
    border: none
}

*[group="frame"],
*[group="context_menu"] {
    border: 1px solid gray
}

*[group="title"] {
    background-color: white;
    font-size: 14px
}

*[group="client"],
*[group="edit-client"] {
    background-color: #FEC;
    background-image: url(:/neko.png)
}

*[group="line_edit"],
*[group="text_edit"],
*[group="key_edit"] {
    background-color: #FFFFF5
}

*[group="check"]::indicator,
*[group="radio"]::indicator,
*[group="get_button"] {
    background-color: #FFFFF5;
    width: 20px;
    height: 20px
}

*[group="check"]::indicator:hover,
*[group="radio"]::indicator:hover,
*[group="get_button"]:hover {
    background-color: #FFD0B0
}

*[group="check"]::indicator:checked,
*[group="radio"]::indicator:checked {
    image: url(:/checked.png)
}

QToolTip,
*[group="combo"],
*[group="combo_body"],
*[group="context_menu"] {
    background-color: #FFFFF5
}

*[group="combo"]::item:hover,
*[group="combo_body"]::item:hover,
*[group="context_menu"]:selected {
    background-color: #FFD0B0;
    color: black
}

*[group="macro-button"],
*[group="settings-button"],
*[group="edit-add_button"],
*[group="edit-edit_button"],
*[group="record-button"] {
    background-color: #FDA
}

*[group="macro-button"]:hover,
*[group="settings-button"]:hover,
*[group="edit-add_button"]:hover,
*[group="edit-edit_button"]:hover,
*[group="record-button"]:hover {
    background-color: #FFD0B0
}

*[group="tab_widget_bar"]::tab:first,
*[group="tab_widget_bar"]::tab,
*[group="tab_widget_bar"]::tab:last,
*[group="ltab_widget"]::tab,
*[group="edit-tab_button"] {
    background-color: #FDA
}

*[group="tab_widget_bar"]::tab:first:hover,
*[group="tab_widget_bar"]::tab:hover,
*[group="tab_widget_bar"]::tab:last:hover,
*[group="ltab_widget"]::tab:hover,
*[group="edit-tab_button"]:hover {
    background-color: #FFD0B0
}

*[group="tab_widget_bar"]::tab:first:selected,
*[group="tab_widget_bar"]::tab:selected,
*[group="tab_widget_bar"]::tab:last:selected,
*[group="ltab_widget"]::tab:selected,
*[group="edit-tab_button"]:disabled {
    background-color: #FEC;
    color: black
}

*[group="group_table"],
*[group="macro_table"],
*[group="action_table"],
*[group="group_table_header"],
*[group="macro_table_header"],
*[group="action_table_header"] {
    background-color: white
}

*[group="macro_table_header"]
{
	background-color: #FFFFE0
}

*[group="macro_table"]::item:hover,
*[group="action_table"]::item:hover {
    background-color: #FFD0B0;
    color: black
}

*[group="macro_table"]::item:selected,
*[group="action_table"]::item:selected {
    background-color: #FDA;
    color: black
}

*[group="frame"]:disabled,
*[group="context_menu"]:disabled,
*[group="title"]:disabled,
*[group="client"]:disabled,
*[group="line_edit"]:disabled,
*[group="text_edit"]:disabled,
*[group="key_edit"]:disabled,
*[group="get_button"]:disabled,
*[group="check"]:disabled,
*[group="radio"]:disabled,
*[group="combo"]:disabled,
*[group="combo_body"]:disabled,
*[group="context_menu"]:disabled,
*[group="macro-button"]:disabled,
*[group="settings-button"]:disabled,
*[group="edit-add_button"]:disabled,
*[group="edit-edit_button"]:disabled,
*[group="record-button"]:disabled,
*[group="tab_widget_bar"]::tab:first:disabled,
*[group="tab_widget_bar"]::tab:disabled,
*[group="tab_widget_bar"]::tab:last:disabled,
*[group="ltab_widget"]::tab:disabled,
*[group="group_table"]:disabled,
*[group="macro_table"]:disabled,
*[group="action_table"]:disabled,
*[group="group_table_header"]:disabled,
*[group="macro_table_header"]:disabled,
*[group="action_table_header"]:disabled {
    color: gray
}
)";
			Qi::ui.themes.append(theme);
		}
		if (true)
		{
			theme.name = "四叶草";
			theme.style = R"(
* {
	color: black;
    font-size: 12px;
    font-family: "Microsoft YaHei";
    border: none
}

*[group="frame"],
*[group="context_menu"] {
    border: 1px solid gray
}

*[group="title"] {
    background-color: white;
    font-size: 14px
}

*[group="client"],
*[group="edit-client"] {
    background-color: #AED;
    background-image: url(:/blade.png)
}

*[group="line_edit"],
*[group="text_edit"],
*[group="key_edit"] {
    background-color: #F5FFFF
}

*[group="check"]::indicator,
*[group="radio"]::indicator,
*[group="get_button"] {
    background-color: #F5FFFF;
    width: 20px;
    height: 20px
}

*[group="check"]::indicator:hover,
*[group="radio"]::indicator:hover,
*[group="get_button"]:hover {
    background-color: #8EB
}

*[group="check"]::indicator:checked,
*[group="radio"]::indicator:checked {
    image: url(:/checked.png)
}

QToolTip,
*[group="combo"],
*[group="combo_body"],
*[group="context_menu"] {
    background-color: #F5FFFF
}

*[group="combo"]::item:hover,
*[group="combo_body"]::item:hover,
*[group="context_menu"]:selected {
    background-color: #8EB;
    color: black
}

*[group="macro-button"],
*[group="settings-button"],
*[group="edit-add_button"],
*[group="edit-edit_button"],
*[group="record-button"] {
    background-color: #9DC
}

*[group="macro-button"]:hover,
*[group="settings-button"]:hover,
*[group="edit-add_button"]:hover,
*[group="edit-edit_button"]:hover,
*[group="record-button"]:hover {
    background-color: #8EB
}

*[group="tab_widget_bar"]::tab:first,
*[group="tab_widget_bar"]::tab,
*[group="tab_widget_bar"]::tab:last,
*[group="ltab_widget"]::tab,
*[group="edit-tab_button"] {
    background-color: #9DC
}

*[group="tab_widget_bar"]::tab:first:hover,
*[group="tab_widget_bar"]::tab:hover,
*[group="tab_widget_bar"]::tab:last:hover,
*[group="ltab_widget"]::tab:hover,
*[group="edit-tab_button"]:hover {
    background-color: #8EB
}

*[group="tab_widget_bar"]::tab:first:selected,
*[group="tab_widget_bar"]::tab:selected,
*[group="tab_widget_bar"]::tab:last:selected,
*[group="ltab_widget"]::tab:selected,
*[group="edit-tab_button"]:disabled {
    background-color: #AED;
    color: black
}

*[group="group_table"],
*[group="macro_table"],
*[group="action_table"],
*[group="group_table_header"],
*[group="macro_table_header"],
*[group="action_table_header"] {
    background-color: white
}

*[group="macro_table_header"]
{
	background-color: #E5FFE5
}

*[group="macro_table"]::item:hover,
*[group="action_table"]::item:hover {
    background-color: #8EB;
    color: black
}

*[group="macro_table"]::item:selected,
*[group="action_table"]::item:selected {
    background-color: #9DC;
    color: black
}

*[group="frame"]:disabled,
*[group="context_menu"]:disabled,
*[group="title"]:disabled,
*[group="client"]:disabled,
*[group="line_edit"]:disabled,
*[group="text_edit"]:disabled,
*[group="key_edit"]:disabled,
*[group="get_button"]:disabled,
*[group="check"]:disabled,
*[group="radio"]:disabled,
*[group="combo"]:disabled,
*[group="combo_body"]:disabled,
*[group="context_menu"]:disabled,
*[group="macro-button"]:disabled,
*[group="settings-button"]:disabled,
*[group="edit-add_button"]:disabled,
*[group="edit-edit_button"]:disabled,
*[group="record-button"]:disabled,
*[group="tab_widget_bar"]::tab:first:disabled,
*[group="tab_widget_bar"]::tab:disabled,
*[group="tab_widget_bar"]::tab:last:disabled,
*[group="ltab_widget"]::tab:disabled,
*[group="group_table"]:disabled,
*[group="macro_table"]:disabled,
*[group="action_table"]:disabled,
*[group="group_table_header"]:disabled,
*[group="macro_table_header"]:disabled,
*[group="action_table_header"]:disabled {
    color: gray
}
)";
			Qi::ui.themes.append(theme);
		}
		if (true)
		{
			theme.name = "黑暗";
			theme.style = R"(
* {
    color: white;
    font-size: 12px;
    font-family: "Microsoft YaHei";
    border: none
}

*[group="frame"],
*[group="context_menu"] {
    border: 1px solid gray
}

*[group="title"] {
    background-color: black;
    font-size: 14px
}

*[group="client"],
*[group="edit-client"] {
    background-color: #333
}

*[group="line_edit"],
*[group="text_edit"],
*[group="key_edit"] {
    background-color: #555
}

*[group="check"]::indicator,
*[group="radio"]::indicator,
*[group="get_button"] {
    background-color: #555;
    width: 20px;
    height: 20px
}

*[group="check"]::indicator:hover,
*[group="radio"]::indicator:hover,
*[group="get_button"]:hover {
    background-color: #777
}

*[group="check"]::indicator:checked,
*[group="radio"]::indicator:checked {
    image: url(:/checked.png)
}

QToolTip,
*[group="combo"],
*[group="combo_body"],
*[group="context_menu"] {
    background-color: #555
}

*[group="combo"]::item:hover,
*[group="combo_body"]::item:hover,
*[group="context_menu"]:selected {
    background-color: #777;
    color: white
}

*[group="macro-button"],
*[group="settings-button"],
*[group="edit-add_button"],
*[group="edit-edit_button"],
*[group="record-button"] {
    background-color: #666
}

*[group="macro-button"]:hover,
*[group="settings-button"]:hover,
*[group="edit-add_button"]:hover,
*[group="edit-edit_button"]:hover,
*[group="record-button"]:hover {
    background-color: #777
}

*[group="tab_widget_bar"]::tab:first,
*[group="tab_widget_bar"]::tab,
*[group="tab_widget_bar"]::tab:last,
*[group="ltab_widget"]::tab,
*[group="edit-tab_button"] {
    background-color: #666
}

*[group="tab_widget_bar"]::tab:first:hover,
*[group="tab_widget_bar"]::tab:hover,
*[group="tab_widget_bar"]::tab:last:hover,
*[group="ltab_widget"]::tab:hover,
*[group="edit-tab_button"]:hover {
    background-color: #777
}

*[group="tab_widget_bar"]::tab:first:selected,
*[group="tab_widget_bar"]::tab:selected,
*[group="tab_widget_bar"]::tab:last:selected,
*[group="ltab_widget"]::tab:selected,
*[group="edit-tab_button"]:disabled {
    background-color: #333;
    color: white
}

*[group="group_table"],
*[group="macro_table"],
*[group="action_table"],
*[group="group_table_header"],
*[group="macro_table_header"],
*[group="action_table_header"] {
    background-color: #555
}

*[group="macro_table_header"]
{
	background-color: #606060
}

*[group="macro_table"]::item:hover,
*[group="action_table"]::item:hover {
    background-color: #777;
    color: white
}

*[group="macro_table"]::item:selected,
*[group="action_table"]::item:selected {
    background-color: #888;
    color: white
}

*[group="frame"]:disabled,
*[group="context_menu"]:disabled,
*[group="title"]:disabled,
*[group="client"]:disabled,
*[group="line_edit"]:disabled,
*[group="text_edit"]:disabled,
*[group="key_edit"]:disabled,
*[group="get_button"]:disabled,
*[group="check"]:disabled,
*[group="radio"]:disabled,
*[group="combo"]:disabled,
*[group="combo_body"]:disabled,
*[group="context_menu"]:disabled,
*[group="macro-button"]:disabled,
*[group="settings-button"]:disabled,
*[group="edit-add_button"]:disabled,
*[group="edit-edit_button"]:disabled,
*[group="record-button"]:disabled,
*[group="tab_widget_bar"]::tab:first:disabled,
*[group="tab_widget_bar"]::tab:disabled,
*[group="tab_widget_bar"]::tab:last:disabled,
*[group="ltab_widget"]::tab:disabled,
*[group="group_table"]:disabled,
*[group="macro_table"]:disabled,
*[group="action_table"]:disabled,
*[group="group_table_header"]:disabled,
*[group="macro_table_header"]:disabled,
*[group="action_table_header"]:disabled {
    color: gray
}
)";
			Qi::ui.themes.append(theme);
		}
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
    if ("sound")
    {
        ResourceTool::find(L"WAVE", [](char* data, size_t size, std::wstring name) { Qi::ui.sounds.append(QString::fromWCharArray(name.c_str())); return true; });
    }
	if (Qi::set.theme >= Qi::ui.themes.size()) Qi::set.theme = 0;
}
int main(int argc, char* argv[])
{
	std::cout << QUICKINPUT_COPYRIGHT_STRING << std::endl;
	std::locale::global(std::locale(".UTF8")); // set utf8 for all std streams
	Process::RunPath(); // reset work path to exe path

#ifdef INTEGRITY_VERIFY
	integrity_verify();
#endif

	std::wstring mutex = Path::toSlash(Process::runPath()); // mutex name, the current directory is only running one
	if (Process::isRunning(mutex.c_str())) // if running
	{
		MsgBox::Warning(L"当前文件夹的程序已经运行，若运行更多程序请复制此文件夹", L"提示");
		return -1;
	}
	CreateMutexW(0, 0, mutex.c_str()); // create mutex
	Init(); // json, font, style

	// ocr
	if (File::PathState(L"OCR")) Qi::ocr = QiOcrInterfaceInit();

	QApplication application(argc, argv);
	Qi::application = &application; // save to global
	Qi::popText = new QPopText; // popup text on screen of global widget
	Qi::popText->setPosition(Qi::ui.pop.p.x, Qi::ui.pop.p.y);
	Qi::popText->setSize(Qi::ui.pop.size);
	Qi::windowSelection = new QWindowSelection; // select a window of global widget
	Qi::interpreter_pop = [](std::string text, int time) { Qi::popText->Popup(time, text.c_str(), RGB(223, 223, 223)); };
	MainUi mainWindow;
	Qi::version = mainWindow.Version();
	application.exec();
	return 0;
}