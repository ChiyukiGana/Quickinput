#include "ui/MainUi.h"
#include "ui/InstallUi.h"
#include "src/minc.h"

QuickInputStruct qis;

void InitUI(bool zoom)
{
	// enlarge high resolution screen
	if (zoom && System::screenSize().cy > 1200) qputenv("QT_SCALE_FACTOR", QByteArray::number(System::screenZoomRote(), 10, 1));

	// symbol
	if (System::Version().dwMajorVersion >= 10)
	{
		qis.ui.text.syOn = u8"✅";
		qis.ui.text.syOff = u8"⛔";
		qis.ui.text.syOk = u8"⭕";
		qis.ui.text.syYes = u8"✔️";
		qis.ui.text.syNot = u8"❌";
		qis.ui.text.syStop = u8"🛑";
		qis.ui.text.syShow = u8"🔼";
		qis.ui.text.syHide = u8"🔽";
		qis.ui.text.syOption = u8"⚙";
		qis.ui.text.syLink = u8"🔗";
		qis.ui.text.syEdit = u8"🔧";
		qis.ui.text.syUp = u8"⬆️";
		qis.ui.text.syDown = u8"⬇️";
		qis.ui.text.syTurn = u8"🔃";
		qis.ui.text.syLeft = u8"🔙";
		qis.ui.text.syTop = u8"🔝";
		qis.ui.text.syRight = u8"🔜";
		qis.ui.text.syMove = u8"🔛";
		qis.ui.text.syTime = u8"⏳";
		qis.ui.text.syText = u8"🅰️";
		qis.ui.text.syLoop = u8"♾️";
		qis.ui.text.syColor = u8"🌈";
		qis.ui.text.syImage = u8"🖼";
	}
	else
	{
		qis.ui.text.syOn = u8"✔";
		qis.ui.text.syOff = u8"✘";
		qis.ui.text.syOk = u8"✔";
		qis.ui.text.syYes = u8"✔";
		qis.ui.text.syNot = u8"✘";
		qis.ui.text.syStop = u8"Ⓢ";
		qis.ui.text.syShow = u8"▲";
		qis.ui.text.syHide = u8"▼";
		qis.ui.text.syOption = u8"✱";
		qis.ui.text.syLink = u8"※";
		qis.ui.text.syEdit = u8"▲";
		qis.ui.text.syUp = u8"↑";
		qis.ui.text.syDown = u8"↓";
		qis.ui.text.syTurn = u8"◈";
		qis.ui.text.syLeft = u8"←";
		qis.ui.text.syTop = u8"↑";
		qis.ui.text.syRight = u8"→";
		qis.ui.text.syMove = u8"↘";
		qis.ui.text.syTime = u8"☽";
		qis.ui.text.syText = u8"A";
		qis.ui.text.syLoop = u8"↩";
		qis.ui.text.syColor = u8"☀";
		qis.ui.text.syImage = u8"☀";
	}

	// text
	{
		qis.ui.text.muOn = (QString::fromUtf8(u8"启用") + qis.ui.text.syOn);
		qis.ui.text.muOff = (QString::fromUtf8(u8"禁用") + qis.ui.text.syOff);
		qis.ui.text.muShow = (QString::fromUtf8(u8"显示") + qis.ui.text.syShow);
		qis.ui.text.muHide = (QString::fromUtf8(u8"隐藏") + qis.ui.text.syHide);
		qis.ui.text.muExit = (QString::fromUtf8(u8"退出") + qis.ui.text.syStop);
		qis.ui.text.acDown = (QString::fromUtf8(u8"按下") + qis.ui.text.syDown);
		qis.ui.text.acUp = (QString::fromUtf8(u8"松开") + qis.ui.text.syUp);
		qis.ui.text.acClick = (QString::fromUtf8(u8"点击") + qis.ui.text.syTurn);
		qis.ui.text.acPos = (QString::fromUtf8(u8"位置") + qis.ui.text.syLeft);
		qis.ui.text.acMove = (QString::fromUtf8(u8"移动") + qis.ui.text.syMove);
		qis.ui.text.acWait = (QString::fromUtf8(u8"等待") + qis.ui.text.syTime);
		qis.ui.text.acText = (QString::fromUtf8(u8"复制") + qis.ui.text.syText);
		qis.ui.text.acLoop = (QString::fromUtf8(u8"循环") + qis.ui.text.syLoop);
		qis.ui.text.acColor = (QString::fromUtf8(u8"找色") + qis.ui.text.syColor);
		qis.ui.text.acEnd = (QString::fromUtf8(u8"结束") + qis.ui.text.syStop);
		qis.ui.text.acEndLoop = (QString::fromUtf8(u8"结束循环") + qis.ui.text.syStop);
		qis.ui.text.acKeyState = (QString::fromUtf8(u8"按键状态") + qis.ui.text.syStop);
		qis.ui.text.acRecoverPos = (QString::fromUtf8(u8"恢复位置") + qis.ui.text.syTurn);
		qis.ui.text.acImage = (QString::fromUtf8(u8"找图") + qis.ui.text.syImage);
		qis.ui.text.acPopText = (QString::fromUtf8(u8"弹出") + qis.ui.text.syText);
		qis.ui.text.acRememberPos = (QString::fromUtf8(u8"记录位置") + qis.ui.text.syTurn);
		qis.ui.text.trOn = (QString::fromUtf8(u8"启用") + qis.ui.text.syOn);
		qis.ui.text.trOff = (QString::fromUtf8(u8"禁用") + qis.ui.text.syOff);
		qis.ui.text.etFunc = (QString::fromUtf8(u8"动作") + qis.ui.text.syOption);
		qis.ui.text.etParam = (QString::fromUtf8(u8"参数") + qis.ui.text.syLink);
		qis.ui.text.etAdd = (QString::fromUtf8(u8"添加") + qis.ui.text.syYes);
		qis.ui.text.etDel = (QString::fromUtf8(u8"删除") + qis.ui.text.syNot);
		qis.ui.text.etChange = (QString::fromUtf8(u8"修改") + qis.ui.text.syOk);
		qis.ui.text.etEdit = (QString::fromUtf8(u8"编辑") + qis.ui.text.syEdit);
		qis.ui.text.rcStart = (QString::fromUtf8(u8"开始") + qis.ui.text.syOk);
		qis.ui.text.rcStop = (QString::fromUtf8(u8"停止") + qis.ui.text.syOk);
		qis.ui.text.rcClose = (QString::fromUtf8(u8"取消") + qis.ui.text.syNot);
	}

	// style
	{
		QiUi::Theme theme;
		theme.name = u8"浅蓝";
		theme.style = u8R"(*{color:black;border:none;outline:none;font-size:12px;font-family:"Microsoft YaHei"}QLineEdit{color:#444}QMainWindow,QDialog{border:1px solid gray}QMenu,QToolTip,QLineEdit,QTextEdit,QKeyEdit,QCheckBox::indicator,QRadioButton::indicator,QHeaderView,QHeaderView::section,QTableWidget,QTableWidget QTableCornerButton::section,QComboBox,QComboBox QAbstractItemView{background-color:white}QPushButton,QTabBar::tab,QTabBar::tab:first,QTabBar::tab:last,QToolBox::tab{background-color:#ADE}QCheckBox::indicator,QRadioButton::indicator{width:20px;height:20px}QComboBox{padding-left:3px}QMenu::item:selected,QPushButton:hover,QCheckBox::indicator:hover,QRadioButton::indicator:hover,QTableWidget::item:hover,QTabBar::tab:hover,QTabBar::tab:first:hover,QTabBar::tab:last:hover{background-color:#ACF;color:black}QTableWidget::item:selected{background-color:#ADF;color:black}QTabBar::tab:selected,QTabBar::tab:first:selected,QTabBar::tab:last:selected{background-color:#CEF;color:black}QCheckBox::indicator:checked,QRadioButton::indicator:checked{image:url(:/checked.png)}QPushButton:disabled,QCheckBox:disabled,QRadioButton:disabled{color:gray}#bnWndSelect,#bnPos,#bnColorRect,#bnImageRect,#bnImageShot{background-color:white}#titleWidget{background-color:white}#titleWidget *{font-size:14px}#clientWidget,QTabWidget>QWidget,QToolBox>QWidget{background-color:#CEF})";
		qis.ui.themes.Add(theme);

		theme.name = u8"浅红";
		theme.style = u8R"(*{color:black;border:none;outline:none;font-size:12px;font-family:"Microsoft YaHei"}QLineEdit{color:#444}QMainWindow,QDialog{border:1px solid gray}QMenu,QToolTip,QLineEdit,QTextEdit,QKeyEdit,QCheckBox::indicator,QRadioButton::indicator,QHeaderView,QHeaderView::section,QTableWidget,QTableWidget QTableCornerButton::section,QComboBox,QComboBox QAbstractItemView{background-color:#FFF5FF}QPushButton,QTabBar::tab,QTabBar::tab:first,QTabBar::tab:last,QToolBox::tab{background-color:#FCE}QCheckBox::indicator,QRadioButton::indicator{width:20px;height:20px}QComboBox{padding-left:3px}QMenu::item:selected,QPushButton:hover,QCheckBox::indicator:hover,QRadioButton::indicator:hover,QTableWidget::item:hover,QTabBar::tab:hover,QTabBar::tab:first:hover,QTabBar::tab:last:hover{background-color:#FBE;color:black}QTableWidget::item:selected{background-color:#FCE;color:black}QTabBar::tab:selected,QTabBar::tab:first:selected,QTabBar::tab:last:selected{background-color:#FDF;color:black}QCheckBox::indicator:checked,QRadioButton::indicator:checked{image:url(:/checked.png)}QPushButton:disabled,QCheckBox:disabled,QRadioButton:disabled{color:gray}#bnWndSelect,#bnPos,#bnColorRect,#bnImageRect,#bnImageShot{background-color:#FFF5FF}#titleWidget{background-color:white}#titleWidget *{font-size:14px}#clientWidget,QTabWidget>QWidget,QToolBox>QWidget{background-color:#FDF})";
		qis.ui.themes.Add(theme);

		theme.name = u8"橘子";
		theme.style = u8R"(*{color:black;border:none;outline:none;font-size:12px;font-family:"Microsoft YaHei"}QLineEdit{color:#444}QMainWindow,QDialog{border:1px solid gray}QMenu,QToolTip,QLineEdit,QTextEdit,QKeyEdit,QCheckBox::indicator,QRadioButton::indicator,QHeaderView,QHeaderView::section,QTableWidget,QTableWidget QTableCornerButton::section,QComboBox,QComboBox QAbstractItemView{background-color:#FFFFF5}QPushButton,QTabBar::tab,QTabBar::tab:first,QTabBar::tab:last,QToolBox::tab{background-color:#FDA}QCheckBox::indicator,QRadioButton::indicator{width:20px;height:20px}QComboBox{padding-left:3px}QMenu::item:selected,QPushButton:hover,QCheckBox::indicator:hover,QRadioButton::indicator:hover,QTableWidget::item:hover,QTabBar::tab:hover,QTabBar::tab:first:hover,QTabBar::tab:last:hover{background-color:#FFD0B0;color:black}QTableWidget::item:selected{background-color:#FFD0B0;color:black}QTabBar::tab:selected,QTabBar::tab:first:selected,QTabBar::tab:last:selected{background-color:#FEC;color:black}QCheckBox::indicator:checked,QRadioButton::indicator:checked{image:url(:/checked.png)}QPushButton:disabled,QCheckBox:disabled,QRadioButton:disabled{color:gray}#bnWndSelect,#bnPos,#bnColorRect,#bnImageRect,#bnImageShot{background-color:#FFFFF5}#titleWidget{background-color:white}#titleWidget *{font-size:14px}#clientWidget,QTabWidget>QWidget,QToolBox>QWidget{background-color:#FEC})";
		qis.ui.themes.Add(theme);

		theme.name = u8"薄荷";
		theme.style = u8R"(*{color:black;border:none;outline:none;font-size:12px;font-family:"Microsoft YaHei"}QLineEdit{color:#444}QMainWindow,QDialog{border:1px solid gray}QMenu,QToolTip,QLineEdit,QTextEdit,QKeyEdit,QCheckBox::indicator,QRadioButton::indicator,QHeaderView,QHeaderView::section,QTableWidget,QTableWidget QTableCornerButton::section,QComboBox,QComboBox QAbstractItemView{background-color:#F5FFFF}QPushButton,QTabBar::tab,QTabBar::tab:first,QTabBar::tab:last,QToolBox::tab{background-color:#9DC}QCheckBox::indicator,QRadioButton::indicator{width:20px;height:20px}QComboBox{padding-left:3px}QMenu::item:selected,QPushButton:hover,QCheckBox::indicator:hover,QRadioButton::indicator:hover,QTableWidget::item:hover,QTabBar::tab:hover,QTabBar::tab:first:hover,QTabBar::tab:last:hover{background-color:#8EB;color:black}QTableWidget::item:selected{background-color:#A5E5D5;color:black}QTabBar::tab:selected,QTabBar::tab:first:selected,QTabBar::tab:last:selected{background-color:#AED;color:black}QCheckBox::indicator:checked,QRadioButton::indicator:checked{image:url(:/checked.png)}QPushButton:disabled,QCheckBox:disabled,QRadioButton:disabled{color:gray}#bnWndSelect,#bnPos,#bnColorRect,#bnImageRect,#bnImageShot{background-color:#F5FFFF}#titleWidget{background-color:white}#titleWidget *{font-size:14px}#clientWidget,QTabWidget>QWidget,QToolBox>QWidget{background-color:#AED})";
		qis.ui.themes.Add(theme);

		theme.name = u8"黑暗";
		theme.style = u8R"(*{color:white;border:none;outline:none;font-size:12px;font-family:"Microsoft YaHei"}QLineEdit{color:#DDD}QMainWindow,QDialog{border:1px solid gray}QToolTip,QLineEdit,QTextEdit,QKeyEdit,QCheckBox::indicator,QRadioButton::indicator,QHeaderView,QHeaderView::section,QTableWidget,QTableWidget QTableCornerButton::section,QComboBox,QComboBox QAbstractItemView{background-color:#555}QPushButton,QTabBar::tab,QTabBar::tab:first,QTabBar::tab:last,QToolBox::tab{background-color:#666}QCheckBox::indicator,QRadioButton::indicator{width:20px;height:20px}QComboBox{padding-left:3px}QMenu::item:selected,QPushButton:hover,QCheckBox::indicator:hover,QRadioButton::indicator:hover,QTableWidget::item:hover,QTabBar::tab:hover,QTabBar::tab:first:hover,QTabBar::tab:last:hover{background-color:#777;color:white}QTableWidget::item:selected{background-color:#888;color:white}QTabBar::tab:selected,QTabBar::tab:first:selected,QTabBar::tab:last:selected{background-color:#333;color:white}QCheckBox::indicator:checked,QRadioButton::indicator:checked{image:url(:/checked.png)}QPushButton:disabled,QCheckBox:disabled,QRadioButton:disabled{color:gray}#bnWndSelect,#bnPos,#bnColorRect,#bnImageRect,#bnImageShot{background-color:#666}#titleWidget{background-color:black}#titleWidget *{font-size:14px}QMenu,#clientWidget,QTabWidget>QWidget,QToolBox>QWidget{background-color:#333})";
		qis.ui.themes.Add(theme);
	}
}

// in temp folder
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

int main(int argc, char* argv[])
{
	std::locale::global(std::locale(".UTF8")); // set utf8 for all streams
	Process::RunPath(); // reset work path to exe path
	// mutex
	std::wstring mutex = Path::toSlash(Process::runPath()); // convert '\' to '/' to support  mutex name
	if (Process::isRunning(mutex.c_str())) { MsgBox::Warning(L"当前文件夹的程序已经运行，若运行更多程序请复制此文件夹", L"提示"); return 0; }
	CreateMutexW(0, 0, mutex.c_str()); // create mutex if this Quick input is not running

	// initialization
	QiJson::LoadJson(); // load config
	InitUI(!qis.set.zoomBlock); // init fonts and zoom
	QApplication app(argc, argv); app.setFont(QFont("Microsoft YaHei")); // init application
	Install(); // if program in temp folder

	qis.screen = System::screenSize(); // screen size
	PopBox::Init(); // thread of PopBox
	MainUi wnd;
	if (qis.set.minMode)
	{
		QiFn::QiHook(true);
		if (qis.set.defOn) QiFn::QiState(true);
	}
	else wnd.show();
	app.exec();

	return 0;
}