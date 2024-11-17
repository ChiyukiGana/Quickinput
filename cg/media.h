#pragma once
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")

#define MINIMP3_IMPLEMENTATION
#include "minimp3/minimp3.h"

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
	struct WavFileStruct
	{
		struct RIFF
		{
			const uint32 id = 0x46464952u; // RIFF
			uint32 fileBytes = 0; // = sizeof(WavFile::RIFF::type) + sizeof(WavFile::FORMAT) + sizeof(WavFile::DATA) + WavFile::DATA::dataBytes
			const uint32 type = 0x45564157u; // WAVE
		};
		struct FORMAT
		{
			const uint32 id = 0x20746D66u; // fmt
			const uint32 blockSize = 0x10;
			const uint16 formatTag = WAVE_FORMAT_PCM;
			uint16 channels = 0;
			uint32 samplesPreSec = 0;
			uint32 bytesPerSec = 0;
			uint16 blockAlign = 0;
			uint16 bitsPerSimple = 0;
		};
		struct DATA
		{
			const uint32 id = 0x61746164u; // data
			uint32 dataBytes = 0;
		};
		RIFF sRiff;
		FORMAT sFormat;
		DATA sData;
		void* pData = nullptr;
	};
	class WavFile : public WavFileStruct
	{
	public:
		WavFile() {}
		~WavFile() { Release(); }
		bool Load(std::wstring path)
		{
			Release();
			HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile)
			{
				size_t size = sizeof(WavFile::RIFF) + sizeof(WavFile::FORMAT) + sizeof(WavFile::DATA);
				DWORD bytesReaded = 0;
				BOOL b = ReadFile(hFile, this, size, &bytesReaded, 0);
				if (b)
				{
					SetFilePointer(hFile, size, 0, FILE_BEGIN);
					char* data = new char[sData.dataBytes];
					b = ReadFile(hFile, data, sData.dataBytes, &bytesReaded, 0);
					if (b) pData = data;
				}
				CloseHandle(hFile);
				if (b) return true;
			}
			return false;
		}
		bool Save(std::wstring path) const
		{
			HANDLE hFile = CreateFileW(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile)
			{
				size_t size = sizeof(WavFile::RIFF) + sizeof(WavFile::FORMAT) + sizeof(WavFile::DATA);
				DWORD bytesWrited = 0;
				BOOL b = WriteFile(hFile, this, size, &bytesWrited, 0);
				if (b)
				{
					SetFilePointer(hFile, size, 0, FILE_BEGIN);
					b = WriteFile(hFile, pData, sData.dataBytes, &bytesWrited, 0);
				}
				CloseHandle(hFile);
				if (b) return true;
			}
			return false;
		}
		void Release() { if (pData) { delete pData; pData = nullptr; } }
	};

	struct SoundDataStruct
	{
		uint32 bits = 0;
		uint32 channels = 0;
		uint32 samples = 0;
		uint32 bytes = 0;
		void* data = nullptr;
	};
	class SoundData : public SoundDataStruct
	{
	public:
		SoundData() {}
		~SoundData() { Release(); }
		bool fromWave(std::wstring file)
		{
			WavFile wavFile;
			if (wavFile.Load(file))
			{
				Release();
				fromWave(std::move(wavFile));
				return true;
			}
			return false;
		}
		void fromWave(const WavFile* wavFile)
		{
			Release();
			bits = wavFile->sFormat.bitsPerSimple;
			channels = wavFile->sFormat.channels;
			samples = wavFile->sFormat.samplesPreSec;
			bytes = wavFile->sData.dataBytes;
			data = new char[bytes];
			memcpy_s(data, bytes, wavFile->pData, wavFile->sData.dataBytes);
		}
		void fromWave(WavFile&& wavFile)
		{
			Release();
			bits = wavFile.sFormat.bitsPerSimple;
			channels = wavFile.sFormat.channels;
			samples = wavFile.sFormat.samplesPreSec;
			bytes = wavFile.sData.dataBytes;
			data = wavFile.pData;
			wavFile.pData = nullptr;
		}
		void toWave(WavFile* wavFile) const
		{
			wavFile->sRiff.fileBytes = sizeof(WavFile::RIFF::type) + sizeof(WavFile::FORMAT) + sizeof(WavFile::DATA) + bytes;
			wavFile->sFormat.channels = channels;
			wavFile->sFormat.samplesPreSec = samples;
			wavFile->sFormat.bytesPerSec = bits * channels * samples;
			wavFile->sFormat.blockAlign = bits * channels >> 3;
			wavFile->sFormat.bitsPerSimple = bits;
			wavFile->sData.dataBytes = bytes;
			wavFile->pData = new char[wavFile->sData.dataBytes];
			memcpy_s(wavFile->pData, wavFile->sData.dataBytes, data, bytes);
		}
		bool fromMp3(std::wstring file)
		{
			Release();

			size_t file_size = File::FileSize(file);
			if (file_size)
			{
				unsigned char* file_data = new unsigned char[file_size];
				if (File::FileRead(file, file_data, file_size))
				{
					mp3dec_t dec;
					mp3dec_init(&dec);

					mp3dec_frame_info_t info;
					mp3d_sample_t frame_data[MINIMP3_MAX_SAMPLES_PER_FRAME];
					mp3dec_decode_frame(&dec, file_data, file_size, frame_data, &info);

					size_t pcm_samples = MINIMP3_MAX_SAMPLES_PER_FRAME;
					size_t current_samples = 0;
					mp3d_sample_t* pcm_data = (mp3d_sample_t*)malloc(sizeof(mp3d_sample_t) * info.channels * pcm_samples);

					unsigned char* file_pointer = file_data;
					while (true)
					{
						size_t samples = mp3dec_decode_frame(&dec, file_pointer, file_size, frame_data, &info);
						if (pcm_samples < (current_samples + samples))
						{
							pcm_samples = pcm_samples << 1;
							mp3d_sample_t* tmp = (mp3d_sample_t*)realloc(pcm_data, sizeof(mp3d_sample_t) * info.channels * pcm_samples);
							if (tmp) pcm_data = tmp;
						}

						memcpy(pcm_data + current_samples * info.channels, frame_data, sizeof(mp3d_sample_t) * info.channels * samples);

						current_samples += samples;
						if (info.frame_bytes <= 0 || file_size <= (info.frame_bytes + 4)) break;
						file_pointer += info.frame_bytes;
						file_size -= info.frame_bytes;
					}
					if (pcm_samples > current_samples)
					{
						mp3d_sample_t* tmp = (mp3d_sample_t*)realloc(pcm_data, sizeof(mp3d_sample_t) * info.channels * current_samples);
						if (tmp) pcm_data = tmp;
					}

					bits = sizeof(mp3d_sample_t) * 8;
					channels = info.channels;
					samples = info.hz;
					bytes = sizeof(mp3d_sample_t) * info.channels * current_samples;
					data = pcm_data;

					delete[] file_data;
					return true;
				}
				delete[] file_data;
			}
			return false;
		}
		void Release() { if (data) { delete data; data = nullptr; } }
	};

	class Media
	{
	public:
		static void WavePlay(LPCWSTR path, bool sync = false) {
			if (sync) PlaySoundW(path, NULL, SND_FILENAME | SND_SYNC);
			else PlaySoundW(path, NULL, SND_FILENAME | SND_ASYNC);
		}

		static void MediaPlay(std::wstring file, bool sync = true) {
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