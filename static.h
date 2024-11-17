#pragma once
#include ".h/QuickInputDef.h"
#include ".h/TipsWindow.h"

//////////////////////////// Pos convert
static POINT RTA(POINT rel) { return { (long)(((float)rel.x / (float)(Global::qi.screen.cx - 1)) * 10000.0f), (long)(((float)rel.y / (float)(Global::qi.screen.cy - 1)) * 10000.0f) }; }
static RECT RTAR(RECT rel) { return { (long)(((float)rel.left / (float)(Global::qi.screen.cx - 1)) * 10000.0f), (long)(((float)rel.top / (float)(Global::qi.screen.cy - 1)) * 10000.0f), (long)(((float)rel.right / (float)(Global::qi.screen.cx - 1)) * 10000.0f), (long)(((float)rel.bottom / (float)(Global::qi.screen.cy - 1)) * 10000.0f) }; }
static POINT ATR(POINT abs) { return { (long)((float)Global::qi.screen.cx / 10000.0f * (float)abs.x), (long)((float)Global::qi.screen.cy / 10000.0f * (float)abs.y) }; }
static RECT ATRR(RECT abs) { return { (long)((float)Global::qi.screen.cx / 10000.0f * (float)abs.left), (long)((float)Global::qi.screen.cy / 10000.0f * (float)abs.top), (long)((float)Global::qi.screen.cx / 10000.0f * (float)abs.right), (long)((float)Global::qi.screen.cy / 10000.0f * (float)abs.bottom) }; }
static POINT WRTA(POINT rel, HWND wnd) { SIZE size = Window::size(wnd); return { (long)(((float)rel.x / ((float)size.cx)) * 10000.0f), (long)(((float)rel.y / ((float)size.cy)) * 10000.0f) }; }
static RECT WRTAR(RECT rel, HWND wnd) { SIZE size = Window::size(wnd); return { (long)(((float)rel.left / ((float)size.cx)) * 10000.0f), (long)(((float)rel.top / ((float)size.cy)) * 10000.0f), (long)(((float)rel.right / ((float)size.cx)) * 10000.0f), (long)(((float)rel.bottom / ((float)size.cy)) * 10000.0f) }; }
static POINT WATR(POINT abs, HWND wnd) { SIZE size = Window::size(wnd); return { (long)((float)(size.cx + 1) / 10000.0f * (float)abs.x), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.y) }; }
static RECT WATRR(RECT abs, HWND wnd) { SIZE size = Window::size(wnd); return { (long)((float)(size.cx + 1) / 10000.0f * (float)abs.left), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.top), (long)((float)(size.cx + 1) / 10000.0f * (float)abs.right), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.bottom) }; }
//////////////////////////// #Pos convert

