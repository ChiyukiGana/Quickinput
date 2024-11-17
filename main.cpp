#include <QtWidgets/qapplication.h>
#include ".h/MainUi.h"
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
		*exInfo = 0; // clear ex info
		return 0;
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

int main(int argc, char* argv[])
{
	std::locale::global(std::locale(".UTF8")); // set utf8 for all streams
	Process::RunPath(); // reset work path to exe path

	// mutex
	std::wstring mutex = Path::PathToUrl(Process::runPath()); // convert '\' to '/' to support  mutex name
	if (Process::isRunning(mutex.c_str())) { MsgBox::Warning(L"当前文件夹的程序已经运行，若运行更多程序请复制此文件夹", L"提示"); return 0; }
	CreateMutexW(0, 0, mutex.c_str()); // create mutex if this Quick input is not running

	// start
	timeBeginPeriod(1); // set clock accuracy, default is 16ms: sleep(1) = sleep(16)

	// init
	LoadJson();
	InitUI(!Global::qi.set.zoomBlock);
	QApplication app(argc, argv);
	app.setFont(QFont("Microsoft YaHei")); // default font

	// run
	Thread::Start(TipsWindow::TipsWindowThread);
	MainUi wnd;
	Global::qi.main = &wnd;
	if (Global::qi.set.minMode)
	{
		HookState(true);
		if (Global::qi.set.defOn) QiState(true);
	}
	else wnd.show();
	app.exec();

	// end
	timeEndPeriod(1);
	return 0;
}