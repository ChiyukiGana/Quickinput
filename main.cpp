#include <QtWidgets/qapplication.h>
#include <qtextcodec.h>
#include ".h/MainUi.h"
#include ".h/TipsWindow.h"
#include ".h/QuickInputDef.h"

QuickInputStruct qis;

QString UI::qiWait;
QString UI::qiDown;
QString UI::qiUp;
QString UI::qiClick;
QString UI::qiPos;
QString UI::qiMove;
QString UI::qiLoop;
QString UI::qiText;
QString UI::qiColor;
QString UI::qiEnd;
QString UI::qiEndLoop;
std::wstring UI::qiOn;
std::wstring UI::qiOff;
QString UI::trOn;
QString UI::trOff;
QString UI::etChange;
QString UI::etAdd;
QString UI::etDel;
QString UI::etEdit;
QString UI::etFunc;
QString UI::etParam;
QString UI::rcStart;
QString UI::rcStop;
QString UI::rcClose;

SIZE  TipsWindow::screen = { 0 };
HANDLE TipsWindow::thread = 0;
HWND TipsWindow::wnd = 0;
UINT TipsWindow::time = 0;
COLORREF  TipsWindow::color = 0;
std::wstring  TipsWindow::text;

void InitUI(bool zoom)
{
	if (zoom)
	{
		if (System::screenSize().cy > 1200)
		{
			qputenv("QT_SCALE_FACTOR", QByteArray::number(System::screenZoomRote(), 10, 1));
		}
	}

	{
		if (System::Version().dwMajorVersion >= 10)
		{
			UI::qiOn = L"已启用✅";
			UI::qiOff = L"已禁用⛔";
			UI::qiWait = u8"等待⏳";
			UI::qiDown = u8"按下⬇";
			UI::qiUp = u8"松开⬆";
			UI::qiClick = u8"点击🔃";
			UI::qiMove = u8"移动🔜";
			UI::qiPos = u8"位置🔝";
			UI::qiText = u8"复制文本🅰";
			UI::qiLoop = u8"循环♾️";
			UI::qiColor = u8"查找颜色🌈";
			UI::qiEnd = u8"结束宏🛑";
			UI::qiEndLoop = u8"结束循环🛑";
			UI::trOn = u8"启用✅";
			UI::trOff = u8"禁用⛔";
			UI::etFunc = u8"功能⚙";
			UI::etParam = u8"参数🔗";
			UI::etAdd = u8"添加✔️";
			UI::etDel = u8"删除❌";
			UI::etChange = u8"修改⭕";
			UI::etEdit = u8"编辑🔧";
			UI::rcStart = u8"开始⭕";
			UI::rcStop = u8"停止⭕";
			UI::rcClose = u8"取消❌";
		}
		else
		{
			UI::qiOn = L"已启用✔";
			UI::qiOff = L"已禁用✘";
			UI::qiWait = u8"等待☽";
			UI::qiDown = u8"按下↓";
			UI::qiUp = u8"松开↑";
			UI::qiClick = u8"点击◈";
			UI::qiMove = u8"移动↘";
			UI::qiPos = u8"位置↗";
			UI::qiText = u8"复制文本";
			UI::qiLoop = u8"循环↩";
			UI::qiColor = u8"查找颜色☀";
			UI::qiEnd = u8"结束宏Ⓢ";
			UI::qiEndLoop = u8"结束循环Ⓢ";
			UI::trOn = u8"启用✔";
			UI::trOff = u8"禁用✘";
			UI::etFunc = u8"功能✱";
			UI::etParam = u8"参数※";
			UI::etAdd = u8"添加✔";
			UI::etDel = u8"删除✘";
			UI::etChange = u8"修改◈";
			UI::etEdit = u8"编辑▲";
			UI::rcStart = u8"开始✔";
			UI::rcStop = u8"停止✔";
			UI::rcClose = u8"取消✘";
		}
	}
}

