#pragma execution_character_set("utf-8")
#include "minc.h"

void InsertInput(const BYTE& key, const bool& state, const POINT& pt)
{
	if (Input::isMouse(key) && InRect(Window::rect((HWND)qis.widget.record->winId()), Input::pos())) return;
	if (qis.recordWindow && !IsWindowVisible(qis.recordWindow))
	{
		QApplication::postEvent(qis.widget.record, new QEvent((QEvent::Type)QiEvent::recClose));
		PopBox::Popup(L"窗口已失效", RGB(255, 64, 64), 2000);
	}

	// delay
	if (qis.recordClock)
	{
		Action& action = qis.record.AddNull(); action.type = Action::_delay;
		clock_t nowClock = clock();
		action.d.delay.tmax = action.d.delay.tmin = nowClock - qis.recordClock;
		qis.recordClock = nowClock;
	}
	else
	{
		qis.recordClock = clock();
	}

	// pos
	if (Input::isMouse(key))
	{
		POINT position;
		Action& action = qis.record.AddNull(); action.type = Action::_mouse;
		if (qis.recordWindow)
		{
			POINT wpt = Window::pos(qis.recordWindow);
			position = QiFn::WRTA({ pt.x - wpt.x, pt.y - wpt.y }, qis.recordWindow);
		}
		else
		{
			position = QiFn::RTA(pt);
		}
		action.d.mouse.x = position.x;
		action.d.mouse.y = position.y;
	}

	// key
	{
		Action& action = qis.record.AddNull(); action.type = Action::_key;
		action.d.key.vk = key;
		if (state) action.d.key.state = QiKey::down;
		else action.d.key.state = QiKey::up;
	}
}

bool _stdcall InputHook::InputProc(BYTE vk, bool state, POINT msPt, PULONG_PTR exInfo)
{
	// LSHIFT to SHIFT
	BYTE key = Input::Convert(vk);
	// Quick input
	if (*exInfo == 214)
	{
		*exInfo = 0;
		return false;
	}
	// Other input
	else if (key)
	{
		if (state)
		{
			if (!qis.keyState[key])
			{
				qis.keyState[key] = true;
				if (qis.recordState)
				{
					if (qis.set.recKey == key) return true;
					if (qis.recording) InsertInput(key, state, msPt);
				}
				else
				{
					QiFn::Trigger(key);
				}
			}
		}
		else
		{
			qis.keyState[key] = false;
			if (qis.recordState)
			{
				// start or stop record
				if (qis.set.recKey == key)
				{
					if (qis.recording) QApplication::postEvent(qis.widget.record, new QEvent((QEvent::Type)QiEvent::recStop));
					else QApplication::postEvent(qis.widget.record, new QEvent((QEvent::Type)QiEvent::recStart));
					return true;
				}
				// record input
				if (qis.recording) InsertInput(key, state, msPt);
			}
			else
			{
				QiFn::Trigger(key);
			}
		}
		// block trigger key
		if (qis.run) for (uint32 i = 0; i < qis.blockKeys.size(); i++) if (qis.blockKeys[i] == key) return true;
	}
	return false;
}