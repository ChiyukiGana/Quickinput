﻿#pragma execution_character_set("utf-8")
#include "func.h"

namespace QiFn
{
	POINT RTA(POINT rel) { return { (long)(((float)rel.x / (float)(qis.screen.cx - 1)) * 10000.0f), (long)(((float)rel.y / (float)(qis.screen.cy - 1)) * 10000.0f) }; }
	RECT RTAR(RECT rel) { return { (long)(((float)rel.left / (float)(qis.screen.cx - 1)) * 10000.0f), (long)(((float)rel.top / (float)(qis.screen.cy - 1)) * 10000.0f), (long)(((float)rel.right / (float)(qis.screen.cx - 1)) * 10000.0f), (long)(((float)rel.bottom / (float)(qis.screen.cy - 1)) * 10000.0f) }; }
	POINT ATR(POINT abs) { return { (long)((float)qis.screen.cx / 10000.0f * (float)abs.x), (long)((float)qis.screen.cy / 10000.0f * (float)abs.y) }; }
	RECT ATRR(RECT abs) { return { (long)((float)qis.screen.cx / 10000.0f * (float)abs.left), (long)((float)qis.screen.cy / 10000.0f * (float)abs.top), (long)((float)qis.screen.cx / 10000.0f * (float)abs.right), (long)((float)qis.screen.cy / 10000.0f * (float)abs.bottom) }; }
	POINT WRTA(POINT rel, HWND wnd) { SIZE size = Window::size(wnd); return { (long)(((float)rel.x / ((float)size.cx)) * 10000.0f), (long)(((float)rel.y / ((float)size.cy)) * 10000.0f) }; }
	RECT WRTAR(RECT rel, HWND wnd) { SIZE size = Window::size(wnd); return { (long)(((float)rel.left / ((float)size.cx)) * 10000.0f), (long)(((float)rel.top / ((float)size.cy)) * 10000.0f), (long)(((float)rel.right / ((float)size.cx)) * 10000.0f), (long)(((float)rel.bottom / ((float)size.cy)) * 10000.0f) }; }
	POINT WATR(POINT abs, HWND wnd) { SIZE size = Window::size(wnd); return { (long)((float)(size.cx + 1) / 10000.0f * (float)abs.x), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.y) }; }
	RECT WATRR(RECT abs, HWND wnd) { SIZE size = Window::size(wnd); return { (long)((float)(size.cx + 1) / 10000.0f * (float)abs.left), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.top), (long)((float)(size.cx + 1) / 10000.0f * (float)abs.right), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.bottom) }; }

	std::wstring ParseQPop(std::wstring text)
	{
		size_t p = text.find(L"@");
		if (p != std::wstring::npos) text.replace(text.find(L"@"), 1, qis.dir);
		return text;
	}
	std::wstring ParseWPop(std::wstring text, std::wstring window)
	{
		size_t p = text.find(L"@");
		if (p != std::wstring::npos) text.replace(text.find(L"@"), 1, window);
		return text;
	}
	std::wstring ParseQcPop(std::wstring text)
	{
		size_t p = text.find(L"@");
		if (p != std::wstring::npos) text.replace(text.find(L"@"), 1, Input::Name(qis.fun.quickClick.key));
		return text;
	}
	std::wstring ParseMacroPop(std::wstring text, std::wstring macro, size_t count)
	{
		size_t p = text.find(L"@");
		if (p != std::wstring::npos) text.replace(text.find(L"@"), 1, macro);
		p = text.find(L"$");
		if (p != std::wstring::npos)
		{
			if (count) text.replace(p, 1, std::to_wstring(count));
			else text.replace(p, 1, L"∞");
		}
		return text;
	}
	void QPop(QiUi::PopBoxBase pop)
	{
		PopBox::Popup(ParseQPop(pop.t), pop.c);
	}
	void WPop(std::wstring text, std::wstring window)
	{
		PopBox::Popup(ParseWPop(text, window));
	}
	void QcPop(QiUi::PopBoxBase pop)
	{
		PopBox::Popup(ParseQcPop(pop.t), pop.c);
	}
	void MacroPop(Macro* macro, QiUi::PopBoxBase pop)
	{
		PopBox::Popup(ParseMacroPop(pop.t, macro->name, macro->count), pop.c);
	}

	bool SelfActive() { if (qis.widget.mainActive || qis.widget.dialogActive || qis.widget.moreActive) return false; }
	std::wstring NameFilter(std::wstring name)
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
	bool TransMove(int sx, int sy, int dx, int dy, int step, POINT& pt)
	{
		int tx = 0;
		int ty = 0;
		int rx = 0;
		int ry = 0;

		if (sx > dx) tx = sx - dx;
		else tx = dx - sx;
		if (sy > dy) ty = sy - dy;
		else ty = dy - sy;

		int len;
		if (tx > ty) len = ((float)ty / (float)tx) * step;
		else len = ((float)tx / (float)ty) * step;

		if (sx > dx)
		{
			if (tx > ty) rx = sx - step;
			else rx = sx - len;
			if (rx < dx) rx = dx;
		}
		else
		{
			if (tx > ty) rx = sx + step;
			else rx = sx + len;
			if (rx > dx) rx = dx;
		}
		if (sy > dy)
		{
			if (tx > ty) ry = sy - len;
			else ry = sy - step;
			if (ry < dy) ry = dy;
		}
		else
		{
			if (tx > ty) ry = sy + len;
			else ry = sy + step;
			if (ry > dy) ry = dy;
		}

		if (rx == dx && ry == dy)
		{
			pt = { rx, ry };
			return false;
		}
		pt = { rx, ry };
		return true;
	}
	WndInfo WindowSelection()
	{
		WndInfo wi;
		sleep(20);
		PopBox::Show(L"按ESC取消，按回车开始，再按回车获取窗口", RGB(0x20, 0xFF, 0x20));
		while (true)
		{
			if (Input::state(VK_RETURN)) break;
			if (Input::state(VK_ESCAPE)) { PopBox::Hide(); return {}; }
			sleep(10);
		}
		Input::Loop(VK_RETURN);
		while (!Input::state(VK_RETURN))
		{
			wi.wnd = GetForegroundWindow();
			wi.wndName = Window::text(wi.wnd);
			wi.wndClass = Window::className(wi.wnd);
			PopBox::Show(wi.wndName);
			sleep(50);
		}
		PopBox::Popup(wi.wndName, RGB(0x20, 0xFF, 0x20));
		return wi;
	}

	uint8 ActionExecute(const Action& action, POINT& cursor, WndInput* wi)
	{
		if (!qis.run || QiThread::PeekExitMsg()) return r_exit;

		switch (action.type)
		{
		case Action::_end: return r_exit;

		case Action::_delay:
		{
			Thread::Sleep(Rand(action.d.delay.tmax, action.d.delay.tmin));
			return r_continue;
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
			return r_continue;
		}

		case Action::_mouse:
		{
			if (wi)
			{
				POINT pt = {};
				pt.x = action.d.mouse.x + (Rand(action.d.mouse.ex, (~action.d.mouse.ex + 1)));
				pt.y = action.d.mouse.y + (Rand(action.d.mouse.ex, (~action.d.mouse.ex + 1)));
				if (action.d.mouse.move) wi->pt.x += pt.x, wi->pt.y += pt.y;
				else wi->pt = WATR({ pt.x, pt.y }, wi->wnd);
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
				POINT pt = { };
				pt.x = action.d.mouse.x + (Rand(action.d.mouse.ex, (~action.d.mouse.ex + 1)));
				pt.y = action.d.mouse.y + (Rand(action.d.mouse.ex, (~action.d.mouse.ex + 1)));
				if (action.d.mouse.move)
				{
					if (action.d.mouse.track)
					{
						POINT spt = {};
						int disx = Distance(action.d.mouse.x, spt.x);
						int disy = Distance(action.d.mouse.y, spt.y);
						if (disx < disy) disx = disy;
						int s = action.d.mouse.speed;
						if (!s) s = 1;
						disx *= ((float)s / 4.0f / 100.0f);
						if (disx < 4) disx = 1;
						POINT prev = {};
						for (int i = 0; true; i += disx)
						{
							POINT rpt;
							bool r = TransMove(spt.x, spt.y, pt.x, pt.y, i, rpt);
							Input::Move(rpt.x - prev.x, rpt.y - prev.y);
							prev = rpt;
							if (!r) break;
							sleep(5);
						}
					}
					else Input::Move(pt.x, pt.y);
				}
				else
				{
					if (action.d.mouse.track)
					{
						POINT spt = {};
						GetCursorPos(&spt);
						spt = RTA(spt);
						int disx = Distance(action.d.mouse.x, spt.x);
						int disy = Distance(action.d.mouse.y, spt.y);
						if (disx < disy) disx = disy;
						int s = action.d.mouse.speed;
						if (!s) s = 1;
						disx *= ((float)s / 4.0f / 100.0f);
						if (disx < 1) disx = 1;
						for (int i = 0; true; i += disx)
						{
							POINT rpt;
							bool r = TransMove(spt.x, spt.y, pt.x, pt.y, i, rpt);
							Input::MoveToA(rpt.x * 6.5535, rpt.y * 6.5535);
							if (!r) break;
							sleep(5);
						}
					}
					else Input::MoveToA(pt.x * 6.5535, pt.y * 6.5535);
				}
			}
			return r_continue;
		}

		case Action::_text:
		{
			System::ClipBoardText(action.d.text.str.str());
			return r_continue;
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
				if (result.find) return r_continue;
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
				else return r_continue;
			}
			for (uint32 i = 0; i < action.next.size(); i++)
			{
				uint8 r = ActionExecute(action.next.at(i), cursor, wi);
				if (r != r_continue) return r;
			}
			return r_continue;
		}

		case Action::_loop:
		{
			uint32 n = 0;
			uint32 e = 0;
			bool uloop = false;
			if (action.d.loop.cmax) uloop = true, e = Rand(action.d.loop.cmax, action.d.loop.cmin);
			while (qis.run && !QiThread::PeekExitMsg())
			{
				if (uloop)
				{
					n++;
					if (n > e) break;
				}
				for (uint32 i = 0; i < action.next.size(); i++)
				{
					uint8 r = ActionExecute(action.next.at(i), cursor, wi);
					if (r != r_continue)
					{
						if (r == r_break) return r_continue;
						else if (r == r_exit) return r_exit;
					}
				}
			}
			return r_continue;
		}

		case Action::_loopEnd: return r_break;

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
				uint8 r = ActionExecute(action.next.at(i), cursor, wi);
				if (r != r_continue) return r;
			}
			return r_continue;
		}

		case Action::_revocerPos:
		{
			Input::MoveTo(cursor.x, cursor.y);
			return r_continue;
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
				if (result.find) return r_continue;
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
				else return r_continue;
			}

			for (uint32 i = 0; i < action.next.size(); i++)
			{
				uint8 r = ActionExecute(action.next.at(i), cursor, wi);
				if (r != r_continue) return r;
			}
			return r_continue;
		}

		case Action::_popText:
		{
			PopBox::Popup(action.d.popText.str.str(), RGB(223, 223, 223), action.d.popText.time);
			return r_continue;
		}

		case Action::_rememberPos:
		{
			cursor = Input::pos();
			return r_continue;
		}
		}
		return r_continue;
	}
	void Trigger(byte vk)
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
		if (qis.fun.showClock.state && qis.fun.showClock.key == vk && qis.keyState[qis.fun.showClock.key]) { PopBox::Popup(Time::toWStringT()); }

		if (!qis.run) return;

		// quick click
		if (qis.fun.quickClick.state && qis.fun.quickClick.key == vk)
		{
			if (qis.keyState[qis.fun.quickClick.key]) // key down
			{
				if (qis.fun.quickClick.mode) // switch mode
				{
					if (QiThread::QuickClickActive())
					{
						QiThread::ExitQuickClick();
						QiThread::AddReleaseKey(qis.fun.quickClick.key);
						if (qis.set.showTips) QcPop(qis.ui.pop.qcd);
					}
					else
					{
						QiThread::StartQuickClick();
						if (qis.set.showTips) QcPop(qis.ui.pop.qce);
					}
				}
				else // press mode
				{
					if (!QiThread::QuickClickActive())
					{
						QiThread::StartQuickClick();
						if (qis.set.showTips) QcPop(qis.ui.pop.qce);
					}
				}
			}
			else // key up
			{
				if (!qis.fun.quickClick.mode) // press mode
				{
					if (QiThread::QuickClickActive())
					{
						QiThread::ExitQuickClick();
						QiThread::AddReleaseKey(qis.fun.quickClick.key);
						if (qis.set.showTips) QcPop(qis.ui.pop.qcd);
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

					switch (qis.macros.at(i).mode)
					{
					case Macro::sw:
					{
						if (k1 && k2)
						{
							if (QiThread::MacroRunActive(&qis.macros.at(i)))
							{
								QiThread::ExitMacroRun(&qis.macros.at(i));
								QiThread::StartMacroEnd(&qis.macros.at(i));
								if (qis.set.showTips) MacroPop(&qis.macros.at(i), qis.ui.pop.swd);
							}
							else
							{
								QiThread::ExitMacroEnd(&qis.macros.at(i));
								QiThread::StartMacroRun(&qis.macros.at(i));
								if (qis.set.showTips) MacroPop(&qis.macros.at(i), qis.ui.pop.swe);
							}
						}
					}
					break;

					case Macro::down:
					{
						if (k1 && k2)
						{
							QiThread::ExitMacroRun(&qis.macros.at(i));
							QiThread::ExitMacroEnd(&qis.macros.at(i));
							QiThread::StartMacroRun(&qis.macros.at(i));
							if (qis.set.showTips) MacroPop(&qis.macros.at(i), qis.ui.pop.dwe);
						}
						else
						{
							if (qis.macros.at(i).count == 0 && QiThread::MacroRunActive(&qis.macros.at(i)))
							{
								QiThread::ExitMacroRun(&qis.macros.at(i));
								QiThread::StartMacroEnd(&qis.macros.at(i));
								if (qis.set.showTips) MacroPop(&qis.macros.at(i), qis.ui.pop.dwd);
							}
						}
					}
					break;

					case Macro::up:
					{
						if (k1 && k2)
						{
							qis.macros.at(i).active = true;
							if (qis.macros.at(i).count == 0 && QiThread::MacroRunActive(&qis.macros.at(i)))
							{
								QiThread::ExitMacroRun(&qis.macros.at(i));
								QiThread::StartMacroEnd(&qis.macros.at(i));
								if (qis.set.showTips) MacroPop(&qis.macros.at(i), qis.ui.pop.upd);
							}
						}
						else
						{
							if (qis.macros.at(i).active)
							{
								qis.macros.at(i).active = false;

								QiThread::ExitMacroRun(&qis.macros.at(i));
								QiThread::ExitMacroEnd(&qis.macros.at(i));
								QiThread::StartMacroRun(&qis.macros.at(i));
								if (qis.set.showTips) MacroPop(&qis.macros.at(i), qis.ui.pop.upe);
							}
						}
					}
					break;
					}
				}
			}
		}
	}

	void QiHook(bool state)
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
	void QiState(bool state)
	{
		if (state)
		{
			// Update block keys
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
			qis.state = true;
			QPop(qis.ui.pop.qe);
			if (qis.set.audFx) Media::WavePlay(audfx_on);
			if (qis.fun.wndActive.state) { if (!QiThread::WindowStateActive()) QiThread::StartWindowState(); }
			else qis.run = true;
		}
		else
		{
			qis.state = false, qis.run = false;
			QPop(qis.ui.pop.qd);
			if (qis.fun.wndActive.state) { if (QiThread::WindowStateActive()) QiThread::ExitWindowState(); }
			if (qis.set.audFx)Media::WavePlay(audfx_off);
		}
	}
}