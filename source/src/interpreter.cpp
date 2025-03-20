#pragma once
#include "interpreter.h"

bool HaveEntry(const Actions& actions)
{
	for (const auto& i : actions)
	{
		if (i.base().debug_entry) return true;
		if (i.base().next.not_empty())
		{
			if (HaveEntry(i.base().next)) return true;
		}
		if (i.base().next2.not_empty())
		{
			if (HaveEntry(i.base().next2)) return true;
		}
	}
	return false;
}

QiInterpreter::QiInterpreter(QiVarMap& varMap, const Actions& actions, float speed, WndInput* wndInput, POINT& cursor) : varMap(varMap), actions(actions), speed(speed), wndInput(wndInput), cursor(cursor)
{
	if (Qi::debug) debug_entry = HaveEntry(actions);
}

void QiInterpreter::DebugContinue()
{
	debug.notify_all();
}

bool QiInterpreter::PeekSleep(clock_t ms)
{
	if (ms > 0) return QiThread::PeekSleep(ms / speed);
	return QiThread::PeekSleep(0);
}

int QiInterpreter::ActionInterpreter(const Actions& current)
{
	if (current.empty())
	{
		if (&current == &actions) return r_exit;
		return r_continue;
	}
	int r_result = r_continue;
	while (true)
	{
		for (const Action& action : current)
		{
			if (!Qi::run || QiThread::PeekExitMsg()) return r_exit;
			if (wndInput && !IsWindow(wndInput->wnd)) { Qi::popText->Popup("´°¿ÚÊ§Ð§"); return r_exit; }
			r_result = r_continue;
			if (debug_entry)
			{
				if (action.base().debug_entry) debug_entry = false;
			}
			else
			{
				if (action.base().debug_break)
				{
					QApplication::postEvent(Qi::widget.edit, new QEvent((QEvent::Type)EditEvent::debug_pause));
					std::unique_lock<std::mutex> lock(debug_mutex);
					debug.wait(lock);
				}
				else if (action.base().debug_exit)
				{
					return r_exit;
				}
			}
			if (!action.base().disable)
			{
				switch (action.index())
				{
				case QiType::end:
				{
					if (jumpId || debug_entry) continue;
					return r_exit;
				} break;
				case QiType::delay:
				{
					if (jumpId || debug_entry) continue;
					const QiDelay& ref = std::get<QiDelay>(action);
					if (PeekSleep(Rand(ref.max, ref.min))) return r_exit;
				} break;
				case QiType::key:
				{
					if (jumpId || debug_entry) continue;
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
					if (jumpId || debug_entry) continue;
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
					if (jumpId || debug_entry) continue;
					const QiCopyText& ref = std::get<QiCopyText>(action);
					std::string text = Qi::interpreter.execute(Qi::interpreter.makeString(ref.text.toStdString()), varMap).toString();
					System::ClipBoardText(String::toWString(text).c_str());
				} break;
				case QiType::color:
				{
					const QiColor& ref = std::get<QiColor>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }
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
						r_result = ActionInterpreter(ref.next);
					}
					else
					{
						r_result = ActionInterpreter(ref.next2);
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
						r_result = ActionInterpreter(ref.next);
						if (debug_entry || jumpId) break;
						if (r_result != r_continue)
						{
							if (r_result == r_break) return r_continue;
							break;
						}
					}
				} break;
				case QiType::loopEnd:
				{
					if (jumpId || debug_entry) continue;
					return r_break;
				} break;
				case QiType::keyState:
				{
					const QiKeyState& ref = std::get<QiKeyState>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }
					if (Input::stateEx(ref.vk)) r_result = ActionInterpreter(ref.next);
					else r_result = ActionInterpreter(ref.next2);
				} break;
				case QiType::resetPos:
				{
					if (jumpId || debug_entry) continue;
					Input::MoveTo(cursor.x, cursor.y, key_info);
				} break;
				case QiType::image:
				{
					const QiImage& ref = std::get<QiImage>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }RgbMap rgbMap;
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
						r_result = ActionInterpreter(ref.next);
					}
					else
					{
						r_result = ActionInterpreter(ref.next2);
					}
				} break;
				case QiType::popText:
				{
					if (jumpId || debug_entry) continue;
					const QiPopText& ref = std::get<QiPopText>(action);
					std::string text = Qi::interpreter.execute(Qi::interpreter.makeString(ref.text.toStdString()), varMap).toString();
					Qi::popText->Popup(ref.time, text.c_str(), RGB(223, 223, 223));
					if (ref.sync)
					{
						if (ref.sync && PeekSleep(ref.time)) r_result = r_exit;
					}
					else Sleep(10);
				} break;
				case QiType::savePos:
				{
					if (jumpId || debug_entry) continue;
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
						r_result = ActionInterpreter(ref.next);
						if (debug_entry || jumpId) break;
						if (r_result != r_continue)
						{
							if (r_result == r_break) break;
							break;
						}
					}
				} break;
				case QiType::jump:
				{
					if (jumpId || debug_entry) continue;
					const QiJump& ref = std::get<QiJump>(action);
					if (ref.id > 0)
					{
						jumpId = ref.id;
						r_result = r_top;
					}
				} break;
				case QiType::jumpPoint:
				{
					if (debug_entry) continue;
					const QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(action);
					if (jumpPoint.id == jumpId) jumpId = 0;
				} break;
				case QiType::dialog:
				{
					const QiDialog& ref = std::get<QiDialog>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }int bn = IDYES;
					std::string text = Qi::interpreter.execute(Qi::interpreter.makeString(ref.text.toStdString()), varMap).toString();
					std::string text2 = Qi::interpreter.execute(Qi::interpreter.makeString(ref.title.toStdString()), varMap).toString();
					if (ref.style == QiDialog::warning) bn = MessageBoxW(GetForegroundWindow(), String::toWString(text).c_str(), String::toWString(text2).c_str(), MB_YESNO | MB_TOPMOST | MB_ICONWARNING);
					else if (ref.style == QiDialog::error) bn = MessageBoxW(GetForegroundWindow(), String::toWString(text).c_str(), String::toWString(text2).c_str(), MB_YESNO | MB_TOPMOST | MB_ICONERROR);
					else bn = MessageBoxW(GetForegroundWindow(), String::toWString(text).c_str(), String::toWString(text2).c_str(), MB_YESNO | MB_TOPMOST);
					if (bn == IDYES) r_result = ActionInterpreter(ref.next);
					else r_result = ActionInterpreter(ref.next2);
				} break;
				case QiType::block:
				{
					const QiBlock& ref = std::get<QiBlock>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }
				} break;
				case QiType::blockExec:
				{
					if (jumpId || debug_entry) continue;
					const QiBlockExec& ref = std::get<QiBlockExec>(action);
					const QiBlock* pBlock = QiFn::FindBlock(actions, ref.id);
					if (pBlock) r_result = ActionInterpreter(pBlock->next);
				} break;
				case QiType::quickInput:
				{
					if (jumpId || debug_entry) continue;
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
					if (jumpId || debug_entry) continue;
					const QiKeyBlock& ref = std::get<QiKeyBlock>(action);
					Qi::keyBlock[ref.vk] = ref.block;
				} break;
				case QiType::clock:
				{
					const QiClock& ref = std::get<QiClock>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }
					if (QiTime::compare(ref.time) < 0) r_result = ActionInterpreter(ref.next);
					else r_result = ActionInterpreter(ref.next2);
				} break;
				case QiType::ocr:
				{
					const QiOcr& ref = std::get<QiOcr>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }
					if (Qi::ocr)
					{
						RECT rect = QiFn::ATRR(ref.rect);
						CImage image;
						HDC hdc;
						if (wndInput)
						{
							hdc = GetDC(wndInput->wnd);
							rect = QiFn::WATRR(ref.rect, wndInput->wnd);
							image = Image::toCImage32(hdc, rect);
							ReleaseDC(wndInput->wnd, hdc);
						}
						else
						{
							hdc = GetDC(nullptr);
							rect = QiFn::ATRR(ref.rect);
							image = Image::toCImage32(hdc, rect);
							ReleaseDC(nullptr, hdc);
						}
						if (!image.IsNull())
						{
							std::string text = Qi::ocr->scan(image);
							image.ReleaseDC();
							text = Qi::interpreter.removeWrap(text);
							if (!text.empty())
							{
								Qi::interpreter.makeValue(ref.var.toStdString(), text, varMap);
								if (ref.text == text.c_str())
								{
									r_result = ActionInterpreter(ref.next);
									break;
								}
							}
						}
						r_result = ActionInterpreter(ref.next2);
					}
				} break;
				case QiType::varOperator:
				{
					if (jumpId || debug_entry) continue;
					const QiVarOperator& ref = std::get<QiVarOperator>(action);
					try { Qi::interpreter.interpretAll(ref.script.toStdString(), varMap); }
					catch (std::runtime_error e) { Qi::interpreter.parseError(e.what()); }
				} break;
				case QiType::varCondition:
				{
					const QiVarCondition& ref = std::get<QiVarCondition>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }
					try {
						QiVar var = Qi::interpreter.execute(ref.script.toStdString(), varMap);
						bool scriptResult = false;
						if (var.isNumber()) scriptResult = var.num;
						else if (var.isString()) scriptResult == !var.str.empty();
						if (scriptResult) r_result = ActionInterpreter(ref.next);
						else r_result = ActionInterpreter(ref.next2);
					}
					catch (std::runtime_error e) { Qi::interpreter.parseError(e.what()); }
				} break;
				}
			}
			if (r_result != r_continue) break;
		}
		if ((r_result == r_top) && (&current == &actions)) continue;
		return r_result;
	}
	return r_exit;
}