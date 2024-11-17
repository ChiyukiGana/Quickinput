#pragma once;

#include <dsound.h>
#include "media.h"
#include "msgbox.h"
#pragma comment(lib,"dsound.lib")

namespace CG
{
	class DxSound
	{
		struct ASyncThreadParam
		{
			IDirectSoundBuffer* buffer;
			HANDLE notify;
		};

		IDirectSound* ds = nullptr;
		bool success = false;

	public:
		class Handle
		{
			IDirectSoundBuffer* _buffer;
			HANDLE _state;
			size_t _size;
		public:

			Handle() noexcept : _buffer(nullptr), _state(nullptr), _size(0) {}
			Handle(IDirectSoundBuffer* pBuffer, HANDLE stateHandle, size_t bufferSize) noexcept : _buffer(pBuffer), _state(stateHandle), _size(bufferSize) {}
			Handle(const Handle&) = delete;
			Handle(Handle&& handle) noexcept
			{
				_buffer = handle._buffer;
				_state = handle._state;
				_size = handle._size;
				handle._buffer = nullptr;
				handle._state = nullptr;
				handle._size = 0;
			}
			void operator=(const Handle&) = delete;
			void operator=(Handle&& handle) noexcept
			{
				_buffer = handle._buffer;
				_state = handle._state;
				_size = handle._size;
				handle._buffer = nullptr;
				handle._state = nullptr;
				handle._size = 0;
			}
			~Handle()
			{
				if (_buffer) _buffer->Release();
				if (_state) CloseHandle(_state);
				_buffer = nullptr;
				_state = nullptr;
				_size = 0;
			}

			// volume -10000 ~ 0
			void setVolume(LONG volume)
			{
				_buffer->SetVolume(volume);
			}

			size_t positonMax()
			{
				return _size - 1;
			}

			size_t position()
			{
				DWORD playPositon;
				DWORD writePositon;
				_buffer->GetCurrentPosition(&playPositon, &writePositon);
				return playPositon;
			}

			void setPosition(size_t position)
			{
				_buffer->SetCurrentPosition(position);
			}

			void pause()
			{
				_buffer->Stop();
			}

			void resume()
			{
				_buffer->Play(0, 0, 0);
			}

			void stop()
			{
				_buffer->Stop();
				setPosition(0);
			}
		};

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

		static DWORD CALLBACK ASyncThread(void* pParam)
		{
			ASyncThreadParam* param = (ASyncThreadParam*)pParam;
			WaitForSingleObject(param->notify, INFINITE);
			CloseHandle(param->notify);
			param->buffer->Release();
			delete param;
			return 0;
		}

		// volume: -10000 ~ 0
		bool Play(const SoundData* sound, LONG volume = 0, bool loop = false, bool async = false, Handle* handle = nullptr)
		{
			if (!success)
			{
				MsgBox::Error(L"Initialize failed");
				return true;
			}

			if (sound->bytes > DSBSIZE_MAX) { MsgBox::Error(L"Failed: file too large"); return true; }

			WAVEFORMATEX fmt = {};
			fmt.cbSize = 0;
			fmt.wFormatTag = WAVE_FORMAT_PCM;
			fmt.nChannels = sound->channels;
			fmt.wBitsPerSample = sound->bits;
			fmt.nSamplesPerSec = sound->samples;
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
			if (FAILED(hr)) { MsgBox::Error(L"Failed: CreateSoundBuffer"); return true; }

			IDirectSoundNotify* notify = nullptr;
			hr = buffer->QueryInterface({ 0xb0210783, 0x89cd, 0x11d0, 0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16 }, (void**)&notify);
			if (FAILED(hr))
			{
				MsgBox::Error(L"Failed: QueryInterface IDirectSoundNotify");
				ds->Release();
				return true;
			}

			hr = notify->SetNotificationPositions(1, &posNotify);
			notify->Release();
			if (FAILED(hr)) {
				MsgBox::Error(L"Failed: SetNotificationPositions");
				ds->Release();
				return true;
			}

			void* data1, * data2;
			DWORD bytes1, bytes2;
			hr = buffer->Lock(0, desc.dwBufferBytes, &data1, &bytes1, &data2, &bytes2, DSBLOCK_ENTIREBUFFER);
			if (FAILED(hr))
			{
				MsgBox::Error(L"Failed: LockSoundBuffer");
				ds->Release();
				return true;
			}

			memcpy_s(data1, bytes1, sound->data, sound->bytes);

			hr = buffer->Unlock(data1, bytes1, data2, bytes2);
			if (FAILED(hr))
			{
				MsgBox::Error(L"Failed: UnLockSoundBuffer");
				ds->Release();
				return true;
			}

			hr = buffer->SetCurrentPosition(0);
			if (FAILED(hr))
			{
				MsgBox::Error(L"Failed: SetCurrentPosition");
				ds->Release();
				return true;
			}

			hr = buffer->SetVolume(volume);
			if (FAILED(hr))
			{
				MsgBox::Error(L"Failed: SetVolume");
				ds->Release();
				return true;
			}

			hr = buffer->Play(0, 0, loop);
			if (FAILED(hr)) {
				MsgBox::Error(L"Failed: Play");
				ds->Release();
				return true;
			}

			if (async)
			{
				if (handle)
				{
					*handle = Handle(buffer, posNotify.hEventNotify, desc.dwBufferBytes);
				}
				else
				{
					ASyncThreadParam* param = new ASyncThreadParam;
					param->buffer = buffer;
					param->notify = posNotify.hEventNotify;
					CreateThread(0, 0, ASyncThread, param, 0, 0);
				}
			}
			else
			{
				WaitForSingleObject(posNotify.hEventNotify, INFINITE);
				buffer->Release();
			}

			return true;
		}
	};
}