uint8 ExcItem(Item* it)
{
	if (!qis.state) return 1;
	switch (it->type)
	{
	case Item::endLoop:
		return 2;
	case Item::end:
		return 1;
	case Item::delay:
		if (it->c)
		{
			int ms;
			ms = it->b + (Rand(it->c, it->c - (it->c * 2)));
			if (ms < 0) ms = 0;
			Thread::Sleep(ms);
		}
		else {
			Thread::Sleep(it->b);
		}
		break;
	case Item::down:
		Input::State(it->a, 1, 214);
		break;
	case Item::up:
		Input::State(it->a, 0, 214);
		break;
	case Item::click:
		Input::State(it->a, 1, 214);
		sleep(10);
		Input::State(it->a, 0, 214);
		sleep(10);
		break;
	case Item::pos:
		sleep(10);
		if (it->a)
		{
			POINT pt;
			pt.x = it->b + (Rand(it->a, (~it->a + 1)));
			pt.y = it->c + (Rand(it->a, (~it->a + 1)));
			Input::MoveToA(pt.x * 6.5535, pt.y * 6.5535);
		}
		else {
			Input::MoveToA(it->b * 6.5535, it->c * 6.5535);
		}
		sleep(10);
		break;
	case Item::move:
		if (it->a)
		{
			POINT pt;
			pt.x = it->b + (Rand(it->a, (~it->a + 1)));
			pt.y = it->c + (Rand(it->a, (~it->a + 1)));
			Input::Move(pt.x, pt.y);
		}
		else {
			Input::Move(it->b, it->c);
		}
		break;
	case Item::text:
		System::ClipBoardText(it->s.c_str());
		break;
	case Item::color:
	{
		QColor color;
		color.setRgba(QRgb(it->a));
		RECT rect;
		POINT pos;
		pos = AbsToRel({ it->b >> 16, it->b & 0xFFFF });
		rect.left = pos.x, rect.top = pos.y;
		pos = AbsToRel({ it->c >> 16, it->c & 0xFFFF });
		rect.right = pos.x, rect.bottom = pos.y;

		bool result = Color::FindOr(qis.hdc, rect, RGB(color.red(), color.green(), color.blue()), color.alpha()).x;
		if (it->d)
		{
			if (!result) break;
		}
		else
		{
			if (result) break;
		}
		for (uint32 u = 0; u < it->next.len(); u++)
		{
			uint8 r = ExcItem(&it->next[u]);
			if (r) return r;
		}
		break;
	}
	case Item::loop:
	{
		if (it->b)
		{
			for (uint32 i = 0; i < it->b; i++)
			{
				for (uint32 ix = 0; ix < it->next.len(); ix++)
				{
					uint8 r = ExcItem(&it->next[ix]);
					if (r == 1) return 1;
					else if (r == 2) return 0;
				}
			}
		}
		else
		{
			while (1)
			{
				for (uint32 ix = 0; ix < it->next.len(); ix++)
				{
					uint8 r = ExcItem(&it->next[ix]);
					if (r == 1) return 1;
					else if (r == 2) return 0;
				}
			}
		}
		break;
	}
	}
	return 0;
}

DWORD CALLBACK ThreadQuickClick(LPVOID)
{
	srand(clock());
	uint32 b = 0;
	uint32 e = 0;
	if (qis.fun.quickClick.delay > 99) b = 50, e = qis.fun.quickClick.delay - 50;
	else if (qis.fun.quickClick.delay > 1) b = qis.fun.quickClick.delay / 2, e = b;
	else e = qis.fun.quickClick.delay;

	while (qis.state)
	{
		Input::State(qis.fun.quickClick.key, 1, 214);
		Thread::Sleep(b);
		Input::State(qis.fun.quickClick.key, 0, 214);
		Thread::Sleep(e);
	}
	return 0;
}
DWORD CALLBACK ThreadMacro(LPVOID lParam)
{
	srand(clock());
	uint32 pos = (UINT)lParam;
	uint32 count = qis.scripts[pos].a;
	uint32 n = 0;
	while (qis.state)
	{
		if (count)
		{
			n++;
			if (n > count) break;
		}
		for (uint32 n = 0; n < qis.scripts[pos].items.len() && qis.state; n++)
		{
			if (ExcItem(&qis.scripts[pos].items[n]))
			{
				qis.scripts[pos].thread = 0;
				return 0;
			}
		}
	}
	qis.scripts[pos].thread = 0;
	return 0;
}
DWORD CALLBACK ThreadRelease(LPVOID key)
{
	Input::State((BYTE)key, 0, 214);
	return 0;
}
DWORD CALLBACK ThreadWndActive(LPVOID lParam)
{
	while (qis.state)
	{
		qis.fun.wndActive.wnd = FindWindowW(0, qis.fun.wndActive.name.c_str());
		if (qis.fun.wndActive.wnd)
		{
			bool active = (GetForegroundWindow() == qis.fun.wndActive.wnd);
			if (!qis.fun.wndActive.active && active)
			{
				qis.fun.wndActive.active = 1;

				if (qis.set.showTips)
				{
					TipsWindow::Popup(L"已启用 - 窗口内", RGB(0xA0, 0xFF, 0xC0));
				}

			}
			else if (qis.fun.wndActive.active && !active)
			{
				qis.fun.wndActive.active = 0;

				if (qis.set.showTips)
				{
					TipsWindow::Popup(L"已禁用 - 窗口外", RGB(0xFF, 0x80, 0x80));
				}
			}
		}
		sleep(100);
	}
	qis.fun.wndActive.thread = 0;
	return 0;
}

