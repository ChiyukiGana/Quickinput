#include <src/inc_header.h>
#include "../ui/RecordUi.h"
void RecordInput(BYTE vk, bool state, POINT pt)
{
	if (Input::isMouse(vk) && InRect(Window::rect((HWND)Qi::widget.record->winId()), Input::pos())) return;
	if (Qi::recordWindow && !IsWindowVisible(Qi::recordWindow))
	{
		QApplication::postEvent(Qi::widget.record, new QEvent((QEvent::Type)RecordUi::_close));
		Qi::popText->Popup(2000, "窗口已失效", RGB(255, 64, 64));
	}
	// delay
	if (Qi::recordClock)
	{
		QiDelay delay;
		clock_t nowClock = clock();
		delay.max = delay.min = nowClock - Qi::recordClock;
		Qi::recordClock = nowClock;
		Qi::record.append(std::move(delay));
	}
	else
	{
		Qi::recordClock = clock();
	}
	// mouse
	if (Input::isMouse(vk))
	{
		QiMouse mouse;
		POINT position;
		if (Qi::recordWindow)
		{
			POINT wpt = Window::pos(Qi::recordWindow);
			position = QiFn::WRTA({ pt.x - wpt.x, pt.y - wpt.y }, Qi::recordWindow);
		}
		else
		{
			position = QiFn::RTA(pt);
		}
		mouse.x = position.x;
		mouse.y = position.y;
		if (Qi::set.recTrack)
		{
			mouse.track = true;
			mouse.speed = 20;
		}
		Qi::record.append(std::move(mouse));
	}
	// key
	{
		QiKey key;
		key.vk = vk;
		if (state) key.state = QiKey::down;
		else key.state = QiKey::up;
		Qi::record.append(std::move(key));
	}
}
struct KeyState
{
	bool state[keySize];
	KeyState(bool* keyState)
	{
		memcpy(state, Qi::keyState, keySize);
	}
};
void InputTask(BYTE key, bool press, POINT cursor, KeyState keyState)
{
	if (Qi::recordState)
	{
		if (Qi::recording)
		{
			if (key == Qi::set.recKey)
			{
				if (press) QApplication::postEvent(Qi::widget.record, new QEvent((QEvent::Type)RecordUi::_stop));
			}
			else RecordInput(key, press, cursor);
		}
		else
		{
			if (key == Qi::set.recKey)
			{
				if (press) QApplication::postEvent(Qi::widget.record, new QEvent((QEvent::Type)RecordUi::_start));
			}
		}
	}
	else
	{
		QiFn::Trigger(key, keyState.state);
	}
}
ThreadQueue inputQueue;
bool _stdcall InputHook::InputProc(BYTE key, bool press, POINT cursor, PULONG_PTR param)
{
	// VK_LSHIFT to VK_SHIFT
	key = Input::Convert(key);
	// is self
	if (*param == key_info)
	{
		*param = 0;
		return false;
	}
	// key
	else if (key)
	{
		if (press)
		{
			if (!Qi::keyState[key])
			{
				Qi::keyState[key] = press;
				inputQueue.enqueue(InputTask, key, press, cursor, KeyState(Qi::keyState));
			}
		}
		else
		{
			Qi::keyState[key] = press;
			inputQueue.enqueue(InputTask, key, press, cursor, KeyState(Qi::keyState));
		}
		// block trigger key
		if (Qi::run) for (int i = 0; i < Qi::blockKeys.size(); i++) if (Qi::blockKeys[i] == key) return true;
	}
	// cursor
	else
	{
		if (Qi::run && Qi::blockCur > 0) return true;
	}
	return false;
}