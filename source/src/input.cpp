#pragma execution_character_set("utf-8")
#include "../src/minc.h"
#include "../ui/RecordUi.h"

void RecordInput(BYTE key, bool state, POINT pt)
{
	if (Input::isMouse(key) && InRect(Window::rect((HWND)qis.widget.record->winId()), Input::pos())) return;
	if (qis.recordWindow && !IsWindowVisible(qis.recordWindow))
	{
		QApplication::postEvent(qis.widget.record, new QEvent((QEvent::Type)RecordUi::_close));
		qis.popText->Popup("窗口已失效", RGB(255, 64, 64), 2000);
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
		if (qis.set.recTrack)
		{
			action.d.mouse.track = true;
			action.d.mouse.speed = 20;
		}
	}

	// key
	{
		Action& action = qis.record.AddNull(); action.type = Action::_key;
		action.d.key.vk = key;
		if (state) action.d.key.state = QiKey::down;
		else action.d.key.state = QiKey::up;
	}
}

struct InputStruct
{
	bool press = false;
	unsigned char key = 0;
	unsigned short x = 0;
	unsigned short y = 0;
};

class InputQueue
{
	struct InputStructEx
	{
		bool notNull = false;
		InputStruct inputStruct;
	};

	HANDLE thread;
	size_t max_size;
	size_t push_pointer;
	size_t drop_pointer;
	InputStructEx* queue;

	static DWORD _stdcall InputQueueProcess(void* pInputQueue)
	{
		// for high performance input process
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
		InputQueue* q = (InputQueue*)pInputQueue;
		InputStruct inputStruct;
		while (true)
		{
			if (q->drop(inputStruct))
			{
				if (qis.recordState)
				{
					if (qis.recording)
					{
						if (inputStruct.key == qis.set.recKey)
						{
							if (inputStruct.press) QApplication::postEvent(qis.widget.record, new QEvent((QEvent::Type)RecordUi::_stop));
						}
						else RecordInput(inputStruct.key, inputStruct.press, { inputStruct.x, inputStruct.y });
					}
					else
					{
						if (inputStruct.key == qis.set.recKey)
						{
							if (inputStruct.press) QApplication::postEvent(qis.widget.record, new QEvent((QEvent::Type)RecordUi::_start));
						}
					}
				}
				else
				{
					QiFn::Trigger(inputStruct.key);
				}
			}
			else
			{
				SuspendThread(GetCurrentThread());
			}
		}
	}

public:
	InputQueue(size_t max_size)
	{
		if (max_size < 1) this->max_size = 1;
		else this->max_size = max_size;
		push_pointer = 0;
		drop_pointer = 0;
		queue = new InputStructEx[max_size];
		thread = CreateThread(0, 0, InputQueueProcess, this, 0, 0);
	}
	~InputQueue()
	{
		delete[] queue;
		TerminateThread(thread, 0);
	}

	void push(const InputStruct& inputStruct)
	{
		if (!queue[push_pointer].notNull)
		{
			queue[push_pointer].inputStruct = inputStruct;
			queue[push_pointer].notNull = true;
			push_pointer++;
			if (push_pointer == max_size) push_pointer = 0;
			ResumeThread(thread);
		}
	}
	bool drop(InputStruct& inputStruct)
	{
		if (queue[drop_pointer].notNull)
		{
			inputStruct = queue[drop_pointer].inputStruct;
			queue[drop_pointer].notNull = false;
			drop_pointer++;
			if (drop_pointer == max_size) drop_pointer = 0;
			return true;
		}
		return false;
	}
};

InputQueue inputQueue(12);
bool _stdcall InputHook::InputProc(BYTE key, bool press, POINT cursor, PULONG_PTR param)
{
	// VK_LSHIFT to VK_SHIFT
	key = Input::Convert(key);
	// is self
	if (*param == 214)
	{
		*param = 0;
		return false;
	}
	// other
	else if (key)
	{
		InputStruct inputStruct;

		if (press)
		{
			if (!qis.keyState[key])
			{
				qis.keyState[key] = true;
				inputQueue.push({true, key, (unsigned short)cursor.x, (unsigned short)cursor.y});
			}
		}
		else
		{
			qis.keyState[key] = false;
			inputQueue.push({false, key, (unsigned short)cursor.x, (unsigned short)cursor.y});
		}
		// block trigger key
		if (qis.run) for (uint32 i = 0; i < qis.blockKeys.size(); i++) if (qis.blockKeys[i] == key) return true;
	}
	return false;
}