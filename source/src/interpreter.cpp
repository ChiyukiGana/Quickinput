#include "interpreter.h"
namespace QiInterpreter
{
	int ActionInterpreter(const Actions& parent, const Actions& current, POINT& cursor, WndInput* wp, int& jumpId)
	{
	go_top:
		int result = r_continue;
		for (const Action& action : current)
		{
			if (!Qi::run || QiThread::PeekExitMsg()) return r_exit;
			if (wp && !IsWindow(wp->wnd)) { 2000, Qi::popText->Popup("´°¿ÚÊ§Ð§"); return r_exit; }
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
					if (!base.next.empty()) ActionInterpreter(parent, base.next, cursor, wp, jumpId);
					if (!base.next2.empty() && jumpId) ActionInterpreter(parent, base.next2, cursor, wp, jumpId);
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
					if (QiThread::PeekSleep(Rand(ref.max, ref.min))) result = r_exit;
				} break;
				case QiType::key:
				{
					const QiKey& ref = std::get<QiKey>(action);
					if (wp)
					{
						if (ref.state == QiKey::up)
						{
							Input::State(wp->current, ref.vk, wp->pt, false);
							if (ref.vk == VK_LBUTTON) wp->mk &= ~MK_LBUTTON;
							else if (ref.vk == VK_RBUTTON) wp->mk &= ~MK_RBUTTON;
							else if (ref.vk == VK_MBUTTON) wp->mk &= ~MK_MBUTTON;
							else if (ref.vk == VK_XBUTTON1) wp->mk &= ~MK_XBUTTON1;
							else if (ref.vk == VK_XBUTTON2) wp->mk &= ~MK_XBUTTON2;
							else if (ref.vk == VK_CONTROL) wp->mk &= ~MK_CONTROL;
							else if (ref.vk == VK_SHIFT) wp->mk &= ~MK_SHIFT;
						}
						else if (ref.state == QiKey::down)
						{
							Input::State(wp->current, ref.vk, wp->pt, true);
							if (ref.vk == VK_LBUTTON) wp->mk |= MK_LBUTTON;
							else if (ref.vk == VK_RBUTTON) wp->mk |= MK_RBUTTON;
							else if (ref.vk == VK_MBUTTON) wp->mk |= MK_MBUTTON;
							else if (ref.vk == VK_XBUTTON1) wp->mk |= MK_XBUTTON1;
							else if (ref.vk == VK_XBUTTON2) wp->mk |= MK_XBUTTON2;
							else if (ref.vk == VK_CONTROL) wp->mk |= MK_CONTROL;
							else if (ref.vk == VK_SHIFT) wp->mk |= MK_SHIFT;
						}
						else if (ref.state == QiKey::click)
						{
							Input::Click(wp->current, ref.vk, wp->pt, 10);
							if (ref.vk == VK_LBUTTON) wp->mk |= MK_LBUTTON;
							else if (ref.vk == VK_RBUTTON) wp->mk |= MK_RBUTTON;
							else if (ref.vk == VK_MBUTTON) wp->mk |= MK_MBUTTON;
							else if (ref.vk == VK_XBUTTON1) wp->mk |= MK_XBUTTON1;
							else if (ref.vk == VK_XBUTTON2) wp->mk |= MK_XBUTTON2;
							else if (ref.vk == VK_CONTROL) wp->mk |= MK_CONTROL;
							else if (ref.vk == VK_SHIFT) wp->mk |= MK_SHIFT;
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
					if (wp)
					{
						POINT pt = {};
						pt.x = ref.x + (Rand(ref.ex, (~ref.ex + 1)));
						pt.y = ref.y + (Rand(ref.ex, (~ref.ex + 1)));

						if (ref.move) pt.x += wp->pt.x, pt.y += wp->pt.y;
						else pt = QiFn::WATR({ pt.x, pt.y }, wp->wnd);

						wp->current = wp->find(pt, pt, wp->child);

						if (wp->current)
						{
							if (ref.track)
							{
								QiFn::SmoothMove(wp->pt.x, wp->pt.y, pt.x, pt.y, ref.speed, [&wp](int x, int y, int stepx, int stepy) {
									Input::MoveTo(wp->current, stepx, stepy, wp->mk);
									Sleep(5);
									});
							}
							else Input::MoveTo(wp->current, pt.x, pt.y, wp->mk);
							wp->pt = pt;
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
					if (wp)
					{
						hdc = GetDC(wp->wnd);
						rect = QiFn::WATRR(ref.rect, wp->wnd);
						Image::toRgbMap(hdc, rgbMap, rect);
						ReleaseDC(wp->wnd, hdc);
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
							if (wp)
							{
								wp->pt = findResult.pt;
								Input::MoveTo(wp->wnd, wp->pt.x, wp->pt.y, wp->mk);
							}
							else Input::MoveTo(findResult.pt.x, findResult.pt.y, key_info);
						}
						result = ActionInterpreter(parent, ref.next, cursor, wp, jumpId);
					}
					else
					{
						result = ActionInterpreter(parent, ref.next2, cursor, wp, jumpId);
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
						int r = ActionInterpreter(parent, ref.next, cursor, wp, jumpId);
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
						result = ActionInterpreter(parent, ref.next, cursor, wp, jumpId);
					}
					else
					{
						result = ActionInterpreter(parent, ref.next2, cursor, wp, jumpId);
					}
				} break;
				case QiType::recoverPos:
				{
					Input::MoveTo(cursor.x, cursor.y, key_info);
				} break;
				case QiType::image:
				{
					const QiImage& ref = std::get<QiImage>(action);
					RgbMap rgbMap;
					RECT rect;
					HDC hdc;
					if (wp)
					{
						hdc = GetDC(wp->wnd);
						rect = QiFn::WATRR(ref.rect, wp->wnd);
						Image::toRgbMap(hdc, rgbMap, rect);
						ReleaseDC(wp->wnd, hdc);
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
							if (wp)
							{
								wp->pt = pt;
								Input::MoveTo(wp->wnd, wp->pt.x, wp->pt.y, wp->mk);
							}
							else Input::MoveTo(pt.x, pt.y, key_info);
						}
						result = ActionInterpreter(parent, ref.next, cursor, wp, jumpId);
					}
					else
					{
						result = ActionInterpreter(parent, ref.next2, cursor, wp, jumpId);
					}
				} break;
				case QiType::popText:
				{
					const QiPopText& ref = std::get<QiPopText>(action);
					Qi::popText->Popup(ref.time, ref.text, RGB(223, 223, 223));
					if (ref.sync)
					{
						if (QiThread::PeekSleep(ref.time)) result = r_exit;
					}
				} break;
				case QiType::rememberPos:
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
						int r = ActionInterpreter(parent, ref.next, cursor, wp, jumpId);
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
						result = ActionInterpreter(parent, ref.next, cursor, wp, jumpId);
					}
					else
					{
						result = ActionInterpreter(parent, ref.next2, cursor, wp, jumpId);
					}
				} break;
				case QiType::block:
				{
				} break;
				case QiType::blockExec:
				{
					const QiBlockExec& ref = std::get<QiBlockExec>(action);
					const QiBlock* pBlock = QiFn::FindBlock(parent, ref.id);
					if (pBlock) result = ActionInterpreter(parent, pBlock->next, cursor, wp, jumpId);
				} break;
				case QiType::quickInput:
				{
					const QiQuickInput& ref = std::get<QiQuickInput>(action);
					for (auto &i : ref.chars)
					{
						if (wp)
						{
							Input::Click(wp->current, i, wp->pt, 10);
							if (i == VK_LBUTTON) wp->mk |= MK_LBUTTON;
							else if (i == VK_RBUTTON) wp->mk |= MK_RBUTTON;
							else if (i == VK_MBUTTON) wp->mk |= MK_MBUTTON;
							else if (i == VK_XBUTTON1) wp->mk |= MK_XBUTTON1;
							else if (i == VK_XBUTTON2) wp->mk |= MK_XBUTTON2;
							else if (i == VK_CONTROL) wp->mk |= MK_CONTROL;
							else if (i == VK_SHIFT) wp->mk |= MK_SHIFT;
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
				}
			}
			if (result != r_continue) break;
		}
		if ((&parent == &current) && result == r_top) goto go_top;
		return result;
	}
}