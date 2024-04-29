#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <mmdeviceapi.h>
#include "msgbox.h"
namespace CG
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
		enum States
		{
			_disabled = 0x00,
			_activated = 0x01,
			_disconnected = 0x02
		};
		enum Attributes
		{
			_none = 0x00,
			_default = 0x01,
			_communications = 0x02
		};

		int state;
		int attribute;
		std::wstring id;
	};

	typedef List<AudioDeviceInfo> AudioDeviceInfos;

	class AudioDevice
	{
	public:
		typedef std::vector<std::wstring> Ids;
		enum {
			_none = 0x00
		};
		enum Types {
			_output = 0x01,
			_input = 0x02
		};
		enum States
		{
			_disabled = 0x01,
			_activated = 0x02,
			_disconnected = 0x04,
			_all = _activated | _disabled | _disconnected,
		};
		enum Attributes
		{
			_default = 0x01,
			_communications = 0x02,
		};

		static bool EnumDevices(AudioDeviceInfos& devicesInfo, int deviceType = Types::_output, int deviceState = States::_all)
		{
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

			IMMDeviceEnumerator* dvEnum = 0;
			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&dvEnum);
			if (FAILED(hr)) return false;

			std::wstring defaultDeviceId, communicationsDeviceId;
			{
				EDataFlow type = eRender;
				if (deviceType == Types::_input) type = eCapture;
				else if (deviceType == Types::_output) type = eRender;

				IMMDevice* dvDef;
				if (SUCCEEDED(dvEnum->GetDefaultAudioEndpoint(type, eConsole, &dvDef)))
				{
					LPWSTR str;
					dvDef->GetId(&str);
					if (str) defaultDeviceId = str;
				}

				IMMDevice* dvCmm;
				if (SUCCEEDED(dvEnum->GetDefaultAudioEndpoint(type, eCommunications, &dvCmm)))
				{
					LPWSTR str;
					dvCmm->GetId(&str);
					if (str) communicationsDeviceId = str;
				}
			}

			IMMDeviceCollection* dvColl = 0;
			{
				DWORD state = 0;
				if (deviceState & States::_activated) state |= DEVICE_STATE_ACTIVE;
				if (deviceState & States::_disabled) state |= DEVICE_STATE_DISABLED;
				if (deviceState & States::_disconnected) state |= DEVICE_STATE_UNPLUGGED;

				if (deviceType == Types::_input)        hr = dvEnum->EnumAudioEndpoints(eCapture, state, &dvColl);
				else if (deviceType == Types::_output)  hr = dvEnum->EnumAudioEndpoints(eRender, state, &dvColl);
				dvEnum->Release();
				if (FAILED(hr)) return false;
			}

			UINT count = 0;
			{
				hr = dvColl->GetCount(&count);
				if (FAILED(hr))
				{
					dvColl->Release();
					return false;
				}
			}

			devicesInfo.resize(0);
			{
				for (UINT u = 0; u < count; u++)
				{
					IMMDevice* dv = 0;
					if (FAILED(dvColl->Item(u, &dv))) continue;

					AudioDeviceInfo dvInfo;
					{
						LPWSTR str;
						dv->GetId(&str);
						if (str) dvInfo.id = str;
					}
					{
						DWORD state;
						dv->GetState(&state);
						dvInfo.state = 0;
						if (state & DEVICE_STATE_ACTIVE) dvInfo.state |= AudioDeviceInfo::States::_activated;
						if (state & DEVICE_STATE_DISABLED) dvInfo.state |= AudioDeviceInfo::States::_disabled;
						if (state & DEVICE_STATE_UNPLUGGED) dvInfo.state |= AudioDeviceInfo::States::_disconnected;
					}
					{
						if (dvInfo.id.size())
						{
							dvInfo.attribute = 0;
							if (defaultDeviceId == dvInfo.id)
							{
								dvInfo.attribute |= AudioDeviceInfo::Attributes::_default;
							}
							if (communicationsDeviceId == dvInfo.id)
							{
								dvInfo.attribute |= AudioDeviceInfo::Attributes::_communications;
							}
						}
					}
					devicesInfo.Add(dvInfo);
					dv->Release();
				}
			}

			dvColl->Release();
			return true;
		}

		static UINT GetCount(int deviceType = Types::_output, int deviceState = States::_all)
		{
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

			IMMDeviceEnumerator* dvEnum = 0;
			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&dvEnum);
			if (FAILED(hr)) return 0;

			DWORD state = 0;
			if (deviceState & States::_activated) state |= DEVICE_STATE_ACTIVE;
			if (deviceState & States::_disabled) state |= DEVICE_STATE_DISABLED;
			if (deviceState & States::_disconnected) state |= DEVICE_STATE_UNPLUGGED;

			IMMDeviceCollection* dvColl = 0;
			if (deviceType == Types::_input)        hr = dvEnum->EnumAudioEndpoints(eCapture, state, &dvColl);
			else if (deviceType == Types::_output)  hr = dvEnum->EnumAudioEndpoints(eRender, state, &dvColl);
			dvEnum->Release();
			if (FAILED(hr)) return 0;

			UINT count = 0;
			hr = dvColl->GetCount(&count);
			if (FAILED(hr))
			{
				dvColl->Release();
				return _none;
			}

			dvColl->Release();
			return count;
		}

		static int GetState(PCWSTR deviceId)
		{
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

			IMMDeviceEnumerator* dvEnum = 0;
			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&dvEnum);
			if (FAILED(hr)) return _none;

			IMMDevice* dv = 0;
			hr = dvEnum->GetDevice(deviceId, &dv);
			dvEnum->Release();
			if (FAILED(hr)) return _none;

			DWORD state;
			hr = dv->GetState(&state);
			dv->Release();
			if (FAILED(hr)) return _none;

			int rState = _none;

			if (state & DEVICE_STATE_ACTIVE) rState |= States::_activated;
			if (state & DEVICE_STATE_DISABLED) rState |= States::_disabled;
			if (state & DEVICE_STATE_UNPLUGGED) rState |= States::_disconnected;

			return rState;
		}

		static std::wstring GetByAttribute(int deviceType = Types::_output, int deviceAttribute = Attributes::_default)
		{
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

			IMMDeviceEnumerator* dvEnum = 0;
			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (LPVOID*)&dvEnum);
			if (FAILED(hr)) return 0;

			EDataFlow type = eRender;
			if (deviceType == Types::_input) type = eCapture;
			else if (deviceType == Types::_output) type = eRender;

			ERole attribute = eConsole;
			if (deviceAttribute == Attributes::_default) attribute = eConsole;
			if (deviceAttribute == Attributes::_communications) attribute = eCommunications;

			IMMDevice* dv = 0;
			hr = dvEnum->GetDefaultAudioEndpoint(type, attribute, &dv);
			dvEnum->Release();
			if (FAILED(hr)) return L"";

			LPWSTR dvId;
			hr = dv->GetId(&dvId);
			dv->Release();
			if (FAILED(hr)) return L"";

			return dvId;
		}

		static bool SetAttribute(PCWSTR deviceId, int deviceAttribute = Attributes::_default)
		{
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

			IPolicyConfig* pPolicyConfig = 0;
			hr = CoCreateInstance(__uuidof(IPolicyConfigClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfig), (LPVOID*)&pPolicyConfig);
			if (FAILED(hr)) return false;

			if (deviceAttribute == Attributes::_default)              hr = pPolicyConfig->SetDefaultEndpoint(deviceId, eConsole);
			else if (deviceAttribute == Attributes::_communications)  hr = pPolicyConfig->SetDefaultEndpoint(deviceId, eCommunications);
			pPolicyConfig->Release();
			if (FAILED(hr)) return false;

			return true;
		}
	};
}