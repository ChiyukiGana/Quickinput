#pragma once
#include "interpreter.h"

bool QiInterpreter::PeekSleep(clock_t ms)
{
	if (ms > 0) return QiThread::PeekSleep(ms / speed);
	return QiThread::PeekSleep(0);
}

int QiInterpreter::ActionInterpreter(const Actions& current, int& jumpId)
{
go_top:
	int result = r_continue;
	for (const Action& action : current)
	{
		if (!Qi::run || QiThread::PeekExitMsg()) return r_exit;
		if (wndInput && !IsWindow(wndInput->wnd)) { 2000, Qi::popText->Popup("´°¿ÚÊ§Ð§"); return r_exit; }
		result = r_continue;
		if (jumpId)
		{
			if (action.index() == QiType::jumpPoint)
			{
				const QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(action);
				if (jumpPoint.id == jumpId) jumpId = 0;
			}
			else
			{
				const QiBase& base = action.base();
				if (!base.next.empty()) ActionInterpreter(base.next, jumpId);
				if (!base.next2.empty() && jumpId) ActionInterpreter(base.next2, jumpId);
			}
		}
		else if (!action.base().disable)
		{
			switch (action.index())
			{
			case QiType::end:
			{
				result = r_exit;
			} break;
			case QiType::delay:
			{
				const QiDelay& ref = std::get<QiDelay>(action);
				if (PeekSleep(Rand(ref.max, ref.min))) result = r_exit;
			} break;
			case QiType::key:
			{
				const QiKey& ref = std::get<QiKey>(action);
				if (wndInput)
				{
					if (ref.state == QiKey::up)
					{
						Input::State(wndInput->current, ref.vk, wndInput->pt, false);
						if (ref.vk == VK_LBUTTON) wndInput->mk &= ~MK_LBUTTON;
						else if (ref.vk == VK_RBUTTON) wndInput->mk &= ~MK_RBUTTON;
						else if (ref.vk == VK_MBUTTON) wndInput->mk &= ~MK_MBUTTON;
						else if (ref.vk == VK_XBUTTON1) wndInput->mk &= ~MK_XBUTTON1;
						else if (ref.vk == VK_XBUTTON2) wndInput->mk &= ~MK_XBUTTON2;
						else if (ref.vk == VK_CONTROL) wndInput->mk &= ~MK_CONTROL;
						else if (ref.vk == VK_SHIFT) wndInput->mk &= ~MK_SHIFT;
					}
					else if (ref.state == QiKey::down)
					{
						Input::State(wndInput->current, ref.vk, wndInput->pt, true);
						if (ref.vk == VK_LBUTTON) wndInput->mk |= MK_LBUTTON;
						else if (ref.vk == VK_RBUTTON) wndInput->mk |= MK_RBUTTON;
						else if (ref.vk == VK_MBUTTON) wndInput->mk |= MK_MBUTTON;
						else if (ref.vk == VK_XBUTTON1) wndInput->mk |= MK_XBUTTON1;
						else if (ref.vk == VK_XBUTTON2) wndInput->mk |= MK_XBUTTON2;
						else if (ref.vk == VK_CONTROL) wndInput->mk |= MK_CONTROL;
						else if (ref.vk == VK_SHIFT) wndInput->mk |= MK_SHIFT;
					}
					else if (ref.state == QiKey::click)
					{
						Input::Click(wndInput->current, ref.vk, wndInput->pt, 10);
						if (ref.vk == VK_LBUTTON) wndInput->mk |= MK_LBUTTON;
						else if (ref.vk == VK_RBUTTON) wndInput->mk |= MK_RBUTTON;
						else if (ref.vk == VK_MBUTTON) wndInput->mk |= MK_MBUTTON;
						else if (ref.vk == VK_XBUTTON1) wndInput->mk |= MK_XBUTTON1;
						else if (ref.vk == VK_XBUTTON2) wndInput->mk |= MK_XBUTTON2;
						else if (ref.vk == VK_CONTROL) wndInput->mk |= MK_CONTROL;
						else if (ref.vk == VK_SHIFT) wndInput->mk |= MK_SHIFT;
					}
				}
				else
				{
					if (ref.state == QiKey::up) Input::State(ref.vk, false, key_info);
					else if (ref.state == QiKey::down) Input::State(ref.vk, true, key_info);
					else if (ref.state == QiKey::click) Input::Click(ref.vk, 10, key_info);
				}
			} break;
			case QiType::mouse:
			{
				const QiMouse& ref = std::get<QiMouse>(action);
				if (wndInput)
				{
					POINT pt = {};
					pt.x = ref.x + (Rand(ref.ex, (~ref.ex + 1)));
					pt.y = ref.y + (Rand(ref.ex, (~ref.ex + 1)));

					if (ref.move) pt.x += wndInput->pt.x, pt.y += wndInput->pt.y;
					else pt = QiFn::WATR({ pt.x, pt.y }, wndInput->wnd);

					wndInput->current = wndInput->find(pt, pt, wndInput->child);

					if (wndInput->current)
					{
						if (ref.track)
						{
							QiFn::SmoothMove(wndInput->pt.x, wndInput->pt.y, pt.x, pt.y, ref.speed, [this](int x, int y, int stepx, int stepy) {
								Input::MoveTo(wndInput->current, stepx, stepy, wndInput->mk);
								Sleep(5);
								});
						}
						else Input::MoveTo(wndInput->current, pt.x, pt.y, wndInput->mk);
						wndInput->pt = pt;
					}
				}
				else
				{
					int x = ref.x + (Rand(ref.ex, (~ref.ex + 1)));
					int y = ref.y + (Rand(ref.ex, (~ref.ex + 1)));
					if (ref.move)
					{
						if (ref.track)
						{
							QiFn::SmoothMove(0, 0, x, y, ref.speed, [](int x, int y, int stepx, int stepy) {
								Input::Move(stepx, stepy, key_info);
								Sleep(5);
								});
						}
						else Input::Move(x, y, key_info);
					}
					else
					{
						if (ref.track)
						{
							POINT spt = {};
							GetCursorPos(&spt);
							spt = QiFn::RTA(spt);
							QiFn::SmoothMove(spt.x, spt.y, x, y, ref.speed, [](int x, int y, int stepx, int stepy)->void {
								Input::MoveToA(x * 6.5535f, y * 6.5535f, key_info);
								Sleep(5);
								});
						}
						else Input::MoveToA(x * 6.5535f, y * 6.5535f, key_info);
					}
				}
			} break;
			case QiType::copyText:
			{
				const QiCopyText& ref = std::get<QiCopyText>(action);
				System::ClipBoardText((LPCWSTR)(ref.text.utf16()));
			} break;
			case QiType::color:
			{
				const QiColor& ref = std::get<QiColor>(action);
				RgbMap rgbMap;
				RECT rect;
				HDC hdc;
				if (wndInput)
				{
					hdc = GetDC(wndInput->wnd);
					rect = QiFn::WATRR(ref.rect, wndInput->wnd);
					Image::toRgbMap(hdc, rgbMap, rect);
					ReleaseDC(wndInput->wnd, hdc);
				}
				else
				{
					hdc = GetDC(nullptr);
					rect = QiFn::ATRR(ref.rect);
					Image::toRgbMap(hdc, rgbMap, rect);
					ReleaseDC(nullptr, hdc);
				}
				Color::FindResult findResult = Color::FindOr(rgbMap, ref.rgbe.toRgb(), ref.rgbe.a);
				if (findResult.find)
				{
					if (ref.move)
					{
						findResult.pt.x += rect.left, findResult.pt.y += rect.top;
						if (wndInput)
						{
							wndInput->pt = findResult.pt;
							Input::MoveTo(wndInput->wnd, wndInput->pt.x, wndInput->pt.y, wndInput->mk);
						}
						else Input::MoveTo(findResult.pt.x, findResult.pt.y, key_info);
					}
					result = ActionInterpreter(ref.next, jumpId);
				}
				else
				{
					result = ActionInterpreter(ref.next2, jumpId);
				}
			} break;
			case QiType::loop:
			{
				const QiLoop& ref = std::get<QiLoop>(action);
				int n = 0;
				int e = 0;
				bool uloop = false;
				if (ref.max) uloop = true, e = Rand(ref.max, ref.min);
				while (Qi::run && !QiThread::PeekExitMsg())
				{
					if (uloop) { n++; if (n > e) break; }
					int r = ActionInterpreter(ref.next, jumpId);
					if (r != r_continue)
					{
						if (r == r_break) break;
						else result = r;
						break;
					}
				}
			} break;
			case QiType::loopEnd:
			{
				result = r_break;
			} break;
			case QiType::keyState:
			{
				const QiKeyState& ref = std::get<QiKeyState>(action);
				if (Input::stateEx(ref.vk))
				{
					result = ActionInterpreter(ref.next, jumpId);
				}
				else
				{
					result = ActionInterpreter(ref.next2, jumpId);
				}
			} break;
			case QiType::resetPos:
			{
				Input::MoveTo(cursor.x, cursor.y, key_info);
			} break;
			case QiType::image:
			{
				const QiImage& ref = std::get<QiImage>(action);
				RgbMap rgbMap;
				RECT rect;
				HDC hdc;
				if (wndInput)
				{
					hdc = GetDC(wndInput->wnd);
					rect = QiFn::WATRR(ref.rect, wndInput->wnd);
					Image::toRgbMap(hdc, rgbMap, rect);
					ReleaseDC(wndInput->wnd, hdc);
				}
				else
				{
					hdc = GetDC(nullptr);
					rect = QiFn::ATRR(ref.rect);
					Image::toRgbMap(hdc, rgbMap, rect);
					ReleaseDC(nullptr, hdc);
				}
				POINT pt = Image::Find(rgbMap, ref.map, ref.sim, 10);

				if (pt.x != Image::npos)
				{
					if (ref.move)
					{
						pt.x += rect.left + (ref.map.width() >> 1), pt.y += rect.top + (ref.map.height() >> 1);
						if (wndInput)
						{
							wndInput->pt = pt;
							Input::MoveTo(wndInput->wnd, wndInput->pt.x, wndInput->pt.y, wndInput->mk);
						}
						else Input::MoveTo(pt.x, pt.y, key_info);
					}
					result = ActionInterpreter(ref.next, jumpId);
				}
				else
				{
					result = ActionInterpreter(ref.next2, jumpId);
				}
			} break;
			case QiType::popText:
			{
				const QiPopText& ref = std::get<QiPopText>(action);
				Qi::popText->Popup(ref.time, ref.text, RGB(223, 223, 223));
				if (ref.sync)
				{
					if (ref.sync && PeekSleep(ref.time)) result = r_exit;
				}
				else Sleep(10);
			} break;
			case QiType::savePos:
			{
				cursor = Input::pos();
			} break;
			case QiType::timer:
			{
				const QiTimer& ref = std::get<QiTimer>(action);
				clock_t time = Rand(ref.max, ref.min);
				clock_t begin = clock();
				while (Qi::run && !QiThread::PeekExitMsg())
				{
					if (!((begin + time) > clock())) { break; }
					int r = ActionInterpreter(ref.next, jumpId);
					if (r != r_continue)
					{
						if (r == r_break) break;
						else result = r;
						break;
					}
				}
			} break;
			case QiType::jump:
			{
				const QiJump& ref = std::get<QiJump>(action);
				if (ref.id > 0)
				{
					jumpId = ref.id;
					result = r_top;
				}
			} break;
			case QiType::jumpPoint:
			{
			} break;
			case QiType::dialog:
			{
				const QiDialog& ref = std::get<QiDialog>(action);
				int bn = IDYES;
				if (ref.style == QiDialog::warning) bn = MessageBoxW(GetForegroundWindow(), (LPCWSTR)(ref.text.utf16()), (LPCWSTR)(ref.title.utf16()), MB_YESNO | MB_TOPMOST | MB_ICONWARNING);
				else if (ref.style == QiDialog::error) bn = MessageBoxW(GetForegroundWindow(), (LPCWSTR)(ref.text.utf16()), (LPCWSTR)(ref.title.utf16()), MB_YESNO | MB_TOPMOST | MB_ICONERROR);
				else bn = MessageBoxW(GetForegroundWindow(), (LPCWSTR)(ref.text.utf16()), (LPCWSTR)(ref.title.utf16()), MB_YESNO | MB_TOPMOST);
				if (bn == IDYES)
				{
					result = ActionInterpreter(ref.next, jumpId);
				}
				else
				{
					result = ActionInterpreter(ref.next2, jumpId);
				}
			} break;
			case QiType::block:
			{
			} break;
			case QiType::blockExec:
			{
				const QiBlockExec& ref = std::get<QiBlockExec>(action);
				const QiBlock* pBlock = QiFn::FindBlock(actions, ref.id);
				if (pBlock) result = ActionInterpreter(pBlock->next, jumpId);
			} break;
			case QiType::quickInput:
			{
				const QiQuickInput& ref = std::get<QiQuickInput>(action);
				for (auto& i : ref.chars)
				{
					if (wndInput)
					{
						Input::Click(wndInput->current, i, wndInput->pt, 10);
						if (i == VK_LBUTTON) wndInput->mk |= MK_LBUTTON;
						else if (i == VK_RBUTTON) wndInput->mk |= MK_RBUTTON;
						else if (i == VK_MBUTTON) wndInput->mk |= MK_MBUTTON;
						else if (i == VK_XBUTTON1) wndInput->mk |= MK_XBUTTON1;
						else if (i == VK_XBUTTON2) wndInput->mk |= MK_XBUTTON2;
						else if (i == VK_CONTROL) wndInput->mk |= MK_CONTROL;
						else if (i == VK_SHIFT) wndInput->mk |= MK_SHIFT;
					}
					else
					{
						Input::Click(i, 10, key_info);
					}
				}
			} break;
			case QiType::keyBlock:
			{
				const QiKeyBlock& ref = std::get<QiKeyBlock>(action);
				Qi::keyBlock[ref.vk] = ref.block;
			} break;
			case QiType::clock:
			{
				const QiClock& ref = std::get<QiClock>(action);
				if (QiTime::compare(ref.time) < 0) result = ActionInterpreter(ref.next, jumpId);
				else result = ActionInterpreter(ref.next2, jumpId);
			} break;
			}
		}
		if (result != r_continue) break;
	}
	if ((&actions == &current) && result == r_top) goto go_top;
	return result;
}