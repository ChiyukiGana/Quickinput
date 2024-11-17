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
		const Action& action = actions[u];
		neb::CJsonObject jItem;
		neb::CJsonObject jNext;

		std::string str = String::toString(action.mark.str());
		jItem.Add("mark", str);
		jItem.Add("type", action.type);

		switch (action.type)
		{
		case Action::_delay:
		{
			jItem.Add("ms", action.d.delay.ms);
			jItem.Add("ex", action.d.delay.ex);
		}
		break;

		case Action::_key:
		{
			jItem.Add("state", action.d.key.state);
			jItem.Add("vk", action.d.key.vk);
		}
		break;

		case Action::_mouse:
		{
			jItem.Add("move", action.d.mouse.move, true);
			jItem.Add("x", action.d.mouse.x);
			jItem.Add("y", action.d.mouse.y);
			jItem.Add("ex", action.d.mouse.ex);
		}
		break;

		case Action::_text:
		{
			jItem.Add("text", String::toString(action.d.text.str.str()));
		}
		break;

		case Action::_color:
		{
			jItem.Add("unfind", action.d.color.unfind, true);
			jItem.Add("move", action.d.color.move, true);
			jItem.Add("left", (int32)action.d.color.rect.left);
			jItem.Add("top", (int32)action.d.color.rect.top);
			jItem.Add("right", (int32)action.d.color.rect.right);
			jItem.Add("bottom", (int32)action.d.color.rect.bottom);
			jItem.Add("rgbe", (uint32)action.d.color.rgbe.toCOLORREF());
			SaveAction(jNext, action.next), jItem.Add("next", jNext);
		}
		break;

		case Action::_loop:
		{
			jItem.Add("count", action.d.loop.count);
			jItem.Add("rand", action.d.loop.rand);
			SaveAction(jNext, action.next), jItem.Add("next", jNext);
		}
		break;

		case Action::_keyState:
		{
			jItem.Add("state", action.d.keyState.state);
			jItem.Add("vk", action.d.keyState.vk);
			SaveAction(jNext, action.next), jItem.Add("next", jNext);
		}
		break;

		case Action::_image:
		{
			jItem.Add("unfind", action.d.image.unfind, true);
			jItem.Add("move", action.d.image.move, true);
			jItem.Add("left", (int32)(action.d.image.rect.left));
			jItem.Add("top", (int32)(action.d.image.rect.top));
			jItem.Add("right", (int32)(action.d.image.rect.right));
			jItem.Add("bottom", (int32)(action.d.image.rect.bottom));
			jItem.Add("sim", action.d.image.sim);

			jItem.Add("width", action.d.image.map.width());
			jItem.Add("height", action.d.image.map.height());
			size_t size = Base64::EncodedLength(action.d.image.map.bytes());
			std::string data = base64_encode((byte*)action.d.image.map.data(), action.d.image.map.bytes());
			if (data.size()) jItem.Add("data", data);
			SaveAction(jNext, action.next), jItem.Add("next", jNext);
		}
		break;

		case Action::_popText:
		{
			jItem.Add("text", String::toString(action.d.popText.str.str()));
			jItem.Add("time", action.d.popText.time);
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
	json.Add("document_ charset", std::string("UTF8"));
	json.Add("type", std::string("QuickInputMacro"));
	json.Add("wndState", macro.wndState);
	json.Add("wndChild", macro.wi.child);
	json.Add("wndName", String::toString(macro.wi.wndName));
	json.Add("wndClass", String::toString(macro.wi.wndClass));
	json.Add("state", macro.state, true);
	json.Add("block", macro.block, true);
	json.Add("mode", macro.mode);
	json.Add("key", macro.key);
	json.Add("count", macro.count);
	json.Add("actions", jActions);
	json.Add("actionsEnding", jActionsEnding);
	std::string jstr = json.ToString();
	File::FolderCreate(L"macro");
	std::wstring path = Path::Append(Path::Append(Process::runPath(), L"macro"), macro.name + L".json");
	File::TextSaveU(path, jstr);
}
static void SaveJson()
{
	neb::CJsonObject cfg;
	std::string str;
	cfg.Add("document_charset", std::string("UTF8"));
	cfg.Add("type", std::string("QuickInputConfig"));
	cfg.Add("theme", Global::qi.set.style);
	cfg.Add("key", Global::qi.set.key);
	cfg.Add("recKey", Global::qi.set.recKey);
	cfg.Add("defOn", Global::qi.set.defOn);
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
	File::TextSaveU(L"QuickInput.json", cfg.ToString());
}
static void LoadAction(const neb::CJsonObject jActions, Actions& actions)
{
	actions.resize(jActions.GetArraySize());
	for (uint32 i = 0; i < jActions.GetArraySize(); i++)
	{
		Action& action = actions.at(i);
		neb::CJsonObject jItem;
		neb::CJsonObject jNext;
		jActions.Get(i, jItem);

		uint32 ui32 = 0;
		std::string str;
		jItem.Get("type", ui32);
		if (ui32)
		{
			switch (ui32)
			{
			case Action::_end: action.type = Action::_end;
				break;

			case Action::_delay: action.type = Action::_delay;
			{
				jItem.Get("ms", action.d.delay.ms);
				jItem.Get("ex", action.d.delay.ex);
			}
			break;

			case Action::_key: action.type = Action::_key;
			{
				jItem.Get("state", action.d.key.state);
				jItem.Get("vk", action.d.key.vk);
			}
			break;

			case Action::_mouse: action.type = Action::_mouse;
			{
				jItem.Get("move", action.d.mouse.move);
				jItem.Get("x", action.d.mouse.x);
				jItem.Get("y", action.d.mouse.y);
				jItem.Get("ex", action.d.mouse.ex);
			}
			break;

			case Action::_text: action.type = Action::_text;
			{
				jItem.Get("text", str);
				action.d.text.str.cpy(String::toWString(str).c_str());
			}
			break;

			case Action::_color: action.type = Action::_color;
			{
				jItem.Get("unfind", action.d.color.unfind);
				jItem.Get("move", action.d.color.move);
				jItem.Get("left", (int32&)(action.d.color.rect.left));
				jItem.Get("top", (int32&)(action.d.color.rect.top));
				jItem.Get("right", (int32&)(action.d.color.rect.right));
				jItem.Get("bottom", (int32&)(action.d.color.rect.bottom));
				jItem.Get("rgbe", ui32); action.d.color.rgbe.set(ui32);
				jItem.Get("next", jNext);
				LoadAction(jNext, action.next);
			}
			break;

			case Action::_loop: action.type = Action::_loop;
			{
				jItem.Get("count", action.d.loop.count);
				jItem.Get("rand", action.d.loop.rand);
				jItem.Get("next", jNext);
				LoadAction(jNext, action.next);
			}
			break;

			case Action::_loopEnd: action.type = Action::_loopEnd;
				break;

			case Action::_keyState: action.type = Action::_keyState;
			{
				jItem.Get("state", action.d.keyState.state);
				jItem.Get("vk", action.d.keyState.vk);
				jItem.Get("next", jNext);
				LoadAction(jNext, action.next);
			}
			break;

			case Action::_revocerPos: action.type = Action::_revocerPos;
				break;

			case Action::_image: action.type = Action::_image;
			{
				jItem.Get("unfind", action.d.image.unfind);
				jItem.Get("move", action.d.image.move);
				jItem.Get("left", (int32&)(action.d.image.rect.left));
				jItem.Get("top", (int32&)(action.d.image.rect.top));
				jItem.Get("right", (int32&)(action.d.image.rect.right));
				jItem.Get("bottom", (int32&)(action.d.image.rect.bottom));
				jItem.Get("sim", action.d.image.sim);

				uint32 width, height;
				jItem.Get("width", width);
				jItem.Get("height", height);
				if (width && height)
				{
					action.d.image.map.create(width, height);
					std::string str; jItem.Get("data", str);
					if (!Base64::Decode(str.c_str(), str.size(), (char*)action.d.image.map.data(), action.d.image.map.bytes())) action.d.image.map.release();
				}
				jItem.Get("next", jNext);
				LoadAction(jNext, action.next);
			}
			break;

			case Action::_popText: action.type = Action::_popText;
			{
				jItem.Get("text", str);
				jItem.Get("time", action.d.popText.time);
				action.d.popText.str.cpy(String::toWString(str).c_str());
			}
			break;

			default: action.type = Action::_none; break;
			}

			jItem.Get("mark", str);
			action.mark.cpy(String::toWString(str).c_str());
		}
	}
}
static void LoadMacro()
{
	Global::qi.macros.clear();
	File::FileList files = File::FindFile(L"macro\\*.json");
	for (uint32 i = 0; i < files.size(); i++) {
		std::wstring path = Path::Append(Path::Append(Process::runPath(), L"macro"), files[i].name);
		Macro& macro = Global::qi.macros.AddNull(); macro.name = Path::RemoveExtension(files[i].name);

		std::string jstr = File::TextReadU(path);
		if (jstr.size())
		{
			neb::CJsonObject jMacro(jstr);
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

			neb::CJsonObject jActions; jMacro.Get("actions", jActions);
			neb::CJsonObject jActionsEnding; jMacro.Get("actionsEnding", jActionsEnding);
			LoadAction(jActions, macro.actions);
			LoadAction(jActionsEnding, macro.actionsEnding);
		}
	}
}
static void LoadJson()
{
	std::wstring path = Path::Append(Process::runPath(), L"QuickInput.json");
	if (File::FileState(path))
	{
		std::string jstr = File::TextReadU(path);
		if (jstr.size())
		{
			neb::CJsonObject cfg(jstr);
			std::string str;
			cfg.Get("theme", Global::qi.set.style);
			cfg.Get("key", Global::qi.set.key);
			cfg.Get("recKey", Global::qi.set.recKey);
			cfg.Get("defOn", Global::qi.set.defOn);
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
			LoadMacro();
			return;
		}
	}
	{
		Global::qi.set.defOn = true;
		Global::qi.set.key = VK_F8;
		Global::qi.set.recKey = VK_F8;
		Global::qi.set.showTips = true;
		Global::qi.set.audFx = false;
		Global::qi.set.minMode = false;
		Global::qi.set.zoomBlock = false;
		Global::qi.fun.quickClick.state = false;
		Global::qi.fun.quickClick.key = VK_LBUTTON;
		Global::qi.fun.quickClick.delay = 200;
		Global::qi.fun.quickClick.mode = 0;
		Global::qi.fun.showClock.state = false;
		Global::qi.fun.showClock.key = VK_MENU;
		Global::qi.fun.wndActive.state = false;
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
		if (Input::state(VK_ESCAPE)) { TipsWindow::Hide(); return {}; }
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
static uint8 ActionExecute(const Action& action, const Macro& macro, WndInput* wi)
{
	if (!Global::qi.run) return 1;

	switch (action.type)
	{
	case Action::_end:
	{
		return 1;
	}

	case Action::_delay:
	{
		if (action.d.delay.ex)
		{
			int32 ms = action.d.delay.ms + (Rand(action.d.delay.ms + action.d.delay.ex, (int32)action.d.delay.ms - (int32)action.d.delay.ex));
			if (ms < 0) ms = 0;
			Thread::Sleep(ms);
		}
		else Thread::Sleep(action.d.delay.ms);
		return 0;
	}

	case Action::_key:
	{
		if (wi)
		{
			if (action.d.key.state == QiKey::up)
			{
				Input::State(wi->current, action.d.key.vk, wi->pt, 0);
				if (action.d.key.vk == VK_LBUTTON) wi->mk &= ~MK_LBUTTON;
				else if (action.d.key.vk == VK_RBUTTON) wi->mk &= ~MK_RBUTTON;
				else if (action.d.key.vk == VK_MBUTTON) wi->mk &= ~MK_MBUTTON;
				else if (action.d.key.vk == VK_XBUTTON1) wi->mk &= ~MK_XBUTTON1;
				else if (action.d.key.vk == VK_XBUTTON2) wi->mk &= ~MK_XBUTTON2;
				else if (action.d.key.vk == VK_CONTROL) wi->mk &= ~MK_CONTROL;
				else if (action.d.key.vk == VK_SHIFT) wi->mk &= ~MK_SHIFT;
			}
			else if (action.d.key.state == QiKey::down)
			{
				Input::State(wi->current, action.d.key.vk, wi->pt, 1);
				if (action.d.key.vk == VK_LBUTTON) wi->mk |= MK_LBUTTON;
				else if (action.d.key.vk == VK_RBUTTON) wi->mk |= MK_RBUTTON;
				else if (action.d.key.vk == VK_MBUTTON) wi->mk |= MK_MBUTTON;
				else if (action.d.key.vk == VK_XBUTTON1) wi->mk |= MK_XBUTTON1;
				else if (action.d.key.vk == VK_XBUTTON2) wi->mk |= MK_XBUTTON2;
				else if (action.d.key.vk == VK_CONTROL) wi->mk |= MK_CONTROL;
				else if (action.d.key.vk == VK_SHIFT) wi->mk |= MK_SHIFT;
			}
			else if (action.d.key.state == QiKey::click)
			{
				Input::Click(wi->current, action.d.key.vk, wi->pt, 10);
				if (action.d.key.vk == VK_LBUTTON) wi->mk |= MK_LBUTTON;
				else if (action.d.key.vk == VK_RBUTTON) wi->mk |= MK_RBUTTON;
				else if (action.d.key.vk == VK_MBUTTON) wi->mk |= MK_MBUTTON;
				else if (action.d.key.vk == VK_XBUTTON1) wi->mk |= MK_XBUTTON1;
				else if (action.d.key.vk == VK_XBUTTON2) wi->mk |= MK_XBUTTON2;
				else if (action.d.key.vk == VK_CONTROL) wi->mk |= MK_CONTROL;
				else if (action.d.key.vk == VK_SHIFT) wi->mk |= MK_SHIFT;
			}
		}
		else
		{
			if (action.d.key.state == QiKey::up) Input::State(action.d.key.vk, 0, 214);
			else if (action.d.key.state == QiKey::down) Input::State(action.d.key.vk, 1, 214);
			else if (action.d.key.state == QiKey::click) Input::Click(action.d.key.vk, 10, 214);
		}
		return 0;
	}

	case Action::_mouse:
	{
		if (wi)
		{
			if (action.d.mouse.ex)
			{
				POINT pt = {};
				pt.x = action.d.mouse.x + (Rand(action.d.mouse.ex, (~action.d.mouse.ex + 1)));
				pt.y = action.d.mouse.y + (Rand(action.d.mouse.ex, (~action.d.mouse.ex + 1)));
				if (action.d.mouse.move) wi->pt.x += pt.x, wi->pt.y += pt.y;
				else wi->pt = WATR({ pt.x, pt.y }, wi->wnd);
			}
			else {
				if (action.d.mouse.move) wi->pt.x += action.d.mouse.x, wi->pt.y += action.d.mouse.y;
				else wi->pt = WATR({ action.d.mouse.x, action.d.mouse.y }, wi->wnd);
			}
			wi->current = 0;
			List<ChildWindow> cws;
			for (uint32 u = wi->children.size(); wi->children.size(); u--)
			{
				if (InRect(wi->children[u].rect, wi->pt)) cws.Add(wi->children[u]);
				if (u == 0) break;
			}
			if (cws.size())
			{
				// select minimum window
				ChildWindow min = {};
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
				wi->current = wi->wnd;
			}
			Input::MoveTo(wi->wnd, wi->pt.x, wi->pt.y, wi->mk);
		}
		else
		{
			if (action.d.mouse.ex)
			{
				POINT pt = { 0 };
				pt.x = action.d.mouse.x + (Rand(action.d.mouse.ex, (~action.d.mouse.ex + 1)));
				pt.y = action.d.mouse.y + (Rand(action.d.mouse.ex, (~action.d.mouse.ex + 1)));
				if (action.d.mouse.move) Input::Move(pt.x, pt.y);
				else Input::MoveToA(pt.x * 6.5535, pt.y * 6.5535);
			}
			else {
				if (action.d.mouse.move) Input::Move(action.d.mouse.x, action.d.mouse.y);
				else Input::MoveToA(action.d.mouse.x * 6.5535f, action.d.mouse.y * 6.5535f);
			}
		}
		return 0;
	}

	case Action::_text:
	{
		System::ClipBoardText(action.d.text.str.str());
		return 0;
	}

	case Action::_color:
	{
		RgbMap rgbMap;
		RECT rect;
		if (wi)
		{
			HDC wdc = GetDC(wi->wnd);
			rect = WATRR(action.d.color.rect, wi->wnd);
			Image::HdcRgbmap(wdc, rgbMap, rect);
			ReleaseDC(wi->wnd, wdc);
		}
		else
		{
			rect = ATRR(action.d.color.rect);
			Image::HdcRgbmap(Global::qi.hdc, rgbMap, rect);
		}
		Color::FindResult result = Color::FindOr(rgbMap, action.d.color.rgbe.toRgb(), action.d.color.rgbe.a);
		if (action.d.color.unfind && result.find) return 0;
		else if (result.find)
		{
			if (action.d.color.move)
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
		for (uint32 i = 0; i < action.next.size(); i++)
		{
			uint8 r = ActionExecute(action.next.at(i), macro, wi);
			if (r == 1) return 1;
			else if (r == 2) return 2;
		}
		return 0;
	}

	case Action::_loop:
	{
		uint32 n = 0;
		uint32 e = 0;
		bool uloop = false;
		if (action.d.loop.count) uloop = true, e = action.d.loop.count;
		if (action.d.loop.rand) uloop = true, e = Rand(action.d.loop.rand, action.d.loop.count);
		while (Global::qi.run)
		{
			if (uloop)
			{
				n++;
				if (n > e) break;
			}
			for (uint32 i = 0; i < action.next.size(); i++)
			{
				uint8 r = ActionExecute(action.next.at(i), macro, wi);
				if (r == 1) return 1;
				else if (r == 2) return 0;
			}
		}
		return 0;
	}

	case Action::_loopEnd:
	{
		return 2;
	}

	case Action::_keyState:
	{
		if (action.d.keyState.state)
		{
			if (!Input::state(action.d.keyState.vk)) return 0;
		}
		else
		{
			if (Input::state(action.d.keyState.vk)) return 0;
		}
		for (uint32 i = 0; i < action.next.size(); i++)
		{
			uint8 r = ActionExecute(action.next.at(i), macro, wi);
			if (r == 1) return 1;
			else if (r == 2) return 2;
		}
		return 0;
	}

	case Action::_revocerPos:
	{
		Input::MoveTo(macro.originPos.x, macro.originPos.y);
		return 0;
	}

	case Action::_image:
	{
		RgbMap rgbMap;
		RECT rect;
		if (wi)
		{
			HDC wdc = GetDC(wi->wnd);
			rect = WATRR(action.d.image.rect, wi->wnd);
			Image::HdcRgbmap(wdc, rgbMap, rect);
			ReleaseDC(wi->wnd, wdc);
		}
		else
		{
			rect = ATRR(action.d.image.rect);
			Image::HdcRgbmap(Global::qi.hdc, rgbMap, rect);
		}
		Image::FindResult result = Image::Find(rgbMap, action.d.image.map, action.d.image.sim, 10);
		if (action.d.image.unfind && result.find) return 0;
		else if (result.find)
		{
			if (action.d.image.move)
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
		for (uint32 i = 0; i < action.next.size(); i++)
		{
			uint8 r = ActionExecute(action.next.at(i), macro, wi);
			if (r == 1) return 1;
			else if (r == 2) return 2;
		}
		return 0;
	}

	case Action::_popText:
	{
		TipsWindow::Popup(action.d.popText.str.str(), RGB(223, 223, 223), action.d.popText.time);
		return 0;
	}
	}
	return 0;
}

//////////////////////////// Threads
static DWORD _stdcall ThreadQuickClick(PVOID)
{
	srand(clock());
	uint32 b = 0, e = 0;
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
static DWORD _stdcall ThreadMacroEnding(PVOID pMacro)
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
		if (ActionExecute(macro->actionsEnding.at(n), *macro, pWi))
		{
			macro->threadEnding = 0;
			return 0;
		}
	}
	macro->threadEnding = 0;
	return 0;
}
static DWORD _stdcall ThreadMacro(PVOID pMacro)
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
			for (uint32 i = 0; i < wnds.size(); i++)
			{
				if (IsWindowVisible(wnds.at(1)))
				{
					wi.children.at(i).wnd = wnds[i];
					wi.children.at(i).rect = Window::childRect(wi.wnd, wi.children.at(i).wnd);
				}
			}
		}

		pWi = &wi;
	}

	GetCursorPos(&macro->originPos);
	uint32 count = 0;
	while (Global::qi.run)
	{
		if (macro->count) { count++; if (count > macro->count) break; } // if count = 0 then while is infinite
		for (uint32 i = 0; i < macro->actions.size(); i++)
		{
			if (ActionExecute(macro->actions.at(i), *macro, pWi))
			{
				macro->thread = 0;
				return 0;
			}
		}
	}
	macro->thread = 0;
	return 0;
}
static DWORD _stdcall ThreadRelease(PVOID key)
{
	Input::State((BYTE)key, 0, 214);
	return 0;
}
static DWORD _stdcall ThreadWndActive(PVOID)
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
		if (!InputHook::State())
		{
			timeBeginPeriod(1); // set clock accuracy, default is 16ms: sleep(1) = sleep(16)
			if (!InputHook::Start(InputHook::all)) MsgBox::Error(L"创建输入Hook失败，检查是否管理员身份运行 或 是否被安全软件拦截。");
		}
	}
	else
	{
		if (InputHook::State())
		{
			timeEndPeriod(1); // reset clock accuracy
			InputHook::Stop(InputHook::all);
		}
	}
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