//////////////////////////// Json
static void SaveAction(neb::CJsonObject& jActions, const Actions& actions)
{
	for (uint32 u = 0; u < actions.size(); u++)
	{
		neb::CJsonObject jItem;
		neb::CJsonObject jNext;

		std::string str = String::toString(actions[u].mark);
		jItem.Add("mark", str);

		switch (actions[u].type)
		{
		case Action::_end:
		{
			jItem.Add("type", Action::_end);
		}
		break;

		case Action::_delay:
		{
			jItem.Add("type", Action::_delay);
			jItem.Add("ms", actions[u].delay.ms);
			jItem.Add("ex", actions[u].delay.ex);
		}
		break;

		case Action::_key:
		{
			jItem.Add("type", Action::_key);
			jItem.Add("state", actions[u].key.state);
			jItem.Add("vk", actions[u].key.vk);
		}
		break;

		case Action::_mouse:
		{
			jItem.Add("type", Action::_mouse);
			jItem.Add("move", actions[u].mouse.move);
			jItem.Add("x", actions[u].mouse.x);
			jItem.Add("y", actions[u].mouse.y);
			jItem.Add("ex", actions[u].mouse.ex);
		}
		break;

		case Action::_text:
		{
			jItem.Add("type", Action::_text);
			jItem.Add("text", String::toString(actions[u].text.str.str));
		}
		break;

		case Action::_color:
		{
			jItem.Add("type", Action::_color);
			jItem.Add("unfind", actions[u].color.unfind);
			jItem.Add("move", actions[u].color.move);
			jItem.Add("left", (int32)actions[u].color.rect.left);
			jItem.Add("top", (int32)actions[u].color.rect.top);
			jItem.Add("right", (int32)actions[u].color.rect.right);
			jItem.Add("bottom", (int32)actions[u].color.rect.bottom);
			jItem.Add("rgbe", (uint32)actions[u].color.rgbe.toCOLORREF());
			SaveAction(jNext, actions[u].color.next);
			jItem.Add("next", jNext);
		}
		break;

		case Action::_loop:
		{
			jItem.Add("type", Action::_loop);
			jItem.Add("count", actions[u].loop.count);
			jItem.Add("rand", actions[u].loop.rand);
			SaveAction(jNext, actions[u].loop.next);
			jItem.Add("next", jNext);
		}
		break;

		case Action::_loopEnd:
		{
			jItem.Add("type", Action::_loopEnd);
		}
		break;

		case Action::_keyState:
		{
			jItem.Add("type", Action::_keyState);
			jItem.Add("state", actions[u].keyState.state);
			jItem.Add("vk", actions[u].keyState.vk);
			SaveAction(jNext, actions[u].keyState.next);
			jItem.Add("next", jNext);
		}
		break;
		}
		jActions.Add(jItem);
	}
}
static void SaveMacro(Macro& macro)
{
	neb::CJsonObject json;
	neb::CJsonObject jActions;
	neb::CJsonObject jActionsEnding;
	SaveAction(jActions, macro.actions);
	SaveAction(jActionsEnding, macro.actionsEnding);
	json.Add("documen_ charset", std::string("UTF8"));
	json.Add("wndState", macro.wndState);
	json.Add("wndChild", macro.wi.child);
	json.Add("wndName", String::toString(macro.wi.wndName));
	json.Add("wndClass", String::toString(macro.wi.wndClass));
	json.Add("state", macro.state);
	json.Add("block", macro.block);
	json.Add("mode", macro.mode);
	json.Add("key", macro.key);
	json.Add("count", macro.count);
	json.Add("actions", jActions);
	json.Add("actionsEnding", jActionsEnding);
	File::FolderCreate(L"macro");
	File::TextSave(String::toString(std::wstring(L"macro\\") + macro.name + std::wstring(L".json")), json.ToString());
}
static void SaveJson()
{
	neb::CJsonObject cfg;
	std::string str;
	cfg.Add("document_charset", std::string("UTF8"));
	cfg.Add("defOn", Global::qi.set.defOn);
	cfg.Add("key", Global::qi.set.key);
	cfg.Add("recKey", Global::qi.set.recKey);
	cfg.Add("showTips", Global::qi.set.showTips);
	cfg.Add("audFx", Global::qi.set.audFx);
	cfg.Add("minMode", Global::qi.set.minMode);
	cfg.Add("zoomBlock", Global::qi.set.zoomBlock);
	cfg.Add("quickClickKey", Global::qi.fun.quickClick.key);
	cfg.Add("quickClickState", Global::qi.fun.quickClick.state);
	cfg.Add("quickClickDelay", Global::qi.fun.quickClick.delay);
	cfg.Add("quickClickMode", Global::qi.fun.quickClick.mode);
	cfg.Add("showClockKey", Global::qi.fun.showClock.key);
	cfg.Add("showClockState", Global::qi.fun.showClock.state);
	cfg.Add("wndActiveState", Global::qi.fun.wndActive.state);
	cfg.Add("wndActiveName", String::toString(Global::qi.fun.wndActive.wi.wndName));
	File::TextSave("QuickInput.json", cfg.ToString());
}
static void LoadAction(const neb::CJsonObject jActions, Actions& actions)
{

#ifdef _DEBUG
	MsgWnd::str(L"Child count: ", 1);
	MsgWnd::log(jActions.GetArraySize());
#endif

	for (uint32 u = 0; u < jActions.GetArraySize(); u++)
	{
		Action action;
		neb::CJsonObject jItem;
		neb::CJsonObject jNext;
		jActions.Get(u, jItem);

		uint32 ui32 = 0;
		jItem.Get("type", ui32);
		if (ui32)
		{
			switch (ui32)
			{
			case Action::_end:
			{
				action.type = Action::_end;
			}
			break;

			case Action::_delay:
			{
				action.type = Action::_delay;
				jItem.Get("ms", action.delay.ms);
				jItem.Get("ex", action.delay.ex);
			}
			break;

			case Action::_key:
			{
				action.type = Action::_key;
				jItem.Get("state", action.key.state);
				jItem.Get("vk", action.key.vk);
			}
			break;

			case Action::_mouse:
			{
				action.type = Action::_mouse;
				jItem.Get("move", action.mouse.move);
				jItem.Get("x", action.mouse.x);
				jItem.Get("y", action.mouse.y);
				jItem.Get("ex", action.mouse.ex);
			}
			break;

			case Action::_text:
			{
				std::string str;
				jItem.Get("text", str);
				action.type = Action::_text;
				action.text.str = String::toWString(str).c_str();
			}
			break;

			case Action::_color:
			{
				action.type = Action::_color;
				jItem.Get("unfind", action.color.unfind);
				jItem.Get("move", action.color.move);
				jItem.Get("left", (int32&)(action.color.rect.left));
				jItem.Get("top", (int32&)(action.color.rect.top));
				jItem.Get("right", (int32&)(action.color.rect.right));
				jItem.Get("bottom", (int32&)(action.color.rect.bottom));
				jItem.Get("rgbe", ui32); action.color.rgbe.set(ui32);
				jItem.Get("next", jNext);
				LoadAction(jNext, action.color.next);
			}
			break;

			case Action::_loop:
			{
				action.type = Action::_loop;
				jItem.Get("count", action.loop.count);
				jItem.Get("rand", action.loop.rand);
				jItem.Get("next", jNext);
				LoadAction(jNext, action.loop.next);
			}
			break;

			case Action::_loopEnd:
			{
				action.type = Action::_loopEnd;
			}
			break;

			case Action::_keyState:
			{
				action.type = Action::_keyState;
				jItem.Get("state", action.keyState.state);
				jItem.Get("vk", action.keyState.vk);
				jItem.Get("next", jNext);
				LoadAction(jNext, action.keyState.next);
			}
			break;

			default: action.type = Action::_none; break;
			}

			std::string str;
			jItem.Get("mark", str);
			action.mark = String::toWString(str);
			actions.Add(action);
		}
	}
}
static void LoadMacro()
{
	File::FileList files = File::FindFile(L"macro\\*.json");

#ifdef _DEBUG
	MsgWnd::msg(L"Load Macros");
	MsgWnd::str(L"Macro count: ", 1);
	MsgWnd::log(files.size());
#endif

	for (uint32 u = 0; u < files.size(); u++) {
		Global::qi.macros.AddNull();
		Macro& macro = Global::qi.macros.Get();
		macro.name = (std::wstring(files[u].name)).substr(0, wcslen(files[u].name) - 5);

		neb::CJsonObject jMacro(File::TextLoad(std::string("macro\\") + String::toString(files[u].name)));
		std::string str;
		jMacro.Get("wndState", macro.wndState);
		jMacro.Get("wndChild", macro.wi.child);
		jMacro.Get("wndName", str); macro.wi.wndName = String::toWString(str);
		jMacro.Get("wndClass", str); macro.wi.wndClass = String::toWString(str);
		jMacro.Get("state", macro.state);
		jMacro.Get("block", macro.block);
		jMacro.Get("mode", macro.mode);
		jMacro.Get("key", macro.key);
		jMacro.Get("count", macro.count);

		neb::CJsonObject jActions;
		neb::CJsonObject jActionsEnding;
		jMacro.Get("actions", jActions);
		jMacro.Get("actionsEnding", jActionsEnding);

#ifdef _DEBUG
		MsgWnd::log(L"Load Actions");
		MsgWnd::str(L"Action count: ", 1);
		MsgWnd::log(jActions.GetArraySize());
#endif

		LoadAction(jActions, macro.actions);
		LoadAction(jActionsEnding, macro.actionsEnding);
	}
}
static void LoadJson()
{
	Global::qi.macros.clear();
	if (File::FileState(L"QuickInput.json"))
	{
		neb::CJsonObject cfg(File::TextLoad("QuickInput.json"));
		std::string str;
		cfg.Get("defOn", Global::qi.set.defOn);
		cfg.Get("key", Global::qi.set.key);
		cfg.Get("recKey", Global::qi.set.recKey);
		cfg.Get("showTips", Global::qi.set.showTips);
		cfg.Get("audFx", Global::qi.set.audFx);
		cfg.Get("minMode", Global::qi.set.minMode);
		cfg.Get("zoomBlock", Global::qi.set.zoomBlock);
		cfg.Get("quickClickState", Global::qi.fun.quickClick.state);
		cfg.Get("quickClickKey", Global::qi.fun.quickClick.key);
		cfg.Get("quickClickDelay", Global::qi.fun.quickClick.delay);
		cfg.Get("quickClickMode", Global::qi.fun.quickClick.mode);
		cfg.Get("showClockState", Global::qi.fun.showClock.state);
		cfg.Get("showClockKey", Global::qi.fun.showClock.key);
		cfg.Get("wndActiveState", Global::qi.fun.wndActive.state);
		cfg.Get("wndActiveName", str); Global::qi.fun.wndActive.wi.wndName = String::toWString(str);
	}
	else
	{
		Global::qi.set.defOn = 1;
		Global::qi.set.key = VK_F8;
		Global::qi.set.recKey = VK_F8;
		Global::qi.set.showTips = 1;
		Global::qi.set.audFx = 0;
		Global::qi.set.minMode = 0;
		Global::qi.set.zoomBlock = 0;
		Global::qi.fun.quickClick.state = 1;
		Global::qi.fun.quickClick.key = VK_LBUTTON;
		Global::qi.fun.quickClick.delay = 200;
		Global::qi.fun.quickClick.mode = 0;
		Global::qi.fun.showClock.state = 0;
		Global::qi.fun.showClock.key = VK_MENU;
		Global::qi.fun.wndActive.state = 0;
		Global::qi.fun.wndActive.wi.wndName = L"";
	}
	LoadMacro();
}
//////////////////////////// #Json

