﻿#include <QtWidgets/qapplication.h>
#include ".h/MainUi.h"
#include ".h/InstallUi.h"
#include "static.h"
void InitUI(bool zoom)
{
	// enlarge high resolution screen
	if (zoom && System::screenSize().cy > 1200) qputenv("QT_SCALE_FACTOR", QByteArray::number(System::screenZoomRote(), 10, 1));

	// symbol
	if (System::Version().dwMajorVersion >= 10)
	{
		UI::syOn = u8"✅";
		UI::syOff = u8"⛔";
		UI::syOk = u8"⭕";
		UI::syYes = u8"✔️";
		UI::syNot = u8"❌";
		UI::syStop = u8"🛑";
		UI::syShow = u8"🔼";
		UI::syHide = u8"🔽";
		UI::syOption = u8"⚙";
		UI::syLink = u8"🔗";
		UI::syEdit = u8"🔧";
		UI::syUp = u8"⬆️";
		UI::syDown = u8"⬇️";
		UI::syTurn = u8"🔃";
		UI::syLeft = u8"🔙";
		UI::syTop = u8"🔝";
		UI::syRight = u8"🔜";
		UI::syMove = u8"🔛";
		UI::syTime = u8"⏳";
		UI::syText = u8"🅰️";
		UI::syLoop = u8"♾️";
		UI::syColor = u8"🌈";
		UI::syImage = u8"🖼";
	}
	else
	{
		UI::syOn = u8"✔";
		UI::syOff = u8"✘";
		UI::syOk = u8"✔";
		UI::syYes = u8"✔";
		UI::syNot = u8"✘";
		UI::syStop = u8"Ⓢ";
		UI::syShow = u8"▲";
		UI::syHide = u8"▼";
		UI::syOption = u8"✱";
		UI::syLink = u8"※";
		UI::syEdit = u8"▲";
		UI::syUp = u8"↑";
		UI::syDown = u8"↓";
		UI::syTurn = u8"◈";
		UI::syLeft = u8"←";
		UI::syTop = u8"↑";
		UI::syRight = u8"→";
		UI::syMove = u8"↘";
		UI::syTime = u8"☽";
		UI::syText = u8"A";
		UI::syLoop = u8"↩";
		UI::syColor = u8"☀";
		UI::syImage = u8"☀";
	}

	// text
	{
		UI::qiOn = (Path::Last(Process::runPath()) + std::wstring(std::wstring(L"　已启用") + (LPCWSTR)UI::syOn.utf16()));
		UI::qiOff = (Path::Last(Process::runPath()) + std::wstring(std::wstring(L"　已禁用") + (LPCWSTR)UI::syOff.utf16()));
		UI::muOn = (QString::fromUtf8(u8"启用") + UI::syOn);
		UI::muOff = (QString::fromUtf8(u8"禁用") + UI::syOff);
		UI::muShow = (QString::fromUtf8(u8"显示") + UI::syShow);
		UI::muHide = (QString::fromUtf8(u8"隐藏") + UI::syHide);
		UI::muExit = (QString::fromUtf8(u8"退出") + UI::syStop);
		UI::acDown = (QString::fromUtf8(u8"按下") + UI::syDown);
		UI::acUp = (QString::fromUtf8(u8"松开") + UI::syUp);
		UI::acClick = (QString::fromUtf8(u8"点击") + UI::syTurn);
		UI::acPos = (QString::fromUtf8(u8"位置") + UI::syLeft);
		UI::acMove = (QString::fromUtf8(u8"移动") + UI::syMove);
		UI::acWait = (QString::fromUtf8(u8"等待") + UI::syTime);
		UI::acText = (QString::fromUtf8(u8"复制") + UI::syText);
		UI::acLoop = (QString::fromUtf8(u8"循环") + UI::syLoop);
		UI::acColor = (QString::fromUtf8(u8"找色") + UI::syColor);
		UI::acEnd = (QString::fromUtf8(u8"结束") + UI::syStop);
		UI::acEndLoop = (QString::fromUtf8(u8"结束循环") + UI::syStop);
		UI::acKeyState = (QString::fromUtf8(u8"按键状态") + UI::syStop);
		UI::acRecoverPos = (QString::fromUtf8(u8"恢复位置") + UI::syTurn);
		UI::acImage = (QString::fromUtf8(u8"找图") + UI::syImage);
		UI::acPopText = (QString::fromUtf8(u8"弹出") + UI::syText);
		UI::trOn = (QString::fromUtf8(u8"启用") + UI::syOn);
		UI::trOff = (QString::fromUtf8(u8"禁用") + UI::syOff);
		UI::etFunc = (QString::fromUtf8(u8"动作") + UI::syOption);
		UI::etParam = (QString::fromUtf8(u8"参数") + UI::syLink);
		UI::etAdd = (QString::fromUtf8(u8"添加") + UI::syYes);
		UI::etDel = (QString::fromUtf8(u8"删除") + UI::syNot);
		UI::etChange = (QString::fromUtf8(u8"修改") + UI::syOk);
		UI::etEdit = (QString::fromUtf8(u8"编辑") + UI::syEdit);
		UI::rcStart = (QString::fromUtf8(u8"开始") + UI::syOk);
		UI::rcStop = (QString::fromUtf8(u8"停止") + UI::syOk);
		UI::rcClose = (QString::fromUtf8(u8"取消") + UI::syNot);
	}

	// style
	{
		Style style;
		style.name = u8"浅蓝";
		style.style = u8R"(*{color:black;border:none;outline:none;font-size:12px;font-family:"Microsoft YaHei"}QMainWindow,QDialog{border:1px solid gray}#titleWidget{background-color:white}#titleWidget *{font-size:14px}#titleWidget QPushButton,#titleWidget QCheckBox::indicator{background-color:#CCC;width:20px;height:20px;border-radius:10px}#titleWidget QCheckBox::indicator:checked{image:url(:/checked.png)}#clientWidget{background-color:#CEF}QMenu,QToolTip,#clientWidget QLineEdit,#clientWidget QTextEdit,#clientWidget QKeyEdit,#clientWidget QCheckBox::indicator,#clientWidget QRadioButton::indicator,#clientWidget QHeaderView,#clientWidget QHeaderView::section,#clientWidget QTableWidget,#clientWidget QTableWidget QTableCornerButton::section,#clientWidget QComboBox,#clientWidget QComboBox QAbstractItemView{background-color:white;color:black}#clientWidget QPushButton,#clientWidget QTabBar::tab,#clientWidget QTabBar::tab:first,#clientWidget QTabBar::tab:last{background-color:#ADE;color:black}#clientWidget QCheckBox::indicator,#clientWidget QRadioButton::indicator{width:20px;height:20px}#clientWidget QComboBox{padding-left:3px}QMenu::item:selected,#clientWidget QPushButton:hover,#clientWidget QCheckBox::indicator:hover,#clientWidget QRadioButton::indicator:hover,#clientWidget QTableWidget::item:hover,#clientWidget QTabBar::tab:hover,#clientWidget QTabBar::tab:first:hover,#clientWidget QTabBar::tab:last:hover{background-color:#BEF;color:black}#clientWidget QTableWidget::item:selected{background-color:#CEF;color:black}#clientWidget QTabBar::tab:selected,#clientWidget QTabBar::tab:first:selected,#clientWidget QTabBar::tab:last:selected{background-color:#CEF;color:black}#clientWidget QCheckBox::indicator:checked,#clientWidget QRadioButton::indicator:checked{image:url(:/checked.png)}#clientWidget QPushButton:disabled,#clientWidget QCheckBox:disabled,#clientWidget QRadioButton:disabled{color:gray}#clientWidget #bnWndActive,#clientWidget #bnPos,#clientWidget #bnColorRect,#clientWidget #bnImageRect,#clientWidget #bnImageShot{background-color:white})";
		Global::qi.styles.Add(style);

		style.name = u8"黑暗";
		style.style = u8R"(*{color:white;border:none;outline:none;font-size:12px;font-family:"Microsoft YaHei"}QMainWindow,QDialog{border:1px solid gray}#titleWidget{background-color:black}#titleWidget *{font-size:14px}#titleWidget QPushButton,#titleWidget QCheckBox::indicator{background-color:gray;width:20px;height:20px;border-radius:10px}#titleWidget QCheckBox::indicator:checked{image:url(:/checked.png)}#clientWidget,QMenu{background-color:#333}QToolTip,#clientWidget QLineEdit,#clientWidget QTextEdit,#clientWidget QKeyEdit,#clientWidget QCheckBox::indicator,#clientWidget QRadioButton::indicator,#clientWidget QHeaderView,#clientWidget QHeaderView::section,#clientWidget QTableWidget,#clientWidget QTableWidget QTableCornerButton::section,#clientWidget QComboBox,#clientWidget QComboBox QAbstractItemView{background-color:#555;color:white}#clientWidget QPushButton,#clientWidget QTabBar::tab,#clientWidget QTabBar::tab:first,#clientWidget QTabBar::tab:last{background-color:#666;color:white}#clientWidget QCheckBox::indicator,#clientWidget QRadioButton::indicator{width:20px;height:20px}#clientWidget QComboBox{padding-left:3px}QMenu::item:selected,#clientWidget QPushButton:hover,#clientWidget QCheckBox::indicator:hover,#clientWidget QRadioButton::indicator:hover,#clientWidget QTableWidget::item:hover,#clientWidget QTabBar::tab:hover,#clientWidget QTabBar::tab:first:hover,#clientWidget QTabBar::tab:last:hover{background-color:#777;color:white}#clientWidget QTableWidget::item:selected{background-color:#888;color:white}#clientWidget QTabBar::tab:selected,#clientWidget QTabBar::tab:first:selected,#clientWidget QTabBar::tab:last:selected{background-color:#333;color:white}#clientWidget QCheckBox::indicator:checked,#clientWidget QRadioButton::indicator:checked{image:url(:/checked.png)}#clientWidget QPushButton:disabled,#clientWidget QCheckBox:disabled,#clientWidget QRadioButton:disabled{color:gray}#clientWidget #bnWndActive,#clientWidget #bnPos,#clientWidget #bnColorRect,#clientWidget #bnImageRect,#clientWidget #bnImageShot{background-color:#666})";
		Global::qi.styles.Add(style);

		style.name = u8"浅红";
		style.style = u8R"(*{color:black;border:none;outline:none;font-size:12px;font-family:"Microsoft YaHei"}QMainWindow,QDialog{border:1px solid gray}#titleWidget{background-color:white}#titleWidget *{font-size:14px}#titleWidget QPushButton,#titleWidget QCheckBox::indicator{background-color:#AAA;width:20px;height:20px;border-radius:10px}#titleWidget QCheckBox::indicator:checked{image:url(:/checked.png)}#clientWidget{background-color:#FDF}QMenu,QToolTip,#clientWidget QLineEdit,#clientWidget QTextEdit,#clientWidget QKeyEdit,#clientWidget QCheckBox::indicator,#clientWidget QRadioButton::indicator,#clientWidget QHeaderView,#clientWidget QHeaderView::section,#clientWidget QTableWidget,#clientWidget QTableWidget QTableCornerButton::section,#clientWidget QComboBox,#clientWidget QComboBox QAbstractItemView{background-color:white;color:black}#clientWidget QPushButton,#clientWidget QTabBar::tab,#clientWidget QTabBar::tab:first,#clientWidget QTabBar::tab:last{background-color:#FCE;color:black}#clientWidget QCheckBox::indicator,#clientWidget QRadioButton::indicator{width:20px;height:20px}#clientWidget QComboBox{padding-left:3px}QMenu::item:selected,#clientWidget QPushButton:hover,#clientWidget QCheckBox::indicator:hover,#clientWidget QRadioButton::indicator:hover,#clientWidget QTableWidget::item:hover,#clientWidget QTabBar::tab:hover,#clientWidget QTabBar::tab:first:hover,#clientWidget QTabBar::tab:last:hover{background-color:#FBE;color:black}#clientWidget QTableWidget::item:selected{background-color:#FCE;color:black}#clientWidget QTabBar::tab:selected,#clientWidget QTabBar::tab:first:selected,#clientWidget QTabBar::tab:last:selected{background-color:#FDF;color:black}#clientWidget QCheckBox::indicator:checked,#clientWidget QRadioButton::indicator:checked{image:url(:/checked.png)}#clientWidget QPushButton:disabled,#clientWidget QCheckBox:disabled,#clientWidget QRadioButton:disabled{color:gray}#clientWidget #bnWndActive,#clientWidget #bnPos,#clientWidget #bnColorRect,#clientWidget #bnImageRect,#clientWidget #bnImageShot{background-color:white})";
		Global::qi.styles.Add(style);
	}
}

