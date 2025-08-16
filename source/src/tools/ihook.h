#pragma once
#include <windows.h>
#include <atomic>
#include <mutex>
#include <array>

#define VK_WHEELUP 0x0A
#define VK_WHEELDOWN 0x0B

namespace QiTools
{
	class InputHook
	{
		inline static std::mutex s_mutex;
		inline static HANDLE s_thread = nullptr;
		inline static HHOOK s_mouseHook = nullptr;
		inline static HHOOK s_keybdHook = nullptr;
		inline static std::atomic<bool> s_createFlag{ false };
		inline static std::atomic<bool> s_mouseState{ false };
		inline static std::atomic<bool> s_keybdState{ false };
		inline static std::atomic<bool> s_blockRep{ false };
		inline static std::array<bool, 256> s_keys{};
		inline static DWORD s_threadId = 0;

		static bool __stdcall InputProc(BYTE key, bool press, POINT cursor, PULONG_PTR param);

		static LRESULT __stdcall MouseHook(int code, WPARAM msg, LPARAM param)
		{
			if (code < 0 || !s_mouseState)
				return CallNextHookEx(s_mouseHook, code, msg, param);

			auto* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(param);
			bool block = false;

			switch (msg)
			{
			case WM_MOUSEMOVE:
				if (InputProc(0, false, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_LBUTTONDOWN:
				if (InputProc(VK_LBUTTON, true, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_LBUTTONUP:
				if (InputProc(VK_LBUTTON, false, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_RBUTTONDOWN:
				if (InputProc(VK_RBUTTON, true, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_RBUTTONUP:
				if (InputProc(VK_RBUTTON, false, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_MBUTTONDOWN:
				if (InputProc(VK_MBUTTON, true, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_MBUTTONUP:
				if (InputProc(VK_MBUTTON, false, ms->pt, &ms->dwExtraInfo)) return 1;
				break;
			case WM_XBUTTONDOWN:
				if ((ms->mouseData >> 16) == XBUTTON1)
				{
					if (InputProc(VK_XBUTTON1, true, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				else if ((ms->mouseData >> 16) == XBUTTON2)
				{
					if (InputProc(VK_XBUTTON2, true, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				break;
			case WM_XBUTTONUP:
				if ((ms->mouseData >> 16) == XBUTTON1)
				{
					if (InputProc(VK_XBUTTON1, false, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				else if ((ms->mouseData >> 16) == XBUTTON2)
				{
					if (InputProc(VK_XBUTTON2, false, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				break;
			case WM_MOUSEWHEEL:
				if ((short)(ms->mouseData >> 16) >= (short)0x78)
				{
					if (InputProc(VK_WHEELUP, true, ms->pt, &ms->dwExtraInfo)) return 1;
					if (InputProc(VK_WHEELUP, false, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				else if ((short)(ms->mouseData >> 16) <= (short)(0xFF88))
				{
					if (InputProc(VK_WHEELDOWN, true, ms->pt, &ms->dwExtraInfo)) return 1;
					if (InputProc(VK_WHEELDOWN, false, ms->pt, &ms->dwExtraInfo)) return 1;
				}
				break;
			}

			return block ? 1 : CallNextHookEx(s_mouseHook, code, msg, param);
		}

		static LRESULT __stdcall KeybdHook(int code, WPARAM msg, LPARAM param)
		{
			if (code < 0 || !s_keybdState)
				return CallNextHookEx(s_keybdHook, code, msg, param);

			auto* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(param);
			const bool isDown = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
			const bool isUp = (msg == WM_KEYUP || msg == WM_SYSKEYUP);

			if (isDown || isUp)
			{
				BYTE vk = static_cast<BYTE>(kb->vkCode);

				if (kb->flags & 1)
				{
					switch (vk)
					{
					case VK_RETURN: vk = VK_SEPARATOR; break;
					}
				}
				else
				{
					switch (vk)
					{
					case VK_CLEAR: vk = VK_NUMPAD5; break;
					case VK_PRIOR: vk = VK_NUMPAD9; break;
					case VK_NEXT: vk = VK_NUMPAD3; break;
					case VK_END: vk = VK_NUMPAD1; break;
					case VK_HOME: vk = VK_NUMPAD7; break;
					case VK_INSERT: vk = VK_NUMPAD0; break;
					case VK_DELETE: vk = VK_DECIMAL; break;
					case VK_LEFT: vk = VK_NUMPAD4; break;
					case VK_UP: vk = VK_NUMPAD8; break;
					case VK_RIGHT: vk = VK_NUMPAD6; break;
					case VK_DOWN: vk = VK_NUMPAD2; break;
					}
				}

				if (vk >= s_keys.size()) return CallNextHookEx(s_keybdHook, code, msg, param);

				bool block = false;

				if (isDown)
				{
					if (!s_blockRep || !s_keys[vk]) block = InputProc(vk, true, {}, &kb->dwExtraInfo);
					if (s_blockRep) s_keys[vk] = true;
				}
				else
				{
					block = InputProc(vk, false, {}, &kb->dwExtraInfo);
					if (s_blockRep) s_keys[vk] = false;
				}

				return block ? 1 : CallNextHookEx(s_keybdHook, code, msg, param);
			}
			return CallNextHookEx(s_keybdHook, code, msg, param);
		}

		static DWORD __stdcall HookThread(LPVOID)
		{
			s_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHook, GetModuleHandleW(nullptr), 0);
			s_keybdHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeybdHook, GetModuleHandleW(nullptr), 0);
			s_createFlag.store(true, std::memory_order_release);

			MSG msg;
			while (GetMessageW(&msg, nullptr, 0, 0))
			{
				if (msg.message == WM_QUIT)
					break;

				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}

			if (s_mouseHook)
			{
				UnhookWindowsHookEx(s_mouseHook);
				s_mouseHook = nullptr;
			}
			if (s_keybdHook)
			{
				UnhookWindowsHookEx(s_keybdHook);
				s_keybdHook = nullptr;
			}

			return 0;
		}

	public:
		typedef unsigned __int8 Type;
		enum Types { none = 0, mouse = 1, keybd = 2, all = mouse | keybd };

		static void BlockRepeat(bool blockRepeat = true)
		{
			s_blockRep.store(blockRepeat, std::memory_order_relaxed);
		}

		static bool IsRunning()
		{
			std::lock_guard lock(s_mutex);
			return s_thread != nullptr;
		}

		static bool State()
		{
			return s_mouseState || s_keybdState;
		}

		static bool Start(Type flags = Types::all, bool blockRepeat = false)
		{
			std::lock_guard lock(s_mutex);

			if (!s_thread)
			{
				s_thread = CreateThread(nullptr, 0, HookThread, nullptr, 0, &s_threadId);
				if (!s_thread)
					return false;

				while (!s_createFlag.load(std::memory_order_acquire))
					Sleep(1);

				if (!s_mouseHook || !s_keybdHook)
				{
					Close();
					return false;
				}
			}

			BlockRepeat(blockRepeat);
			s_keys.fill(false);
			s_mouseState.store(flags & mouse, std::memory_order_relaxed);
			s_keybdState.store(flags & keybd, std::memory_order_relaxed);
			return true;
		}

		static void Stop(Type flags = Types::all)
		{
			if (flags & mouse) s_mouseState.store(false, std::memory_order_relaxed);
			if (flags & keybd) s_keybdState.store(false, std::memory_order_relaxed);
			s_keys.fill(false);
		}

		static void Close()
		{
			std::lock_guard lock(s_mutex);

			if (s_thread)
			{
				PostThreadMessageW(s_threadId, WM_QUIT, 0, 0);
				WaitForSingleObject(s_thread, 1000);
				CloseHandle(s_thread);
				s_thread = nullptr;
			}

			s_createFlag.store(false, std::memory_order_relaxed);
			s_mouseState.store(false, std::memory_order_relaxed);
			s_keybdState.store(false, std::memory_order_relaxed);
			s_blockRep.store(false, std::memory_order_relaxed);
			s_keys.fill(false);
			s_threadId = 0;
		}

		static void Wait()
		{
			std::lock_guard lock(s_mutex);
			if (s_thread)
				WaitForSingleObject(s_thread, INFINITE);
		}
	};
}