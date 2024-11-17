#include "header.h"

void InsertInput(const BYTE& key, const bool& state, const POINT& pt)
{
	if (Input::Type(key) && InRect(Window::rect((HWND)qis.widget.record->winId()), Input::pos())) return;
	if (qis.recordWindow && !IsWindowVisible(qis.recordWindow))
	{
		QApplication::postEvent(qis.widget.record, new QEvent((QEvent::Type)QiEvent::recClose));
		MsgPop::Popup(L"������ʧЧ", RGB(255, 64, 64), 2000);
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
	if (Input::Type(key))
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
	BYTE key = Input::Convert(vk);
	if (*exInfo == 214) // from Quick input
	{
		*exInfo = 0; // remove ex info
		return false;
	}
	else if (key) // other input
	{
		if (state) // down
		{
			if (!qis.keyState[key]) // set state
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
		else // up
		{
			qis.keyState[key] = false; // reset state
			if (qis.recordState)
			{
				// start or stop record
				if (qis.set.recKey == key)
				{
					if (qis.recording) QApplication::postEvent(qis.widget.record, new QEvent((QEvent::Type)QiEvent::recStop));
					else QApplication::postEvent(qis.widget.record, new QEvent((QEvent::Type)QiEvent::recStart));
					return true;
				}
				if (qis.recording) InsertInput(key, state, msPt);
			}
			else
			{
				QiFn::Trigger(key);
			}
		}
		if (qis.run) for (uint32 i = 0; i < qis.blockKeys.size(); i++) if (qis.blockKeys[i] == key) return true; // block trigger key
	}
	return false;
}