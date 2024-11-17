#pragma once
#include <string>
#include <Xinput.h>
#pragma comment(lib,"xinput.lib")
namespace CG {
	class XBoxPad
	{
		static constexpr UINT msg_exit = WM_USER + 1;
		HANDLE thread = nullptr;

	public:
		enum
		{
			start = 0x0100,
			back,
			a,
			b,
			x,
			y,
			up,
			down,
			left,
			right,
			l_joy_press,
			l_joy_up,
			l_joy_down,
			l_joy_left,
			l_joy_right,
			r_joy_press,
			r_joy_up,
			r_joy_down,
			r_joy_left,
			r_joy_right,
			l_top,
			r_top,
			l_bottom,
			r_bottom,
		};

	public:
		typedef void(EventCallback)(short, short);
		static constexpr UINT key_begin = start;
		static constexpr UINT key_end = r_bottom;

		struct State
		{
			bool start;
			bool back;
			bool a;
			bool b;
			bool x;
			bool y;
			bool up;
			bool down;
			bool left;
			bool right;
			bool l_joy_press;
			short l_joy_up;
			short l_joy_down;
			short l_joy_left;
			short l_joy_right;
			bool r_joy_press;
			short r_joy_up;
			short r_joy_down;
			short r_joy_left;
			short r_joy_right;
			bool l_top;
			bool r_top;
			unsigned char l_bottom;
			unsigned char r_bottom;
		};

		static std::wstring name(short bnCode)
		{
			switch (bnCode)
			{
			case a: return L"PadA";
			case b: return L"PadB";
			case x: return L"PadX";
			case y: return L"PadY";
			case up: return L"Pad↑";
			case down: return L"Pad↓";
			case left: return L"Pad←";
			case right: return L"Pad→";
			case l_joy_up: return L"LJoy↑";
			case l_joy_down: return L"LJoy↓";
			case l_joy_left: return L"LJoy←";
			case l_joy_right: return L"LJoy→";
			case r_joy_up: return L"RJoy↑";
			case r_joy_down: return L"RJoy↓";
			case r_joy_left: return L"RJoy←";
			case r_joy_right: return L"RJoy→";
			case l_top: return L"PadLT";
			case l_bottom: return L"PadLB";
			case r_top: return L"PadRT";
			case r_bottom: return L"PadRB";
			}
			return L"None";
		}

