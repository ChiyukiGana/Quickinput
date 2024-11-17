#include "interpreter.h"

namespace QiInterpreter
{
	int ActionInterpreter(const Actions& actions, POINT& cursor, WndInput* wi, int& jumpId, bool parent)
	{
		for (size_t i = 0; i < actions.size();)
		{
			if (!Qi::run || QiThread::PeekExitMsg()) return r_exit;
			int r = std::visit([&cursor, &wi, &jumpId](auto&& var)
				{
					using T = std::decay_t<decltype(var)>;
					if (jumpId)
					{
						if constexpr (std::is_same_v<T, QiJumpPoint>)
						{
							const QiJumpPoint& jumpPoint = var;
							if (jumpPoint.id == jumpId) jumpId = 0;
						}
						else
						{
							const Actions* next = nullptr;
							const Actions* next2 = nullptr;

							if constexpr (std::is_same_v<T, QiColor>)
							{
								const QiColor& color = var;
								if (color.next.size()) next = &color.next;
								if (color.next2.size()) next2 = &color.next2;
							}
							else if constexpr (std::is_same_v<T, QiLoop>)
							{
								const QiLoop& loop = var;
								if (loop.next.size()) next = &loop.next;
							}
							else if constexpr (std::is_same_v<T, QiKeyState>)
							{
								const QiKeyState& keyState = var;
								if (keyState.next.size()) next = &keyState.next;
								if (keyState.next2.size()) next2 = &keyState.next2;
							}
							else if constexpr (std::is_same_v<T, QiImage>)
							{
								const QiImage& image = var;
								if (image.next.size()) next = &image.next;
								if (image.next2.size()) next2 = &image.next2;
							}
							else if constexpr (std::is_same_v<T, QiTimer>)
							{
								const QiTimer& timer = var;
								if (timer.next.size()) next = &timer.next;
							}
							else if constexpr (std::is_same_v<T, QiDialog>)
							{
								const QiDialog& dialog = var;
								if (dialog.next.size()) next = &dialog.next;
								if (dialog.next2.size()) next2 = &dialog.next2;
							}

							if (next) ActionInterpreter(*next, cursor, wi, jumpId, false);
							if (next2 && jumpId) ActionInterpreter(*next2, cursor, wi, jumpId, false);
						}
					}
					else
					{
						if constexpr (std::is_same_v<T, QiEnd>)
						{
							const QiEnd& end = var;
							return r_exit;
						}
						else if constexpr (std::is_same_v<T, QiDelay>)
						{
							const QiDelay& delay = var;
							Thread::Sleep(Rand(delay.max, delay.min));
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
								else wi->pt = QiFn::WATR({ pt.x, pt.y }, wi->wnd);
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
								int x = mouse.x + (Rand(mouse.ex, (~mouse.ex + 1)));
								int y = mouse.y + (Rand(mouse.ex, (~mouse.ex + 1)));
								if (mouse.move)
								{
									if (mouse.track)
									{
										QiFn::SmoothMove(0, 0, x, y, mouse.speed, [](int x, int y, int stepx, int stepy)->void {
											Input::Move(stepx, stepy);
											Thread::Sleep(5);
											});
									}
									else Input::Move(x, y);
								}
								else
								{
									if (mouse.track)
									{
										POINT spt = {};
										GetCursorPos(&spt);
										spt = QiFn::RTA(spt);
										QiFn::SmoothMove(spt.x, spt.y, x, y, mouse.speed, [](int x, int y, int stepx, int stepy)->void {
											Input::MoveToA(x * 6.5535f, y * 6.5535f);
											Thread::Sleep(5);
											});
									}
									else Input::MoveToA(x * 6.5535f, y * 6.5535f);
								}
							}
						}
						else if constexpr (std::is_same_v<T, QiText>)
						{
							const QiText& text = var;
							System::ClipBoardText(text.str.c_str());
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
								rect = QiFn::WATRR(color.rect, wi->wnd);
								Image::toRgbMap(hdc, rgbMap, rect);
								ReleaseDC(wi->wnd, hdc);
							}
							else
							{
								hdc = GetDC(nullptr);
								rect = QiFn::ATRR(color.rect);
								Image::toRgbMap(hdc, rgbMap, rect);
								ReleaseDC(nullptr, hdc);
							}
							Color::FindResult result = Color::FindOr(rgbMap, color.rgbe.toRgb(), color.rgbe.a);
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
								return ActionInterpreter(color.next, cursor, wi, jumpId, false);
							}
							else
							{
								return ActionInterpreter(color.next2, cursor, wi, jumpId, false);
							}
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
								if (uloop) { n++; if (n > e) return r_continue; }
								int r = ActionInterpreter(loop.next, cursor, wi, jumpId, false);
								if (r != r_continue)
								{
									if (r == r_break) return r_continue;
									else return r;
								}
							}
						}
						else if constexpr (std::is_same_v<T, QiLoopEnd>)
						{
							const QiLoopEnd& loopEnd = var;
							return r_break;
						}
						else if constexpr (std::is_same_v<T, QiKeyState>)
						{
							const QiKeyState& keyState = var;
							if (Input::stateEx(keyState.vk))
							{
								return ActionInterpreter(keyState.next, cursor, wi, jumpId, false);
							}
							else
							{
								return ActionInterpreter(keyState.next2, cursor, wi, jumpId, false);
							}
						}
						else if constexpr (std::is_same_v<T, QiRecoverPos>)
						{
							const QiRecoverPos& recoverPos = var;
							Input::MoveTo(cursor.x, cursor.y);
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
								rect = QiFn::WATRR(image.rect, wi->wnd);
								Image::toRgbMap(hdc, rgbMap, rect);
								ReleaseDC(wi->wnd, hdc);
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
									if (wi)
									{
										wi->pt = pt;
										Input::MoveTo(wi->wnd, wi->pt.x, wi->pt.y, wi->mk);
									}
									else Input::MoveTo(pt.x, pt.y);
								}
								return ActionInterpreter(image.next, cursor, wi, jumpId, false);
							}
							else
							{
								return ActionInterpreter(image.next2, cursor, wi, jumpId, false);
							}

						}
						else if constexpr (std::is_same_v<T, QiPopText>)
						{
							const QiPopText& popText = var;
							Qi::popText->Popup(popText.time, WToQString(popText.str), RGB(223, 223, 223));
							if (popText.sync) Thread::Sleep(popText.time);
						}
						else if constexpr (std::is_same_v<T, QiRememberPos>)
						{
							const QiRememberPos& rememberPos = var;
							cursor = Input::pos();
						}
						else if constexpr (std::is_same_v<T, QiTimer>)
						{
							const QiTimer& timer = var;
							clock_t time = Rand(timer.max, timer.min);
							clock_t begin = clock();
							while (Qi::run && !QiThread::PeekExitMsg())
							{
								if (!((begin + time) > clock())) return r_continue;
								int r = ActionInterpreter(timer.next, cursor, wi, jumpId, false);
								if (r != r_continue)
								{
									if (r == r_break) return r_continue;
									else return r;
								}
							}
						}
						else if constexpr (std::is_same_v<T, QiJump>)
						{
							const QiJump& jump = var;
							if (jump.id > 0) return r_jump(jump.id);
						}
						else if constexpr (std::is_same_v<T, QiDialog>)
						{
							const QiDialog& dialog = var;
							int bn = IDYES;
							if (dialog.style == QiDialog::warning) bn = MessageBoxW(GetForegroundWindow(), dialog.text.c_str(), dialog.title.c_str(), MB_YESNO | MB_TOPMOST | MB_ICONWARNING);
							else if (dialog.style == QiDialog::error) bn = MessageBoxW(GetForegroundWindow(), dialog.text.c_str(), dialog.title.c_str(), MB_YESNO | MB_TOPMOST | MB_ICONERROR);
							else bn = MessageBoxW(GetForegroundWindow(), dialog.text.c_str(), dialog.title.c_str(), MB_YESNO | MB_TOPMOST);
							if (bn == IDYES)
							{
								return ActionInterpreter(dialog.next, cursor, wi, jumpId, false);
							}
							else
							{
								return ActionInterpreter(dialog.next2, cursor, wi, jumpId, false);
							}
						}
					}
					return r_continue;
			}, actions[i]);

			if (r != r_continue)
			{
				if (is_jump(r))
				{
					if (parent)
					{
						jumpId = jump_id(r);
						i = 0;
					}
					else return r;
				}
				else return r;
			}
			else i++;
		}
		return r_continue;
	}
}