void TriggerKey(BYTE vk)
{
	// state swtich
	if (vk == (Global::qi.set.key & 0xFFFF) || vk == (Global::qi.set.key >> 16))
	{
		bool k1 = Global::keyState[Global::qi.set.key & 0xFFFF];
		bool k2 = true;
		if (Global::qi.set.key >> 16) k2 = Global::keyState[Global::qi.set.key >> 16];
		if (k1 && k2)
		{
			if (Global::qi.state) QiState(0);
			else Global::qi.ReScreen(), QiState(1);
		}
	}

	// show clock
	if (Global::qi.fun.showClock.state && Global::qi.fun.showClock.key == vk && Global::keyState[Global::qi.fun.showClock.key]) { TipsWindow::Popup(Time::toWStringT()); }

	if (!Global::qi.run) return;

	// quick click
	if (Global::qi.fun.quickClick.state && Global::qi.fun.quickClick.key == vk)
	{
		if (Global::keyState[Global::qi.fun.quickClick.key])
		{
			if (Global::qi.fun.quickClick.mode) // switch mode
			{
				if (Global::qi.fun.quickClick.thread)
				{
					TerminateThread(Global::qi.fun.quickClick.thread, 0); Global::qi.fun.quickClick.thread = 0;
					Thread::Start(ThreadRelease, (PVOID)Global::qi.fun.quickClick.key);
					if (Global::qi.set.showTips) TipsWindow::Popup(std::wstring(Input::Name(Global::qi.fun.quickClick.key)) + L"ㅤ连点结束", RGB(0xFF, 0xFF, 0x60));
				}
				else
				{
					Global::qi.fun.quickClick.thread = Thread::Start(ThreadQuickClick);
					if (Global::qi.set.showTips) TipsWindow::Popup(std::wstring(Input::Name(Global::qi.fun.quickClick.key)) + L"ㅤ连点开始", RGB(0xFF, 0xAF, 0xEF));
				}
			}
			else // press mode
			{
				if (!Global::qi.fun.quickClick.thread)
				{
					Global::qi.fun.quickClick.thread = Thread::Start(ThreadQuickClick);
					if (Global::qi.set.showTips) TipsWindow::Popup(std::wstring(Input::Name(Global::qi.fun.quickClick.key)) + L"ㅤ连点开始", RGB(0x20, 0xFF, 0x40));
				}
			}
		}
		else
		{
			if (!Global::qi.fun.quickClick.mode) // press mode
			{
				if (Global::qi.fun.quickClick.thread)
				{
					TerminateThread(Global::qi.fun.quickClick.thread, 0); Global::qi.fun.quickClick.thread = 0;
					Thread::Start(ThreadRelease, (LPVOID)Global::qi.fun.quickClick.key);
					if (Global::qi.set.showTips) TipsWindow::Popup(std::wstring(Input::Name(Global::qi.fun.quickClick.key)) + L"ㅤ连点结束", RGB(0xFF, 0xFF, 0x60));
				}
			}
		}
	}

	// macros
	for (uint32 i = 0; i < Global::qi.macros.size(); i++)
	{
		if (vk == (Global::qi.macros.at(i).key & 0xFFFF) || vk == (Global::qi.macros.at(i).key >> 16))
		{
			if (Global::qi.macros.at(i).state)
			{
				bool k1 = Global::keyState[Global::qi.macros.at(i).key & 0xFFFF];
				bool k2 = true;
				if (Global::qi.macros.at(i).key >> 16) k2 = Global::keyState[Global::qi.macros.at(i).key >> 16];

				switch (Global::qi.macros.at(i).mode)
				{
				case Macro::sw:
				{
					if (k1 && k2) // trigger keys is pressed
					{
						if (Global::qi.macros.at(i).thread) // Running
						{
							// Close running
							Thread::Close(Global::qi.macros.at(i).thread); Global::qi.macros.at(i).thread = 0;
							// Start ending
							if (Global::qi.macros.at(i).actionsEnding.size()) Global::qi.macros.at(i).threadEnding = Thread::Start(ThreadMacroEnding, &Global::qi.macros.at(i));
							// Show Tips
							if (Global::qi.set.showTips) TipsWindow::Popup(Global::qi.macros.at(i).name + L"ㅤ结束", RGB(0xFF, 0xFF, 0x60));
						}
						else // None or ending
						{
							// Close ending
							Thread::Close(Global::qi.macros.at(i).threadEnding); Global::qi.macros.at(i).threadEnding = 0;
							// Start running
							Global::qi.macros.at(i).thread = Thread::Start(ThreadMacro, &Global::qi.macros.at(i));
							if (Global::qi.set.showTips) TipsWindow::Popup(Global::qi.macros.at(i).name + L"ㅤ开始", RGB(0xFF, 0xAF, 0xEF));
						}
					}
				}
				break;

				case Macro::down:
				{
					if (k1 && k2) // trigger keys is pressed
					{
						// Close running
						Thread::Close(Global::qi.macros.at(i).thread); Global::qi.macros.at(i).thread = 0;
						// Close ending
						Thread::Close(Global::qi.macros.at(i).threadEnding); Global::qi.macros.at(i).threadEnding = 0;
						// Start running
						Global::qi.macros.at(i).thread = Thread::Start(ThreadMacro, &Global::qi.macros.at(i));
						// Show Tips
						if (Global::qi.set.showTips)
						{
							if (Global::qi.macros.at(i).count) TipsWindow::Popup(Global::qi.macros.at(i).name + L"ㅤ" + String::toWString(Global::qi.macros.at(i).count) + L"次", RGB(0x20, 0xFF, 0x40));
							else TipsWindow::Popup(Global::qi.macros.at(i).name + L"ㅤ循环", RGB(0x20, 0xFF, 0x40));
						}
					}
					else // trigger keys is released
					{
						if (Global::qi.macros.at(i).thread && Global::qi.macros.at(i).count == 0)
						{
							// Close running
							Thread::Close(Global::qi.macros.at(i).thread); Global::qi.macros.at(i).thread = 0;
							// Start ending
							if (Global::qi.macros.at(i).actionsEnding.size()) Global::qi.macros.at(i).threadEnding = Thread::Start(ThreadMacroEnding, &Global::qi.macros.at(i));
							// Show Tips
							if (Global::qi.set.showTips) TipsWindow::Popup(Global::qi.macros.at(i).name + L"ㅤ结束", RGB(0xFF, 0xFF, 0x60));
						}
					}
				}
				break;

				case Macro::up:
				{
					if (k1 && k2) // trigger keys is pressed
					{
						Global::qi.macros.at(i).active = true;
						if (Global::qi.macros.at(i).thread && Global::qi.macros.at(i).count == 0)
						{
							// Close running
							Thread::Close(Global::qi.macros.at(i).thread); Global::qi.macros.at(i).thread = 0;
							// Start ending
							if (Global::qi.macros.at(i).actionsEnding.size()) Global::qi.macros.at(i).threadEnding = Thread::Start(ThreadMacroEnding, &Global::qi.macros.at(i));
							// Show Tips
							if (Global::qi.set.showTips) TipsWindow::Popup(Global::qi.macros.at(i).name + L"ㅤ结束", RGB(0xFF, 0xFF, 0x60));
						}
					}
					else // trigger keys is released
					{
						if (Global::qi.macros.at(i).active)
						{
							Global::qi.macros.at(i).active = false;

							// Close running
							Thread::Close(Global::qi.macros.at(i).thread); Global::qi.macros.at(i).thread = 0;
							// Close ending
							Thread::Close(Global::qi.macros.at(i).threadEnding); Global::qi.macros.at(i).threadEnding = 0;
							// Start running
							Global::qi.macros.at(i).thread = Thread::Start(ThreadMacro, &Global::qi.macros.at(i));
							// Show Tips
							if (Global::qi.set.showTips)
							{
								if (Global::qi.macros.at(i).count) TipsWindow::Popup(Global::qi.macros.at(i).name + L"ㅤ" + String::toWString(Global::qi.macros.at(i).count) + L"次", RGB(0xA0, 0xFF, 0x50));
								else TipsWindow::Popup(Global::qi.macros.at(i).name + L"ㅤ循环", RGB(0xA0, 0xFF, 0x50));
							}
						}
					}
				}
				break;

				}
			}
		}
	}
}