byte block = 0;

void SwitchKey(BYTE vk, bool state)
{
	if ((qis.set.key & 0xFFFF) == vk) qis.set.k1 = state;
	if (!(qis.set.key >> 16)) qis.set.k2 = 1;
	else if ((qis.set.key >> 16) == vk) qis.set.k2 = state;

	if (qis.set.k1 && qis.set.k2)
	{
		if (qis.state)
		{
			qis.state = 0;
			TipsWindow::Popup(UI::qiOff, RGB(0xFF, 0x50, 0x50));

			if (qis.set.audFx)Media::WavePlay(audfx.off);
		}
		else
		{
			qis.state = 1;
			qis.screen = System::screenSize();
			TipsWindow::screen = qis.screen;

			if (qis.fun.wndActive.state) qis.fun.wndActive.thread = Thread::Start(ThreadWndActive);
			else qis.fun.wndActive.active = 1;

			TipsWindow::Popup(UI::qiOn);

			if (qis.set.audFx)Media::WavePlay(audfx.on);
		}
		qis.set.k1 = 0, qis.set.k2 = 0;
	}
}

void TriggerKey(BYTE vk, bool state)
{
	std::wstring text;

	if (qis.fun.showClock.state && qis.fun.showClock.key == vk)
	{
		System::TimeStruct ts = System::Time();
		text = String::toWString(ts.hour) + L" : " + String::toWString(ts.min) + L" - " + String::toWString(ts.sec);
		TipsWindow::Popup(text);
	}

	if (!qis.state) return;
	if (!qis.fun.wndActive.active) return;

	if (qis.fun.quickClick.state && qis.fun.quickClick.key == vk)
	{
		if (qis.fun.quickClick.mode)
		{
			if (!state)
			{
				if (!qis.fun.quickClick.thread)
				{
					qis.fun.quickClick.thread = Thread::Start(ThreadQuickClick);
					if (qis.set.showTips) TipsWindow::Popup(L"快捷连点ㅤ开始");
				}
				else
				{
					Thread::Start(ThreadRelease, (LPVOID)qis.fun.quickClick.key);
					TerminateThread(qis.fun.quickClick.thread, 0);
					qis.fun.quickClick.thread = 0;
					if (qis.set.showTips) TipsWindow::Popup(L"快捷连点ㅤ停止", RGB(0xFF, 0xAA, 0xFF));
				}
			}
		}
		else
		{
			if (state)
			{
				if (!qis.fun.quickClick.thread)
				{
					qis.fun.quickClick.thread = Thread::Start(ThreadQuickClick);
					if (qis.set.showTips) TipsWindow::Popup(L"快捷连点ㅤ执行中", RGB(0xFF, 0xFF, 0x60));
				}
			}
			else
			{
				if (qis.fun.quickClick.thread)
				{
					Thread::Start(ThreadRelease, (LPVOID)qis.fun.quickClick.key);
					TerminateThread(qis.fun.quickClick.thread, 0);
					qis.fun.quickClick.thread = 0;
					if (qis.set.showTips) TipsWindow::Popup(L"快捷连点ㅤ停止", RGB(0xFF, 0xAA, 0xFF));
				}
			}
		}
	}

	for (UINT n = 0; n < qis.scripts.len(); n++)
	{
		if (qis.scripts[n].state)
		{
			if ((qis.scripts[n].key & 0xFFFF) == vk) qis.scripts[n].k1 = state, qis.scripts[n].block ? block = vk : 0;
			if ((qis.scripts[n].key >> 16) == 0) qis.scripts[n].k2 = 1;
			else if ((qis.scripts[n].key >> 16) == vk) qis.scripts[n].k2 = state, qis.scripts[n].block ? block = vk : 0;

			if (qis.scripts[n].mode == Script::sw)
			{
				if (qis.scripts[n].k1 && qis.scripts[n].k2)
				{
					if (!qis.scripts[n].thread)
					{
						qis.scripts[n].thread = Thread::Start(ThreadMacro, (LPVOID)n);
						if (qis.set.showTips)
						{
							text = qis.scripts[n].name + L"ㅤ开始";
							TipsWindow::Popup(text, RGB(0xFF, 0xAA, 0xFF));
						}
					}
					else
					{
						TerminateThread(qis.scripts[n].thread, 0);
						qis.scripts[n].thread = 0;
						if (qis.set.showTips)
						{
							text = qis.scripts[n].name + L"ㅤ结束";
							TipsWindow::Popup(text, RGB(0xFF, 0xAA, 0x00));
						}
					}
				}
			}
			else if (qis.scripts[n].mode == Script::down)
			{
				if (qis.scripts[n].k1 && qis.scripts[n].k2)
				{
					if (!qis.scripts[n].thread)
					{
						qis.scripts[n].thread = Thread::Start(ThreadMacro, (LPVOID)n);
						if (qis.set.showTips)
						{
							if (qis.scripts[n].a) text = qis.scripts[n].name + L"ㅤ" + String::toWString(qis.scripts[n].a) + L"次";
							else text = qis.scripts[n].name + L"ㅤ循环";
							TipsWindow::Popup(text, RGB(0x40, 0x60, 0xFF));
						}
					}
				}
				else
				{
					if (qis.scripts[n].thread && qis.scripts[n].a == 0)
					{
						TerminateThread(qis.scripts[n].thread, 0);
						qis.scripts[n].thread = 0;
						if (qis.set.showTips)
						{
							text = qis.scripts[n].name + L"ㅤ结束";
							TipsWindow::Popup(text, RGB(0xFF, 0xAA, 0x00));
						}
					}
				}
			}
			else if (qis.scripts[n].mode == Script::up)
			{
				if (qis.scripts[n].k1 && qis.scripts[n].k2)
				{
					qis.scripts[n].active = 1;
					if (qis.scripts[n].thread && qis.scripts[n].a == 0)
					{
						TerminateThread(qis.scripts[n].thread, 0);
						qis.scripts[n].thread = 0;
						if (qis.set.showTips)
						{
							text = qis.scripts[n].name + L"ㅤ结束";
							TipsWindow::Popup(text, RGB(0xFF, 0xAA, 0x00));
						}
					}
				}
				else if (qis.scripts[n].active)
				{
					qis.scripts[n].active = 0;
					if (!qis.scripts[n].thread)
					{
						qis.scripts[n].thread = Thread::Start(ThreadMacro, (LPVOID)n);
						if (qis.set.showTips)
						{
							if (qis.scripts[n].a) text = qis.scripts[n].name + L"ㅤ" + String::toWString(qis.scripts[n].a) + L"次";
							else text = qis.scripts[n].name + L"ㅤ循环";
							TipsWindow::Popup(text, RGB(0x40, 0xFF, 0x60));
						}
					}
				}
			}
		}
	}
}

