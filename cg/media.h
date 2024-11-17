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
	struct WavFile
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
			uint32 simplesPreSec = 0;
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
		void Release() { if (pData) delete pData; }
	};

	struct SoundData
	{
		uint32 bits = 0;
		uint32 channels = 0;
		uint32 simples = 0;
		uint32 bytes = 0;
		void* data = nullptr;

		void fromWave(const WavFile* wavFile)
		{
			Release();
			bits = wavFile->sFormat.bitsPerSimple;
			channels = wavFile->sFormat.channels;
			simples = wavFile->sFormat.simplesPreSec;
			bytes = wavFile->sData.dataBytes;
			data = new char[bytes];
			memcpy_s(data, bytes, wavFile->pData, wavFile->sData.dataBytes);
		}
		void toWave(WavFile* wavFile) const
		{
			wavFile->sRiff.fileBytes = sizeof(WavFile::RIFF::type) + sizeof(WavFile::FORMAT) + sizeof(WavFile::DATA) + bytes;
			wavFile->sFormat.channels = channels;
			wavFile->sFormat.simplesPreSec = simples;
			wavFile->sFormat.bytesPerSec = bits * channels * simples;
			wavFile->sFormat.blockAlign = bits * channels >> 3;
			wavFile->sFormat.bitsPerSimple = bits;
			wavFile->sData.dataBytes = bytes;
			wavFile->pData = new char[wavFile->sData.dataBytes];
			memcpy_s(wavFile->pData, wavFile->sData.dataBytes, data, bytes);
		}
		void Release() { if (data) delete data; }
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
		static void MediaOpen(u16str file) { mciSendStringW((u16str(L"open \"") + file + u16str(L"\" alias audio")).str(), 0, 0, 0); }
		static void MediaPlay(bool sync = true) { if (sync) mciSendStringW(L"play audio wait", 0, 0, 0); else mciSendStringW(L"play audio", 0, 0, 0); }
		static void MediaPause() { mciSendStringW(L"pause audio", 0, 0, 0); }
		static void MediaResume() { mciSendStringW(L"resume audio", 0, 0, 0); }
		static void MediaStop() { mciSendStringW(L"stop audio", 0, 0, 0); }
		static void MediaClose() { mciSendStringW(L"close audio", 0, 0, 0); }
	};
}