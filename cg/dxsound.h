#pragma once;

#include <dsound.h>
#include "media.h"
#pragma comment(lib,"dsound.lib")

namespace CG
{
	class DxSound
	{
		IDirectSound* ds = nullptr;
		bool success = false;
	public:
		DxSound()
		{
			HRESULT hr = DirectSoundCreate(0, &ds, 0);
			if (FAILED(hr)) { MsgBox::Error(L"Failed: CreateDirectSoundDevice"); return; }

			hr = ds->SetCooperativeLevel(GetDesktopWindow(), DSSCL_NORMAL);
			if (FAILED(hr)) { MsgBox::Error(L"Failed: SetCooperativeLevel"); return; }

			success = true;
		}
		~DxSound()
		{
			if (ds) ds->Release();
		}

		// volume: -10000 ~ 0
		bool Play(const SoundData* sound, LONG volume = 0)
		{
			if (!success)
			{
				MsgBox::Error(L"Initialize failed");
				return false;
			}

			if (sound->bytes > DSBSIZE_MAX) { MsgBox::Error(L"Failed: file too large"); return false; }

			WAVEFORMATEX fmt = {};
			fmt.cbSize = 0;
			fmt.wFormatTag = WAVE_FORMAT_PCM;
			fmt.nChannels = sound->channels;
			fmt.wBitsPerSample = sound->bits;
			fmt.nSamplesPerSec = sound->simples;
			fmt.nBlockAlign = (fmt.nChannels * fmt.wBitsPerSample) >> 3;
			fmt.nAvgBytesPerSec = fmt.nSamplesPerSec * fmt.nBlockAlign;

			DSBUFFERDESC desc = {};
			desc.dwSize = sizeof(desc);
			desc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY;
			desc.guid3DAlgorithm = DS3DALG_DEFAULT;
			desc.lpwfxFormat = &fmt;
			desc.dwBufferBytes = sound->bytes;

			DSBPOSITIONNOTIFY posNotify = {};
			posNotify.dwOffset = DSBPN_OFFSETSTOP;
			posNotify.hEventNotify = CreateEventW(0, 0, 0, 0);

			IDirectSoundBuffer* buffer = nullptr;
			HRESULT hr = ds->CreateSoundBuffer(&desc, &buffer, 0);
			if (FAILED(hr)) { MsgBox::Error(L"Failed: CreateSoundBuffer"); return false; }

			IDirectSoundNotify* notify = nullptr;
			hr = buffer->QueryInterface({ 0xb0210783, 0x89cd, 0x11d0, 0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16 }, (void**)&notify);
			if (FAILED(hr))
			{
				MsgBox::Error(L"Failed: QueryInterface IDirectSoundNotify");
				ds->Release();
				return false;
			}

			hr = notify->SetNotificationPositions(1, &posNotify);
			notify->Release();
			if (FAILED(hr)) {
				MsgBox::Error(L"Failed: SetNotificationPositions");
				ds->Release();
				return false;
			}

			void* data1, * data2;
			DWORD bytes1, bytes2;
			hr = buffer->Lock(0, desc.dwBufferBytes, &data1, &bytes1, &data2, &bytes2, DSBLOCK_ENTIREBUFFER);
			if (FAILED(hr))
			{
				MsgBox::Error(L"Failed: LockSoundBuffer");
				ds->Release();
				return false;
			}

			memcpy_s(data1, bytes1, sound->data, sound->bytes);

			hr = buffer->Unlock(data1, bytes1, data2, bytes2);
			if (FAILED(hr))
			{
				MsgBox::Error(L"Failed: UnLockSoundBuffer");
				ds->Release();
				return false;
			}

			hr = buffer->SetCurrentPosition(0);
			if (FAILED(hr))
			{
				MsgBox::Error(L"Failed: SetCurrentPosition");
				ds->Release();
				return false;
			}

			hr = buffer->SetVolume(volume);
			if (FAILED(hr))
			{
				MsgBox::Error(L"Failed: SetVolume");
				ds->Release();
				return false;
			}

			hr = buffer->Play(0, 0, 0);
			if (FAILED(hr)) {
				MsgBox::Error(L"Failed: Play");
				ds->Release();
				return false;
			}

			WaitForSingleObject(posNotify.hEventNotify, INFINITE);

			buffer->Release();
			return true;
		}
	};
}