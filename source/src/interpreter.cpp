#pragma once
#include "interpreter.h"
#include "QTextDialog.h"

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

QiInterpreter::QiInterpreter(Macro& macro, bool isRunning) :
	actions(isRunning ? macro.acRun : macro.acEnd),
	varMap(macro.varMap),
	cursor(macro.cursor),
	speed(macro.speed),
	timer(macro.timer),
	timerStart(macro.timerStart),
	timerEnd(macro.timerEnd),
	moveScaleX(macro.moveScaleX),
	moveScaleY(macro.moveScaleY),
	posScaleX(macro.posScaleX),
	posScaleY(macro.posScaleY)
{
	macro.interpreter = this;
	if (isRunning)
	{
		macro.varMap.clear();
		Qi::widget.varViewReload();
		GetCursorPos(&cursor);
	}
	if (macro.wndState) wndInput = &macro.wndInput;
	if (Qi::debug) debug_entry = HaveEntry(actions);
}

bool QiInterpreter::isInvalid()
{
	if (Qi::debug) return Qi::PeekExitMsg();
	return !Qi::run || Qi::PeekExitMsg() || (timer && !QiTime::in(timerStart, timerEnd));
}

void QiInterpreter::DebugContinue()
{
	debug_condition.notify_all();
}

bool QiInterpreter::PeekSleep(clock_t ms)
{
	return Qi::PeekSleep(ms / speed);
}