InputHookProc()
{
	if (*exInfo == 214)
	{
		*exInfo = 0;
		return 0;
	}
	else if (vk)
	{
		block = 0;
		
		if (state)
		{
			if (qis.rec)
			{
				if (((RecordUi*)qis.rec)->State()) ((RecordUi*)qis.rec)->AddItems(Item::down, Input::Convert(vk), msPt);
			}
			else
			{
				SwitchKey(Input::Convert(vk), state);
				TriggerKey(Input::Convert(vk), 1);
			}
		}
		else
		{
			if (qis.rec)
			{
				if (((RecordUi*)qis.rec)->State()) ((RecordUi*)qis.rec)->AddItems(Item::up, Input::Convert(vk), msPt);
				if (qis.set.recKey == Input::Convert(vk)) ((RecordUi*)qis.rec)->EndRec();
			}
			else
			{
				SwitchKey(Input::Convert(vk), state);
				TriggerKey(Input::Convert(vk), 0);
			}
		}

		if (block == vk) return 1;
	}

	return 0;
}

void SetHookState(bool state)
{
	if (state)
	{
		if (!InputHook::Start(InputHook::all, 1)) MsgBox::Error(L"创建输入Hook失败，检查是否管理员身份运行 或 是否被安全软件拦截。");
	}
	else InputHook::Stop(InputHook::all);
}

int main(int argc, char* argv[])
{
	if (Process::isRunning(File::PathToUrl(Process::runPath()).c_str()))
	{
		MsgBox::Warning(L"当文件夹的程序已经运行，若运行更多程序请复制此文件夹", L"提示");
		exit(0);
	}

	timeBeginPeriod(1);

	qis.hdc = GetDC(0);
	qis.Ptrs(SetHookState);
	qis.screen = System::screenSize();
	TipsWindow::screen = qis.screen;
	LoadJson(&qis);

	InitUI(!qis.set.wndZoom);

	QApplication app(argc, argv);
	MainUi wnd(0, &qis);

	if (!qis.set.minMode)
	{
		wnd.show();
		qis.HookState(0);
		qis.state = 0;
	}
	else
	{
		wnd.hide();
		qis.HookState(1);
		qis.state = 0;
	}

	app.exec();

	timeEndPeriod(1);
	return 0;
}