static WndInfo WindowSelection()
{
	WndInfo wi;
	sleep(20);
	TipsWindow::Show(L"按ESC取消，按回车开始，再按回车获取窗口", RGB(0x20, 0xFF, 0x20));
	while (true)
	{
		if (Input::state(VK_RETURN)) break;
		if (Input::state(VK_ESCAPE)) { TipsWindow::Hide(); return wi; }
		sleep(10);
	}
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
	case Action::_end:
	{
#ifdef _DEBUG
		MsgWnd::msg(L"Action::End");
#endif
	}
	return 1;

	case Action::_delay:
	{

#ifdef _DEBUG
		MsgWnd::msg(L"Action::Delay");
#endif

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

#ifdef _DEBUG
			MsgWnd::msg(L"Action-Wnd::Key");
#endif

			if (action.key.state == QiKey::up)
			{
				Input::State(wi->current, action.key.vk, wi->pt, 0);
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
				Input::State(wi->current, action.key.vk, wi->pt, 1);
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
				Input::Click(wi->current, action.key.vk, wi->pt, 10);
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

#ifdef _DEBUG
			MsgWnd::msg(L"Action::Key");
#endif

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

#ifdef _DEBUG
			MsgWnd::msg(L"Action-Wnd::Mouse");
#endif

			if (action.mouse.ex)
			{
				POINT pt = {};
				pt.x = action.mouse.x + (Rand(action.mouse.ex, (~action.mouse.ex + 1)));
				pt.y = action.mouse.y + (Rand(action.mouse.ex, (~action.mouse.ex + 1)));
				if (action.mouse.move) wi->pt.x += pt.x, wi->pt.y += pt.y;
				else wi->pt = WATR({ pt.x, pt.y }, wi->wnd);
			}
			else {
				if (action.mouse.move) wi->pt.x += action.mouse.x, wi->pt.y += action.mouse.y;
				else wi->pt = WATR({ action.mouse.x, action.mouse.y }, wi->wnd);
			}
			wi->current = 0;
			List<ChildWindow> cws;
			for (uint32 u = wi->children.size(); wi->children.size(); u--)
			{
				if (InRect(wi->children[u].rect, wi->pt)) cws.Add(wi->children[u]);
				if (u == 0) break;
			}

#ifdef _DEBUG
			MsgWnd::str(L"ChildCount: ", 1);
			MsgWnd::log(cws.size());
#endif

			if (cws.size())
			{

#ifdef _DEBUG
				MsgWnd::log(L"Currented: ChildWindow", 1);
#endif

				// select minimum window
				ChildWindow min;
				uint64 minArea = uint64Max;
				for (uint32 u = 0; u < cws.size(); u++)
				{
					uint64 area = RectArea(cws[u].rect);
					if (area < minArea) min = cws[u], minArea = area;
				}
				if (min.wnd) wi->current = min.wnd;
				else wi->current = wi->wnd;

				wi->pt = PointInRect(min.rect, wi->pt);
			}
			else
			{

#ifdef _DEBUG
				MsgWnd::log(L"Currented: ParentWindow", 1);
#endif

				wi->current = wi->wnd;
			}

#ifdef _DEBUG
			MsgWnd::log(Window::text(wi->current), 2);
#endif

			Input::MoveTo(wi->wnd, wi->pt.x, wi->pt.y, wi->mk);
		}
		else
		{

#ifdef _DEBUG
			MsgWnd::msg(L"Action::Mouse");
#endif

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

	case Action::_text:
	{

#ifdef _DEBUG
		MsgWnd::msg(L"Action::Text");
#endif

		System::ClipBoardText(action.text.str.str);
	}
	return 0;

	case Action::_color:
	{
		RgbMap rgbMap;
		RECT rect;
		if (wi)
		{

#ifdef _DEBUG
			MsgWnd::msg(L"Action-Wnd::Mouse");
#endif

			HDC wdc = GetDC(wi->wnd);
			rect = WATRR(action.color.rect, wi->wnd);
			Image::HdcRgbmap(wdc, rgbMap, rect);
			ReleaseDC(wi->wnd, wdc);
		}
		else
		{

#ifdef _DEBUG
			MsgWnd::msg(L"Action::Color");
#endif

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
					result.pt.x += rect.left, result.pt.y += rect.top;
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

#ifdef _DEBUG
		MsgWnd::msg(L"Action::Loop");
#endif

		uint32 n = 0;
		uint32 e = 0;
		bool uloop = false;
		if (action.loop.count) uloop = true, e = action.loop.count;
		if (action.loop.rand) uloop = true, e = Rand(action.loop.rand, action.loop.count);
		while (Global::qi.run)
		{
			if (uloop)
			{
				n++;
				if (n > e) break;
			}
			for (uint32 u = 0; u < action.loop.next.size(); u++)
			{
				uint8 r = ActionExecute(action.loop.next[u], wi);
				if (r == 1) return 1;
				else if (r == 2) return 0;
			}
		}
		return 0;
	}

	case Action::_loopEnd:
	{
#ifdef _DEBUG
		MsgWnd::msg(L"Action::LoopEnd");
#endif
	}
	return 2;

	case Action::_keyState:
	{

#ifdef _DEBUG
		MsgWnd::msg(L"Action::KeyState");
#endif

		if (action.keyState.state)
		{
			if (!Input::state(action.keyState.vk)) return 0;
		}
		else
		{
			if (Input::state(action.keyState.vk)) return 0;
		}
		for (uint32 u = 0; u < action.keyState.next.size(); u++)
		{
			uint8 r = ActionExecute(action.keyState.next[u], wi);
			if (r == 1) return 1;
			else if (r == 2) return 0;
		}
	}
	break;
	}
	return 0;
}

//////////////////////////// Threads
static DWORD CALLBACK ThreadQuickClick(PVOID)
{
	srand(clock());
	uint32 b, e;
	if (Global::qi.fun.quickClick.delay > 99) b = Rand(70, 30), e = Global::qi.fun.quickClick.delay - Rand(70, 30);
	else if (Global::qi.fun.quickClick.delay > 1) b = Rand(Global::qi.fun.quickClick.delay, Global::qi.fun.quickClick.delay >> 2), e = Rand(Global::qi.fun.quickClick.delay, Global::qi.fun.quickClick.delay >> 2);
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
static DWORD CALLBACK ThreadMacroEnding(PVOID pMacro)
{
	srand(clock());
	Macro* macro = (Macro*)pMacro;
	WndInput wi;
	WndInput* pWi = 0;
	if (macro->wndState)
	{
		// window is not found
		macro->wi.wnd = FindWindowW(macro->wi.wndClass.c_str(), macro->wi.wndName.c_str());
		if (!macro->wi.wnd)
		{
			macro->threadEnding = 0;
			return 0;
		}
	}

	for (uint32 n = 0; n < macro->actionsEnding.size(); n++)
	{
		if (ActionExecute(macro->actionsEnding[n], pWi))
		{
			macro->threadEnding = 0;
			return 0;
		}
	}
	macro->threadEnding = 0;
	return 0;
}
static DWORD CALLBACK ThreadMacro(PVOID pMacro)
{
	srand(clock());
	Macro* macro = (Macro*)pMacro;
	WndInput wi;
	WndInput* pWi = 0;
	if (macro->wndState)
	{
		// window is not found
		macro->wi.wnd = FindWindowW(macro->wi.wndClass.c_str(), macro->wi.wndName.c_str());
		if (!macro->wi.wnd) macro->wi = WindowSelection();
		if (!macro->wi.wnd)
		{
			macro->thread = 0;
			return 0;
		}

		wi.wnd = macro->wi.wnd;

		// select child windows
		if (macro->wi.child)
		{
			Window::HWNDS wnds; Window::FindChild(wi.wnd, wnds);
			wi.children.resize(wnds.size());
			for (uint32 u = 0; u < wnds.size(); u++)
			{
				if (IsWindowVisible(wnds[u]))
				{
					wi.children[u].wnd = wnds[u];
					wi.children[u].rect = Window::childRect(wi.wnd, wi.children[u].wnd);
				}
			}
		}
		
		pWi = &wi;
	}

	uint32 count = 0;
	while (Global::qi.run)
	{
		if (macro->count) { count++; if (count > macro->count) break; } // if count = 0 then while is infinite
		for (uint32 n = 0; n < macro->actions.size(); n++)
		{
			if (ActionExecute(macro->actions[n], pWi))
			{
				macro->thread = 0;
				if (macro->actionsEnding.size()) macro->threadEnding = Thread::Start(ThreadMacroEnding, macro);
				return 0;
			}
		}
	}
	macro->thread = 0;
	if (macro->actionsEnding.size()) macro->threadEnding = Thread::Start(ThreadMacroEnding, macro);
	return 0;
}
static DWORD CALLBACK ThreadRelease(PVOID key)
{
	Input::State((BYTE)key, 0, 214);
	return 0;
}
static DWORD CALLBACK ThreadWndActive(PVOID)
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
//////////////////////////// #Threads

static std::wstring NameFilter(std::wstring name)
{
	if (!Global::qi.macros.size()) return name;
	for (uint32 n = 0;; n++)
	{
		for (uint32 p = 0; p < Global::qi.macros.size(); p++)
		{
			std::wstring find = name + L" " + String::toWString(n + 1);
			if (Global::qi.macros[p].name == find) break;
			if (p >= Global::qi.macros.size() - 1) return find;
		}
	}
	return L"";
}
static void UpdateBlockKeys()
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
	memset(Global::keyState, 0, sizeof(Global::keyState));
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
		UpdateBlockKeys();
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