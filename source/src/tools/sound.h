#pragma once
#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <avrt.h>
#include <cmath>
#pragma comment(lib, "Winmm.lib")

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
		static void SoundPlay(std::wstring file, bool sync = true)
		{
			mciSendStringW((std::wstring(L"open \"") + file + std::wstring(L"\" alias audio")).c_str(), 0, 0, 0);
			if (sync) mciSendStringW(L"play audio wait", 0, 0, 0);
			else mciSendStringW(L"play audio", 0, 0, 0);
		}
		static void SoundOpen(std::wstring file) { mciSendStringW((std::wstring(L"open \"") + file + std::wstring(L"\" alias audio")).c_str(), 0, 0, 0); }
		static void SoundPlay(bool sync = true) { if (sync) mciSendStringW(L"play audio wait", 0, 0, 0); else mciSendStringW(L"play audio", 0, 0, 0); }
		static void SoundPause() { mciSendStringW(L"pause audio", 0, 0, 0); }
		static void SoundResume() { mciSendStringW(L"resume audio", 0, 0, 0); }
		static void SoundStop() { mciSendStringW(L"stop audio", 0, 0, 0); }
		static void SoundClose() { mciSendStringW(L"close audio", 0, 0, 0); }

		static float SpeakerVolume(DWORD time, bool avg_max = false) {
			HRESULT hr;
			IMMDeviceEnumerator* pEnumerator = nullptr;
			IMMDevice* pDevice = nullptr;
			IAudioClient* pAudioClient = nullptr;
			IAudioCaptureClient* pCaptureClient = nullptr;
			WAVEFORMATEX* pWaveFormat = nullptr;

			hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
			if (FAILED(hr)) return 0.0f;

			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
				CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
				reinterpret_cast<void**>(&pEnumerator));
			if (FAILED(hr)) goto done;

			hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
			if (FAILED(hr)) goto done;

			hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL,
				nullptr, reinterpret_cast<void**>(&pAudioClient));
			if (FAILED(hr)) goto done;

			hr = pAudioClient->GetMixFormat(&pWaveFormat);
			if (FAILED(hr)) goto done;

			bool isFloatFormat = false;
			if (pWaveFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
				WAVEFORMATEXTENSIBLE* wfex = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pWaveFormat);
				if (wfex->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
					isFloatFormat = true;
				}
			}
			else if (pWaveFormat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
				isFloatFormat = true;
			}
			if (!isFloatFormat) goto done;

			hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
				AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, pWaveFormat, nullptr);
			if (FAILED(hr)) goto done;

			hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient),
				reinterpret_cast<void**>(&pCaptureClient));
			if (FAILED(hr)) goto done;

			hr = pAudioClient->Start();
			if (FAILED(hr)) goto done;

			LARGE_INTEGER freq, start;
			QueryPerformanceFrequency(&freq);
			QueryPerformanceCounter(&start);

			float totalMax = 0.0f;
			float totalSum = 0.0f;
			UINT32 totalSamples = 0;

			do {
				UINT32 packetSize;
				hr = pCaptureClient->GetNextPacketSize(&packetSize);
				if (FAILED(hr)) break;

				if (packetSize == 0) {
					LARGE_INTEGER now;
					QueryPerformanceCounter(&now);
					double elapsedMs = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
					if (elapsedMs >= double(time)) break;
					Sleep(1);
					continue;
				}

				BYTE* pData;
				UINT32 numFrames;
				DWORD flags;
				hr = pCaptureClient->GetBuffer(&pData, &numFrames, &flags, nullptr, nullptr);
				if (FAILED(hr)) break;

				if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
					pData = nullptr;
				}

				UINT32 numChannels = pWaveFormat->nChannels;
				UINT32 numSamples = numFrames * numChannels;
				if (pData != nullptr) {
					float* samples = reinterpret_cast<float*>(pData);
					for (UINT32 i = 0; i < numSamples; ++i) {
						float sample = fabsf(samples[i]);
						totalSum += sample;
						if (sample > totalMax) totalMax = sample;
					}
				}
				totalSamples += numSamples;

				pCaptureClient->ReleaseBuffer(numFrames);
			} while (true);

			float volume = 0.0f;
			if (totalSamples > 0) {
				avg_max ? volume = totalMax : volume = totalSum / totalSamples;
			}

		done:
			if (pCaptureClient) pCaptureClient->Release();
			if (pAudioClient) {
				pAudioClient->Stop();
				pAudioClient->Release();
			}
			if (pWaveFormat) CoTaskMemFree(pWaveFormat);
			if (pDevice) pDevice->Release();
			if (pEnumerator) pEnumerator->Release();
			CoUninitialize();

			return volume;
		}
	};
}