#pragma once
#ifndef CG_AUDDV_H
#define CG_AUDDV_H

#include <string>
#include <vector>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include "vector.h"
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"oleaut32.lib")
namespace QiTools
{
	interface DECLSPEC_UUID("f8679f50-850a-41cf-9c72-430f290290c8") IPolicyConfig;
	interface DECLSPEC_UUID("870af99c-171d-4f9e-af0d-e63df40c2bc9") IPolicyConfigClient;

	interface IPolicyConfig : public IUnknown
	{
		virtual HRESULT GetMixFormat(
			_In_ PCWSTR,
			_Out_ WAVEFORMATEX**
		) = 0;

		virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(
			_In_ PCWSTR,
			_In_ INT,
			_Out_ WAVEFORMATEX**
		) = 0;

		virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(
			_In_ PCWSTR
		) = 0;

		virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(
			_In_ PCWSTR,
			_In_ WAVEFORMATEX*,
			_In_ WAVEFORMATEX*
		) = 0;

		virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(
			_In_ PCWSTR,
			_In_ INT,
			_Out_ PINT64,
			_Out_ PINT64
		) = 0;

		virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(
			_In_ PCWSTR,
			_In_ PINT64
		) = 0;

		virtual HRESULT STDMETHODCALLTYPE GetShareMode(
			_In_ PCWSTR,
			_Out_ struct DeviceShareMode*
		) = 0;

		virtual HRESULT STDMETHODCALLTYPE SetShareMode(
			_In_ PCWSTR,
			_In_ struct DeviceShareMode*
		) = 0;

		virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(
			_In_ PCWSTR,
			_In_ const PROPERTYKEY&,
			_Out_ PROPVARIANT*
		) = 0;

		virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(
			_In_ PCWSTR,
			_In_ const PROPERTYKEY&,
			_Out_ PROPVARIANT*
		) = 0;

		virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(
			_In_  PCWSTR wszDeviceId,
			_In_  ERole eRole
		) = 0;

		virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(
			_In_ PCWSTR,
			_In_ INT
		) = 0;
	};

	struct AudioDeviceInfo
	{
		enum Type
		{
			OUTPUT = 1,
			INPUT = 2,
		};

		enum State
		{
			ACTIVATED = 1,
			DISABLED = 2,
			DISCONNECTED = 4,
			ALL_STATE = ACTIVATED | DISABLED | DISCONNECTED
		};

		enum Attribute
		{
			NONE = 1,
			DEFAULT = 2,
			COMMUNICATION = 4,
			ALL_ATTRIBUTE = NONE | DEFAULT | COMMUNICATION
		};

		bool ok = false;
		int type = OUTPUT;
		int state = ACTIVATED;
		int attribute = NONE;
		std::wstring id;
		std::wstring name;
	};

	typedef std::vector<AudioDeviceInfo> AudioDeviceInfos;

	class AudioDevice
	{
		IMMDeviceEnumerator* dvEnum = nullptr;
	public:
		AudioDevice()
		{
			CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
			CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&dvEnum);
		}
		operator bool() const { return dvEnum; }
		bool valid() const { return dvEnum; }

