#pragma execution_character_set("utf-8")
#include "../src/minc.h"
#include "../ui/RecordUi.h"

void RecordInput(BYTE vk, bool state, POINT pt)
{
	if (Input::isMouse(vk) && InRect(Window::rect((HWND)Qi::widget.record->winId()), Input::pos())) return;
	if (Qi::recordWindow && !IsWindowVisible(Qi::recordWindow))
	{
		QApplication::postEvent(Qi::widget.record, new QEvent((QEvent::Type)RecordUi::_close));
		Qi::popText->Popup("窗口已失效", RGB(255, 64, 64), 2000);
	}

	// delay
	if (Qi::recordClock)
	{
		QiDelay delay;
		clock_t nowClock = clock();
		delay.max = delay.min = nowClock - Qi::recordClock;
		Qi::recordClock = nowClock;
		Qi::record.Add(std::move(delay));
	}
	else
	{
		Qi::recordClock = clock();
	}

	// pos
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
		Qi::record.Add(std::move(mouse));
	}

	// key
	{
		QiKey key;
		key.vk = vk;
		if (state) key.state = QiKey::down;
		else key.state = QiKey::up;
		Qi::record.Add(std::move(key));
	}
}

struct InputStruct
{
	bool press;
	unsigned char key;
	unsigned short x;
	unsigned short y;
	bool state[Qi::keySize];
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
				if (Qi::recordState)
				{
					if (Qi::recording)
					{
						if (inputStruct.key == Qi::set.recKey)
						{
							if (inputStruct.press) QApplication::postEvent(Qi::widget.record, new QEvent((QEvent::Type)RecordUi::_stop));
						}
						else RecordInput(inputStruct.key, inputStruct.press, { inputStruct.x, inputStruct.y });
					}
					else
					{
						if (inputStruct.key == Qi::set.recKey)
						{
							if (inputStruct.press) QApplication::postEvent(Qi::widget.record, new QEvent((QEvent::Type)RecordUi::_start));
						}
					}
				}
				else
				{
					QiFn::Trigger(inputStruct.key, inputStruct.state);
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
	if (*param == key_info)
	{
		*param = 0;
		return false;
	}
	// other
	else if (key)
	{

		if (press)
		{
			if (!Qi::keyState[key])
			{
				Qi::keyState[key] = true;
				InputStruct is;
				is.key = key,is.press = true,is.x = cursor.x,is.y = cursor.y;
				memcpy(&is.state, Qi::keyState, Qi::keySize);
				inputQueue.push(is);
			}
		}
		else
		{
			Qi::keyState[key] = false;
			InputStruct is;
			is.key = key, is.press = true, is.x = cursor.x, is.y = cursor.y;
			memcpy(&is.state, Qi::keyState, Qi::keySize);
			inputQueue.push(is);
		}
		// block trigger key
		if (Qi::run) for (uint32 i = 0; i < Qi::blockKeys.size(); i++) if (Qi::blockKeys[i] == key) return true;
	}
	return false;
}