int QiInterpreter::ActionInterpreter(const Actions& current, int layer)
{
	if (current.empty())
	{
		if (&current == &actions) Sleep(1);
		return r_continue;
	}
	int r_result = r_continue;
	while (true)
	{
		for (size_t i = 0; i < current.size(); i++)
		{
			const Action& action = current[i];
			if (isInvalid()) return r_exit;
			if (wndInput && !IsWindow(wndInput->wnd)) { Qi::popText->Popup("窗口失效"); return r_exit; }
			r_result = r_continue;
			if (debug_entry)
			{
				if (action.base().debug_entry) debug_entry = false;
			}
			else if (Qi::debug)
			{
				if (action.base().debug_break)
				{
					Qi::widget.editDebugPause();
					std::unique_lock<std::mutex> lock(debug_mutex);
					debug_condition.wait(lock);
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
					int min = 1;
					int max = 1;
					try { min = ref.v_min.isEmpty() ? ref.min : varMap.at(ref.v_min.toStdString()).toInteger(); }
					catch (...) { MsgBox::Error(std::wstring(L"等待变量无效：") + ref.v_min.toStdWString() + std::wstring(L"\n\n位于层：") + std::to_wstring(layer) + std::wstring(L"，行：") + std::to_wstring(i + 1)); break; }
					try { max = ref.v_max.isEmpty() ? ref.max : varMap.at(ref.v_max.toStdString()).toInteger(); }
					catch (...) { MsgBox::Error(std::wstring(L"等待变量无效：") + ref.v_max.toStdWString() + std::wstring(L"\n\n位于层：") + std::to_wstring(layer) + std::wstring(L"，行：") + std::to_wstring(i + 1)); break; }

					if (PeekSleep(Rand(max, min))) return r_exit;
				} break;
				case QiType::key:
				{
					if (jumpId || debug_entry) continue;
					const QiKey& ref = std::get<QiKey>(action);
					if (wndInput)
					{
						POINT pt;
						HWND current = wndInput->find(wndInput->pt, pt, wndInput->child);
						if (current)
						{
							if (ref.state == QiKey::up)
							{
								Input::State(current, ref.vk, pt, false);
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
								Input::State(current, ref.vk, pt, true);
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
								Input::State(current, ref.vk, pt, true);
								Sleep(Rand(20, 10));
								Input::State(current, ref.vk, pt, false);
								Sleep(Rand(20, 10));
								if (ref.vk == VK_LBUTTON) wndInput->mk &= ~MK_LBUTTON;
								else if (ref.vk == VK_RBUTTON) wndInput->mk &= ~MK_RBUTTON;
								else if (ref.vk == VK_MBUTTON) wndInput->mk &= ~MK_MBUTTON;
								else if (ref.vk == VK_XBUTTON1) wndInput->mk &= ~MK_XBUTTON1;
								else if (ref.vk == VK_XBUTTON2) wndInput->mk &= ~MK_XBUTTON2;
								else if (ref.vk == VK_CONTROL) wndInput->mk &= ~MK_CONTROL;
								else if (ref.vk == VK_SHIFT) wndInput->mk &= ~MK_SHIFT;
							}
						}
					}
					else
					{
						if (ref.state == QiKey::up) Input::State(ref.vk, false, Qi::key_info);
						else if (ref.state == QiKey::down) Input::State(ref.vk, true, Qi::key_info);
						else if (ref.state == QiKey::click)
						{
							Input::State(ref.vk, true, Qi::key_info);
							Sleep(Rand(20, 10));
							Input::State(ref.vk, false, Qi::key_info);
							Sleep(Rand(20, 10));
						}
					}
				} break;
				case QiType::mouse:
				{
					if (jumpId || debug_entry) continue;
					const QiMouse& ref = std::get<QiMouse>(action);
					if (wndInput)
					{
						POINT pt = { ref.x + (Rand(ref.ex, (~ref.ex + 1))) ,ref.y + (Rand(ref.ex, (~ref.ex + 1))) };
						if (ref.move) pt.x += wndInput->pt.x * moveScaleX, pt.y += wndInput->pt.y * moveScaleY;
						else pt = QiFn::P_WATR({ pt.x, pt.y }, wndInput->wnd);
						POINT scale = QiFn::PF_WATR(QPointF(posScaleX, posScaleY), wndInput->wnd);
						wndInput->pt = { pt.x + scale.x, pt.y + scale.y };
						if (ref.track)
						{
							QiFn::SmoothMove(wndInput->pt.x, wndInput->pt.y, wndInput->pt.x, wndInput->pt.y, ref.speed, [this](int x, int y, int stepx, int stepy) {
								Input::MoveTo(wndInput->wnd, stepx, stepy, wndInput->mk);
								PeekSleep(10);
								});
						}
						else Input::MoveTo(wndInput->wnd, wndInput->pt.x, wndInput->pt.y, wndInput->mk);
					}
					else
					{
						int x = ref.x + (Rand(ref.ex, (~ref.ex + 1)));
						int y = ref.y + (Rand(ref.ex, (~ref.ex + 1)));
						if (ref.move)
						{
							if (ref.track)
							{
								QiFn::SmoothMove(0, 0, x * moveScaleX, y * moveScaleY, ref.speed, [this](int x, int y, int stepx, int stepy) {
									Input::Move(stepx, stepy, Qi::key_info);
									PeekSleep(10);
									});
							}
							else Input::Move(x * moveScaleX, y * moveScaleY, Qi::key_info);
						}
						else
						{
							POINT scale = QiFn::P_ATA(QPointF(posScaleX, posScaleY));
							if (ref.track)
							{
								POINT spt = QiFn::P_SRTA(Input::pos());
								QiFn::SmoothMove(spt.x, spt.y, x + scale.x, y + scale.y, ref.speed, [this, scale](int x, int y, int stepx, int stepy) {
									Input::MoveToA(x * 6.5535f, y * 6.5535f, Qi::key_info);
									PeekSleep(10);
									});
							}
							else Input::MoveToA((x + scale.x) * 6.5535f, (y + scale.y) * 6.5535f, Qi::key_info);
						}
					}
				} break;
				case QiType::copyText:
				{
					if (jumpId || debug_entry) continue;
					const QiCopyText& ref = std::get<QiCopyText>(action);
					try
					{
						std::string text = Qi::interpreter.execute(Qi::interpreter.makeString(ref.text.toStdString()), varMap).toString();
						System::ClipBoardText(String::toWString(text).c_str());
					}
					catch (std::exception e) { QiScriptInterpreter::showError(e.what(), std::string("位于层：") + std::to_string(layer) + std::string("，行：") + std::to_string(i + 1)); }
				} break;
				case QiType::color:
				{
					const QiColor& ref = std::get<QiColor>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					RgbMap rgbMap;
					RECT rect;
					HDC hdc;
					if (wndInput)
					{
						hdc = GetDC(wndInput->wnd);
						rect = QiFn::R_WATR(ref.rect, wndInput->wnd);
						Image::toRgbMap(hdc, rgbMap, rect);
						ReleaseDC(wndInput->wnd, hdc);
					}
					else
					{
						hdc = GetDC(nullptr);
						rect = QiFn::R_SATR(ref.rect);
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
							else Input::MoveTo(findResult.pt.x, findResult.pt.y, Qi::key_info);
						}
						r_result = ActionInterpreter(ref.next, layer + 1);
					}
					else
					{
						r_result = ActionInterpreter(ref.next2, layer + 1);
					}
				} break;
				case QiType::loop:
				{
					const QiLoop& ref = std::get<QiLoop>(action);
					int min = 1;
					int max = 1;
					try { min = ref.v_min.isEmpty() ? ref.min : varMap.at(ref.v_min.toStdString()).toInteger(); }
					catch (...) { MsgBox::Error(std::wstring(L"循环变量无效：") + ref.v_min.toStdWString() + std::wstring(L"\n\n位于层：") + std::to_wstring(layer) + std::wstring(L"，行：") + std::to_wstring(i + 1)); break; }
					try { max = ref.v_max.isEmpty() ? ref.max : varMap.at(ref.v_max.toStdString()).toInteger(); }
					catch (...) { MsgBox::Error(std::wstring(L"循环变量无效：") + ref.v_max.toStdWString() + std::wstring(L"\n\n位于层：") + std::to_wstring(layer) + std::wstring(L"，行：") + std::to_wstring(i + 1)); break; }

					bool infinite = (min < 1) && (max < 1);
					int count = Rand(max, min);
					int i = 0;

					for (size_t i = 0; !isInvalid() && (infinite || i < count); infinite ? true : i++)
					{
						r_result = ActionInterpreter(ref.next, layer + 1);
						if (debug_entry || jumpId) break;
						if (r_result != r_continue)
						{
							if (r_result == r_break) r_result = r_continue;
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
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					if (Input::stateEx(ref.vk)) r_result = ActionInterpreter(ref.next, layer + 1);
					else r_result = ActionInterpreter(ref.next2, layer + 1);
				} break;
				case QiType::resetPos:
				{
					if (jumpId || debug_entry) continue;
					Input::MoveTo(cursor.x, cursor.y, Qi::key_info);
				} break;
				case QiType::image:
				{
					const QiImage& ref = std::get<QiImage>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }RgbMap rgbMap;
					RECT rect;
					HDC hdc;
					if (wndInput)
					{
						hdc = GetDC(wndInput->wnd);
						rect = QiFn::R_WATR(ref.rect, wndInput->wnd);
						Image::toRgbMap(hdc, rgbMap, rect);
						ReleaseDC(wndInput->wnd, hdc);
					}
					else
					{
						hdc = GetDC(nullptr);
						rect = QiFn::R_SATR(ref.rect);
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
							else Input::MoveTo(pt.x, pt.y, Qi::key_info);
						}
						r_result = ActionInterpreter(ref.next, layer + 1);
					}
					else
					{
						r_result = ActionInterpreter(ref.next2, layer + 1);
					}
				} break;
				case QiType::popText:
				{
					if (jumpId || debug_entry) continue;
					const QiPopText& ref = std::get<QiPopText>(action);
					try
					{
						std::string text = Qi::interpreter.execute(Qi::interpreter.makeString(ref.text.toStdString()), varMap).toString();
						Qi::popText->Popup(ref.time, text.c_str(), RGB(223, 223, 223));
						if (ref.sync)
						{
							if (ref.sync && PeekSleep(ref.time)) r_result = r_exit;
						}
						else Sleep(10);
					}
					catch (std::exception e) { QiScriptInterpreter::showError(e.what(), std::string("位于层：") + std::to_string(layer) + std::string("，行：") + std::to_string(i + 1)); }
				} break;
				case QiType::savePos:
				{
					if (jumpId || debug_entry) continue;
					cursor = Input::pos();
				} break;
				case QiType::timer:
				{
					const QiTimer& ref = std::get<QiTimer>(action);
					int min = 1;
					int max = 1;
					try { min = ref.v_min.isEmpty() ? ref.min : varMap.at(ref.v_min.toStdString()).toInteger(); }
					catch (...) { MsgBox::Error(std::wstring(L"定时变量无效：") + ref.v_min.toStdWString() + std::wstring(L"\n\n位于层：") + std::to_wstring(layer) + std::wstring(L"，行：") + std::to_wstring(i + 1)); break; }
					try { max = ref.v_max.isEmpty() ? ref.max : varMap.at(ref.v_max.toStdString()).toInteger(); }
					catch (...) { MsgBox::Error(std::wstring(L"定时变量无效：") + ref.v_max.toStdWString() + std::wstring(L"\n\n位于层：") + std::to_wstring(layer) + std::wstring(L"，行：") + std::to_wstring(i + 1)); break; }

					clock_t time = Rand(max, min);
					clock_t begin = clock();
					while (!isInvalid())
					{
						if (!((begin + time) > clock())) { break; }
						r_result = ActionInterpreter(ref.next, layer + 1);
						if (debug_entry || jumpId) break;
						if (r_result != r_continue)
						{
							if (r_result == r_break) r_result = r_continue;
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
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					try
					{
						std::string title = Qi::interpreter.execute(Qi::interpreter.makeString(ref.title.toStdString()), varMap).toString();
						std::string text = Qi::interpreter.execute(Qi::interpreter.makeString(ref.text.toStdString()), varMap).toString();
						DWORD style = MB_YESNO | MB_TOPMOST;
						if (ref.style == QiDialog::warning) style |= MB_ICONWARNING;
						else if (ref.style == QiDialog::error) style |= MB_ICONERROR;
						if (MessageBoxW(nullptr, String::toWString(text).c_str(), String::toWString(title).c_str(), style) == IDYES) r_result = ActionInterpreter(ref.next, layer + 1);
						else r_result = ActionInterpreter(ref.next2, layer + 1);
					}
					catch (std::exception e) { QiScriptInterpreter::showError(e.what(), std::string("位于层：") + std::to_string(layer) + std::string("，行：") + std::to_string(i + 1)); }
				} break;
				case QiType::block:
				{
					const QiBlock& ref = std::get<QiBlock>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
				} break;
				case QiType::blockExec:
				{
					if (jumpId || debug_entry) continue;
					const QiBlockExec& ref = std::get<QiBlockExec>(action);
					const QiBlock* pBlock = QiFn::FindBlock(actions, ref.id);
					if (pBlock) r_result = ActionInterpreter(pBlock->next, layer + 1);
				} break;
				case QiType::quickInput:
				{
					if (jumpId || debug_entry) continue;
					const QiQuickInput& ref = std::get<QiQuickInput>(action);
					for (auto& i : ref.chars)
					{
						if (wndInput)
						{
							POINT pt;
							HWND current = wndInput->find(wndInput->pt, pt, wndInput->child);
							if (current)
							{
								Input::Click(current, i, pt, 10);
								if (i == VK_LBUTTON) wndInput->mk |= MK_LBUTTON;
								else if (i == VK_RBUTTON) wndInput->mk |= MK_RBUTTON;
								else if (i == VK_MBUTTON) wndInput->mk |= MK_MBUTTON;
								else if (i == VK_XBUTTON1) wndInput->mk |= MK_XBUTTON1;
								else if (i == VK_XBUTTON2) wndInput->mk |= MK_XBUTTON2;
								else if (i == VK_CONTROL) wndInput->mk |= MK_CONTROL;
								else if (i == VK_SHIFT) wndInput->mk |= MK_SHIFT;
							}
						}
						else
						{
							Input::Click(i, 10, Qi::key_info);
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
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					if (QiTime::compare(ref.time) < 0) r_result = ActionInterpreter(ref.next, layer + 1);
					else r_result = ActionInterpreter(ref.next2, layer + 1);
				} break;
				case QiType::ocr:
				{
					const QiOcr& ref = std::get<QiOcr>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					try
					{
						if (Qi::ocr)
						{
							RECT rect = QiFn::R_SATR(ref.rect);
							CImage image;
							HDC hdc;
							if (wndInput)
							{
								hdc = GetDC(wndInput->wnd);
								rect = QiFn::R_WATR(ref.rect, wndInput->wnd);
								image = Image::toCImage32(hdc, rect);
								ReleaseDC(wndInput->wnd, hdc);
							}
							else
							{
								hdc = GetDC(nullptr);
								rect = QiFn::R_SATR(ref.rect);
								image = Image::toCImage32(hdc, rect);
								ReleaseDC(nullptr, hdc);
							}

							if (!image.IsNull())
							{
								std::string text = Qi::ocr->scan(image, ref.row);
								image.ReleaseDC();
								if (!text.empty())
								{
									bool rr = Qi::interpreter.makeValue(ref.var.toStdString(), text, varMap);
									if ((ref.match && ref.text == text.c_str()) || (!ref.match && text.find(ref.text.toStdString()) != std::string::npos))
									{
										r_result = ActionInterpreter(ref.next, layer + 1);
										break;
									}
								}
							}
							r_result = ActionInterpreter(ref.next2, layer + 1);
						}
						else
						{
							MsgBox::Error(std::wstring(L"未安装OCR组件\n\n位于层：") + std::to_wstring(layer) + std::wstring(L"，行：") + std::to_wstring(i + 1));
						}
					}
					catch (std::exception e) { QiScriptInterpreter::showError(e.what(), std::string("位于层：") + std::to_string(layer) + std::string("，行：") + std::to_string(i + 1)); }
				} break;
				case QiType::varOperator:
				{
					if (jumpId || debug_entry) continue;
					const QiVarOperator& ref = std::get<QiVarOperator>(action);
					try
					{
						Qi::interpreter.interpretAll(ref.script.toStdString(), varMap);
					}
					catch (std::runtime_error e) { Qi::interpreter.showError(e.what(), std::string("位于层：") + std::to_string(layer) + std::string("，行：") + std::to_string(i + 1)); }
					Qi::widget.varViewReload();
				} break;
				case QiType::varCondition:
				{
					const QiVarCondition& ref = std::get<QiVarCondition>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					try
					{
						QiVar var = Qi::interpreter.execute(ref.script.toStdString(), varMap);
						if (var.toBool()) r_result = ActionInterpreter(ref.next, layer + 1);
						else r_result = ActionInterpreter(ref.next2, layer + 1);
					}
					catch (std::runtime_error e) { Qi::interpreter.showError(e.what(), std::string("位于层：") + std::to_string(layer) + std::string("，行：") + std::to_string(i + 1)); }
					Qi::widget.varViewReload();
				} break;
				case QiType::mouseTrack:
				{
					if (jumpId || debug_entry) continue;
					const QiMouseTrack& ref = std::get<QiMouseTrack>(action);
					const clock_t begin = clock();
					if (wndInput)
					{
						POINT scale = QiFn::PF_WATR(QPointF(posScaleX, posScaleY), wndInput->wnd);
						for (const auto& i : ref.s)
						{
							while (!isInvalid())
							{
								if ((i.t / speed) <= (clock() - begin))
								{
									POINT pt = QiFn::P_WATR({ i.x, i.y }, wndInput->wnd);
									wndInput->pt = { pt.x + scale.x, pt.y + scale.y };
									Input::MoveTo(wndInput->wnd, wndInput->pt.x, wndInput->pt.y, wndInput->mk);
									break;
								}
								Sleep(0);
							}
						}
					}
					else
					{
						POINT scale = QiFn::P_ATA(QPointF(posScaleX, posScaleY));
						for (const auto& i : ref.s)
						{
							while (!isInvalid())
							{
								if ((i.t / speed) <= (clock() - begin))
								{
									Input::MoveToA(i.x * 6.5535f, i.y * 6.5535f, Qi::key_info);
									break;
								}
								Sleep(0);
							}
						}
					}
				} break;
				case QiType::open:
				{
					if (jumpId || debug_entry) continue;
					const QiOpen& ref = std::get<QiOpen>(action);
					ShellExecuteW(nullptr, L"open", (const wchar_t*)ref.url.utf16(), nullptr, nullptr, SW_SHOW);
				} break;
				case QiType::editDialog:
				{
					if (jumpId || debug_entry) continue;
					const QiEditDialog& ref = std::get<QiEditDialog>(action);
					try
					{
						std::string title = Qi::interpreter.execute(Qi::interpreter.makeString(ref.title.toStdString()), varMap).toString();
						std::string text = Qi::interpreter.execute(Qi::interpreter.makeString(ref.text.toStdString()), varMap).toString();
						text = String::toString(TextEditBox(nullptr, String::toWString(title).c_str(), String::toWString(text).c_str(), ref.mult, WS_EX_TOPMOST, L"ICOAPP"));
						if (!text.empty()) Qi::interpreter.makeValue(ref.var.toStdString(), text, varMap);
					}
					catch (std::exception e) { QiScriptInterpreter::showError(e.what(), std::string("位于层：") + std::to_string(layer) + std::string("，行：") + std::to_string(i + 1)); }
				} break;
				case QiType::volume:
				{
					const QiVolume& ref = std::get<QiVolume>(action);
					if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next, layer + 1); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2, layer + 1); continue; }
					if (Sound::SpeakerVolume(ref.time > 5 ? ref.time : 5, ref.max) > ref.volume) r_result = ActionInterpreter(ref.next, layer + 1);
					else r_result = ActionInterpreter(ref.next2, layer + 1);
				} break;
				case QiType::soundPlay:
				{
					if (jumpId || debug_entry) continue;
					const QiSoundPlay& ref = std::get<QiSoundPlay>(action);
					if (ref.state == QiSoundPlay::play)
					{
						QiFn::SoundPlay(ref.file, ref.sync);
						if (!ref.sync) Sleep(10);
					}
					else if (ref.state == QiSoundPlay::pause)
					{
						Sound::SoundPause();
					}
					else if (ref.state == QiSoundPlay::resume)
					{
						Sound::SoundResume();
					}
					else
					{
						Sound::SoundClose();
						Sound::WavePlay(std::wstring());
					}
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