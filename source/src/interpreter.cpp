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
					const QiDelay& delay = std::get<QiDelay>(action);
					if (QiThread::PeekSleep(Rand(delay.max, delay.min))) result = r_exit;
				} break;
				case QiType::key:
				{
					const QiKey& key = std::get<QiKey>(action);
					if (wp)
					{
						if (key.state == QiKey::up)
						{
							Input::State(wp->current, key.vk, wp->pt, false);
							if (key.vk == VK_LBUTTON) wp->mk &= ~MK_LBUTTON;
							else if (key.vk == VK_RBUTTON) wp->mk &= ~MK_RBUTTON;
							else if (key.vk == VK_MBUTTON) wp->mk &= ~MK_MBUTTON;
							else if (key.vk == VK_XBUTTON1) wp->mk &= ~MK_XBUTTON1;
							else if (key.vk == VK_XBUTTON2) wp->mk &= ~MK_XBUTTON2;
							else if (key.vk == VK_CONTROL) wp->mk &= ~MK_CONTROL;
							else if (key.vk == VK_SHIFT) wp->mk &= ~MK_SHIFT;
						}
						else if (key.state == QiKey::down)
						{
							Input::State(wp->current, key.vk, wp->pt, true);
							if (key.vk == VK_LBUTTON) wp->mk |= MK_LBUTTON;
							else if (key.vk == VK_RBUTTON) wp->mk |= MK_RBUTTON;
							else if (key.vk == VK_MBUTTON) wp->mk |= MK_MBUTTON;
							else if (key.vk == VK_XBUTTON1) wp->mk |= MK_XBUTTON1;
							else if (key.vk == VK_XBUTTON2) wp->mk |= MK_XBUTTON2;
							else if (key.vk == VK_CONTROL) wp->mk |= MK_CONTROL;
							else if (key.vk == VK_SHIFT) wp->mk |= MK_SHIFT;
						}
						else if (key.state == QiKey::click)
						{
							Input::Click(wp->current, key.vk, wp->pt, 10);
							if (key.vk == VK_LBUTTON) wp->mk |= MK_LBUTTON;
							else if (key.vk == VK_RBUTTON) wp->mk |= MK_RBUTTON;
							else if (key.vk == VK_MBUTTON) wp->mk |= MK_MBUTTON;
							else if (key.vk == VK_XBUTTON1) wp->mk |= MK_XBUTTON1;
							else if (key.vk == VK_XBUTTON2) wp->mk |= MK_XBUTTON2;
							else if (key.vk == VK_CONTROL) wp->mk |= MK_CONTROL;
							else if (key.vk == VK_SHIFT) wp->mk |= MK_SHIFT;
						}
					}
					else
					{
						if (key.state == QiKey::up) Input::State(key.vk, false, key_info);
						else if (key.state == QiKey::down) Input::State(key.vk, true, key_info);
						else if (key.state == QiKey::click) Input::Click(key.vk, 10, key_info);
					}
				} break;
				case QiType::mouse:
				{
					const QiMouse& mouse = std::get<QiMouse>(action);
					if (wp)
					{
						POINT pt = {};
						pt.x = mouse.x + (Rand(mouse.ex, (~mouse.ex + 1)));
						pt.y = mouse.y + (Rand(mouse.ex, (~mouse.ex + 1)));

						if (mouse.move) pt.x += wp->pt.x, pt.y += wp->pt.y;
						else pt = QiFn::WATR({ pt.x, pt.y }, wp->wnd);

						wp->current = wp->find(pt, pt, wp->child);

						if (wp->current)
						{
							if (mouse.track)
							{
								QiFn::SmoothMove(wp->pt.x, wp->pt.y, pt.x, pt.y, mouse.speed, [&wp](int x, int y, int stepx, int stepy) {
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
						int x = mouse.x + (Rand(mouse.ex, (~mouse.ex + 1)));
						int y = mouse.y + (Rand(mouse.ex, (~mouse.ex + 1)));
						if (mouse.move)
						{
							if (mouse.track)
							{
								QiFn::SmoothMove(0, 0, x, y, mouse.speed, [](int x, int y, int stepx, int stepy) {
									Input::Move(stepx, stepy, key_info);
									Sleep(5);
									});
							}
							else Input::Move(x, y, key_info);
						}
						else
						{
							if (mouse.track)
							{
								POINT spt = {};
								GetCursorPos(&spt);
								spt = QiFn::RTA(spt);
								QiFn::SmoothMove(spt.x, spt.y, x, y, mouse.speed, [](int x, int y, int stepx, int stepy)->void {
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
					const QiCopyText& text = std::get<QiCopyText>(action);
					System::ClipBoardText((LPCWSTR)(text.text.utf16()));
				} break;
				case QiType::color:
				{
					const QiColor& color = std::get<QiColor>(action);
					RgbMap rgbMap;
					RECT rect;
					HDC hdc;
					if (wp)
					{
						hdc = GetDC(wp->wnd);
						rect = QiFn::WATRR(color.rect, wp->wnd);
						Image::toRgbMap(hdc, rgbMap, rect);
						ReleaseDC(wp->wnd, hdc);
					}
					else
					{
						hdc = GetDC(nullptr);
						rect = QiFn::ATRR(color.rect);
						Image::toRgbMap(hdc, rgbMap, rect);
						ReleaseDC(nullptr, hdc);
					}
					Color::FindResult findResult = Color::FindOr(rgbMap, color.rgbe.toRgb(), color.rgbe.a);
					if (findResult.find)
					{
						if (color.move)
						{
							findResult.pt.x += rect.left, findResult.pt.y += rect.top;
							if (wp)
							{
								wp->pt = findResult.pt;
								Input::MoveTo(wp->wnd, wp->pt.x, wp->pt.y, wp->mk);
							}
							else Input::MoveTo(findResult.pt.x, findResult.pt.y);
						}
						result = ActionInterpreter(parent, color.next, cursor, wp, jumpId);
					}
					else
					{
						result = ActionInterpreter(parent, color.next2, cursor, wp, jumpId);
					}
				} break;
				case QiType::loop:
				{
					const QiLoop& loop = std::get<QiLoop>(action);
					int n = 0;
					int e = 0;
					bool uloop = false;
					if (loop.max) uloop = true, e = Rand(loop.max, loop.min);
					while (Qi::run && !QiThread::PeekExitMsg())
					{
						if (uloop) { n++; if (n > e) break; }
						int r = ActionInterpreter(parent, loop.next, cursor, wp, jumpId);
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
					const QiKeyState& keyState = std::get<QiKeyState>(action);
					if (Input::stateEx(keyState.vk))
					{
						result = ActionInterpreter(parent, keyState.next, cursor, wp, jumpId);
					}
					else
					{
						result = ActionInterpreter(parent, keyState.next2, cursor, wp, jumpId);
					}
				} break;
				case QiType::recoverPos:
				{
					Input::MoveTo(cursor.x, cursor.y);
				} break;
				case QiType::image:
				{
					const QiImage& image = std::get<QiImage>(action);
					RgbMap rgbMap;
					RECT rect;
					HDC hdc;
					if (wp)
					{
						hdc = GetDC(wp->wnd);
						rect = QiFn::WATRR(image.rect, wp->wnd);
						Image::toRgbMap(hdc, rgbMap, rect);
						ReleaseDC(wp->wnd, hdc);
					}
					else
					{
						hdc = GetDC(nullptr);
						rect = QiFn::ATRR(image.rect);
						Image::toRgbMap(hdc, rgbMap, rect);
						ReleaseDC(nullptr, hdc);
					}
					POINT pt = Image::Find(rgbMap, image.map, image.sim, 10);

					if (pt.x != Image::npos)
					{
						if (image.move)
						{
							pt.x += rect.left + (image.map.width() >> 1), pt.y += rect.top + (image.map.height() >> 1);
							if (wp)
							{
								wp->pt = pt;
								Input::MoveTo(wp->wnd, wp->pt.x, wp->pt.y, wp->mk);
							}
							else Input::MoveTo(pt.x, pt.y);
						}
						result = ActionInterpreter(parent, image.next, cursor, wp, jumpId);
					}
					else
					{
						result = ActionInterpreter(parent, image.next2, cursor, wp, jumpId);
					}
				} break;
				case QiType::popText:
				{
					const QiPopText& popText = std::get<QiPopText>(action);
					Qi::popText->Popup(popText.time, popText.text, RGB(223, 223, 223));
					if (popText.sync) Sleep(popText.time);
				} break;
				case QiType::rememberPos:
				{
					cursor = Input::pos();
				} break;
				case QiType::timer:
				{
					const QiTimer& timer = std::get<QiTimer>(action);
					clock_t time = Rand(timer.max, timer.min);
					clock_t begin = clock();
					while (Qi::run && !QiThread::PeekExitMsg())
					{
						if (!((begin + time) > clock())) { break; }
						int r = ActionInterpreter(parent, timer.next, cursor, wp, jumpId);
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
					const QiJump& jump = std::get<QiJump>(action);
					if (jump.id > 0)
					{
						jumpId = jump.id;
						result = r_top;
					}
				} break;
				case QiType::jumpPoint:
				{
				} break;
				case QiType::dialog:
				{
					const QiDialog& dialog = std::get<QiDialog>(action);
					int bn = IDYES;
					if (dialog.style == QiDialog::warning) bn = MessageBoxW(GetForegroundWindow(), (LPCWSTR)(dialog.text.utf16()), (LPCWSTR)(dialog.title.utf16()), MB_YESNO | MB_TOPMOST | MB_ICONWARNING);
					else if (dialog.style == QiDialog::error) bn = MessageBoxW(GetForegroundWindow(), (LPCWSTR)(dialog.text.utf16()), (LPCWSTR)(dialog.title.utf16()), MB_YESNO | MB_TOPMOST | MB_ICONERROR);
					else bn = MessageBoxW(GetForegroundWindow(), (LPCWSTR)(dialog.text.utf16()), (LPCWSTR)(dialog.title.utf16()), MB_YESNO | MB_TOPMOST);
					if (bn == IDYES)
					{
						result = ActionInterpreter(parent, dialog.next, cursor, wp, jumpId);
					}
					else
					{
						result = ActionInterpreter(parent, dialog.next2, cursor, wp, jumpId);
					}
				} break;
				case QiType::block:
				{
				} break;
				case QiType::blockExec:
				{
					const QiBlockExec& blockExec = std::get<QiBlockExec>(action);
					const QiBlock* pBlock = QiFn::FindBlock(parent, blockExec.id);
					if (pBlock) result = ActionInterpreter(parent, pBlock->next, cursor, wp, jumpId);
				} break;
				}
			}
			if (result != r_continue) break;
		}
		if ((&parent == &current) && result == r_top) goto go_top;
		return result;
	}
}