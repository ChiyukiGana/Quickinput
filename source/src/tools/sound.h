#pragma once
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#define sound_err L"C:/Windows/Media/Windows Foreground.wav"
#define sound_fail L"C:/Windows/Media/Windows Hardware Fail.wav"
#define sound_war L"C:/Windows/Media/Windows Background.wav"
#define sound_war1 L"C:/Windows/Media/Windows Balloon.wav"
#define sound_stop L"C:/Windows/Media/Windows Critical Stop.wav"
#define sound_on L"C:/Windows/Media/Speech On.wav"
#define sound_off L"C:/Windows/Media/Speech Off.wav"
#define sound_sleep L"C:/Windows/Media/Speech Sleep.wav"
#define sound_ok L"C:/Windows/Media/Windows Notify Calendar.wav"
#define sound_ok1 L"C:/Windows/Media/Windows Default.wav"
namespace QiTools {
	class Sound
	{
	public:
		static void WavePlay(std::wstring path, bool sync = false)
		{
			if (sync) PlaySoundW(path.c_str(), NULL, SND_FILENAME | SND_SYNC);
			else PlaySoundW(path.c_str(), NULL, SND_FILENAME | SND_ASYNC);
		}
		static void MediaPlay(std::wstring file, bool sync = true)
		{
			mciSendStringW((std::wstring(L"open \"") + file + std::wstring(L"\" alias audio")).c_str(), 0, 0, 0);
			if (sync) mciSendStringW(L"play audio wait", 0, 0, 0);
			else mciSendStringW(L"play audio", 0, 0, 0);
		}
		static void MediaOpen(std::wstring file) { mciSendStringW((std::wstring(L"open \"") + file + std::wstring(L"\" alias audio")).c_str(), 0, 0, 0); }
		static void MediaPlay(bool sync = true) { if (sync) mciSendStringW(L"play audio wait", 0, 0, 0); else mciSendStringW(L"play audio", 0, 0, 0); }
		static void MediaPause() { mciSendStringW(L"pause audio", 0, 0, 0); }
		static void MediaResume() { mciSendStringW(L"resume audio", 0, 0, 0); }
		static void MediaStop() { mciSendStringW(L"stop audio", 0, 0, 0); }
		static void MediaClose() { mciSendStringW(L"close audio", 0, 0, 0); }
	};
}