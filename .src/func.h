#pragma once
#include "type.h"
#include "MsgPop.h"

class QiFn
{
public:
	//////////////////////////// Pos convert
	static POINT RTA(POINT rel) { return { (long)(((float)rel.x / (float)(qis.screen.cx - 1)) * 10000.0f), (long)(((float)rel.y / (float)(qis.screen.cy - 1)) * 10000.0f) }; }
	static RECT RTAR(RECT rel) { return { (long)(((float)rel.left / (float)(qis.screen.cx - 1)) * 10000.0f), (long)(((float)rel.top / (float)(qis.screen.cy - 1)) * 10000.0f), (long)(((float)rel.right / (float)(qis.screen.cx - 1)) * 10000.0f), (long)(((float)rel.bottom / (float)(qis.screen.cy - 1)) * 10000.0f) }; }
	static POINT ATR(POINT abs) { return { (long)((float)qis.screen.cx / 10000.0f * (float)abs.x), (long)((float)qis.screen.cy / 10000.0f * (float)abs.y) }; }
	static RECT ATRR(RECT abs) { return { (long)((float)qis.screen.cx / 10000.0f * (float)abs.left), (long)((float)qis.screen.cy / 10000.0f * (float)abs.top), (long)((float)qis.screen.cx / 10000.0f * (float)abs.right), (long)((float)qis.screen.cy / 10000.0f * (float)abs.bottom) }; }
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
				jItem.Add("state", action.d.keyState.state, true);
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
		SaveAction(jActions, macro.acRun);
		SaveAction(jActionsEnding, macro.acEnd);
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
		cfg.Add("theme", qis.set.theme);
		cfg.Add("key", qis.set.key);
		cfg.Add("recKey", qis.set.recKey);
		cfg.Add("defOn", qis.set.defOn, true);
		cfg.Add("showTips", qis.set.showTips, true);
		cfg.Add("audFx", qis.set.audFx, true);
		cfg.Add("minMode", qis.set.minMode, true);
		cfg.Add("zoomBlock", qis.set.zoomBlock, true);
		cfg.Add("quickClickKey", qis.fun.quickClick.key);
		cfg.Add("quickClickState", qis.fun.quickClick.state, true);
		cfg.Add("quickClickDelay", qis.fun.quickClick.delay);
		cfg.Add("quickClickMode", qis.fun.quickClick.mode);
		cfg.Add("showClockKey", qis.fun.showClock.key);
		cfg.Add("showClockState", qis.fun.showClock.state, true);
		cfg.Add("wndActiveState", qis.fun.wndActive.state, true);
		cfg.Add("wndActiveName", String::toString(qis.fun.wndActive.wi.wndName));
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
		qis.macros.clear();
		File::FileList files = File::FindFile(L"macro\\*.json");
		for (uint32 i = 0; i < files.size(); i++) {
			std::wstring path = Path::Append(Path::Append(Process::runPath(), L"macro"), files[i].name);
			Macro& macro = qis.macros.AddNull(); macro.name = Path::RemoveExtension(files[i].name);

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
				LoadAction(jActions, macro.acRun);
				LoadAction(jActionsEnding, macro.acEnd);
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
				cfg.Get("theme", qis.set.theme);
				cfg.Get("key", qis.set.key);
				cfg.Get("recKey", qis.set.recKey);
				cfg.Get("defOn", qis.set.defOn);
				cfg.Get("showTips", qis.set.showTips);
				cfg.Get("audFx", qis.set.audFx);
				cfg.Get("minMode", qis.set.minMode);
				cfg.Get("zoomBlock", qis.set.zoomBlock);
				cfg.Get("quickClickState", qis.fun.quickClick.state);
				cfg.Get("quickClickKey", qis.fun.quickClick.key);
				cfg.Get("quickClickDelay", qis.fun.quickClick.delay);
				cfg.Get("quickClickMode", qis.fun.quickClick.mode);
				cfg.Get("showClockState", qis.fun.showClock.state);
				cfg.Get("showClockKey", qis.fun.showClock.key);
				cfg.Get("wndActiveState", qis.fun.wndActive.state);
				cfg.Get("wndActiveName", str); qis.fun.wndActive.wi.wndName = String::toWString(str);
				LoadMacro();
				return;
			}
		}
		{
			qis.set.defOn = true;
			qis.set.key = VK_F8;
			qis.set.recKey = VK_F8;
			qis.set.showTips = true;
			qis.set.audFx = false;
			qis.set.minMode = false;
			qis.set.zoomBlock = false;
			qis.fun.quickClick.state = false;
			qis.fun.quickClick.key = VK_LBUTTON;
			qis.fun.quickClick.delay = 200;
			qis.fun.quickClick.mode = 0;
			qis.fun.showClock.state = false;
			qis.fun.showClock.key = VK_MENU;
			qis.fun.wndActive.state = false;
			qis.fun.wndActive.wi.wndName = L"";
		}
		LoadMacro();
	}
	//////////////////////////// #Json

	static WndInfo WindowSelection()
	{
		WndInfo wi;
		sleep(20);
		MsgPop::Show(L"按ESC取消，按回车开始，再按回车获取窗口", RGB(0x20, 0xFF, 0x20));
		while (true)
		{
			if (Input::state(VK_RETURN)) break;
			if (Input::state(VK_ESCAPE)) { MsgPop::Hide(); return {}; }
			sleep(10);
		}
		Input::Loop(VK_RETURN);
		while (!Input::state(VK_RETURN))
		{
			wi.wnd = GetForegroundWindow();
			wi.wndName = Window::text(wi.wnd);
			wi.wndClass = Window::className(wi.wnd);
			MsgPop::Show(wi.wndName);
			sleep(50);
		}
		MsgPop::Popup(wi.wndName, RGB(0x20, 0xFF, 0x20));
		return wi;
	}

	// return 0: next, return 1: stop all, return 2: stop loop;
	static uint8 ActionExecute(const Action& action, bool& state, MSG& msg, const POINT& origin, WndInput* wi)
	{
		if (PeekMessageW(&msg, 0, WM_USER, WM_USER, PM_REMOVE) || (!qis.run)) state = false;
		if (state)
		{
			switch (action.type)
			{
			case Action::_end: return 1;

			case Action::_delay:
			{
				int32 ms = Rand(action.d.delay.ms + action.d.delay.ex, (int32)action.d.delay.ms - (int32)action.d.delay.ex);
				if (ms < 0) ms = 0;
				Thread::Sleep(ms);
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
					if (action.d.key.state == QiKey::up) Input::State(action.d.key.vk, false, 214);
					else if (action.d.key.state == QiKey::down) Input::State(action.d.key.vk, true, 214);
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

						wi->pt = InRectPos(min.rect, wi->pt);
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
				HDC hdc;
				if (wi)
				{
					hdc = GetDC(wi->wnd);
					rect = WATRR(action.d.color.rect, wi->wnd);
					Image::toRgbMap(hdc, rgbMap, rect);
					ReleaseDC(wi->wnd, hdc);
				}
				else
				{
					hdc = GetDC(nullptr);
					rect = ATRR(action.d.color.rect);
					Image::toRgbMap(hdc, rgbMap, rect);
					ReleaseDC(nullptr, hdc);
				}
				Color::FindResult result = Color::FindOr(rgbMap, action.d.color.rgbe.toRgb(), action.d.color.rgbe.a);
				if (action.d.color.unfind)
				{
					if (result.find) return 0;
				}
				else
				{
					if (result.find)
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
				}
				for (uint32 i = 0; i < action.next.size(); i++)
				{
					uint8 r = ActionExecute(action.next.at(i), state, msg, origin, wi);
					if (r != 0) return r;
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
				while (qis.run)
				{
					if (uloop)
					{
						n++;
						if (n > e) break;
					}
					for (uint32 i = 0; i < action.next.size(); i++)
					{
						uint8 r = ActionExecute(action.next.at(i), state, msg, origin, wi);
						if (r != 0)
						{
							if (r == 2) return 0;
							return 1;
						}
					}
				}
				return 0;
			}

			case Action::_loopEnd: return 2;

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
					uint8 r = ActionExecute(action.next.at(i), state, msg, origin, wi);
					if (r != 0) return r;
				}
				return 0;
			}

			case Action::_revocerPos:
			{
				Input::MoveTo(origin.x, origin.y);
				return 0;
			}

			case Action::_image:
			{
				RgbMap rgbMap;
				RECT rect;
				HDC hdc;
				if (wi)
				{
					hdc = GetDC(wi->wnd);
					rect = WATRR(action.d.image.rect, wi->wnd);
					Image::toRgbMap(hdc, rgbMap, rect);
					ReleaseDC(wi->wnd, hdc);
				}
				else
				{
					hdc = GetDC(nullptr);
					rect = ATRR(action.d.image.rect);
					Image::toRgbMap(hdc, rgbMap, rect);
					ReleaseDC(nullptr, hdc);
				}
				Image::FindResult result = Image::Find(rgbMap, action.d.image.map, action.d.image.sim, 10);

				if (action.d.image.unfind)
				{
					if (result.find) return 0;
				}
				else
				{
					if (result.find)
					{
						if (action.d.image.move)
						{
							result.pt.x += rect.left + (action.d.image.map.width() >> 1), result.pt.y += rect.top + (action.d.image.map.height() >> 1);
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

				for (uint32 i = 0; i < action.next.size(); i++)
				{
					uint8 r = ActionExecute(action.next.at(i), state, msg, origin, wi);
					if (r != 0) return r;
				}
				return 0;
			}

			case Action::_popText:
			{
				MsgPop::Popup(action.d.popText.str.str(), RGB(223, 223, 223), action.d.popText.time);
				return 0;
			}
			}
		}
		return 1;
	}

	//////////////////////////// Threads
	static DWORD _stdcall ThreadQuickClick(PVOID)
	{
		srand(clock());
		uint32 b = 0, e = 0;
		if (qis.fun.quickClick.delay > 99) b = Rand(70, 30), e = qis.fun.quickClick.delay - Rand(70, 30);
		else if (qis.fun.quickClick.delay > 1) b = Rand(qis.fun.quickClick.delay, qis.fun.quickClick.delay >> 2), e = Rand(qis.fun.quickClick.delay, qis.fun.quickClick.delay >> 2);
		else e = qis.fun.quickClick.delay;

		while (qis.run)
		{
			Input::State(qis.fun.quickClick.key, 1, 214);
			Thread::Sleep(b);
			Input::State(qis.fun.quickClick.key, 0, 214);
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
			if (!macro->wi.wnd) return 0;
		}

		POINT origin = macro->origin;
		bool state = true;
		MSG msg;
		for (uint32 i = 0; i < macro->acEnd.size(); i++)
		{
			if (ActionExecute(macro->acEnd.at(i), state, msg, origin, pWi)) return 0;
		}
		return 0;
	}
	static DWORD _stdcall ThreadMacro(PVOID pMacro)
	{
		srand(clock());
		Macro* macro = (Macro*)pMacro;
		WndInput wi;
		WndInput* pWi = nullptr;
		if (macro->wndState)
		{
			// window is not found
			macro->wi.wnd = FindWindowW(macro->wi.wndClass.c_str(), macro->wi.wndName.c_str());
			if (!macro->wi.wnd) macro->wi = WindowSelection();
			if (!macro->wi.wnd)
			{
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

		GetCursorPos(&macro->origin);
		POINT origin = macro->origin;
		bool state = true;
		MSG msg;
		uint32 count = 0;
		while (state)
		{
			if (PeekMessageW(&msg, 0, WM_USER, WM_USER, PM_REMOVE) || (!qis.run)) state = false;
			if (macro->count) { count++; if (count > macro->count) break; } // if count = 0 then while is infinite
			for (uint32 i = 0; i < macro->acRun.size(); i++)
			{
				if (ActionExecute(macro->acRun.at(i), state, msg, origin, pWi)) return 0;
			}
		}
		return 0;
	}
	static DWORD _stdcall ThreadRelease(PVOID key)
	{
		Input::State((BYTE)key, 0, 214);
		return 0;
	}
	static DWORD _stdcall ThreadWndActive(PVOID)
	{
		while (qis.state)
		{
			qis.fun.wndActive.wi.wnd = FindWindowW(0, qis.fun.wndActive.wi.wndName.c_str());
			if (qis.fun.wndActive.wi.wnd)
			{
				bool active = (GetForegroundWindow() == qis.fun.wndActive.wi.wnd);
				if (!qis.run && active)
				{
					qis.run = 1;
					if (qis.set.showTips) MsgPop::Popup(L"已启用 - 窗口内", RGB(0xA0, 0xFF, 0xC0));
				}
				else if (qis.run && !active)
				{
					qis.run = 0;
					if (qis.set.showTips) MsgPop::Popup(L"已禁用 - 窗口外", RGB(0xFF, 0x80, 0x80));
				}
			}
			else if (qis.run)
			{
				qis.run = 0;
				if (qis.set.showTips) MsgPop::Popup(L"已禁用 - 窗口外", RGB(0xFF, 0x80, 0x80));
			}
			sleep(100);
		}
		qis.fun.wndActive.thread = 0;
		return 0;
	}
	//////////////////////////// #Threads

	static std::wstring NameFilter(std::wstring name)
	{
		if (!qis.macros.size()) return name;
		for (uint32 n = 0;; n++)
		{
			for (uint32 p = 0; p < qis.macros.size(); p++)
			{
				std::wstring find = name + L" " + String::toWString(n + 1);
				if (qis.macros[p].name == find) break;
				if (p >= qis.macros.size() - 1) return find;
			}
		}
		return L"";
	}
	static void UpdateBlockKeys()
	{
		qis.blockKeys.clear();
		for (uint32 u = 0; u < qis.macros.size(); u++)
		{
			if (qis.macros[u].state && qis.macros[u].block)
			{
				if ((qis.macros[u].key & 0xFFFF)) qis.blockKeys.Add((qis.macros[u].key & 0xFFFF));
				if (qis.macros[u].key >> 16) qis.blockKeys.Add(qis.macros[u].key >> 16);
			}
		}
		memset(qis.keyState, 0, sizeof(qis.keyState));
	}

	static void QiHook(bool state)
	{
		if (state)
		{
			if (!InputHook::State())
			{
				timeBeginPeriod(1); // set clock accuracy, default is 16ms: sleep(1) = sleep(16)
				if (!InputHook::Start()) MsgBox::Error(L"创建输入Hook失败，检查是否管理员身份运行 或 是否被安全软件拦截。");
			}
		}
		else
		{
			if (InputHook::State())
			{
				timeEndPeriod(1); // reset clock accuracy
				InputHook::Close();
			}
		}
	}
	static void QiState(bool state)
	{
		if (state)
		{
			UpdateBlockKeys();
			qis.state = true;
			qis.screen = System::screenSize();
			MsgPop::Popup(qis.ui.qiOn);
			if (qis.set.audFx) Media::WavePlay(audfx_on);
			if (qis.fun.wndActive.state) { if (!qis.fun.wndActive.thread) qis.fun.wndActive.thread = Thread::Start(ThreadWndActive); }
			else qis.run = true;
		}
		else
		{
			qis.state = false, qis.run = false;
			MsgPop::Popup(qis.ui.qiOff, RGB(0xFF, 0x50, 0x50));
			if (qis.set.audFx)Media::WavePlay(audfx_off);
		}
	}

	static void Trigger(BYTE vk)
	{
		// state swtich
		if (vk == (qis.set.key & 0xFFFF) || vk == (qis.set.key >> 16))
		{
			bool k1 = qis.keyState[qis.set.key & 0xFFFF];
			bool k2 = true;
			if (qis.set.key >> 16) k2 = qis.keyState[qis.set.key >> 16];
			if (k1 && k2)
			{
				if (qis.state) QiState(false);
				else QiState(true);
			}
		}

		// show clock
		if (qis.fun.showClock.state && qis.fun.showClock.key == vk && qis.keyState[qis.fun.showClock.key]) { MsgPop::Popup(Time::toWStringT()); }

		if (!qis.run) return;

		// quick click
		if (qis.fun.quickClick.state && qis.fun.quickClick.key == vk)
		{
			if (qis.keyState[qis.fun.quickClick.key])
			{
				if (qis.fun.quickClick.mode) // switch mode
				{
					if (qis.fun.quickClick.thread)
					{
						Thread::Close(qis.fun.quickClick.thread); qis.fun.quickClick.thread = 0;
						Thread::Start(ThreadRelease, (PVOID)qis.fun.quickClick.key);
						if (qis.set.showTips) MsgPop::Popup(std::wstring(Input::Name(qis.fun.quickClick.key)) + L"ㅤ连点结束", RGB(0xFF, 0xFF, 0x60));
					}
					else
					{
						qis.fun.quickClick.thread = Thread::Start(ThreadQuickClick);
						if (qis.set.showTips) MsgPop::Popup(std::wstring(Input::Name(qis.fun.quickClick.key)) + L"ㅤ连点开始", RGB(0xFF, 0xAF, 0xEF));
					}
				}
				else // press mode
				{
					if (!qis.fun.quickClick.thread)
					{
						qis.fun.quickClick.thread = Thread::Start(ThreadQuickClick);
						if (qis.set.showTips) MsgPop::Popup(std::wstring(Input::Name(qis.fun.quickClick.key)) + L"ㅤ连点开始", RGB(0x20, 0xFF, 0x40));
					}
				}
			}
			else
			{
				if (!qis.fun.quickClick.mode) // press mode
				{
					if (qis.fun.quickClick.thread)
					{
						Thread::Close(qis.fun.quickClick.thread); qis.fun.quickClick.thread = 0;
						Thread::Start(ThreadRelease, (LPVOID)qis.fun.quickClick.key);
						if (qis.set.showTips) MsgPop::Popup(std::wstring(Input::Name(qis.fun.quickClick.key)) + L"ㅤ连点结束", RGB(0xFF, 0xFF, 0x60));
					}
				}
			}
		}

		// macros
		for (uint32 i = 0; i < qis.macros.size(); i++)
		{
			if (vk == (qis.macros.at(i).key & 0xFFFF) || vk == (qis.macros.at(i).key >> 16))
			{
				if (qis.macros.at(i).state)
				{
					bool k1 = qis.keyState[qis.macros.at(i).key & 0xFFFF];
					bool k2 = true;
					if (qis.macros.at(i).key >> 16) k2 = qis.keyState[qis.macros.at(i).key >> 16];

					DWORD exitCode = 0;
					GetExitCodeThread(qis.macros.at(i).thRun, &exitCode);
					switch (qis.macros.at(i).mode)
					{
					case Macro::sw:
					{
						if (k1 && k2) // trigger keys is pressed
						{
							if (exitCode == STILL_ACTIVE) // Running
							{
								// Close running
								PostThreadMessageW(GetThreadId(qis.macros.at(i).thRun), WM_USER, 0, 0);
								// Start ending
								if (qis.macros.at(i).acEnd.size()) qis.macros.at(i).thEnd = Thread::Start(ThreadMacroEnding, &qis.macros.at(i));
								// Show Tips
								if (qis.set.showTips) MsgPop::Popup(qis.macros.at(i).name + L"ㅤ结束", RGB(0xFF, 0xFF, 0x60));
							}
							else // None or ending
							{
								// Close ending
								PostThreadMessageW(GetThreadId(qis.macros.at(i).thEnd), WM_USER, 0, 0);
								// Start running
								qis.macros.at(i).thRun = Thread::Start(ThreadMacro, &qis.macros.at(i));
								if (qis.set.showTips) MsgPop::Popup(qis.macros.at(i).name + L"ㅤ开始", RGB(0xFF, 0xAF, 0xEF));
							}
						}
					}
					break;

					case Macro::down:
					{
						if (k1 && k2) // trigger keys is pressed
						{
							// Close running
							PostThreadMessageW(GetThreadId(qis.macros.at(i).thRun), WM_USER, 0, 0);
							// Close ending
							PostThreadMessageW(GetThreadId(qis.macros.at(i).thEnd), WM_USER, 0, 0);
							// Start running
							qis.macros.at(i).thRun = Thread::Start(ThreadMacro, &qis.macros.at(i));
							// Show Tips
							if (qis.set.showTips)
							{
								if (qis.macros.at(i).count) MsgPop::Popup(qis.macros.at(i).name + L"ㅤ" + String::toWString(qis.macros.at(i).count) + L"次", RGB(0x20, 0xFF, 0x40));
								else MsgPop::Popup(qis.macros.at(i).name + L"ㅤ循环", RGB(0x20, 0xFF, 0x40));
							}
						}
						else // trigger keys is released
						{
							if ((exitCode == STILL_ACTIVE) && qis.macros.at(i).count == 0)
							{
								// Close running
								PostThreadMessageW(GetThreadId(qis.macros.at(i).thRun), WM_USER, 0, 0);
								// Start ending
								if (qis.macros.at(i).acEnd.size()) qis.macros.at(i).thEnd = Thread::Start(ThreadMacroEnding, &qis.macros.at(i));
								// Show Tips
								if (qis.set.showTips) MsgPop::Popup(qis.macros.at(i).name + L"ㅤ结束", RGB(0xFF, 0xFF, 0x60));
							}
						}
					}
					break;

					case Macro::up:
					{
						if (k1 && k2) // trigger keys is pressed
						{
							qis.macros.at(i).active = true;
							if ((exitCode == STILL_ACTIVE) && qis.macros.at(i).count == 0)
							{
								// Close running
								PostThreadMessageW(GetThreadId(qis.macros.at(i).thRun), WM_USER, 0, 0);
								// Start ending
								if (qis.macros.at(i).acEnd.size()) qis.macros.at(i).thEnd = Thread::Start(ThreadMacroEnding, &qis.macros.at(i));
								// Show Tips
								if (qis.set.showTips) MsgPop::Popup(qis.macros.at(i).name + L"ㅤ结束", RGB(0xFF, 0xFF, 0x60));
							}
						}
						else // trigger keys is released
						{
							if (qis.macros.at(i).active)
							{
								qis.macros.at(i).active = false;

								// Close running
								PostThreadMessageW(GetThreadId(qis.macros.at(i).thRun), WM_USER, 0, 0);
								// Close ending
								PostThreadMessageW(GetThreadId(qis.macros.at(i).thEnd), WM_USER, 0, 0);
								// Start running
								qis.macros.at(i).thRun = Thread::Start(ThreadMacro, &qis.macros.at(i));
								// Show Tips
								if (qis.set.showTips)
								{
									if (qis.macros.at(i).count) MsgPop::Popup(qis.macros.at(i).name + L"ㅤ" + String::toWString(qis.macros.at(i).count) + L"次", RGB(0xA0, 0xFF, 0x50));
									else MsgPop::Popup(qis.macros.at(i).name + L"ㅤ循环", RGB(0xA0, 0xFF, 0x50));
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
};