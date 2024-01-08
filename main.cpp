#include <QtWidgets/qapplication.h>
#include <qtextcodec.h>
#include ".h/MainUi.h"
#include "static.h"

void SwitchKey(BYTE vk, bool state)
{
	// force stop
	if (vk == VK_SHIFT) Global::qi.set.shift = state;
	else if (vk == VK_F12) Global::qi.set.f12 = state;
	if (Global::qi.set.shift && Global::qi.set.f12) { QiState(0); return; }

	if ((Global::qi.set.key & 0xFFFF) == vk) Global::qi.set.k1 = state;
	if (!(Global::qi.set.key >> 16)) Global::qi.set.k2 = 1;
	else if ((Global::qi.set.key) >> 16 == vk) Global::qi.set.k2 = state;
	if (Global::qi.set.k1 && Global::qi.set.k2)
	{
		if (Global::qi.state) QiState(0);
		else Global::qi.ReScreen(), QiState(1);
		Global::qi.set.k1 = 0, Global::qi.set.k2 = 0;
	}
}

void TriggerKey(BYTE vk, bool state)
{
	// show clock
	if (Global::qi.fun.showClock.state && Global::qi.fun.showClock.key == vk && state) { TipsWindow::Popup(Time::toWStringT()); }

	if (!Global::qi.run) return;

	// quick click
	if (Global::qi.fun.quickClick.state && Global::qi.fun.quickClick.key == vk)
	{
		if (state)
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
		if (Global::qi.macros[n].state)
		{
			if ((Global::qi.macros[n].key & 0xFFFF) == vk) Global::qi.macros[n].k1 = state;
			if ((Global::qi.macros[n].key >> 16) == 0) Global::qi.macros[n].k2 = true; // is not double keys
			else if ((Global::qi.macros[n].key >> 16) == vk) Global::qi.macros[n].k2 = state;

			if (Global::qi.macros[n].k1 && Global::qi.macros[n].k2) // trigger keys is pressed
			{
				switch (Global::qi.macros[n].mode)
				{
				case Macro::sw:
				{
					if (!Global::qi.macros[n].thread)
					{
						Global::qi.macros[n].thread = Thread::Start(ThreadMacro, (PVOID)&Global::qi.macros[n]);
						if (Global::qi.set.showTips) TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ开始", RGB(0xFF, 0xAF, 0xEF));
					}
					else
					{
						TerminateThread(Global::qi.macros[n].thread, 0); Global::qi.macros[n].thread = 0;
						if (Global::qi.set.showTips) TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ结束", RGB(0xFF, 0xFF, 0x60));
					}
				}
				break;

				case Macro::down:
				{
					if (!Global::qi.macros[n].thread)
					{
						Global::qi.macros[n].thread = Thread::Start(ThreadMacro, (PVOID)&Global::qi.macros[n]);
						if (Global::qi.set.showTips)
						{
							if (Global::qi.macros[n].count) TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ" + String::toWString(Global::qi.macros[n].count) + L"次", RGB(0x20, 0xFF, 0x40));
							else TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ循环", RGB(0x20, 0xFF, 0x40));
						}
					}
				}
				break;

				case Macro::up:
				{
					Global::qi.macros[n].active = true;
					if (Global::qi.macros[n].thread && Global::qi.macros[n].count == 0)
					{
						TerminateThread(Global::qi.macros[n].thread, 0); Global::qi.macros[n].thread = 0;
						if (Global::qi.set.showTips) TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ结束", RGB(0xFF, 0xFF, 0x60));
					}
				}
				break;

				}
			}
			else // trigger keys is released
			{
				switch (Global::qi.macros[n].mode)
				{
				case Macro::down:
				{
					if (Global::qi.macros[n].thread && Global::qi.macros[n].count == 0)
					{
						TerminateThread(Global::qi.macros[n].thread, 0); Global::qi.macros[n].thread = 0;
						if (Global::qi.set.showTips) TipsWindow::Popup(Global::qi.macros[n].name + L"ㅤ结束", RGB(0xFF, 0xFF, 0x60));
					}
				}
				break;

				case Macro::up:
				{
					if (Global::qi.macros[n].active)
					{
						Global::qi.macros[n].active = false;
						if (!Global::qi.macros[n].thread)
						{
							Global::qi.macros[n].thread = Thread::Start(ThreadMacro, (PVOID)&Global::qi.macros[n]);
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
	if (*exInfo == 214) // from Quick input
	{
		*exInfo = 0; // clear ex info
		return 0;
	}
	else if (vk) // other input
	{
		if (state) // down
		{
			if (!Global::blockRep[vk]) // block rep
			{
				Global::blockRep[vk] = 1;
				if (Global::qi.rec)
				{
					if (Global::qi.set.recKey == Input::Convert(vk)) return 1;
					else if (((RecordUi*)Global::qi.rec)->State()) ((RecordUi*)Global::qi.rec)->AddItems(Input::Convert(vk), state, msPt);
				}
				else
				{
					SwitchKey(Input::Convert(vk), state);
					TriggerKey(Input::Convert(vk), 1);
				}
			}
		}
		else // up
		{
			Global::blockRep[vk] = 0; // reset rep
			if (Global::qi.rec)
			{
				if (Global::qi.set.recKey == Input::Convert(vk)) { ((RecordUi*)Global::qi.rec)->SetRec(); return 1; }
				else if (((RecordUi*)Global::qi.rec)->State()) ((RecordUi*)Global::qi.rec)->AddItems(Input::Convert(vk), state, msPt);
			}
			else
			{
				SwitchKey(Input::Convert(vk), state);
				TriggerKey(Input::Convert(vk), 0);
			}
		}
		if (Global::qi.run) for (uint32 u = 0; u < Global::trBlock.size(); u++) if (Global::trBlock[u] == vk) return 1; // block trigger key
	}
	return 0;
}

int main(int argc, char* argv[])
{
	std::locale::global(std::locale(".UTF8")); // set utf8 for all streams
	std::wstring mutex = File::PathToUrl(Process::runPath()); // convert '\' to '/' to support  mutex name

	if (Process::isRunning(mutex.c_str())) { MsgBox::Warning(L"当前文件夹的程序已经运行，若运行更多程序请复制此文件夹", L"提示"); return 0; }
	CreateMutexW(0, 0, mutex.c_str()); // create mutex if this Quick input is not running
	timeBeginPeriod(1); // set clock accuracy, default is 16ms: sleep(1) = sleep(16)

	// init
	LoadJson();
	InitUI(!Global::qi.set.zoomBlock);
	QApplication app(argc, argv);

	// run
	Thread::Start(TipsWindow::TipsWindowThread);
	MainUi wnd;
	if (Global::qi.set.minMode)
	{
		HookState(1);
		if (Global::qi.set.defOn) QiState(1);
	}
	else wnd.show();
	app.exec();

	// end
	timeEndPeriod(1);
	return 0;
}