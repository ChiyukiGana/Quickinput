#pragma once

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")

#define audfx_err L"C:/Windows/Media/Windows Foreground.wav"
#define audfx_fail L"C:/Windows/Media/Windows Hardware Fail.wav"
#define audfx_war L"C:/Windows/Media/Windows Background.wav"
#define audfx_war1 L"C:/Windows/Media/Windows Balloon.wav"
#define audfx_stop L"C:/Windows/Media/Windows Critical Stop.wav"
#define audfx_on L"C:/Windows/Media/Speech On.wav"
#define audfx_off L"C:/Windows/Media/Speech Off.wav"
#define audfx_sleep L"C:/Windows/Media/Speech Sleep.wav"
#define audfx_ok L"C:/Windows/Media/Windows Notify Calendar.wav"
#define audfx_ok1 L"C:/Windows/Media/Windows Default.wav"

namespace CG {
	class Media
	{
	public:

		static void WavePlay(LPCWSTR path, bool sync = 0) {
			if (sync) PlaySoundW(path, NULL, SND_FILENAME | SND_SYNC);
			else PlaySoundW(path, NULL, SND_FILENAME | SND_ASYNC);
		}

		// on the thread
		static void MediaPlay(LPCWSTR path, bool sync = 0) {
			WCHAR cmd[MAX_PATH];
			swprintf_s(cmd, MAX_PATH, L"open \"%s\" alias audio", path);
			mciSendStringW(cmd, 0, 0, 0);
			if (sync) mciSendStringW(L"play audio wait", 0, 0, 0);
			else mciSendStringW(L"play audio", 0, 0, 0);
		}

		// on the thread
		static void MediaPause() { mciSendStringW(L"pause audio", 0, 0, 0); }

		// on the thread
		static void MediaResume() { mciSendStringW(L"resume audio", 0, 0, 0); }

		// on the thread
		static void MediaStop() { mciSendStringW(L"stop audio", 0, 0, 0); }

		// on the thread
		static void MediaClose() { mciSendStringW(L"close audio", 0, 0, 0); }
	};
}