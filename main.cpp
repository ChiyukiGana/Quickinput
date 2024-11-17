#include <QtWidgets/qapplication.h>
#include ".src/ui/MainUi.h"
#include ".src/ui/InstallUi.h"
#include "header.h"

void InitUI(bool zoom)
{
	// enlarge high resolution screen
	if (zoom && System::screenSize().cy > 1200) qputenv("QT_SCALE_FACTOR", QByteArray::number(System::screenZoomRote(), 10, 1));

	// symbol
	if (System::Version().dwMajorVersion >= 10)
	{
		qis.ui.syOn = u8"✅";
		qis.ui.syOff = u8"⛔";
		qis.ui.syOk = u8"⭕";
		qis.ui.syYes = u8"✔️";
		qis.ui.syNot = u8"❌";
		qis.ui.syStop = u8"🛑";
		qis.ui.syShow = u8"🔼";
		qis.ui.syHide = u8"🔽";
		qis.ui.syOption = u8"⚙";
		qis.ui.syLink = u8"🔗";
		qis.ui.syEdit = u8"🔧";
		qis.ui.syUp = u8"⬆️";
		qis.ui.syDown = u8"⬇️";
		qis.ui.syTurn = u8"🔃";
		qis.ui.syLeft = u8"🔙";
		qis.ui.syTop = u8"🔝";
		qis.ui.syRight = u8"🔜";
		qis.ui.syMove = u8"🔛";
		qis.ui.syTime = u8"⏳";
		qis.ui.syText = u8"🅰️";
		qis.ui.syLoop = u8"♾️";
		qis.ui.syColor = u8"🌈";
		qis.ui.syImage = u8"🖼";
	}
	else
	{
		qis.ui.syOn = u8"✔";
		qis.ui.syOff = u8"✘";
		qis.ui.syOk = u8"✔";
		qis.ui.syYes = u8"✔";
		qis.ui.syNot = u8"✘";
		qis.ui.syStop = u8"Ⓢ";
		qis.ui.syShow = u8"▲";
		qis.ui.syHide = u8"▼";
		qis.ui.syOption = u8"✱";
		qis.ui.syLink = u8"※";
		qis.ui.syEdit = u8"▲";
		qis.ui.syUp = u8"↑";
		qis.ui.syDown = u8"↓";
		qis.ui.syTurn = u8"◈";
		qis.ui.syLeft = u8"←";
		qis.ui.syTop = u8"↑";
		qis.ui.syRight = u8"→";
		qis.ui.syMove = u8"↘";
		qis.ui.syTime = u8"☽";
		qis.ui.syText = u8"A";
		qis.ui.syLoop = u8"↩";
		qis.ui.syColor = u8"☀";
		qis.ui.syImage = u8"☀";
	}

	// text
	{
		qis.ui.qiOn = (Path::Last(Process::runPath()) + std::wstring(std::wstring(L"　已启用") + (LPCWSTR)qis.ui.syOn.utf16()));
		qis.ui.qiOff = (Path::Last(Process::runPath()) + std::wstring(std::wstring(L"　已禁用") + (LPCWSTR)qis.ui.syOff.utf16()));
		qis.ui.muOn = (QString::fromUtf8(u8"启用") + qis.ui.syOn);
		qis.ui.muOff = (QString::fromUtf8(u8"禁用") + qis.ui.syOff);
		qis.ui.muShow = (QString::fromUtf8(u8"显示") + qis.ui.syShow);
		qis.ui.muHide = (QString::fromUtf8(u8"隐藏") + qis.ui.syHide);
		qis.ui.muExit = (QString::fromUtf8(u8"退出") + qis.ui.syStop);
		qis.ui.acDown = (QString::fromUtf8(u8"按下") + qis.ui.syDown);
		qis.ui.acUp = (QString::fromUtf8(u8"松开") + qis.ui.syUp);
		qis.ui.acClick = (QString::fromUtf8(u8"点击") + qis.ui.syTurn);
		qis.ui.acPos = (QString::fromUtf8(u8"位置") + qis.ui.syLeft);
		qis.ui.acMove = (QString::fromUtf8(u8"移动") + qis.ui.syMove);
		qis.ui.acWait = (QString::fromUtf8(u8"等待") + qis.ui.syTime);
		qis.ui.acText = (QString::fromUtf8(u8"复制") + qis.ui.syText);
		qis.ui.acLoop = (QString::fromUtf8(u8"循环") + qis.ui.syLoop);
		qis.ui.acColor = (QString::fromUtf8(u8"找色") + qis.ui.syColor);
		qis.ui.acEnd = (QString::fromUtf8(u8"结束") + qis.ui.syStop);
		qis.ui.acEndLoop = (QString::fromUtf8(u8"结束循环") + qis.ui.syStop);
		qis.ui.acKeyState = (QString::fromUtf8(u8"按键状态") + qis.ui.syStop);
		qis.ui.acRecoverPos = (QString::fromUtf8(u8"恢复位置") + qis.ui.syTurn);
		qis.ui.acImage = (QString::fromUtf8(u8"找图") + qis.ui.syImage);
		qis.ui.acPopText = (QString::fromUtf8(u8"弹出") + qis.ui.syText);
		qis.ui.trOn = (QString::fromUtf8(u8"启用") + qis.ui.syOn);
		qis.ui.trOff = (QString::fromUtf8(u8"禁用") + qis.ui.syOff);
		qis.ui.etFunc = (QString::fromUtf8(u8"动作") + qis.ui.syOption);
		qis.ui.etParam = (QString::fromUtf8(u8"参数") + qis.ui.syLink);
		qis.ui.etAdd = (QString::fromUtf8(u8"添加") + qis.ui.syYes);
		qis.ui.etDel = (QString::fromUtf8(u8"删除") + qis.ui.syNot);
		qis.ui.etChange = (QString::fromUtf8(u8"修改") + qis.ui.syOk);
		qis.ui.etEdit = (QString::fromUtf8(u8"编辑") + qis.ui.syEdit);
		qis.ui.rcStart = (QString::fromUtf8(u8"开始") + qis.ui.syOk);
		qis.ui.rcStop = (QString::fromUtf8(u8"停止") + qis.ui.syOk);
		qis.ui.rcClose = (QString::fromUtf8(u8"取消") + qis.ui.syNot);
	}

	// style
	{
		Theme theme;
		theme.name = u8"浅蓝";
		theme.style = u8R"(*{color:black;border:none;outline:none;font-size:12px;font-family:"Microsoft YaHei"}QMainWindow,QDialog{border:1px solid gray}#titleWidget{background-color:white}#titleWidget *{font-size:14px}#titleWidget QPushButton,#titleWidget QCheckBox::indicator{background-color:#CCC;width:20px;height:20px;border-radius:10px}#titleWidget QCheckBox::indicator:checked{image:url(:/checked.png)}#clientWidget{background-color:#CEF}QMenu,QToolTip,#clientWidget QLineEdit,#clientWidget QTextEdit,#clientWidget QKeyEdit,#clientWidget QCheckBox::indicator,#clientWidget QRadioButton::indicator,#clientWidget QHeaderView,#clientWidget QHeaderView::section,#clientWidget QTableWidget,#clientWidget QTableWidget QTableCornerButton::section,#clientWidget QComboBox,#clientWidget QComboBox QAbstractItemView{background-color:white;color:black}#clientWidget QPushButton,#clientWidget QTabBar::tab,#clientWidget QTabBar::tab:first,#clientWidget QTabBar::tab:last{background-color:#ADE;color:black}#clientWidget QCheckBox::indicator,#clientWidget QRadioButton::indicator{width:20px;height:20px}#clientWidget QComboBox{padding-left:3px}QMenu::item:selected,#clientWidget QPushButton:hover,#clientWidget QCheckBox::indicator:hover,#clientWidget QRadioButton::indicator:hover,#clientWidget QTableWidget::item:hover,#clientWidget QTabBar::tab:hover,#clientWidget QTabBar::tab:first:hover,#clientWidget QTabBar::tab:last:hover{background-color:#BEF;color:black}#clientWidget QTableWidget::item:selected{background-color:#CEF;color:black}#clientWidget QTabBar::tab:selected,#clientWidget QTabBar::tab:first:selected,#clientWidget QTabBar::tab:last:selected{background-color:#CEF;color:black}#clientWidget QCheckBox::indicator:checked,#clientWidget QRadioButton::indicator:checked{image:url(:/checked.png)}#clientWidget QPushButton:disabled,#clientWidget QCheckBox:disabled,#clientWidget QRadioButton:disabled{color:gray}#clientWidget #bnWndActive,#clientWidget #bnPos,#clientWidget #bnColorRect,#clientWidget #bnImageRect,#clientWidget #bnImageShot{background-color:white})";
		qis.themes.Add(theme);

		theme.name = u8"黑暗";
		theme.style = u8R"(*{color:white;border:none;outline:none;font-size:12px;font-family:"Microsoft YaHei"}QMainWindow,QDialog{border:1px solid gray}#titleWidget{background-color:black}#titleWidget *{font-size:14px}#titleWidget QPushButton,#titleWidget QCheckBox::indicator{background-color:gray;width:20px;height:20px;border-radius:10px}#titleWidget QCheckBox::indicator:checked{image:url(:/checked.png)}#clientWidget,QMenu{background-color:#333}QToolTip,#clientWidget QLineEdit,#clientWidget QTextEdit,#clientWidget QKeyEdit,#clientWidget QCheckBox::indicator,#clientWidget QRadioButton::indicator,#clientWidget QHeaderView,#clientWidget QHeaderView::section,#clientWidget QTableWidget,#clientWidget QTableWidget QTableCornerButton::section,#clientWidget QComboBox,#clientWidget QComboBox QAbstractItemView{background-color:#555;color:white}#clientWidget QPushButton,#clientWidget QTabBar::tab,#clientWidget QTabBar::tab:first,#clientWidget QTabBar::tab:last{background-color:#666;color:white}#clientWidget QCheckBox::indicator,#clientWidget QRadioButton::indicator{width:20px;height:20px}#clientWidget QComboBox{padding-left:3px}QMenu::item:selected,#clientWidget QPushButton:hover,#clientWidget QCheckBox::indicator:hover,#clientWidget QRadioButton::indicator:hover,#clientWidget QTableWidget::item:hover,#clientWidget QTabBar::tab:hover,#clientWidget QTabBar::tab:first:hover,#clientWidget QTabBar::tab:last:hover{background-color:#777;color:white}#clientWidget QTableWidget::item:selected{background-color:#888;color:white}#clientWidget QTabBar::tab:selected,#clientWidget QTabBar::tab:first:selected,#clientWidget QTabBar::tab:last:selected{background-color:#333;color:white}#clientWidget QCheckBox::indicator:checked,#clientWidget QRadioButton::indicator:checked{image:url(:/checked.png)}#clientWidget QPushButton:disabled,#clientWidget QCheckBox:disabled,#clientWidget QRadioButton:disabled{color:gray}#clientWidget #bnWndActive,#clientWidget #bnPos,#clientWidget #bnColorRect,#clientWidget #bnImageRect,#clientWidget #bnImageShot{background-color:#666})";
		qis.themes.Add(theme);

		theme.name = u8"浅红";
		theme.style = u8R"(*{color:black;border:none;outline:none;font-size:12px;font-family:"Microsoft YaHei"}QMainWindow,QDialog{border:1px solid gray}#titleWidget{background-color:white}#titleWidget *{font-size:14px}#titleWidget QPushButton,#titleWidget QCheckBox::indicator{background-color:#AAA;width:20px;height:20px;border-radius:10px}#titleWidget QCheckBox::indicator:checked{image:url(:/checked.png)}#clientWidget{background-color:#FDF}QMenu,QToolTip,#clientWidget QLineEdit,#clientWidget QTextEdit,#clientWidget QKeyEdit,#clientWidget QCheckBox::indicator,#clientWidget QRadioButton::indicator,#clientWidget QHeaderView,#clientWidget QHeaderView::section,#clientWidget QTableWidget,#clientWidget QTableWidget QTableCornerButton::section,#clientWidget QComboBox,#clientWidget QComboBox QAbstractItemView{background-color:white;color:black}#clientWidget QPushButton,#clientWidget QTabBar::tab,#clientWidget QTabBar::tab:first,#clientWidget QTabBar::tab:last{background-color:#FCE;color:black}#clientWidget QCheckBox::indicator,#clientWidget QRadioButton::indicator{width:20px;height:20px}#clientWidget QComboBox{padding-left:3px}QMenu::item:selected,#clientWidget QPushButton:hover,#clientWidget QCheckBox::indicator:hover,#clientWidget QRadioButton::indicator:hover,#clientWidget QTableWidget::item:hover,#clientWidget QTabBar::tab:hover,#clientWidget QTabBar::tab:first:hover,#clientWidget QTabBar::tab:last:hover{background-color:#FBE;color:black}#clientWidget QTableWidget::item:selected{background-color:#FCE;color:black}#clientWidget QTabBar::tab:selected,#clientWidget QTabBar::tab:first:selected,#clientWidget QTabBar::tab:last:selected{background-color:#FDF;color:black}#clientWidget QCheckBox::indicator:checked,#clientWidget QRadioButton::indicator:checked{image:url(:/checked.png)}#clientWidget QPushButton:disabled,#clientWidget QCheckBox:disabled,#clientWidget QRadioButton:disabled{color:gray}#clientWidget #bnWndActive,#clientWidget #bnPos,#clientWidget #bnColorRect,#clientWidget #bnImageRect,#clientWidget #bnImageShot{background-color:white})";
		qis.themes.Add(theme);
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

// if QuickInput.json is not found
void WriteDefaultMacro()
{
	if (!File::FileState(L"QuickInput.json"))
	{
		std::string json = u8R"({"document_charset":"UTF8","defOn":true,"key":119,"recKey":119,"showTips":true,"audFx":false,"minMode":false,"zoomBlock":false,"quickClickKey":1,"quickClickState":false,"quickClickDelay":200,"quickClickMode":0,"showClockKey":18,"showClockState":false,"wndActiveState":false,"wndActiveName":""})";
		File::TextSaveU(L"QuickInput.json", json);

		std::string click = u8R"({"document_ charset":"UTF8","type":"QuickInputMacro","wndState":0,"wndChild":0,"wndName":"","wndClass":"","state":false,"block":true,"mode":0,"key":1,"count":0,"actions":[{"mark":"","type":3,"state":1,"vk":1},{"mark":"范围30~70","type":2,"ms":50,"ex":20},{"mark":"","type":3,"state":0,"vk":1},{"mark":"50-20~50+20","type":2,"ms":50,"ex":20}],"actionsEnding":[{"mark":"","type":3,"state":0,"vk":1}]})";
		std::string state = u8R"({"document_ charset":"UTF8","type":"QuickInputMacro","wndState":0,"wndChild":0,"wndName":"","wndClass":"","state":false,"block":true,"mode":0,"key":1,"count":0,"actions":[{"mark":"","type":3,"state":1,"vk":1},{"mark":"","type":7,"count":0,"rand":0,"next":[{"mark":"","type":2,"ms":100,"ex":0}]}],"actionsEnding":[{"mark":"","type":3,"state":0,"vk":1}]})";
		std::string pullDown = u8R"({"document_ charset":"UTF8","type":"QuickInputMacro","wndState":0,"wndChild":0,"wndName":"","wndClass":"","state":false,"block":false,"mode":1,"key":1,"count":0,"actions":[{"mark":"速度","type":4,"move":true,"x":0,"y":10,"ex":0},{"mark":"精度","type":2,"ms":10,"ex":0}]})";
		std::string message = u8R"({"document_ charset":"UTF8","type":"QuickInputMacro","wndState":0,"wndChild":0,"wndName":"","wndClass":"","state":false,"block":true,"mode":0,"key":17,"count":0,"actions":[{"mark":"复制到剪贴板","type":5,"text":"这些文本将复制到剪贴板，之后通过Ctrl + V粘贴到对应位置。"},{"mark":"","type":3,"state":1,"vk":17},{"mark":"粘贴","type":3,"state":2,"vk":86},{"mark":"","type":3,"state":0,"vk":17},{"mark":"回车发送","type":3,"state":2,"vk":13},{"mark":"","type":2,"ms":200,"ex":0}]})";
		File::FolderCreate(L"macro");
		std::wstring path = Process::runPath();
		File::TextSaveU(Path::Append(path, L"macro\\连点.json"), click);
		File::TextSaveU(Path::Append(path, L"macro\\长按.json"), state);
		File::TextSaveU(Path::Append(path, L"macro\\压枪.json"), pullDown);
		File::TextSaveU(Path::Append(path, L"macro\\消息.json"), message);
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

	// write default >> load config >> init zoom >> qapplication >> start

	WriteDefaultMacro(); // #1
	QiFn::LoadJson(); // #2
	InitUI(!qis.set.zoomBlock); // #3
	QApplication app(argc, argv); app.setFont(QFont("Microsoft YaHei")); // #4
	Install(); // #5

	qis.ReScreen(); // screen size
	MsgPop::Init(); // thread of MsgPop
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