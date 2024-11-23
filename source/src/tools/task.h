#pragma once
#include <string>
#include <windows.h>
#include <comdef.h>
#include <wincred.h>
#include <taskschd.h>
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")
#pragma comment(lib, "taskschd.lib")
namespace QiTools
{
	class Task
	{
		struct InitStruct
		{
			ITaskService** pTaskService = 0;
			ITaskFolder** pTaskFolder = 0;
		};

		static bool InitCom(InitStruct& com)
		{
			HRESULT hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
			if (FAILED(hr))
			{
				if (hr == RPC_E_CHANGED_MODE) return false;
			}
			hr = CoInitializeSecurity(0, -1, 0, 0, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, 0, 0, 0);
			if (FAILED(hr))
			{
				if (hr != RPC_E_TOO_LATE) return false;
			}
			hr = CoCreateInstance(CLSID_TaskScheduler, 0, CLSCTX_ALL, IID_ITaskService, (LPVOID*)com.pTaskService);
			if (FAILED(hr)) return false;
			hr = (*com.pTaskService)->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
			if (FAILED(hr)) return false;
			hr = (*com.pTaskService)->GetFolder(_bstr_t(L"\\"), com.pTaskFolder);
			if (FAILED(hr))
			{
				(*com.pTaskService)->Release();
				return false;
			}
			return true;
		}
	public:
		// return false: error, return true: task registered
		static bool Register(std::wstring taskName, bool admini = IsUserAnAdmin(), std::wstring exePath = Process::exePath(), std::wstring runPath = Path::GetDir(Process::exePath()), std::wstring author = L"unknow")
		{
			HRESULT hr = 0;
			ITaskService* pTaskService = 0;
			ITaskFolder* pTaskFolder = 0;
			InitStruct com;
			com.pTaskService = &pTaskService;
			com.pTaskFolder = &pTaskFolder;
			if (!InitCom(com)) return false;
			std::wstring userPath = System::environmentVariable(L"COMPUTERNAME") + L"\\" + System::environmentVariable(L"USERNAME");

			ITaskDefinition* pTask = 0;
			hr = pTaskService->NewTask(0, &pTask);
			if (FAILED(hr))
			{
				pTaskFolder->Release();
				pTaskService->Release();
				return false;
			}
			// Get Task information
			IRegistrationInfo* pRegInfo = 0;
			hr = pTask->get_RegistrationInfo(&pRegInfo);
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			// Set Author name
			hr = pRegInfo->put_Author(_bstr_t(author.c_str()));
			pRegInfo->Release();
			// Get Task principal
			IPrincipal* pPrincipal = 0;
			hr = pTask->get_Principal(&pPrincipal);
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			// Set Principal name
			hr = pPrincipal->put_Id(_bstr_t(L"Principal 1"));
			if (FAILED(hr))
			// Set Logon type to InterActive
			hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				pPrincipal->Release();
				return false;
			}
			// Set Run with high user privileges
			if (admini) hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
			else hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_LUA);
			pPrincipal->Release();
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			// Get Task settings
			ITaskSettings* pSettings = 0;
			hr = pTask->get_Settings(&pSettings);
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			// Uncheck: Power - Start the task only if the computer is on AC power
			hr = pSettings->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				pSettings->Release();
				return false;
			}
			// Check: Run task as soon as possible after a scheduled start is missed
			hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				pSettings->Release();
				return false;
			}
			// Uncheck: Stop the task if it runs loger than
			hr = pSettings->put_ExecutionTimeLimit(_bstr_t(L"PT0S"));
			pSettings->Release();
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			// Get Trigger collection
			ITriggerCollection* pTriggerCollection = 0;
			hr = pTask->get_Triggers(&pTriggerCollection);
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			// Create User logon trigger
			ITrigger* pTrigger = 0;
			hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
			pTriggerCollection->Release();
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			// Create Logon tirgger
			ILogonTrigger* pLogonTrigger = 0;
			hr = pTrigger->QueryInterface(IID_ILogonTrigger, (void**)&pLogonTrigger);
			pTrigger->Release();
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			// Set Trigger name
			hr = pLogonTrigger->put_Id(_bstr_t(L"Logon trigger"));
			if (FAILED(hr))
			// Set Logon user path
			hr = pLogonTrigger->put_UserId(_bstr_t(userPath.c_str()));
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				pLogonTrigger->Release();
				return false;
			}
			// Set Run delay
			hr = pLogonTrigger->put_Delay(_bstr_t(L"PT0S"));
			pLogonTrigger->Release();
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			// Get Action collection
			IActionCollection* pActionCollection = 0;
			hr = pTask->get_Actions(&pActionCollection);
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			// Create Actiton
			IAction* pAction = 0;
			hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
			pActionCollection->Release();
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			// Create Exec action
			IExecAction* pExecAction = 0;
			hr = pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);
			pAction->Release();
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			// Set Process path
			hr = pExecAction->put_Path(_bstr_t(exePath.c_str()));
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				pExecAction->Release();
				return false;
			}
			// Set Process work path
			hr = pExecAction->put_WorkingDirectory(_bstr_t(runPath.c_str()));
			pExecAction->Release();
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			// Register Task
			IRegisteredTask* pRegisteredTask = 0;
			hr = pTaskFolder->RegisterTaskDefinition(_bstr_t(taskName.c_str()), pTask, TASK_CREATE_OR_UPDATE, _variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(), &pRegisteredTask);
			if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}
			pTask->Release();
			pTaskFolder->Release();
			pTaskService->Release();
			pRegisteredTask->Release();
			return true;
		}

		// return false: no task or error, return true: task found
		static bool Find(std::wstring taskName)
		{
			HRESULT hr = 0;
			ITaskService* pTaskService = 0;
			ITaskFolder* pTaskFolder = 0;
			InitStruct com;
			com.pTaskService = &pTaskService;
			com.pTaskFolder = &pTaskFolder;
			if (!InitCom(com)) return false;
			IRegisteredTask* pRegisteredTask = 0;
			hr = pTaskFolder->GetTask(_bstr_t(taskName.c_str()), &pRegisteredTask);
			pTaskService->Release();
			pTaskFolder->Release();
			if (HRESULT_CODE(hr) == 2) return false;
			if (FAILED(hr)) return false;
			pRegisteredTask->Release();
			return true;
		}

		// return false: error, return true: no task or task deleted
		static bool Delete(std::wstring taskName)
		{
			HRESULT hr = 0;
			ITaskService* pTaskService = 0;
			ITaskFolder* pTaskFolder = 0;
			InitStruct com;
			com.pTaskService = &pTaskService;
			com.pTaskFolder = &pTaskFolder;
			if (!InitCom(com)) return false;
			IRegisteredTask* pIRegisteredTask = 0;
			hr = pTaskFolder->GetTask(_bstr_t(taskName.c_str()), &pIRegisteredTask);
			if (HRESULT_CODE(hr) == 2)
			{
				pTaskService->Release();
				pTaskFolder->Release();
				return true;
			}
			else if (FAILED(hr))
			{
				pTaskService->Release();
				pTaskFolder->Release();
				return false;
			}
			hr = pTaskFolder->DeleteTask(_bstr_t(taskName.c_str()), 0);
			pTaskService->Release();
			pTaskFolder->Release();
			pIRegisteredTask->Release();
			if (FAILED(hr))
			{
				return false;
			}
			return true;
		}
	};
}