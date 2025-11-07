#include <src/inc_header.h>
#include "../ui/RecordUi.h"
QiMouseTrack mouseTrack;
clock_t mouseTrack_prev = 0;
void RecordInput(BYTE vk, bool state, POINT pt)
{
	if (Input::isMouse(vk) && InRect(Window::rect((HWND)Qi::widget.record->winId()), Input::pos())) return;
	if (Qi::recordWindow && !IsWindowVisible(Qi::recordWindow))
	{
		Qi::widget.recordClose();
		Qi::popText->Popup(2000, "窗口已失效", RGB(255, 64, 64));
	}

	if (vk)
	{
		// delay
		if (Qi::recordClock)
		{
			clock_t nowClock = clock();
			if (mouseTrack.s.empty())
			{
				QiDelay delay;
				delay.max = delay.min = nowClock - Qi::recordClock;
				Qi::record.append(std::move(delay));
			}
			else
			{
				Qi::record.append(std::move(mouseTrack));
				mouseTrack.reset();
			}
			Qi::recordClock = nowClock;
		}
		else
		{
			Qi::recordClock = clock();
			mouseTrack.reset();
		}
		// mouse
		if (Input::isMouse(vk))
		{
			QiMouse mouse;
			POINT position;
			if (Qi::recordWindow)
			{
				POINT wpt = Window::pos(Qi::recordWindow);
				position = QiCvt::WP_RtA({ pt.x - wpt.x, pt.y - wpt.y }, Qi::recordWindow);
			}
			else
			{
				position = QiCvt::SP_RtA(pt);
			}
			mouse.x = position.x;
			mouse.y = position.y;
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
	else if (Qi::set.recTrack && Qi::recordClock)
	{
		if (mouseTrack.s.empty())
		{
			mouseTrack.reset();
			mouseTrack_prev = 0;
		}
		// fps
		if (mouseTrack_prev + 10 < clock())
		{
			mouseTrack_prev = clock();
			if (Qi::recordWindow)
			{
				POINT wpt = Window::pos(Qi::recordWindow);
				mouseTrack.append(QiCvt::WP_RtA({ pt.x - wpt.x, pt.y - wpt.y }, Qi::recordWindow));
			}
			else
			{
				mouseTrack.append(QiCvt::SP_RtA(pt));
			}
		}
	}
}
struct KeyState
{
	bool state[Qi::key_size];
	KeyState(bool* keyState)
	{
		memcpy(state, keyState, Qi::key_size);
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
				if (press) Qi::widget.recordStop();
			}
			else RecordInput(key, press, cursor);
		}
		else
		{
			if (key == Qi::set.recKey)
			{
				if (press) Qi::widget.recordStart();
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
	// is self
	if (*param == Qi::key_info)
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
		// block
		if (Qi::run && Qi::keyBlock[key]) return true;
	}
	// cursor
	else
	{
		if (Qi::recording) inputQueue.enqueue(InputTask, 0, 0, cursor, KeyState(Qi::keyState));
		// block
		if (Qi::run && (Qi::curBlock > 0 || Qi::keyBlock[0])) return true;
	}
	return false;
}