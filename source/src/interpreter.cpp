﻿#pragma once
#include "interpreter.h"
#include <QTextDialog.h>

constexpr int interpreter_max_stack = 256;

QiInterpreter::QiInterpreter(Macro& macro, bool isRunning, QiWorker& worker) :
	worker(worker),
	macro(macro),
	actions(isRunning ? macro.acRun : macro.acEnd),
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
		macro.script_interpreter.clearLocal();
		Qi::widget.varViewReload();
		GetCursorPos(&cursor);
	}
	if (macro.wndState) wndInput = &macro.wndInput;
	else wndInput = nullptr;
	if (Qi::debug) debug_entry = actions.iter([](const Action& a) { return a.base().debug_entry; });
}

int QiInterpreter::rand(int max, int min)
{
	min = Rand(max, min);
	macro.script_interpreter.setValue(QiScriptInterpreter::var_rand_last, min);
	return min;
}

void QiInterpreter::setLastPos(int x, int y)
{
	macro.script_interpreter.setValue(QiScriptInterpreter::var_cur_last_x, x);
	macro.script_interpreter.setValue(QiScriptInterpreter::var_cur_last_y, y);
	POINT p = wndInput ? QiFn::P_WRTA(POINT({ x, y }), wndInput->wnd) : QiFn::P_SRTA(POINT({ x,y }));
	macro.script_interpreter.setValue(QiScriptInterpreter::var_cur_last_ax, p.x);
	macro.script_interpreter.setValue(QiScriptInterpreter::var_cur_last_ay, p.y);
}

bool QiInterpreter::isInvalid()
{
	if (Qi::debug) return worker.m_stop;
	return !Qi::run || worker.m_stop || (timer && !QiTime::in(timerStart, timerEnd));
}

void QiInterpreter::DebugContinue()
{
	debug_condition.notify_all();
}

