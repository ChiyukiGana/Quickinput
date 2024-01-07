#pragma once
#include ".h/QuickInputDef.h"
#include ".h/TipsWindow.h"

static void InitUI(bool zoom)
{
	UI::zoomRote = 1.0f;
	if (zoom && System::screenSize().cy > 1200)
	{
		UI::zoomRote = System::screenZoomRote();
		qputenv("QT_SCALE_FACTOR", QByteArray::number(UI::zoomRote, 10, 1));
	}
	{
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
		{
			UI::qiOn = (File::PathLast(Process::runPath()) + std::wstring(L"　已启用" + UI::syOn.toStdWString()));
			UI::qiOff = (File::PathLast(Process::runPath()) + std::wstring(L"　已禁用" + UI::syOff.toStdWString()));
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
}

static WndInfo WindowSelection()
{
	WndInfo wi;
	sleep(20);
	TipsWindow::Show(L"按回车开始，在对应窗口中再按回车获取窗口", RGB(0x20, 0xFF, 0x20));
	while (!Input::state(VK_RETURN)) sleep(10);
	Input::Loop(VK_RETURN);
	while (!Input::state(VK_RETURN))
	{
		wi.wnd = GetForegroundWindow();
		wi.wndName = Window::text(wi.wnd);
		wi.wndClass = Window::className(wi.wnd);
		TipsWindow::Show(wi.wndName);
		sleep(50);
	}
	TipsWindow::Popup(wi.wndName, RGB(0x20, 0xFF, 0x20));
	return wi;
}

static uint8 ActionExecute(Action& action, WndInput* wi)
{
	if (!Global::qi.run) return 1;
	switch (action.type)
	{
	case Action::_end: return 1;

	case Action::_delay:
	{
		if (action.delay.ex)
		{
			int32 ms = action.delay.ms + (Rand(action.delay.ex, action.delay.ex - (action.delay.ex * 2)));
			if (ms < 0) ms = 0;
			Thread::Sleep(ms);
		}
		else Thread::Sleep(action.delay.ms);
		return 0;
	}

	case Action::_key:
	{
		if (wi)
		{
			if (action.key.state == QiKey::up)
			{
				Input::State(wi->wnd, action.key.vk, wi->pt, 0);
				if (action.key.vk == VK_LBUTTON) wi->mk &= ~MK_LBUTTON;
				else if (action.key.vk == VK_RBUTTON) wi->mk &= ~MK_RBUTTON;
				else if (action.key.vk == VK_MBUTTON) wi->mk &= ~MK_MBUTTON;
				else if (action.key.vk == VK_XBUTTON1) wi->mk &= ~MK_XBUTTON1;
				else if (action.key.vk == VK_XBUTTON2) wi->mk &= ~MK_XBUTTON2;
				else if (action.key.vk == VK_CONTROL) wi->mk &= ~MK_CONTROL;
				else if (action.key.vk == VK_SHIFT) wi->mk &= ~MK_SHIFT;
			}
			else if (action.key.state == QiKey::down)
			{
				Input::State(wi->wnd, action.key.vk, wi->pt, 1);
				if (action.key.vk == VK_LBUTTON) wi->mk |= MK_LBUTTON;
				else if (action.key.vk == VK_RBUTTON) wi->mk |= MK_RBUTTON;
				else if (action.key.vk == VK_MBUTTON) wi->mk |= MK_MBUTTON;
				else if (action.key.vk == VK_XBUTTON1) wi->mk |= MK_XBUTTON1;
				else if (action.key.vk == VK_XBUTTON2) wi->mk |= MK_XBUTTON2;
				else if (action.key.vk == VK_CONTROL) wi->mk |= MK_CONTROL;
				else if (action.key.vk == VK_SHIFT) wi->mk |= MK_SHIFT;
			}
			else if (action.key.state == QiKey::click)
			{
				Input::Click (wi->wnd, action.key.vk, wi->pt, 10);
				if (action.key.vk == VK_LBUTTON) wi->mk |= MK_LBUTTON;
				else if (action.key.vk == VK_RBUTTON) wi->mk |= MK_RBUTTON;
				else if (action.key.vk == VK_MBUTTON) wi->mk |= MK_MBUTTON;
				else if (action.key.vk == VK_XBUTTON1) wi->mk |= MK_XBUTTON1;
				else if (action.key.vk == VK_XBUTTON2) wi->mk |= MK_XBUTTON2;
				else if (action.key.vk == VK_CONTROL) wi->mk |= MK_CONTROL;
				else if (action.key.vk == VK_SHIFT) wi->mk |= MK_SHIFT;
			}
		}
		else
		{
			if (action.key.state == QiKey::up) Input::State(action.key.vk, 0, 214);
			else if (action.key.state == QiKey::down) Input::State(action.key.vk, 1, 214);
			else if (action.key.state == QiKey::click) Input::Click(action.key.vk, 10, 214);
		}
		return 0;
	}

	case Action::_mouse:
	{
		if (wi)
		{
			if (action.mouse.ex)
			{
				POINT pt = { 0 };
				pt.x = action.mouse.x + (Rand(action.mouse.ex, (~action.mouse.ex + 1)));
				pt.y = action.mouse.y + (Rand(action.mouse.ex, (~action.mouse.ex + 1)));
				if (action.mouse.move) wi->pt.x += pt.x, wi->pt.y += pt.y;
				else wi->pt = WATR({ pt.x, pt.y }, wi->wnd);
			}
			else {
				if (action.mouse.move) wi->pt.x += action.mouse.x, wi->pt.y += action.mouse.y;
				else wi->pt = WATR({ action.mouse.x, action.mouse.y }, wi->wnd);
			}
			Input::MoveTo(wi->wnd, wi->pt.x, wi->pt.y, wi->mk);
		}
		else
		{
			if (action.mouse.ex)
			{
				POINT pt = { 0 };
				pt.x = action.mouse.x + (Rand(action.mouse.ex, (~action.mouse.ex + 1)));
				pt.y = action.mouse.y + (Rand(action.mouse.ex, (~action.mouse.ex + 1)));
				if (action.mouse.move) Input::Move(pt.x, pt.y);
				else Input::MoveToA(pt.x * 6.5535, pt.y * 6.5535);
			}
			else {
				if (action.mouse.move) Input::Move(action.mouse.x, action.mouse.y);
				else Input::MoveToA(action.mouse.x * 6.5535f, action.mouse.y * 6.5535f);
			}
		}
		return 0;
	}

	case Action::_text: System::ClipBoardText(action.text.str.str); return 0;

	case Action::_color:
	{
		RgbMap rgbMap;
		RECT rect;
		if (wi)
		{
			HDC wdc = GetDC(wi->wnd);
			rect = WATRR(action.color.rect, wi->wnd);
			Image::HdcRgbmap(wdc, rgbMap, rect);
			ReleaseDC(wi->wnd, wdc);
		}
		else
		{
			rect = ATRR(action.color.rect);
			Image::HdcRgbmap(Global::qi.hdc, rgbMap, rect);
		}
		Color::FindOrStatus result = Color::FindOr(rgbMap, action.color.rgbe.toRgb(), action.color.rgbe.a);
		if (action.color.unfind) { if (result.find) return 0; }
		else
		{
			if (result.find) {
				if (action.color.move == 1)
				{
					if (wi)
					{
						wi->pt = result.pt;
						Input::MoveTo(wi->wnd, wi->pt.x, wi->pt.y, wi->mk);
					}
					else Input::MoveTo(result.pt.x, result.pt.y);
				}
			}
			else return 0;
		}
		for (uint32 u = 0; u < action.color.next.size(); u++)
		{
			uint8 r = ActionExecute(action.color.next[u], wi);
			if (r) return r;
		}
		return 0;
	}

	case Action::_loop:
	{
		uint32 n = 0;
		while (Global::qi.run)
		{
			if (action.loop.count) { n++; if (n > action.loop.count) break; }
			for (uint32 u = 0; u < action.loop.next.size(); u++)
			{
				uint8 r = ActionExecute(action.loop.next[u], wi);
				if (r == 1) return 1;
				else if (r == 2) return 0;
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

	while (Global::qi.run)
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
	uint32 n = 0;
	WndInput wi;
	if (Global::qi.macros[pos].wndState)
	{
		Global::qi.macros[pos].wi.wnd = FindWindowW(Global::qi.macros[pos].wi.wndClass.c_str(), Global::qi.macros[pos].wi.wndName.c_str());
		if (!Global::qi.macros[pos].wi.wnd) Global::qi.macros[pos].wi = WindowSelection();
	}
	while (Global::qi.run)
	{
		if (Global::qi.macros[pos].count) { n++; if (n > Global::qi.macros[pos].count) break; }
		for (uint32 n = 0; n < Global::qi.macros[pos].actions.size() && Global::qi.run; n++)
		{
			if (Global::qi.macros[pos].wndState)
			{
				if (Global::qi.macros[pos].wi.wnd)
				{
					wi.wnd = Global::qi.macros[pos].wi.wnd;
					if (ActionExecute(Global::qi.macros[pos].actions[n], &wi))
					{
						Global::qi.macros[pos].thread = 0;
						return 0;
					}
				}
			}
			else
			{
				if (ActionExecute(Global::qi.macros[pos].actions[n], 0))
				{
					Global::qi.macros[pos].thread = 0;
					return 0;
				}
			}
		}
	}
	Global::qi.macros[pos].thread = 0;
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
		Global::qi.fun.wndActive.wi.wnd = FindWindowW(0, Global::qi.fun.wndActive.wi.wndName.c_str());
		if (Global::qi.fun.wndActive.wi.wnd)
		{
			bool active = (GetForegroundWindow() == Global::qi.fun.wndActive.wi.wnd);
			if (!Global::qi.run && active)
			{
				Global::qi.run = 1;
				if (Global::qi.set.showTips) TipsWindow::Popup(L"已启用 - 窗口内", RGB(0xA0, 0xFF, 0xC0));
			}
			else if (Global::qi.run && !active)
			{
				Global::qi.run = 0;
				if (Global::qi.set.showTips) TipsWindow::Popup(L"已禁用 - 窗口外", RGB(0xFF, 0x80, 0x80));
			}
		}
		else if (Global::qi.run)
		{
			Global::qi.run = 0;
			if (Global::qi.set.showTips) TipsWindow::Popup(L"已禁用 - 窗口外", RGB(0xFF, 0x80, 0x80));
		}
		sleep(100);
	}
	Global::qi.fun.wndActive.thread = 0;
	return 0;
}

static void UpdateBlock()
{
	Global::trBlock.clear();
	for (uint32 u = 0; u < Global::qi.macros.size(); u++)
	{
		if (Global::qi.macros[u].state && Global::qi.macros[u].block)
		{
			if ((Global::qi.macros[u].key & 0xFFFF)) Global::trBlock.Add((Global::qi.macros[u].key & 0xFFFF));
			if (Global::qi.macros[u].key >> 16) Global::trBlock.Add(Global::qi.macros[u].key >> 16);
		}
	}
	memset(Global::blockRep, 0, sizeof(Global::blockRep));
}
static void HookState(bool state)
{
	if (state)
	{
		if (!InputHook::Start(InputHook::all)) MsgBox::Error(L"创建输入Hook失败，检查是否管理员身份运行 或 是否被安全软件拦截。");
	}
	else InputHook::Stop(InputHook::all);
}
static void QiState(bool state)
{
	if (state)
	{
		UpdateBlock();
		Global::qi.state = 1;
		Global::qi.ReScreen();
		TipsWindow::Popup(UI::qiOn);
		if (Global::qi.set.audFx) Media::WavePlay(audfx_on);
		if (Global::qi.fun.wndActive.state) { if (!Global::qi.fun.wndActive.thread) Global::qi.fun.wndActive.thread = Thread::Start(ThreadWndActive); }
		else Global::qi.run = 1;
	}
	else
	{
		Global::qi.state = 0, Global::qi.run = 0;
		TipsWindow::Popup(UI::qiOff, RGB(0xFF, 0x50, 0x50));
		if (Global::qi.set.audFx)Media::WavePlay(audfx_off);
	}
}