InputHookProc()
{
	BYTE key = Input::Convert(vk);
	if (*exInfo == 214) // from Quick input
	{
		*exInfo = 0; // remove ex info
		return false;
	}
	else if (key) // other input
	{
		if (state) // down
		{
			if (!Global::keyState[key]) // set state
			{
				Global::keyState[key] = true;
				if (Global::qi.rec)
				{
					if (Global::qi.set.recKey == key) return true;
					else if (((RecordUi*)Global::qi.rec)->State()) ((RecordUi*)Global::qi.rec)->AddItems(key, state, msPt);
				}
				else
				{
					TriggerKey(key);
				}
			}
		}
		else // up
		{
			Global::keyState[key] = false; // reset state
			if (Global::qi.rec)
			{
				if (Global::qi.set.recKey == key) { ((RecordUi*)Global::qi.rec)->BeginRec(); return true; }
				else if (((RecordUi*)Global::qi.rec)->State()) ((RecordUi*)Global::qi.rec)->AddItems(key, state, msPt);
			}
			else
			{
				TriggerKey(key);
			}
		}
		if (Global::qi.run) for (uint32 u = 0; u < Global::trBlock.size(); u++) if (Global::trBlock[u] == key) return true; // block trigger key
	}
	return false;
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
		std::string pullDown = u8R"({"document_ charset":"UTF8","type":"QuickInputMacro","wndState":0,"wndChild":0,"wndName":"","wndClass":"","state":false,"block":false,"mode":1,"key":1,"count":0,"actions":[{"mark":"速度","type":4,"move":1,"x":0,"y":10,"ex":0},{"mark":"精度","type":2,"ms":10,"ex":0}]})";
		std::string message = u8R"({"document_ charset":"UTF8","type":"QuickInputMacro","wndState":0,"wndChild":0,"wndName":"","wndClass":"","state":false,"block":true,"mode":0,"key":17,"count":0,"actions":[{"mark":"复制到剪贴板","type":5,"text":"这些文本将复制到剪贴板，之后通过Ctrl + V粘贴到对应位置。"},{"mark":"","type":3,"state":1,"vk":17},{"mark":"粘贴","type":3,"state":2,"vk":86},{"mark":"","type":3,"state":0,"vk":17},{"mark":"回车发送","type":3,"state":2,"vk":13},{"mark":"","type":2,"ms":200,"ex":0}]})";
		std::string window = u8R"({"document_ charset":"UTF8","type":"QuickInputMacro","wndState":0,"wndChild":0,"wndName":"","wndClass":"","state":false,"block":false,"mode":1,"key":16,"count":1,"actions":[{"mark":"","type":4,"move":0,"x":9900,"y":100,"ex":0},{"mark":"","type":3,"state":2,"vk":1},{"mark":"","type":2,"ms":100,"ex":0},{"mark":"","type":1},{"mark":"","type":12,"text":"*以下为说明，实际效果为前两行","time":1000},{"mark":"","type":12,"text":"测试使用浏览器，后台点击右上角关闭","time":1000},{"mark":"","type":12,"text":"1：点击标题栏右侧的+选择你的浏览器","time":1000},{"mark":"","type":12,"text":"2：点击第一行[位置]，如果准心在浏览器关闭处即可","time":1000},{"mark":"","type":12,"text":"2.1：如果不是，请双击[位置]并重新选择","time":1000},{"mark":"","type":12,"text":"3：关闭此窗口，之后按触发键（Shift）来测试","time":1000}]})";
		File::FolderCreate(L"macro");
		std::wstring path = Process::runPath();
		File::TextSaveU(Path::Append(path, L"macro\\连点.json"), click);
		File::TextSaveU(Path::Append(path, L"macro\\长按.json"), state);
		File::TextSaveU(Path::Append(path, L"macro\\压枪.json"), pullDown);
		File::TextSaveU(Path::Append(path, L"macro\\消息.json"), message);
		File::TextSaveU(Path::Append(path, L"macro\\后台.json"), window);
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
	LoadJson(); // #2
	InitUI(!Global::qi.set.zoomBlock); // #3
	QApplication app(argc, argv); app.setFont(QFont("Microsoft YaHei")); // #4
	Install(); // #5

	Thread::Start(TipsWindow::TipsWindowThread);
	MainUi wnd; Global::qi.main = &wnd;
	if (Global::qi.set.minMode)
	{
		HookState(true);
		if (Global::qi.set.defOn) QiState(true);
	}
	else wnd.show();
	app.exec();

	return 0;
}