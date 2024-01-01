#pragma once
#include "D:/#CGDATA/Code/cpp/cg.h"
#include "D:/#CGDATA/Code/cpp/CJsonObject.h"
#include ".h/QuickInputDef.h"
#include ".h/TipsWindow.h"

static void InitUI(bool zoom)
{
	if (zoom && System::screenSize().cy > 1200) qputenv("QT_SCALE_FACTOR", QByteArray::number(System::screenZoomRote(), 10, 1));
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

static uint8 ExcItem(Action& item)
{
	if (!Global::qi.state) return 1;
	switch (item.type)
	{
	case Action::_end: return 1;

	case Action::_delay:
	{
		if (item.delay.ex)
		{
			int32 ms;
			ms = item.delay.ms + (Rand(item.delay.ex, item.delay.ex - (item.delay.ex * 2)));
			if (ms < 0) ms = 0;
			Thread::Sleep(ms);
		}
		else Thread::Sleep(item.delay.ms);
		return 0;
	}

	case Action::_key:
	{
		if (item.key.state == Action::Key::up) Input::State(item.key.vk, 0, 214);
		else if (item.key.state == Action::Key::down) Input::State(item.key.vk, 1, 214);
		else if (item.key.state == Action::Key::click)
		{
			Thread::Sleep(10);
			Input::State(item.key.vk, 1, 214);
			Thread::Sleep(10);
			Input::State(item.key.vk, 0, 214);
			Thread::Sleep(10);
		}
		return 0;
	}

	case Action::_mouse:
	{
		Thread::Sleep(10);
		if (item.mouse.ex)
		{
			POINT pt = { 0 };
			pt.x = item.mouse.x + (Rand(item.mouse.ex, (~item.mouse.ex + 1)));
			pt.y = item.mouse.y + (Rand(item.mouse.ex, (~item.mouse.ex + 1)));
			if (item.mouse.move) Input::Move(pt.x, pt.y);
			else Input::MoveToA(pt.x * 6.5535, pt.y * 6.5535);
		}
		else {
			if (item.mouse.move) Input::Move(item.mouse.x, item.mouse.y);
			else Input::MoveToA(item.mouse.x * 6.5535, item.mouse.y * 6.5535);
		}
		Thread::Sleep(10);
		return 0;
	}

	case Action::_text: System::ClipBoardText(item.text.str.str); return 0;

	case Action::_color:
	{
		RECT rect = { 0 };
		POINT pt = { 0 };
		pt = AbsToRel({ item.color.rect.left, item.color.rect.top });
		rect.left = pt.x, rect.top = pt.y;
		pt = AbsToRel({ item.color.rect.right, item.color.rect.bottom });
		rect.right = pt.x, rect.bottom = pt.y;
		RgbMap rgbMap;
		Image::HdcRgbmap(Global::qi.hdc, Global::qi.screen, rgbMap);
		Color::FindOrStatus result = Color::FindOr(rgbMap, rect, item.color.rgbe.toRgb(), item.color.rgbe.a);
		if (item.color.unfind)
		{
			if (!result.find)
			{
				for (uint32 u = 0; u < item.color.next.size(); u++)
				{
					uint8 r = ExcItem(item.color.next[u]);
					if (r) return r;
				}
			}
		}
		else
		{
			if (result.find)
			{
				if (item.color.move == 1) Input::MoveTo(result.pt.x, result.pt.y);
				for (uint32 u = 0; u < item.color.next.size(); u++)
				{
					uint8 r = ExcItem(item.color.next[u]);
					if (r) return r;
				}
			}
		}
		return 0;
	}

	case Action::_loop:
	{
		if (item.loop.count)
		{
			for (uint32 u = 0; u < item.loop.count; u++)
			{
				for (uint32 ux = 0; ux < item.loop.next.size(); ux++)
				{
					uint8 r = ExcItem(item.loop.next[ux]);
					if (r == 1) return r;
					else if (r == 2) return 0;
				}
			}
		}
		else
		{
			while (1)
			{
				for (uint32 u = 0; u < item.loop.next.size(); u++)
				{
					uint8 r = ExcItem(item.loop.next[u]);
					if (r == 1) return 1;
					else if (r == 2) return 0;
				}
			}
		}
		return 0;
	}

	case Action::_loopEnd: return 2;
	}
	return 0;
}
static DWORD CALLBACK ThreadQuickClick(LPVOID)
{
	srand(clock());
	uint32 b = 0;
	uint32 e = 0;
	if (Global::qi.fun.quickClick.delay > 99) b = 50, e = Global::qi.fun.quickClick.delay - 50;
	else if (Global::qi.fun.quickClick.delay > 1) b = Global::qi.fun.quickClick.delay / 2, e = b;
	else e = Global::qi.fun.quickClick.delay;

	while (Global::qi.state)
	{
		Input::State(Global::qi.fun.quickClick.key, 1, 214);
		Thread::Sleep(b);
		Input::State(Global::qi.fun.quickClick.key, 0, 214);
		Thread::Sleep(e);
	}
	return 0;
}
static DWORD CALLBACK ThreadMacro(LPVOID lParam)
{
	srand(clock());
	uint32 pos = (UINT)lParam;
	uint32 count = Global::qi.scripts[pos].a;
	uint32 n = 0;
	while (Global::qi.state)
	{
		if (count)
		{
			n++;
			if (n > count) break;
		}
		for (uint32 n = 0; n < Global::qi.scripts[pos].actions.size() && Global::qi.state; n++)
		{
			if (ExcItem(Global::qi.scripts[pos].actions[n]))
			{
				Global::qi.scripts[pos].thread = 0;
				return 0;
			}
		}
	}
	Global::qi.scripts[pos].thread = 0;
	return 0;
}
static DWORD CALLBACK ThreadRelease(LPVOID key)
{
	Input::State((BYTE)key, 0, 214);
	return 0;
}
static DWORD CALLBACK ThreadWndActive(LPVOID)
{
	while (Global::qi.state)
	{
		Global::qi.fun.wndActive.wnd = FindWindowW(0, Global::qi.fun.wndActive.name.c_str());
		if (Global::qi.fun.wndActive.wnd)
		{
			bool active = (GetForegroundWindow() == Global::qi.fun.wndActive.wnd);
			if (!Global::qi.fun.wndActive.active && active)
			{
				Global::qi.fun.wndActive.active = 1;

				if (Global::qi.set.showTips)
				{
					TipsWindow::Popup(L"已启用 - 窗口内", RGB(0xA0, 0xFF, 0xC0));
				}

			}
			else if (Global::qi.fun.wndActive.active && !active)
			{
				Global::qi.fun.wndActive.active = 0;

				if (Global::qi.set.showTips)
				{
					TipsWindow::Popup(L"已禁用 - 窗口外", RGB(0xFF, 0x80, 0x80));
				}
			}
		}
		sleep(100);
	}
	Global::qi.fun.wndActive.thread = 0;
	return 0;
}

static void HookState(bool state)
{
	if (state)
	{
		if (!InputHook::Start(InputHook::all, 1)) MsgBox::Error(L"创建输入Hook失败，检查是否管理员身份运行 或 是否被安全软件拦截。");
	}
	else InputHook::Stop(InputHook::all);
}
static void QiState(bool state)
{
	if (state)
	{
		Global::qi.state = 1;
		Global::qi.ReScreen();
		TipsWindow::screen = Global::qi.screen;
		TipsWindow::Popup(UI::qiOn);
		if (Global::qi.set.audFx)Media::WavePlay(audfx.on);
		if (Global::qi.fun.wndActive.state) Global::qi.fun.wndActive.thread = Thread::Start(ThreadWndActive);
		else Global::qi.fun.wndActive.active = 1;
	}
	else
	{
		Global::qi.state = 0;
		TipsWindow::Popup(UI::qiOff, RGB(0xFF, 0x50, 0x50));
		if (Global::qi.set.audFx)Media::WavePlay(audfx.off);
	}
}