		static bool state(State* pPadState)
		{
			memset(pPadState, 0, sizeof(State));
			XINPUT_STATE state = {};
			if (XInputGetState(0, &state) == NO_ERROR)
			{
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) pPadState->start = true;
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) pPadState->back = true;
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) pPadState->a = true;
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) pPadState->b = true;
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) pPadState->x = true;
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) pPadState->y = true;
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) pPadState->up = true;
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) pPadState->down = true;
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) pPadState->left = true;
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) pPadState->right = true;
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) pPadState->l_joy_press = true;
				if (state.Gamepad.sThumbLY > 0) pPadState->l_joy_up = state.Gamepad.sThumbLY;
				if (state.Gamepad.sThumbLY < 0) pPadState->l_joy_down = (~(state.Gamepad.sThumbLY)) + 1;
				if (state.Gamepad.sThumbLX < 0) pPadState->l_joy_left = state.Gamepad.sThumbLX;
				if (state.Gamepad.sThumbLX > 0) pPadState->l_joy_right = (~(state.Gamepad.sThumbLX)) + 1;
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) pPadState->r_joy_press = true;
				if (state.Gamepad.sThumbRY > 0) pPadState->r_joy_up = state.Gamepad.sThumbRY;
				if (state.Gamepad.sThumbRY < 0) pPadState->r_joy_down = (~(state.Gamepad.sThumbRY)) + 1;
				if (state.Gamepad.sThumbRX < 0) pPadState->r_joy_left = state.Gamepad.sThumbRX;
				if (state.Gamepad.sThumbRX > 0) pPadState->r_joy_right = (~(state.Gamepad.sThumbRX)) + 1;
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) pPadState->l_top = true;
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) pPadState->r_top = true;
				pPadState->l_bottom = state.Gamepad.bLeftTrigger;
				pPadState->r_bottom = state.Gamepad.bRightTrigger;
				return true;
			}
			return false;
		}

		static short state(short bnCode)
		{
			XINPUT_STATE state = {};
			if (XInputGetState(0, &state) == NO_ERROR)
			{
				switch (bnCode)
				{
				case start: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_START;
				case back: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK;
				case a: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_A;
				case b: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_B;
				case x: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_X;
				case y: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_Y;
				case up: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
				case down: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
				case left: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
				case right: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
				case l_joy_press: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
				case l_joy_up: return state.Gamepad.sThumbLY > 0 ? state.Gamepad.sThumbLY : 0;
				case l_joy_down: return state.Gamepad.sThumbLY < 0 ? ((~state.Gamepad.sThumbLY) + 1) : 0;
				case l_joy_left: return state.Gamepad.sThumbLX > 0 ? state.Gamepad.sThumbLX : 0;
				case l_joy_right: return state.Gamepad.sThumbLX < 0 ? ((~state.Gamepad.sThumbLX) + 1) : 0;
				case r_joy_press: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
				case r_joy_up: return state.Gamepad.sThumbRY > 0 ? state.Gamepad.sThumbRY : 0;
				case r_joy_down: return state.Gamepad.sThumbRY < 0 ? ((~state.Gamepad.sThumbRY) + 1) : 0;
				case r_joy_left: return state.Gamepad.sThumbRX > 0 ? state.Gamepad.sThumbRX : 0;
				case r_joy_right: return state.Gamepad.sThumbRX < 0 ? ((~state.Gamepad.sThumbRX) + 1) : 0;
				case l_top: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
				case r_top: return (bool)state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
				case l_bottom: return state.Gamepad.bLeftTrigger;
				case r_bottom: return state.Gamepad.bRightTrigger;
				}
			}
			return 0;
		}

		static DWORD _stdcall StateEventThread(void* pParam)
		{
			MSG msg = {};
			EventCallback* callBack = (EventCallback*)pParam;
			State prev = {};
			State next = {};
			XBoxPad::state(&next);
			prev = next;
			while (!PeekMessageW(&msg, 0, msg_exit, msg_exit, PM_REMOVE))
			{
				if (XBoxPad::state(&next))
				{
					if (next.start != prev.start) callBack(start, next.start);
					if (next.back != prev.back) callBack(back, next.back);
					if (next.a != prev.a) callBack(a, next.a);
					if (next.b != prev.b) callBack(b, next.b);
					if (next.x != prev.x) callBack(x, next.x);
					if (next.y != prev.y) callBack(y, next.y);
					if (next.up != prev.up) callBack(up, next.up);
					if (next.down != prev.down) callBack(down, next.down);
					if (next.left != prev.left) callBack(left, next.left);
					if (next.right != prev.right) callBack(right, next.right);
					if (next.l_joy_press != prev.l_joy_press) callBack(l_joy_press, next.l_joy_press);
					if (next.l_joy_up != prev.l_joy_up) callBack(l_joy_up, next.l_joy_up);
					if (next.l_joy_down != prev.l_joy_down) callBack(l_joy_down, next.l_joy_down);
					if (next.l_joy_left != prev.l_joy_left) callBack(l_joy_left, next.l_joy_left);
					if (next.l_joy_right != prev.l_joy_right) callBack(l_joy_right, next.l_joy_right);
					if (next.r_joy_press != prev.r_joy_press) callBack(r_joy_press, next.r_joy_press);
					if (next.r_joy_up != prev.r_joy_up) callBack(r_joy_up, next.r_joy_up);
					if (next.r_joy_down != prev.r_joy_down) callBack(r_joy_down, next.r_joy_down);
					if (next.r_joy_left != prev.r_joy_left) callBack(r_joy_left, next.r_joy_left);
					if (next.r_joy_right != prev.r_joy_right) callBack(r_joy_right, next.r_joy_right);
					if (next.l_top != prev.l_top) callBack(l_top, next.l_top);
					if (next.r_top != prev.r_top) callBack(r_top, next.r_top);
					if (next.l_bottom != prev.l_bottom) callBack(l_bottom, next.l_bottom);
					if (next.r_bottom != prev.r_bottom) callBack(r_bottom, next.r_bottom);
					prev = next;
				}
				Sleep(1);
			}
			return 0;
		}

		static DWORD _stdcall PressEventThread(void* pParam)
		{
			MSG msg = {};
			EventCallback* callBack = (EventCallback*)pParam;
			State prev = {};
			State next = {};
			XBoxPad::state(&next);
			prev = next;
			while (!PeekMessageW(&msg, 0, msg_exit, msg_exit, PM_REMOVE))
			{
				if (XBoxPad::state(&next))
				{
					if ((bool)next.start != ((bool)prev.start)) callBack(start, (bool)next.start);
					if ((bool)next.back != ((bool)prev.back)) callBack(back, (bool)next.back);
					if ((bool)next.a != ((bool)prev.a)) callBack(a, (bool)next.a);
					if ((bool)next.b != (bool)prev.b) callBack(b, (bool)next.b);
					if ((bool)next.x != (bool)prev.x) callBack(x, (bool)next.x);
					if ((bool)next.y != (bool)prev.y) callBack(y, (bool)next.y);
					if ((bool)next.up != (bool)prev.up) callBack(up, (bool)next.up);
					if ((bool)next.down != (bool)prev.down) callBack(down, (bool)next.down);
					if ((bool)next.left != (bool)prev.left) callBack(left, (bool)next.left);
					if ((bool)next.right != (bool)prev.right) callBack(right, (bool)next.right);
					if ((bool)next.l_joy_press != (bool)prev.l_joy_press) callBack(l_joy_press, (bool)next.l_joy_press);
					if ((bool)next.l_joy_up != (bool)prev.l_joy_up) callBack(l_joy_up, (bool)next.l_joy_up);
					if ((bool)next.l_joy_down != (bool)prev.l_joy_down) callBack(l_joy_down, (bool)next.l_joy_down);
					if ((bool)next.l_joy_left != (bool)prev.l_joy_left) callBack(l_joy_left, (bool)next.l_joy_left);
					if ((bool)next.l_joy_right != (bool)prev.l_joy_right) callBack(l_joy_right, (bool)next.l_joy_right);
					if ((bool)next.r_joy_press != (bool)prev.r_joy_press) callBack(r_joy_press, (bool)next.r_joy_press);
					if ((bool)next.r_joy_up != (bool)prev.r_joy_up) callBack(r_joy_up, (bool)next.r_joy_up);
					if ((bool)next.r_joy_down != (bool)prev.r_joy_down) callBack(r_joy_down, (bool)next.r_joy_down);
					if ((bool)next.r_joy_left != (bool)prev.r_joy_left) callBack(r_joy_left, (bool)next.r_joy_left);
					if ((bool)next.r_joy_right != (bool)prev.r_joy_right) callBack(r_joy_right, (bool)next.r_joy_right);
					if ((bool)next.l_top != (bool)prev.l_top) callBack(l_top, (bool)next.l_top);
					if ((bool)next.r_top != (bool)prev.r_top) callBack(r_top, (bool)next.r_top);
					if ((bool)next.l_bottom != (bool)prev.l_bottom) callBack(l_bottom, (bool)next.l_bottom);
					if ((bool)next.r_bottom != (bool)prev.r_bottom) callBack(r_bottom, (bool)next.r_bottom);
					prev = next;
				}
				Sleep(1);
			}
			return 0;
		}

		void setStateEvent(EventCallback callBack, bool onlyPress = true)
		{
			if (!thread)
			{
				if (onlyPress) thread = CreateThread(0, 0, PressEventThread, callBack, 0, 0);
				else thread = CreateThread(0, 0, StateEventThread, callBack, 0, 0);
			}
		}

		void closeStateEvent()
		{
			if (thread)
			{
				PostThreadMessageW(GetThreadId(thread), msg_exit, 0, 0);
				thread = nullptr;
			}
		}
	};
}