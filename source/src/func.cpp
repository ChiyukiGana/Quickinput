#pragma execution_character_set("utf-8")
#include "func.h"

namespace QiFn
{
	POINT RTA(POINT rel) { return { (long)(((float)rel.x / (float)(Qi::screen.cx - 1)) * 10000.0f), (long)(((float)rel.y / (float)(Qi::screen.cy - 1)) * 10000.0f) }; }
	RECT RTAR(RECT rel) { return { (long)(((float)rel.left / (float)(Qi::screen.cx - 1)) * 10000.0f), (long)(((float)rel.top / (float)(Qi::screen.cy - 1)) * 10000.0f), (long)(((float)rel.right / (float)(Qi::screen.cx - 1)) * 10000.0f), (long)(((float)rel.bottom / (float)(Qi::screen.cy - 1)) * 10000.0f) }; }
	POINT ATR(POINT abs) { return { (long)((float)Qi::screen.cx / 10000.0f * (float)abs.x), (long)((float)Qi::screen.cy / 10000.0f * (float)abs.y) }; }
	RECT ATRR(RECT abs) { return { (long)((float)Qi::screen.cx / 10000.0f * (float)abs.left), (long)((float)Qi::screen.cy / 10000.0f * (float)abs.top), (long)((float)Qi::screen.cx / 10000.0f * (float)abs.right), (long)((float)Qi::screen.cy / 10000.0f * (float)abs.bottom) }; }
	POINT WRTA(POINT rel, HWND wnd) { SIZE size = Window::size(wnd); return { (long)(((float)rel.x / ((float)size.cx)) * 10000.0f), (long)(((float)rel.y / ((float)size.cy)) * 10000.0f) }; }
	RECT WRTAR(RECT rel, HWND wnd) { SIZE size = Window::size(wnd); return { (long)(((float)rel.left / ((float)size.cx)) * 10000.0f), (long)(((float)rel.top / ((float)size.cy)) * 10000.0f), (long)(((float)rel.right / ((float)size.cx)) * 10000.0f), (long)(((float)rel.bottom / ((float)size.cy)) * 10000.0f) }; }
	POINT WATR(POINT abs, HWND wnd) { SIZE size = Window::size(wnd); return { (long)((float)(size.cx + 1) / 10000.0f * (float)abs.x), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.y) }; }
	RECT WATRR(RECT abs, HWND wnd) { SIZE size = Window::size(wnd); return { (long)((float)(size.cx + 1) / 10000.0f * (float)abs.left), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.top), (long)((float)(size.cx + 1) / 10000.0f * (float)abs.right), (long)((float)(size.cy + 1) / 10000.0f * (float)abs.bottom) }; }

	QString ParseCustom(QString text, QString name, QString number) { text.replace("@", name); return text.replace("$", number); }
	QString ParseState(QString text) { return text.replace("@", QString::fromWCharArray(Qi::dir.c_str())); }
	QString ParseWindow(QString text, QString window) { return text.replace("@", window); }
	QString ParseQuickClick(QString text, short key) { return text.replace("@", QKeyEdit::keyName(key)); }
	QString ParseMacro(QString text, QString macro, int count) { if (count) text.replace("$", QString::number(count)); else text.replace("$", "∞"); return text.replace("@", macro); }
	void StatePop(bool state)
	{
		if (state) Qi::popText->Popup(ParseState(Qi::ui.pop.qe.t), Qi::ui.pop.qe.c, Qi::ui.pop.time);
		else Qi::popText->Popup(ParseState(Qi::ui.pop.qd.t), Qi::ui.pop.qd.c, Qi::ui.pop.time);
	}
	void WindowPop(std::wstring window, bool state)
	{
		if (state) Qi::popText->Popup(ParseWindow(Qi::ui.pop.we.t, QString::fromWCharArray(window.c_str())), Qi::ui.pop.we.c, Qi::ui.pop.time);
		else Qi::popText->Popup(ParseWindow(Qi::ui.pop.wd.t, QString::fromWCharArray(window.c_str())), Qi::ui.pop.wd.c, Qi::ui.pop.time);
	}
	void QuickClickPop(bool state)
	{
		if (state) Qi::popText->Popup(ParseQuickClick(Qi::ui.pop.qce.t, Qi::fun.quickClick.key), Qi::ui.pop.qce.c, Qi::ui.pop.time);
		else Qi::popText->Popup(ParseQuickClick(Qi::ui.pop.qcd.t, Qi::fun.quickClick.key), Qi::ui.pop.qcd.c, Qi::ui.pop.time);
	}
	void MacroPop(Macro* macro, bool state)
	{
		if (macro->mode == Macro::sw)
		{
			if (state) Qi::popText->Popup(ParseMacro(Qi::ui.pop.swe.t, QString::fromWCharArray(macro->name.c_str()), 0), Qi::ui.pop.swe.c, Qi::ui.pop.time);
			else Qi::popText->Popup(ParseMacro(Qi::ui.pop.swd.t, QString::fromWCharArray(macro->name.c_str()), 0), Qi::ui.pop.swd.c, Qi::ui.pop.time);
		}
		else if (macro->mode == Macro::down)
		{
			if (state) Qi::popText->Popup(ParseMacro(Qi::ui.pop.dwe.t, QString::fromWCharArray(macro->name.c_str()), macro->count), Qi::ui.pop.dwe.c, Qi::ui.pop.time);
			else Qi::popText->Popup(ParseMacro(Qi::ui.pop.dwd.t, QString::fromWCharArray(macro->name.c_str()), macro->count), Qi::ui.pop.dwd.c, Qi::ui.pop.time);
		}
		else if (macro->mode == Macro::up)
		{
			if (state) Qi::popText->Popup(ParseMacro(Qi::ui.pop.upe.t, QString::fromWCharArray(macro->name.c_str()), macro->count), Qi::ui.pop.upe.c, Qi::ui.pop.time);
			else Qi::popText->Popup(ParseMacro(Qi::ui.pop.upd.t, QString::fromWCharArray(macro->name.c_str()), macro->count), Qi::ui.pop.upd.c, Qi::ui.pop.time);
		}
	}

	bool SelfActive() { if (Qi::widget.mainActive || Qi::widget.dialogActive || Qi::widget.moreActive) return false; }
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
		QWindowSelection ws;
		wi.wnd = ws.Start();
		wi.wndClass = Window::className(wi.wnd);
		wi.wndName = Window::text(wi.wnd);
		return wi;
	}
	std::wstring AllocName(std::wstring name)
	{
		wstringList names;
		for (size_t i = 0; i < Qi::macros.size(); i++)
		{
			names.push_back(Qi::macros[i].name);
		}
		return File::Unique(names, name);
	}

	int ActionExecute(const Action& action, POINT& cursor, WndInput* wi)
	{
		if (!Qi::run || QiThread::PeekExitMsg()) return r_exit;

		return std::visit([&cursor, &wi](auto&& var)
			{
				using T = std::decay_t<decltype(var)>;
				if constexpr (std::is_same_v<T, QiEnd>)
				{
					const QiEnd& end = var;
					return r_exit;
				}
				else if constexpr (std::is_same_v<T, QiDelay>)
				{
					const QiDelay& delay = var;
					Thread::Sleep(Rand(delay.max, delay.min));
					return r_continue;
				}
				else if constexpr (std::is_same_v<T, QiKey>)
				{
					const QiKey& key = var;
					if (wi)
					{
						if (key.state == QiKey::up)
						{
							Input::State(wi->current, key.vk, wi->pt, false);
							if (key.vk == VK_LBUTTON) wi->mk &= ~MK_LBUTTON;
							else if (key.vk == VK_RBUTTON) wi->mk &= ~MK_RBUTTON;
							else if (key.vk == VK_MBUTTON) wi->mk &= ~MK_MBUTTON;
							else if (key.vk == VK_XBUTTON1) wi->mk &= ~MK_XBUTTON1;
							else if (key.vk == VK_XBUTTON2) wi->mk &= ~MK_XBUTTON2;
							else if (key.vk == VK_CONTROL) wi->mk &= ~MK_CONTROL;
							else if (key.vk == VK_SHIFT) wi->mk &= ~MK_SHIFT;
						}
						else if (key.state == QiKey::down)
						{
							Input::State(wi->current, key.vk, wi->pt, true);
							if (key.vk == VK_LBUTTON) wi->mk |= MK_LBUTTON;
							else if (key.vk == VK_RBUTTON) wi->mk |= MK_RBUTTON;
							else if (key.vk == VK_MBUTTON) wi->mk |= MK_MBUTTON;
							else if (key.vk == VK_XBUTTON1) wi->mk |= MK_XBUTTON1;
							else if (key.vk == VK_XBUTTON2) wi->mk |= MK_XBUTTON2;
							else if (key.vk == VK_CONTROL) wi->mk |= MK_CONTROL;
							else if (key.vk == VK_SHIFT) wi->mk |= MK_SHIFT;
						}
						else if (key.state == QiKey::click)
						{
							Input::Click(wi->current, key.vk, wi->pt, 10);
							if (key.vk == VK_LBUTTON) wi->mk |= MK_LBUTTON;
							else if (key.vk == VK_RBUTTON) wi->mk |= MK_RBUTTON;
							else if (key.vk == VK_MBUTTON) wi->mk |= MK_MBUTTON;
							else if (key.vk == VK_XBUTTON1) wi->mk |= MK_XBUTTON1;
							else if (key.vk == VK_XBUTTON2) wi->mk |= MK_XBUTTON2;
							else if (key.vk == VK_CONTROL) wi->mk |= MK_CONTROL;
							else if (key.vk == VK_SHIFT) wi->mk |= MK_SHIFT;
						}
					}
					else
					{
						if (key.state == QiKey::up) Input::State(key.vk, false, key_info);
						else if (key.state == QiKey::down) Input::State(key.vk, true, key_info);
						else if (key.state == QiKey::click) Input::Click(key.vk, 10, key_info);
					}
					return r_continue;
				}
				else if constexpr (std::is_same_v<T, QiMouse>)
				{
					const QiMouse& mouse = var;
					if (wi)
					{
						POINT pt = {};
						pt.x = mouse.x + (Rand(mouse.ex, (~mouse.ex + 1)));
						pt.y = mouse.y + (Rand(mouse.ex, (~mouse.ex + 1)));
						if (mouse.move) wi->pt.x += pt.x, wi->pt.y += pt.y;
						else wi->pt = WATR({ pt.x, pt.y }, wi->wnd);
						wi->current = 0;
						List<ChildWindow> cws;
						for (uint32 u = wi->children.size(); wi->children.size(); u--)
						{
							if (InRect(wi->children[u].rect, wi->pt)) cws.Add(wi->children[u]);
							if (u == 0) return r_continue;
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
						pt.x = mouse.x + (Rand(mouse.ex, (~mouse.ex + 1)));
						pt.y = mouse.y + (Rand(mouse.ex, (~mouse.ex + 1)));
						if (mouse.move)
						{
							if (mouse.track)
							{
								POINT spt = {};
								int disx = Distance(mouse.x, spt.x);
								int disy = Distance(mouse.y, spt.y);
								if (disx < disy) disx = disy;
								int s = mouse.speed;
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
									if (!r) return r_continue;
									Thread::Sleep(5);
								}
							}
							else Input::Move(pt.x, pt.y);
						}
						else
						{
							if (mouse.track)
							{
								POINT spt = {};
								GetCursorPos(&spt);
								spt = RTA(spt);
								int disx = Distance(mouse.x, spt.x);
								int disy = Distance(mouse.y, spt.y);
								if (disx < disy) disx = disy;
								int s = mouse.speed;
								if (!s) s = 1;
								disx *= ((float)s / 4.0f / 100.0f);
								if (disx < 1) disx = 1;
								for (int i = 0; true; i += disx)
								{
									POINT rpt;
									bool r = TransMove(spt.x, spt.y, pt.x, pt.y, i, rpt);
									Input::MoveToA(rpt.x * 6.5535, rpt.y * 6.5535);
									if (!r) return r_continue;
									Thread::Sleep(5);
								}
							}
							else Input::MoveToA(pt.x * 6.5535, pt.y * 6.5535);
						}
					}
					return r_continue;
				}
				else if constexpr (std::is_same_v<T, QiText>)
				{
					const QiText& text = var;
					System::ClipBoardText(text.str.c_str());
					return r_continue;
				}
				else if constexpr (std::is_same_v<T, QiColor>)
				{
					const QiColor& color = var;
					RgbMap rgbMap;
					RECT rect;
					HDC hdc;
					if (wi)
					{
						hdc = GetDC(wi->wnd);
						rect = WATRR(color.rect, wi->wnd);
						Image::toRgbMap(hdc, rgbMap, rect);
						ReleaseDC(wi->wnd, hdc);
					}
					else
					{
						hdc = GetDC(nullptr);
						rect = ATRR(color.rect);
						Image::toRgbMap(hdc, rgbMap, rect);
						ReleaseDC(nullptr, hdc);
					}
					Color::FindResult result = Color::FindOr(rgbMap, color.rgbe.toRgb(), color.rgbe.a);
					if (color.unfind)
					{
						if (result.find) return r_continue;
					}
					else
					{
						if (result.find)
						{
							if (color.move)
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
					for (uint32 i = 0; i < color.next.size(); i++)
					{
						int r = ActionExecute(color.next.at(i), cursor, wi);
						if (r != r_continue) return r;
					}
					return r_continue;
				}
				else if constexpr (std::is_same_v<T, QiLoop>)
				{
					const QiLoop& loop = var;
					uint32 n = 0;
					uint32 e = 0;
					bool uloop = false;
					if (loop.max) uloop = true, e = Rand(loop.max, loop.min);
					while (Qi::run && !QiThread::PeekExitMsg())
					{
						if (uloop)
						{
							n++;
							if (n > e) return r_continue;
						}
						for (uint32 i = 0; i < loop.next.size(); i++)
						{
							int r = ActionExecute(loop.next.at(i), cursor, wi);
							if (r != r_continue)
							{
								if (r == r_break) return r_continue;
								else if (r == r_exit) return r_exit;
							}
						}
					}
					return r_continue;
				}
				else if constexpr (std::is_same_v<T, QiLoopEnd>)
				{
					const QiLoopEnd& loopEnd = var;
					return r_break;
				}
				else if constexpr (std::is_same_v<T, QiKeyState>)
				{
					const QiKeyState& keyState = var;
					if (keyState.state)
					{
						if (!Input::stateEx(keyState.vk)) return r_continue;
					}
					else
					{
						if (Input::stateEx(keyState.vk)) return r_continue;
					}
					for (uint32 i = 0; i < keyState.next.size(); i++)
					{
						int r = ActionExecute(keyState.next.at(i), cursor, wi);
						if (r != r_continue) return r;
					}
					return r_continue;
				}
				else if constexpr (std::is_same_v<T, QiRecoverPos>)
				{
					const QiRecoverPos& recoverPos = var;
					Input::MoveTo(cursor.x, cursor.y);
					return r_continue;
				}
				else if constexpr (std::is_same_v<T, QiImage>)
				{
					const QiImage& image = var;
					RgbMap rgbMap;
					RECT rect;
					HDC hdc;
					if (wi)
					{
						hdc = GetDC(wi->wnd);
						rect = WATRR(image.rect, wi->wnd);
						Image::toRgbMap(hdc, rgbMap, rect);
						ReleaseDC(wi->wnd, hdc);
					}
					else
					{
						hdc = GetDC(nullptr);
						rect = ATRR(image.rect);
						Image::toRgbMap(hdc, rgbMap, rect);
						ReleaseDC(nullptr, hdc);
					}
					POINT pt = Image::Find(rgbMap, image.map, image.sim, 10);

					if (image.unfind)
					{
						if (pt.x != Image::npos) return r_continue;
					}
					else
					{
						if (pt.x != Image::npos)
						{
							if (image.move)
							{
								pt.x += rect.left + (image.map.width() >> 1), pt.y += rect.top + (image.map.height() >> 1);
								if (wi)
								{
									wi->pt = pt;
									Input::MoveTo(wi->wnd, wi->pt.x, wi->pt.y, wi->mk);
								}
								else Input::MoveTo(pt.x, pt.y);
							}
						}
						else return r_continue;
					}

					for (uint32 i = 0; i < image.next.size(); i++)
					{
						int r = ActionExecute(image.next.at(i), cursor, wi);
						if (r != r_continue) return r;
					}
					return r_continue;
				}
				else if constexpr (std::is_same_v<T, QiPopText>)
				{
					const QiPopText& popText = var;
					Qi::popText->Popup(WToQString(popText.str), RGB(223, 223, 223), popText.time);
					return r_continue;
				}
				else if constexpr (std::is_same_v<T, QiRememberPos>)
				{
					const QiRememberPos& rememberPos = var;
					cursor = Input::pos();
					return r_continue;
				}
				else if constexpr (std::is_same_v<T, QiTimer>)
				{
					const QiTimer& timer = var;
					clock_t time = Rand(timer.max, timer.min);
					clock_t begin = clock();
					while (Qi::run && !QiThread::PeekExitMsg())
					{
						if (!((begin + time) > clock())) return r_continue;
						for (uint32 i = 0; i < timer.next.size(); i++)
						{
							int r = ActionExecute(timer.next.at(i), cursor, wi);
							if (r != r_continue)
							{
								if (r == r_break) return r_continue;
								else if (r == r_exit) return r_exit;
							}
						}
					}
					return r_continue;
				}
				else
				{
					return r_continue;
				}
			}, action
		);
	}
	void Trigger(short vk, const bool* state)
	{
		// state swtich
		if (vk == (Qi::set.key & 0xFFFF) || vk == (Qi::set.key >> 16))
		{
			bool k1 = state[Qi::set.key & 0xFFFF];
			bool k2 = true;
			if (Qi::set.key >> 16) k2 = state[Qi::set.key >> 16];
			if (k1 && k2)
			{
				if (Qi::state) QiState(false);
				else QiState(true);
			}
		}

		// show clock
		if (Qi::fun.showClock.state && Qi::fun.showClock.key == vk && state[Qi::fun.showClock.key]) { Qi::popText->Popup(QString::fromWCharArray(Time::toWStringT().c_str())); }

		if (!Qi::run) return;

		// quick click
		if (Qi::fun.quickClick.state && Qi::fun.quickClick.key == vk)
		{
			if (state[Qi::fun.quickClick.key]) // key down
			{
				if (Qi::fun.quickClick.mode) // switch mode
				{
					if (QiThread::QuickClickActive())
					{
						QiThread::ExitQuickClick();
						QiThread::AddReleaseKey(Qi::fun.quickClick.key);
						if (Qi::set.showTips) QuickClickPop(false);
					}
					else
					{
						QiThread::StartQuickClick();
						if (Qi::set.showTips) QuickClickPop(true);
					}
				}
				else // press mode
				{
					if (!QiThread::QuickClickActive())
					{
						QiThread::StartQuickClick();
						if (Qi::set.showTips) QuickClickPop(true);
					}
				}
			}
			else // key up
			{
				if (!Qi::fun.quickClick.mode) // press mode
				{
					if (QiThread::QuickClickActive())
					{
						QiThread::ExitQuickClick();
						QiThread::AddReleaseKey(Qi::fun.quickClick.key);
						if (Qi::set.showTips) QuickClickPop(false);
					}
				}
			}
		}

		// macros
		for (uint32 i = 0; i < Qi::macros.size(); i++)
		{
			if (vk == (Qi::macros.at(i).key & 0xFFFF) || vk == (Qi::macros.at(i).key >> 16))
			{
				if (Qi::macros.at(i).state)
				{
					bool k1 = state[Qi::macros.at(i).key & 0xFFFF];
					bool k2 = true;
					if (Qi::macros.at(i).key >> 16) k2 = state[Qi::macros.at(i).key >> 16];

					switch (Qi::macros.at(i).mode)
					{
					case Macro::sw:
					{
						if (k1 && k2)
						{
							if (QiThread::MacroRunActive(&Qi::macros.at(i)))
							{
								QiThread::ExitMacroRun(&Qi::macros.at(i));
								QiThread::StartMacroEnd(&Qi::macros.at(i));
								if (Qi::set.showTips) MacroPop(&Qi::macros.at(i), false);
							}
							else
							{
								QiThread::ExitMacroEnd(&Qi::macros.at(i));
								QiThread::StartMacroRun(&Qi::macros.at(i));
								if (Qi::set.showTips) MacroPop(&Qi::macros.at(i), true);
							}
						}
					}
					break;

					case Macro::down:
					{
						if (k1 && k2)
						{
							QiThread::ExitMacroRun(&Qi::macros.at(i));
							QiThread::ExitMacroEnd(&Qi::macros.at(i));
							QiThread::StartMacroRun(&Qi::macros.at(i));
							if (Qi::set.showTips) MacroPop(&Qi::macros.at(i), true);
						}
						else
						{
							if (Qi::macros.at(i).count == 0 && QiThread::MacroRunActive(&Qi::macros.at(i)))
							{
								QiThread::ExitMacroRun(&Qi::macros.at(i));
								QiThread::StartMacroEnd(&Qi::macros.at(i));
								if (Qi::set.showTips) MacroPop(&Qi::macros.at(i), false);
							}
						}
					}
					break;

					case Macro::up:
					{
						if (k1 && k2)
						{
							Qi::macros.at(i).active = true;
							if (Qi::macros.at(i).count == 0 && QiThread::MacroRunActive(&Qi::macros.at(i)))
							{
								QiThread::ExitMacroRun(&Qi::macros.at(i));
								QiThread::StartMacroEnd(&Qi::macros.at(i));
								if (Qi::set.showTips) MacroPop(&Qi::macros.at(i), false);
							}
						}
						else
						{
							if (Qi::macros.at(i).active)
							{
								Qi::macros.at(i).active = false;

								QiThread::ExitMacroRun(&Qi::macros.at(i));
								QiThread::ExitMacroEnd(&Qi::macros.at(i));
								QiThread::StartMacroRun(&Qi::macros.at(i));
								if (Qi::set.showTips) MacroPop(&Qi::macros.at(i), true);
							}
						}
					}
					break;
					}
				}
			}
		}
	}

	void XBoxPadProc(short keyCode, short state)
	{
		Qi::keyState[keyCode] = (bool)state;
		Trigger(keyCode, Qi::keyState);
	}

	void QiHook(bool state)
	{
		if (state)
		{
			memset(Qi::keyState, 0, sizeof(Qi::keyState));
			if (!InputHook::State())
			{
				timeBeginPeriod(1); // set clock accuracy
				if (!InputHook::Start()) MsgBox::Error(L"创建输入Hook失败，检查是否管理员身份运行 或 是否被安全软件拦截。");
				Qi::xboxpad.setStateEvent(XBoxPadProc, true);
			}
		}
		else
		{
			if (InputHook::State())
			{
				timeEndPeriod(1); // reset clock accuracy
				InputHook::Close();
				Qi::xboxpad.closeStateEvent();
			}
		}
	}
	void QiState(bool state)
	{
		if (state)
		{
			// Update block keys
			{
				Qi::blockKeys.clear();
				for (uint32 u = 0; u < Qi::macros.size(); u++)
				{
					if (Qi::macros[u].state && Qi::macros[u].block)
					{
						if ((Qi::macros[u].key & 0xFFFF)) Qi::blockKeys.Add((Qi::macros[u].key & 0xFFFF));
						if (Qi::macros[u].key >> 16) Qi::blockKeys.Add(Qi::macros[u].key >> 16);
					}
				}
			}
			Qi::state = true;
			StatePop(true);
			if (Qi::set.audFx) Media::WavePlay(audfx_on);
			if (Qi::fun.wndActive.state) { if (!QiThread::WindowStateActive()) QiThread::StartWindowState(); }
			else Qi::run = true;
		}
		else
		{
			Qi::state = false, Qi::run = false;
			StatePop(false);
			if (Qi::fun.wndActive.state) { if (QiThread::WindowStateActive()) QiThread::ExitWindowState(); }
			if (Qi::set.audFx)Media::WavePlay(audfx_off);
		}
	}
}