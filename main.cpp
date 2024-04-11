#include <QtWidgets/qapplication.h>
#include ".h/MainUi.h"
#include ".h/InstallUi.h"
#include "static.h"

#ifdef _DEBUG
DMsgWnd();
#endif

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
	for (uint32 n = 0; n < Global::qi.macros.size(); n++)
	{
		if (vk == (Global::qi.macros[n].key & 0xFFFF) || vk == (Global::qi.macros[n].key >> 16))
		{
			if (Global::qi.macros[n].state)
			{
				bool k1 = Global::keyState[Global::qi.macros[n].key & 0xFFFF];
				bool k2 = true;
				if (Global::qi.macros[n].key >> 16) k2 = Global::keyState[Global::qi.macros[n].key >> 16];

				switch (Global::qi.macros[n].mode)
				{
				case Macro::sw:
				{
					if (k1 && k2) // trigger keys is pressed
					{
						if (Global::qi.macros[n].thread) // Running
						{
							// Close running
							Thread::Close(Global::qi.macros[n].thread); Global::qi.macros[n].thread = 0;
							// Start ending
							if (Global::qi.macros[n].actionsEnding.size()) Global::qi.macros[n].threadEnding = Thread::Start(ThreadMacroEnding, &Global::qi.macros[n]);
							// Show Tips
							if (Global::qi.set.showTips) TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ结束", RGB(0xFF, 0xFF, 0x60));
						}
						else // None or ending
						{
							// Close ending
							Thread::Close(Global::qi.macros[n].threadEnding); Global::qi.macros[n].threadEnding = 0;
							// Start running
							Global::qi.macros[n].thread = Thread::Start(ThreadMacro, &Global::qi.macros[n]);
							if (Global::qi.set.showTips) TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ开始", RGB(0xFF, 0xAF, 0xEF));
						}
					}
				}
				break;

				case Macro::down:
				{
					if (k1 && k2) // trigger keys is pressed
					{
						// Close running
						Thread::Close(Global::qi.macros[n].thread); Global::qi.macros[n].thread = 0;
						// Close ending
						Thread::Close(Global::qi.macros[n].threadEnding); Global::qi.macros[n].threadEnding = 0;
						// Start running
						Global::qi.macros[n].thread = Thread::Start(ThreadMacro, &Global::qi.macros[n]);
						// Show Tips
						if (Global::qi.set.showTips)
						{
							if (Global::qi.macros[n].count) TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ" + String::toWString(Global::qi.macros[n].count) + L"次", RGB(0x20, 0xFF, 0x40));
							else TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ循环", RGB(0x20, 0xFF, 0x40));
						}
					}
					else // trigger keys is released
					{
						if (Global::qi.macros[n].thread && Global::qi.macros[n].count == 0)
						{
							// Close running
							Thread::Close(Global::qi.macros[n].thread); Global::qi.macros[n].thread = 0;
							// Start ending
							if (Global::qi.macros[n].actionsEnding.size()) Global::qi.macros[n].threadEnding = Thread::Start(ThreadMacroEnding, &Global::qi.macros[n]);
							// Show Tips
							if (Global::qi.set.showTips) TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ结束", RGB(0xFF, 0xFF, 0x60));
						}
					}
				}
				break;

				case Macro::up:
				{
					if (k1 && k2) // trigger keys is pressed
					{
						Global::qi.macros[n].active = true;
						if (Global::qi.macros[n].thread && Global::qi.macros[n].count == 0)
						{
							// Close running
							Thread::Close(Global::qi.macros[n].thread); Global::qi.macros[n].thread = 0;
							// Start ending
							if (Global::qi.macros[n].actionsEnding.size()) Global::qi.macros[n].threadEnding = Thread::Start(ThreadMacroEnding, &Global::qi.macros[n]);
							// Show Tips
							if (Global::qi.set.showTips) TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ结束", RGB(0xFF, 0xFF, 0x60));
						}
					}
					else // trigger keys is released
					{
						if (Global::qi.macros[n].active)
						{
							Global::qi.macros[n].active = false;

							// Close running
							Thread::Close(Global::qi.macros[n].thread); Global::qi.macros[n].thread = 0;
							// Close ending
							Thread::Close(Global::qi.macros[n].threadEnding); Global::qi.macros[n].threadEnding = 0;
							// Start running
							Global::qi.macros[n].thread = Thread::Start(ThreadMacro, &Global::qi.macros[n]);
							// Show Tips
							if (Global::qi.set.showTips)
							{
								if (Global::qi.macros[n].count) TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ" + String::toWString(Global::qi.macros[n].count) + L"次", RGB(0xA0, 0xFF, 0x50));
								else TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ循环", RGB(0xA0, 0xFF, 0x50));
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
		std::wstring json = LR"({"document_charset":"UTF8","defOn":true,"key":119,"recKey":119,"showTips":true,"audFx":false,"minMode":false,"zoomBlock":false,"quickClickKey":1,"quickClickState":false,"quickClickDelay":200,"quickClickMode":0,"showClockKey":18,"showClockState":false,"wndActiveState":false,"wndActiveName":""})";
		File::TextSave(L"QuickInput.json", json);
		
		std::wstring click = LR"({"documen_ charset":"UTF8","wndState":false,"wndChild":false,"wndName":"","wndClass":"","state":false,"block":true,"mode":0,"key":1,"count":0,"actions":[{"mark":"","type":3,"state":1,"vk":1},{"mark":"范围30~70","type":2,"ms":50,"ex":20},{"mark":"","type":3,"state":0,"vk":1},{"mark":"50-20~50+20","type":2,"ms":50,"ex":20}],"actionsEnding":[{"mark":"","type":3,"state":0,"vk":1}]})";
		std::wstring state = LR"({"documen_ charset":"UTF8","wndState":false,"wndChild":false,"wndName":"","wndClass":"","state":false,"block":true,"mode":0,"key":1,"count":0,"actions":[{"mark":"","type":3,"state":1,"vk":1},{"mark":"","type":7,"count":0,"rand":0,"next":[{"mark":"","type":2,"ms":100,"ex":0}]}],"actionsEnding":[{"mark":"","type":3,"state":0,"vk":1}]})";
		std::wstring pullDown = LR"({"documen_ charset":"UTF8","wndState":false,"wndChild":false,"wndName":"","wndClass":"","state":false,"block":false,"mode":1,"key":1,"count":0,"actions":[{"mark":"速度","type":4,"move":true,"x":0,"y":10,"ex":0},{"mark":"精度","type":2,"ms":10,"ex":0}]})";
		std::wstring message = LR"({"documen_ charset":"UTF8","wndState":false,"wndChild":false,"wndName":"","wndClass":"","state":false,"block":true,"mode":1,"key":17,"count":0,"actions":[{"mark":"复制到剪贴板","type":5,"text":"这些文本将复制到剪贴板，之后通过Ctrl + V粘贴到对应位置。"},{"mark":"","type":3,"state":1,"vk":17},{"mark":"粘贴","type":3,"state":2,"vk":86},{"mark":"","type":3,"state":0,"vk":17},{"mark":"回车发送","type":3,"state":2,"vk":13},{"mark":"","type":2,"ms":200,"ex":0}]})";
		std::wstring window = LR"({"documen_ charset":"UTF8","wndState":true,"wndChild":false,"wndName":"","wndClass":"","state":false,"block":false,"mode":1,"key":16,"count":1,"actions":[{"mark":"","type":4,"move":false,"x":9900,"y":100,"ex":0},{"mark":"","type":3,"state":2,"vk":1},{"mark":"","type":2,"ms":100,"ex":0},{"mark":"","type":1},{"mark":"","type":5,"text":"*以下为说明，实际效果为前两行"},{"mark":"","type":5,"text":"测试使用浏览器，后台点击右上角关闭"},{"mark":"","type":5,"text":"1：点击标题栏右侧的+选择你的浏览器"},{"mark":"","type":5,"text":"2：点击第一行[位置]，如果准心在浏览器关闭处即可"},{"mark":"","type":5,"text":"2.1：如果不是，请双击[位置]并重新选择"},{"mark":"","type":5,"text":"3：关闭此窗口，之后按触发键（Shift）来测试"}]})";
		File::FolderCreate(L"macro");
		File::TextSave(L"macro\\连点.json", click);
		File::TextSave(L"macro\\长按.json", state);
		File::TextSave(L"macro\\压枪.json", pullDown);
		File::TextSave(L"macro\\消息.json", message);
		File::TextSave(L"macro\\后台.json", window);
	}
}

int main(int argc, char* argv[])
{

#ifdef _DEBUG
	MsgWnd::msg(L"Debug");
#endif

	std::locale::global(std::locale(".UTF8")); // set utf8 for all streams
	Process::RunPath(); // reset work path to exe path

	// mutex
	std::wstring mutex = Path::PathToUrl(Process::runPath()); // convert '\' to '/' to support  mutex name
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