		AudioDeviceInfo getDevice(AudioDeviceInfo::Type type, AudioDeviceInfo::Attribute attribute)
		{
			IMMDevice* dv;
			if (!dvEnum || FAILED(dvEnum->GetDefaultAudioEndpoint(type == AudioDeviceInfo::Type::INPUT ? eCapture : eRender, attribute == AudioDeviceInfo::Attribute::COMMUNICATION ? eCommunications : eConsole, &dv))) return {};

			AudioDeviceInfo info;
			LPWSTR str;
			if (SUCCEEDED(dv->GetId(&str))) info.id = str;

			IPropertyStore* prop;
			if (SUCCEEDED(dv->OpenPropertyStore(STGM_READ, &prop)))
			{
				PROPVARIANT name;
				PropVariantInit(&name);
				if (SUCCEEDED(prop->GetValue(PKEY_Device_DeviceDesc, &name))) info.name = name.pwszVal;
				prop->Release();
			}

			info.type = type;

			DWORD state;
			if (SUCCEEDED(dv->GetState(&state)))
			{
				if (state & DEVICE_STATE_ACTIVE) info.state |= AudioDeviceInfo::State::ACTIVATED;
				if (state & DEVICE_STATE_DISABLED) info.state |= AudioDeviceInfo::State::DISABLED;
				if (state & DEVICE_STATE_UNPLUGGED) info.state |= AudioDeviceInfo::State::DISCONNECTED;
			}

			info.attribute = attribute == AudioDeviceInfo::Attribute::COMMUNICATION ? AudioDeviceInfo::Attribute::COMMUNICATION : AudioDeviceInfo::Attribute::DEFAULT;

			info.ok = true;

			dv->Release();
			return info;
		}
		AudioDeviceInfo getInputDefault()
		{
			return getDevice(AudioDeviceInfo::Type::INPUT, AudioDeviceInfo::Attribute::DEFAULT);
		}
		AudioDeviceInfo getInputCommunication()
		{
			return getDevice(AudioDeviceInfo::Type::INPUT, AudioDeviceInfo::Attribute::COMMUNICATION);
		}
		AudioDeviceInfo getOutputDefault()
		{
			return getDevice(AudioDeviceInfo::Type::OUTPUT, AudioDeviceInfo::Attribute::DEFAULT);
		}
		AudioDeviceInfo getOutputCommunication()
		{
			return getDevice(AudioDeviceInfo::Type::OUTPUT, AudioDeviceInfo::Attribute::COMMUNICATION);
		}
		AudioDeviceInfos findDevice(AudioDeviceInfo::Type deviceType = AudioDeviceInfo::Type::OUTPUT, AudioDeviceInfo::State deviceState = AudioDeviceInfo::State::ALL_STATE)
		{
			if (!dvEnum) return {};

			HRESULT hr;
			IMMDeviceCollection* dvColl;

			DWORD state = 0;
			if (deviceState & AudioDeviceInfo::State::ACTIVATED) state |= DEVICE_STATE_ACTIVE;
			if (deviceState & AudioDeviceInfo::State::DISABLED) state |= DEVICE_STATE_DISABLED;
			if (deviceState & AudioDeviceInfo::State::DISCONNECTED) state |= DEVICE_STATE_UNPLUGGED;

			if (deviceType == AudioDeviceInfo::Type::INPUT) hr = dvEnum->EnumAudioEndpoints(eCapture, state, &dvColl);
			else hr = dvEnum->EnumAudioEndpoints(eRender, state, &dvColl);
			if (FAILED(hr)) return {};

			AudioDeviceInfo def, com;
			if (deviceType == AudioDeviceInfo::Type::OUTPUT)
			{
				def = getOutputDefault();
				com = getOutputCommunication();
			}
			else if (deviceType == AudioDeviceInfo::Type::INPUT)
			{
				def = getOutputDefault();
				com = getOutputCommunication();
			}

			UINT count = 0;
			hr = dvColl->GetCount(&count);
			if (FAILED(hr))
			{
				dvColl->Release();
				return {};
			}

			AudioDeviceInfos infos;
			for (UINT i = 0; i < count; i++)
			{
				IMMDevice* dv = 0;
				if (FAILED(dvColl->Item(i, &dv))) continue;

				AudioDeviceInfo info;
				LPWSTR str;
				if (SUCCEEDED(dv->GetId(&str))) info.id = str;

				IPropertyStore* prop;
				if (SUCCEEDED(dv->OpenPropertyStore(STGM_READ, &prop)))
				{
					PROPVARIANT name;
					PropVariantInit(&name);
					if (SUCCEEDED(prop->GetValue(PKEY_Device_DeviceDesc, &name))) info.name = name.pwszVal;
					prop->Release();
				}

				info.type = deviceType;

				DWORD state;
				if (SUCCEEDED(dv->GetState(&state)))
				{
					if (state & DEVICE_STATE_ACTIVE) info.state |= AudioDeviceInfo::State::ACTIVATED;
					if (state & DEVICE_STATE_DISABLED) info.state |= AudioDeviceInfo::State::DISABLED;
					if (state & DEVICE_STATE_UNPLUGGED) info.state |= AudioDeviceInfo::State::DISCONNECTED;
				}

				if (def.ok && def.id == info.id) info.attribute |= AudioDeviceInfo::Attribute::DEFAULT;
				if (com.ok && com.id == info.id) info.attribute |= AudioDeviceInfo::Attribute::COMMUNICATION;

				info.ok = true;
				infos.push_back(std::move(info));
				dv->Release();
			}
			return infos;
		}
		bool setOutputAttributes(const std::wstring& deviceId, AudioDeviceInfo::Attribute attribute)
		{
			IPolicyConfig* pPolicyConfig;
			if (FAILED(CoCreateInstance(__uuidof(IPolicyConfigClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfig), (LPVOID*)&pPolicyConfig))) return false;

			bool def = false, com = false;
			if (attribute & AudioDeviceInfo::Attribute::DEFAULT) def = SUCCEEDED(pPolicyConfig->SetDefaultEndpoint(deviceId.c_str(), eConsole));
			if (attribute & AudioDeviceInfo::Attribute::COMMUNICATION) com = SUCCEEDED(pPolicyConfig->SetDefaultEndpoint(deviceId.c_str(), eCommunications));
			pPolicyConfig->Release();

			return def && com;
		}
		bool setVolume(const std::wstring& deviceId, float volume)
		{
			if (!dvEnum) return false;
			IMMDevice* dv;
			if (FAILED(dvEnum->GetDevice(deviceId.c_str(), &dv))) return false;
			IAudioEndpointVolume* vol;
			if (FAILED(dv->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&vol)))
			{
				dv->Release();
				return false;
			}
			dv->Release();
			if (FAILED(vol->SetMasterVolumeLevelScalar(std::clamp(volume, 0.0f, 1.0f), NULL)))
			{
				vol->Release();
				return false;
			}
			vol->Release();
			return true;
		}
		bool getVolume(const std::wstring& deviceId, float& volume)
		{
			if (!dvEnum) return false;
			IMMDevice* dv;
			if (FAILED(dvEnum->GetDevice(deviceId.c_str(), &dv))) return false;
			IAudioEndpointVolume* vol;
			if (FAILED(dv->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&vol)))
			{
				dv->Release();
				return false;
			}
			dv->Release();
			if (FAILED(vol->GetMasterVolumeLevelScalar(&volume)))
			{
				vol->Release();
				return false;
			}
			vol->Release();
			return true;
		}
		bool getPeakVolume(const std::wstring& deviceId, clock_t time, float& volume)
		{
			if (!dvEnum) return false;
			IMMDevice* dv;
			if (FAILED(dvEnum->GetDevice(deviceId.c_str(), &dv))) return false;
			IAudioMeterInformation* vol;
			if (FAILED(dv->Activate(__uuidof(IAudioMeterInformation), CLSCTX_ALL, NULL, (void**)&vol)))
			{
				dv->Release();
				return false;
			}
			dv->Release();

			float r = 0.0f;
			const clock_t end = clock() + time;
			do {
				float v;
				if (FAILED(vol->GetPeakValue(&v)))
				{
					vol->Release();
					return false;
				}
				if (v > r) r = v;
				Sleep(1);
			} while (end > clock());
			vol->Release();
			volume = r;
			return true;
		}
		bool getAverageVolume(const std::wstring& deviceId, clock_t time, float& volume)
		{
			if (!dvEnum) return false;
			IMMDevice* dv;
			if (FAILED(dvEnum->GetDevice(deviceId.c_str(), &dv))) return false;
			IAudioMeterInformation* vol;
			if (FAILED(dv->Activate(__uuidof(IAudioMeterInformation), CLSCTX_ALL, NULL, (void**)&vol)))
			{
				dv->Release();
				return false;
			}
			dv->Release();

			float r = 0.0f;
			size_t count = 0;
			const clock_t end = clock() + time;
			do {
				float v;
				if (FAILED(vol->GetPeakValue(&v)))
				{
					vol->Release();
					return false;
				}
				r += v;
				count++;
				Sleep(1);
			} while (end > clock());
			vol->Release();
			volume = r / static_cast<float>(count);
			return true;
		}

		static AudioDeviceInfo GetDevice(AudioDeviceInfo::Type type, AudioDeviceInfo::Attribute attribute) { return AudioDevice().getDevice(type, attribute); }
		static AudioDeviceInfo GetInputDefault() { return AudioDevice().getInputDefault(); }
		static AudioDeviceInfo GetInputCommunication() { return AudioDevice().getInputCommunication(); }
		static AudioDeviceInfo GetOutputDefault() { return AudioDevice().getOutputDefault(); }
		static AudioDeviceInfo GetOutputCommunication() { return AudioDevice().getOutputCommunication(); }
		static AudioDeviceInfos FindDevice(AudioDeviceInfo::Type deviceType = AudioDeviceInfo::Type::OUTPUT, AudioDeviceInfo::State deviceState = AudioDeviceInfo::State::ALL_STATE) { return AudioDevice().findDevice(deviceType, deviceState); }
		static bool SetOutputAttributes(const std::wstring& deviceId, AudioDeviceInfo::Attribute attribute) { return AudioDevice().setOutputAttributes(deviceId, attribute); }
		static bool SetVolume(const std::wstring& deviceId, float volume) { return AudioDevice().setVolume(deviceId, volume); }
		static bool GetVolume(const std::wstring& deviceId, float& volume) { return AudioDevice().getVolume(deviceId, volume); }
		static bool GetPeakVolume(const std::wstring& deviceId, clock_t time, float& volume) { return AudioDevice().getPeakVolume(deviceId, time, volume); }
		static bool GetAverageVolume(const std::wstring& deviceId, clock_t time, float& volume) { return AudioDevice().getAverageVolume(deviceId, time, volume); }
	};
}
#endif