bool QiInterpreter::PeekSleep(clock_t ms)
{
	clock_t end = clock() + (ms / speed);
	if (ms > 5)
	{
		while (!worker.m_stop && (clock() < end)) std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	else
	{
		while (!worker.m_stop && (clock() < end)) std::this_thread::yield();;
	}
	return worker.m_stop;
}

QString QiInterpreter::makePath()
{
	QString p;
	for (const auto& i : path) p += i + QString(" / ");
	if (p.size() > 3) return p.mid(0, p.size() - 3);
	return p;
}

std::string QiInterpreter::errPath()
{
	return std::string("路径：") + makePath().toStdString();
}
std::wstring QiInterpreter::werrPath()
{
	return std::wstring(L"\n\n路径：") + makePath().toStdWString();
}

InterpreterResult QiInterpreter::ActionInterpreter(const Actions& current)
{
	if (current.empty())
	{
		if (&current == &actions) Sleep(1);
		return InterpreterResult::r_continue;
	}
	else if (path.size() > interpreter_max_stack)
	{
		MsgBox::Error(std::wstring(L"宏：\"") + macro.name.toStdWString() + std::wstring(L"\"\n\n栈深度超过") + std::to_wstring(interpreter_max_stack) + L"\n\n为防止崩溃已停止运行", L"Quickinput Interpreter");
		return InterpreterResult::r_exit;
	}
	else
	{
		struct PathObject { QiVector<QString>& path; PathObject(QiVector<QString>& path) : path(path) { path.append(QString()); } ~PathObject() { path.remove(); } } patho(path);
		InterpreterResult r_result = InterpreterResult::r_continue;
	r_top_entry:
		for (size_t i = 0; i < current.size(); i++)
		{
			const Action& action = current[i];
			if (isInvalid()) return InterpreterResult::r_exit;
			if (wndInput && !IsWindow(wndInput->wnd)) { Qi::popText->Popup("窗口失效"); return InterpreterResult::r_exit; }

			if (debug_entry)
			{
				if (action.base().debug_entry) debug_entry = false;
				else if (action.typeNext() == QiTypeNext::none) continue;
			}
			else if (jumpId)
			{
				if (action.type() == QiType::jumpPoint)
				{
					if (action.base().id == jumpId) jumpId = 0;
					continue;
				}
				else if (action.typeNext() == QiTypeNext::none) continue;
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
					return InterpreterResult::r_exit;
				}
			}

			if (action.base().disable) continue;
			path.back() = QString("[") + QString::number(i + 1) + QString("]") + action.base().name();
			r_result = InterpreterResult::r_continue;
			switch (action.type())
			{
			case QiType::end: return InterpreterResult::r_exit;
			case QiType::delay:
			{
				const QiDelay& ref = action.to<QiDelay>();
				const int min = ref.v_min.isEmpty() ? ref.min : macro.script_interpreter.value(ref.v_min.toStdString()).toInteger();
				const int max = ref.v_max.isEmpty() ? ref.max : macro.script_interpreter.value(ref.v_max.toStdString()).toInteger();
				if (PeekSleep(rand(max, min))) return InterpreterResult::r_exit;
			} break;
			case QiType::key:
			{
				const QiKey& ref = action.to<QiKey>();
				if (wndInput)
				{
					POINT pt;
					HWND current_wnd = wndInput->find(wndInput->pt, pt);
					if (current_wnd)
					{
						if (ref.state == QiKey::up)
						{
							Input::State(current_wnd, ref.vk, pt, false);
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
							Input::State(current_wnd, ref.vk, pt, true);
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
							Input::State(current_wnd, ref.vk, pt, true);
							Sleep(Rand(20, 10));
							Input::State(current_wnd, ref.vk, pt, false);
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
					if (ref.state == QiKey::up) QiFn::Key(ref.vk, false);
					else if (ref.state == QiKey::down) QiFn::Key(ref.vk, true);
					else if (ref.state == QiKey::click)
					{
						QiFn::Key(ref.vk, true);
						Sleep(Rand(20, 10));
						QiFn::Key(ref.vk, false);
						Sleep(Rand(20, 10));
					}
				}
			} break;
			case QiType::mouse:
			{
				const QiMouse& ref = action.to<QiMouse>();
				const int x = (ref.v_x.isEmpty() ? ref.x : macro.script_interpreter.value(ref.v_x.toStdString()).toInteger()) + Rand(ref.ex, (~ref.ex) + 1);
				const int y = (ref.v_y.isEmpty() ? ref.y : macro.script_interpreter.value(ref.v_y.toStdString()).toInteger()) + Rand(ref.ex, (~ref.ex) + 1);

				if (wndInput)
				{
					POINT pt = { x, y };
					if (ref.move) pt.x += wndInput->pt.x * moveScaleX, pt.y += wndInput->pt.y * moveScaleY;
					else pt = QiFn::P_WATR(pt, wndInput->wnd);
					POINT scale = QiFn::PF_WATR(POINTF({ posScaleX, posScaleY }), wndInput->wnd);
					wndInput->pt = { pt.x + scale.x, pt.y + scale.y };
					if (ref.track)
					{
						QiFn::SmoothMove(wndInput->pt.x, wndInput->pt.y, wndInput->pt.x, wndInput->pt.y, ref.speed, [this](int, int, int stepx, int stepy) {
							Input::MoveTo(wndInput->wnd, stepx, stepy, wndInput->mk);
							PeekSleep(10);
							});
					}
					else Input::MoveTo(wndInput->wnd, wndInput->pt.x, wndInput->pt.y, wndInput->mk);
				}
				else
				{
					if (ref.move)
					{
						if (ref.track)
						{
							QiFn::SmoothMove(0, 0, x * moveScaleX, y * moveScaleY, ref.speed, [this](int, int, int stepx, int stepy) {
								QiFn::Move(stepx, stepy);
								PeekSleep(10);
								});
						}
						else QiFn::Move(x * moveScaleX, y * moveScaleY);
					}
					else
					{
						POINT scale = QiFn::P_ATA(POINTF({ posScaleX, posScaleY }));
						if (ref.track)
						{
							POINT spt = QiFn::P_SRTA(Input::pos());
							QiFn::SmoothMove(spt.x, spt.y, x + scale.x, y + scale.y, ref.speed, [this](int x, int y, int, int) {
								QiFn::MoveToA(x, y);
								PeekSleep(10);
								});
						}
						else QiFn::MoveToA(x + scale.x, y + scale.y);
					}
				}
			} break;
			case QiType::copyText:
			{
				const QiCopyText& ref = action.to<QiCopyText>();
				try
				{
					std::string text = macro.script_interpreter.execute(macro.script_interpreter.makeString(ref.text.toStdString())).toString();
					System::ClipBoardText(String::toWString(text).c_str());
				}
				catch (std::exception e) { QiFn::UnBlock(); QiScriptInterpreter::showError(e.what(), errPath()); return InterpreterResult::r_exit; }
			} break;
			case QiType::color:
			{
				const QiColor& ref = action.to<QiColor>();
				RECT rect = {
					ref.v_left.isEmpty() ? ref.rect.left : macro.script_interpreter.value(ref.v_left.toStdString()).toInteger(),
					ref.v_top.isEmpty() ? ref.rect.top : macro.script_interpreter.value(ref.v_top.toStdString()).toInteger(),
					ref.v_right.isEmpty() ? ref.rect.right : macro.script_interpreter.value(ref.v_right.toStdString()).toInteger(),
					ref.v_bottom.isEmpty() ? ref.rect.bottom : macro.script_interpreter.value(ref.v_bottom.toStdString()).toInteger()
				};

				if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
				else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }
				RgbMap rgbMap;
				HDC hdc;
				if (wndInput)
				{
					hdc = GetWindowDC(wndInput->wnd);
					rect = QiFn::R_WATR(rect, wndInput->wnd);
					Image::toRgbMap(hdc, rgbMap, rect);
					ReleaseDC(wndInput->wnd, hdc);
				}
				else
				{
					hdc = GetDC(nullptr);
					rect = QiFn::R_SATR(rect);
					Image::toRgbMap(hdc, rgbMap, rect);
					ReleaseDC(nullptr, hdc);
				}
				Color::FindResult findResult = Color::FindOr(rgbMap, ref.rgbe.toRgb(), ref.rgbe.a);
				if (findResult.find)
				{
					setLastPos(findResult.pt.x += rect.left, findResult.pt.y += rect.top);
					if (ref.move)
					{
						if (wndInput)
						{
							wndInput->pt = findResult.pt;
							Input::MoveTo(wndInput->wnd, wndInput->pt.x, wndInput->pt.y, wndInput->mk);
						}
						else QiFn::MoveTo(findResult.pt.x, findResult.pt.y);
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
				const QiLoop& ref = action.to<QiLoop>();
				int min = ref.v_min.isEmpty() ? ref.min : macro.script_interpreter.value(ref.v_min.toStdString()).toInteger();
				int max = ref.v_max.isEmpty() ? ref.max : macro.script_interpreter.value(ref.v_max.toStdString()).toInteger();

				bool infinite = (min < 1) && (max < 1);
				int count = rand(max, min);

				for (size_t i = 0; !isInvalid() && (infinite || i < count); infinite ? true : i++)
				{
					r_result = ActionInterpreter(ref.next);
					if (debug_entry || jumpId) break;
					if (r_result != InterpreterResult::r_continue)
					{
						if (r_result == InterpreterResult::r_break) r_result = InterpreterResult::r_continue;
						break;
					}
				}
			} break;
			case QiType::loopEnd: return InterpreterResult::r_break;
			case QiType::keyState:
			{
				const QiKeyState& ref = action.to<QiKeyState>();
				if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
				else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }
				if (Input::stateEx(ref.vk)) r_result = ActionInterpreter(ref.next);
				else r_result = ActionInterpreter(ref.next2);
			} break;
			case QiType::resetPos: QiFn::MoveTo(cursor.x, cursor.y); break;
			case QiType::image:
			{
				const QiImage& ref = action.to<QiImage>();
				RECT rect = {
					ref.v_left.isEmpty() ? ref.rect.left : macro.script_interpreter.value(ref.v_left.toStdString()).toInteger(),
					ref.v_top.isEmpty() ? ref.rect.top : macro.script_interpreter.value(ref.v_top.toStdString()).toInteger(),
					ref.v_right.isEmpty() ? ref.rect.right : macro.script_interpreter.value(ref.v_right.toStdString()).toInteger(),
					ref.v_bottom.isEmpty() ? ref.rect.bottom : macro.script_interpreter.value(ref.v_bottom.toStdString()).toInteger()
				};

				if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
				else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }RgbMap rgbMap;
				HDC hdc;
				if (wndInput)
				{
					hdc = GetWindowDC(wndInput->wnd);
					rect = QiFn::R_WATR(rect, wndInput->wnd);
					Image::toRgbMap(hdc, rgbMap, rect);
					ReleaseDC(wndInput->wnd, hdc);
				}
				else
				{
					hdc = GetDC(nullptr);
					rect = QiFn::R_SATR(rect);
					Image::toRgbMap(hdc, rgbMap, rect);
					ReleaseDC(nullptr, hdc);
				}
				POINT pt = Image::Find(rgbMap, ref.map, ref.sim, 10);
				if (pt.x != Image::npos)
				{
					setLastPos(pt.x += rect.left + (ref.map.width() >> 1), pt.y += rect.top + (ref.map.height() >> 1));
					if (ref.move)
					{
						if (wndInput)
						{
							wndInput->pt = pt;
							Input::MoveTo(wndInput->wnd, wndInput->pt.x, wndInput->pt.y, wndInput->mk);
						}
						else QiFn::MoveTo(pt.x, pt.y);
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
				const QiPopText& ref = action.to<QiPopText>();
				try
				{
					std::string text = macro.script_interpreter.execute(macro.script_interpreter.makeString(ref.text.toStdString())).toString();
					Qi::popText->Popup(ref.time, text.c_str(), RGB(223, 223, 223));
					if (ref.sync)
					{
						if (ref.sync && PeekSleep(ref.time)) r_result = InterpreterResult::r_exit;
					}
					else Sleep(1);
				}
				catch (std::exception e) { QiFn::UnBlock(); QiScriptInterpreter::showError(e.what(), errPath()); return InterpreterResult::r_exit; }
			} break;
			case QiType::savePos: cursor = Input::pos(); break;
			case QiType::timer:
			{
				const QiTimer& ref = action.to<QiTimer>();
				clock_t min = ref.v_min.isEmpty() ? ref.min : macro.script_interpreter.value(ref.v_min.toStdString()).toInteger();
				clock_t max = ref.v_max.isEmpty() ? ref.max : macro.script_interpreter.value(ref.v_max.toStdString()).toInteger();
				clock_t time = rand(max, min);
				clock_t begin = clock();
				while (!isInvalid())
				{
					if (!((begin + time) > clock())) { break; }
					r_result = ActionInterpreter(ref.next);
					if (debug_entry || jumpId) break;
					if (r_result != InterpreterResult::r_continue) break;
				}
				if (r_result == InterpreterResult::r_continue)
				{
					ActionInterpreter(ref.next2);
				}
				else if (r_result == InterpreterResult::r_break)
				{
					r_result = InterpreterResult::r_continue;
				}
			} break;
			case QiType::jump:
			{
				const QiJump& ref = action.to<QiJump>();
				if (ref.id > 0)
				{
					const Action* jumpPoint = actions.find(QiType::jumpPoint, ref.id);
					if (jumpPoint)
					{
						jumpId = ref.id;
						r_result = InterpreterResult::r_top;
						break;
					}
				}
			} break;
			case QiType::dialog:
			{
				const QiDialog& ref = action.to<QiDialog>();
				if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
				else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }
				try
				{
					std::string title = macro.script_interpreter.execute(macro.script_interpreter.makeString(ref.title.toStdString())).toString();
					std::string text = macro.script_interpreter.execute(macro.script_interpreter.makeString(ref.text.toStdString())).toString();
					DWORD style = MB_YESNO | MB_TOPMOST;
					if (ref.style == QiDialog::warning) style |= MB_ICONWARNING;
					else if (ref.style == QiDialog::error) style |= MB_ICONERROR;
					if (MessageBoxW(nullptr, String::toWString(text).c_str(), String::toWString(title).c_str(), style) == IDYES) r_result = ActionInterpreter(ref.next);
					else r_result = ActionInterpreter(ref.next2);
				}
				catch (std::exception e) { QiFn::UnBlock(); QiScriptInterpreter::showError(e.what(), errPath()); return InterpreterResult::r_exit; }
			} break;
			case QiType::blockExec:
			{
				const QiBlockExec& ref = action.to<QiBlockExec>();
				const Action* block = actions.find(QiType::block, ref.id);
				if (block) r_result = ActionInterpreter(block->base().next);
			} break;
			case QiType::quickInput:
			{
				const QiQuickInput& ref = action.to<QiQuickInput>();
				for (auto& i : ref.chars)
				{
					if (wndInput)
					{
						POINT pt;
						HWND current_wnd = wndInput->find(wndInput->pt, pt);
						if (current_wnd)
						{
							Input::Click(current_wnd, i, pt, 10);
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
						QiFn::Key(i, true);
						Sleep(Rand(20, 10));
						QiFn::Key(i, false);
						Sleep(Rand(20, 10));
					}
				}
			} break;
			case QiType::keyBlock:
			{
				const QiKeyBlock& ref = action.to<QiKeyBlock>();
				Qi::keyBlock[ref.vk] = ref.block;
			} break;
			case QiType::clock:
			{
				const QiClock& ref = action.to<QiClock>();
				if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
				else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }
				if (QiTime::compare(ref.time) < 0) r_result = ActionInterpreter(ref.next);
				else r_result = ActionInterpreter(ref.next2);
			} break;
			case QiType::ocr:
			{
				const QiOcr& ref = action.to<QiOcr>();
				RECT rect = {
					ref.v_left.isEmpty() ? ref.rect.left : macro.script_interpreter.value(ref.v_left.toStdString()).toInteger(),
					ref.v_top.isEmpty() ? ref.rect.top : macro.script_interpreter.value(ref.v_top.toStdString()).toInteger(),
					ref.v_right.isEmpty() ? ref.rect.right : macro.script_interpreter.value(ref.v_right.toStdString()).toInteger(),
					ref.v_bottom.isEmpty() ? ref.rect.bottom : macro.script_interpreter.value(ref.v_bottom.toStdString()).toInteger()
				};

				if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
				else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }
				try
				{
					if (Qi::ocr.valid())
					{
						CImage image;
						HDC hdc;
						if (wndInput)
						{
							hdc = GetWindowDC(wndInput->wnd);
							rect = QiFn::R_WATR(rect, wndInput->wnd);
							image = Image::toCImage32(hdc, rect);
							ReleaseDC(wndInput->wnd, hdc);
						}
						else
						{
							hdc = GetDC(nullptr);
							rect = QiFn::R_SATR(rect);
							image = Image::toCImage32(hdc, rect);
							ReleaseDC(nullptr, hdc);
						}

						if (!image.IsNull())
						{
							std::vector<POINT> centers;
							std::vector<std::string> text = Qi::ocr.scan_list(image, ref.row, &centers);
							image.ReleaseDC();
							if (!ref.var.isEmpty())
							{
								std::string str;
								for (const auto& s : text)
								{
									if (str.empty()) str += s;
									else str += std::string(" ") + s;
								}
								macro.script_interpreter.setValue(ref.var.toStdString(), str);
								Qi::widget.varViewReload();
							}
							if (!text.empty())
							{
								bool condition = false;
								if (ref.match) condition = text.size() == 1 && ref.text == text.front().c_str();
								else
								{
									for (size_t i = 0; i < text.size(); i++)
									{
										if (text[i].find(ref.text.toStdString()) != std::string::npos)
										{
											condition = true;
											POINT pt = centers.size() > i ? centers[i] : POINT();
											setLastPos(pt.x += rect.left, pt.y += rect.top);
											if (ref.move)
											{
												if (Qi::ocr_ver > 1)
												{
													if (wndInput)
													{
														wndInput->pt = pt;
														Input::MoveTo(wndInput->wnd, wndInput->pt.x, wndInput->pt.y, wndInput->mk);
													}
													else QiFn::MoveTo(pt.x, pt.y);
												}
												else MsgBox::Warning(std::wstring(L"文字识别版本低于2，不支持找到并移动，需要更新"), L"Quickinput Interpreter");
											}
											break;
										}
									}
								}
								if (condition)
								{
									r_result = ActionInterpreter(ref.next);
									break;
								}
							}
						}
						r_result = ActionInterpreter(ref.next2);
					}
					else
					{
						MsgBox::Error(std::wstring(L"未安装OCR组件") + werrPath(), L"Quickinput Interpreter"); return InterpreterResult::r_exit;
					}
				}
				catch (std::exception e) { QiFn::UnBlock(); QiScriptInterpreter::showError(e.what(), errPath()); return InterpreterResult::r_exit; }
			} break;
			case QiType::varOperator:
			{
				const QiVarOperator& ref = action.to<QiVarOperator>();
				try
				{
					macro.script_interpreter.interpretAll(ref.script.toStdString());
				}
				catch (std::runtime_error e) { QiFn::UnBlock(); QiScriptInterpreter::showError(e.what(), errPath()); return InterpreterResult::r_exit; }
				Qi::widget.varViewReload();
			} break;
			case QiType::varCondition:
			{
				const QiVarCondition& ref = action.to<QiVarCondition>();
				if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
				else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }
				try
				{
					QiVar var = macro.script_interpreter.execute(ref.script.toStdString());
					if (var.toBool()) r_result = ActionInterpreter(ref.next);
					else r_result = ActionInterpreter(ref.next2);
				}
				catch (std::runtime_error e) { QiFn::UnBlock(); QiScriptInterpreter::showError(e.what(), errPath()); return InterpreterResult::r_exit; }
				Qi::widget.varViewReload();
			} break;
			case QiType::mouseTrack:
			{
				const QiMouseTrack& ref = action.to<QiMouseTrack>();
				const clock_t begin = clock();
				if (wndInput)
				{
					POINT scale = QiFn::PF_WATR(POINTF({ posScaleX, posScaleY }), wndInput->wnd);
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
					POINT scale = QiFn::P_ATA(POINTF({ posScaleX, posScaleY }));
					for (const auto& i : ref.s)
					{
						while (!isInvalid())
						{
							if ((i.t / speed) <= (clock() - begin))
							{
								QiFn::MoveToA(i.x, i.y);
								break;
							}
							Sleep(0);
						}
					}
				}
			} break;
			case QiType::open: ShellExecuteW(nullptr, L"open", (const wchar_t*)action.to<QiOpen>().url.utf16(), nullptr, nullptr, SW_SHOW); break;
			case QiType::editDialog:
			{
				const QiEditDialog& ref = action.to<QiEditDialog>();
				try
				{
					std::string title = macro.script_interpreter.execute(macro.script_interpreter.makeString(ref.title.toStdString())).toString();
					std::string text = macro.script_interpreter.execute(macro.script_interpreter.makeString(ref.text.toStdString())).toString();
					text = String::toString(TextEditBox(nullptr, String::toWString(title).c_str(), String::toWString(text).c_str(), ref.mult, WS_EX_TOPMOST, Qi::ico));
					if (!text.empty()) macro.script_interpreter.setValue(ref.var.toStdString(), text);
				}
				catch (std::exception e) { QiFn::UnBlock(); QiScriptInterpreter::showError(e.what(), errPath()); return InterpreterResult::r_exit; }
			} break;
			case QiType::volume:
			{
				const QiVolume& ref = action.to<QiVolume>();
				if (debug_entry) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && debug_entry) r_result = ActionInterpreter(ref.next2); continue; }
				else if (jumpId) { if (ref.next.not_empty()) r_result = ActionInterpreter(ref.next); if (ref.next2.not_empty() && jumpId) r_result = ActionInterpreter(ref.next2); continue; }
				if (Sound::SpeakerVolume(ref.time > 5 ? ref.time : 5, ref.max) > ref.volume) r_result = ActionInterpreter(ref.next);
				else r_result = ActionInterpreter(ref.next2);
			} break;
			case QiType::soundPlay:
			{
				const QiSoundPlay& ref = action.to<QiSoundPlay>();
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
			case QiType::msgView:
			{
				const QiMsgView& ref = action.to<QiMsgView>();
				try
				{
					std::string text = macro.script_interpreter.execute(macro.script_interpreter.makeString(ref.text.toStdString())).toString();
					if (ref.option == QiMsgView::set) Qi::widget.msgViewSet(QString::fromStdString(text));
					else if (ref.option == QiMsgView::add)
					{
						MsgViewInfo info;
						info.level = ref.level;
						info.time = time(nullptr);
						info.macro = macro.name;
						info.text = text.c_str();
						Qi::widget.msgViewAdd(info);
					}
					else if (ref.option == QiMsgView::clear) Qi::widget.msgViewClear();
					else if (ref.option == QiMsgView::show) Qi::widget.msgViewShow();
					else if (ref.option == QiMsgView::hide) Qi::widget.msgViewHide();
				}
				catch (std::exception e) { QiFn::UnBlock(); QiScriptInterpreter::showError(e.what(), errPath()); return InterpreterResult::r_exit; }
			} break;
			}
			if (r_result != InterpreterResult::r_continue) break;
		}
		if ((r_result == InterpreterResult::r_top) && (&current == &actions)) goto r_top_entry;
		return r_result;
	}
	return InterpreterResult::r_exit;
}