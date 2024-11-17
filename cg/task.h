#pragma once

#include "file.h"
#include "process.h"
#include "system.h"
#include <string>
#include <windows.h>
#include <comdef.h>
#include <wincred.h>
#include <taskschd.h>
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")
#pragma comment(lib, "taskschd.lib")

namespace CG
{
	class Task
	{
		struct InitStruct
		{
			std::wstring* log;
			ITaskService** pTaskService = 0;
			ITaskFolder** pTaskFolder = 0;
		};

		static bool InitCom(InitStruct& com)
		{
			*com.log = L"Initialize: \n";

			HRESULT hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
			if (FAILED(hr))
			{
				if (hr == S_FALSE)
				{
					*com.log += L"  warning: CoInitializeEx\n";
					*com.log += L"    result: ";
					*com.log += String::toWString(HRESULT_CODE(hr));
					*com.log += L"\n";
				}
				else if (hr == RPC_E_CHANGED_MODE)
				{
					*com.log += L"error: CoInitializeEx\n";
					*com.log += L"  result: ";
					*com.log += String::toWString(HRESULT_CODE(hr));
					*com.log += L"\n";
					return false;
				}
			}

			hr = CoInitializeSecurity(0, -1, 0, 0, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, 0, 0, 0);
			if (FAILED(hr))
			{
				if (hr == RPC_E_TOO_LATE)
				{
					*com.log += L"  warning: CoInitializeSecurity\n";
					*com.log += L"    result: ";
					*com.log += String::toWString(HRESULT_CODE(hr));
					*com.log += L"\n";
				}
				else
				{
					*com.log += L"  error: CoInitializeSecurity\n";
					*com.log += L"    result: ";
					*com.log += String::toWString(HRESULT_CODE(hr));
					*com.log += L"\n";
					return false;
				}
			}

			hr = CoCreateInstance(CLSID_TaskScheduler, 0, CLSCTX_ALL, IID_ITaskService, (LPVOID*)com.pTaskService);
			if (FAILED(hr))
			{
				*com.log += L"  error: CoCreateInstance\n";
				*com.log += L"    result: ";
				*com.log += String::toWString(HRESULT_CODE(hr));
				*com.log += L"\n";
				return false;
			}

			hr = (*com.pTaskService)->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
			if (FAILED(hr))
			{
				*com.log += L"  error: pTaskService->Connect\n";
				*com.log += L"    result: ";
				*com.log += String::toWString(HRESULT_CODE(hr));
				*com.log += L"\n";
				(*com.pTaskService)->Release();
				return false;
			}

			hr = (*com.pTaskService)->GetFolder(_bstr_t(L"\\"), com.pTaskFolder);
			if (FAILED(hr))
			{
				*com.log += L"  error: pTaskService->GetFolder\n";
				*com.log += L"    result: ";
				*com.log += String::toWString(HRESULT_CODE(hr));
				*com.log += L"\n";
				(*com.pTaskService)->Release();
				return false;
			}

			return true;
		}
	public:
		// return false: error, return true: task registered
		static bool Register(std::wstring taskName, std::wstring exePath = Process::exePath(), std::wstring runPath = Path::GetDir(Process::exePath()), std::wstring author = L"unknow")
		{
			std::wstring log;
			HRESULT hr = 0;
			ITaskService* pTaskService = 0;
			ITaskFolder* pTaskFolder = 0;

			InitStruct com;
			com.log = &log;
			com.pTaskService = &pTaskService;
			com.pTaskFolder = &pTaskFolder;

			if (!InitCom(com)) return false;

			std::wstring taskPath = String::toWString(L"\\") + taskName;
			std::wstring userPath = System::environmentVariable(L"COMPUTERNAME") + String::toWString(L"\\") + System::environmentVariable(L"USERNAME");

			log += L"Register: \n";

			ITaskDefinition* pTask = 0;
			hr = pTaskService->NewTask(0, &pTask);
			if (FAILED(hr))
			{
				log += L"  error: pTaskService->NewTask\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
				pTaskFolder->Release();
				pTaskService->Release();
				return false;
			}

			// Get Task information
			IRegistrationInfo* pRegInfo = 0;
			hr = pTask->get_RegistrationInfo(&pRegInfo);
			if (FAILED(hr))
			{
				log += L"  error: pTask->get_RegistrationInfo\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}

			// Set Author name
			hr = pRegInfo->put_Author(_bstr_t(author.c_str()));
			pRegInfo->Release();
			if (FAILED(hr))
			{
				log += L"warning: pRegInfo->put_Author\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
			}

			// Get Task principal
			IPrincipal* pPrincipal = 0;
			hr = pTask->get_Principal(&pPrincipal);
			if (FAILED(hr))
			{
				log += L"  error: get_Principal\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}

			// Set Principal name
			hr = pPrincipal->put_Id(_bstr_t(L"Principal 1"));
			if (FAILED(hr))
			{
				log += L"warning: pPrincipal->put_Id\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
			}

			// Set Logon type to InterActive
			hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
			if (FAILED(hr))
			{
				log += L"  error: pPrincipal->put_LogonType\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				pPrincipal->Release();
				return false;
			}

			// Set Run with high user privileges
			hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
			pPrincipal->Release();
			if (FAILED(hr))
			{
				log += L"  error: pPrincipal->put_RunLevel\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
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
				log += L"  error: pTask->get_Settings\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}

			// Uncheck: Power - Start the task only if the computer is on AC power
			hr = pSettings->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
			if (FAILED(hr))
			{
				log += L"  error: pSettings->put_DisallowStartIfOnBatteries\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
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
				log += L"  error: pSettings->put_StartWhenAvailable\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
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
				log += L"  error: pIRepetitionPattern->put_StopAtDurationEnd\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
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
				log += L"  error: pTask->get_Triggers\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
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
				log += L"  error: pTriggerCollection->Create\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
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
				log += L"  error: pTrigger->QueryInterface\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}

			// Set Trigger name
			hr = pLogonTrigger->put_Id(_bstr_t(L"Logon trigger"));
			if (FAILED(hr))
			{
				log += L"warning: pLogonTrigger->put_Id\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
			}

			// Set Logon user path
			hr = pLogonTrigger->put_UserId(_bstr_t(userPath.c_str()));
			if (FAILED(hr))
			{
				log += L"  error: pLogonTrigger->put_UserId\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
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
				log += L"  error: pLogonTrigger->put_Delay\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
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
				log += L"  error: pTask->get_Actions\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
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
				log += L"  error: pActionCollection->Create\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
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
				log += L"  error: pAction->QueryInterface\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
				pTaskService->Release();
				pTaskFolder->Release();
				pTask->Release();
				return false;
			}

			// Set Process path
			hr = pExecAction->put_Path(_bstr_t(exePath.c_str()));
			if (FAILED(hr))
			{
				log += L"  error: pExecAction->put_Path\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
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
				log += L"  error: pExecAction->put_WorkingDirectory\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
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
				log += L"  error: pTaskFolder->RegisterTaskDefinition\n";
				log += L"      result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
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
			std::wstring log;
			HRESULT hr = 0;
			ITaskService* pTaskService = 0;
			ITaskFolder* pTaskFolder = 0;

			InitStruct com;
			com.log = &log;
			com.pTaskService = &pTaskService;
			com.pTaskFolder = &pTaskFolder;

			if (!InitCom(com)) return false;

			std::wstring taskPath = String::toWString(L"\\") + taskName;

			log += L"Find: \n";

			IRegisteredTask* pRegisteredTask = 0;
			hr = pTaskFolder->GetTask(_bstr_t(taskPath.c_str()), &pRegisteredTask);
			pTaskService->Release();
			pTaskFolder->Release();
			if (HRESULT_CODE(hr) == 2)
			{
				return false;
			}
			if (FAILED(hr))
			{
				log += L"  error: pTaskFolder->GetTask\n";
				log += L"    result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
				return false;
			}

			pRegisteredTask->Release();
			return true;
		}

		// return false: error, return true: no task or task deleted
		static bool Delete(std::wstring taskName)
		{
			std::wstring log;
			HRESULT hr = 0;
			ITaskService* pTaskService = 0;
			ITaskFolder* pTaskFolder = 0;

			InitStruct com;
			com.log = &log;
			com.pTaskService = &pTaskService;
			com.pTaskFolder = &pTaskFolder;

			if (!InitCom(com)) return false;

			std::wstring taskPath = String::toWString(L"\\") + taskName;

			log += L"Delete: \n";

			IRegisteredTask* pIRegisteredTask = 0;
			hr = pTaskFolder->GetTask(_bstr_t(taskPath.c_str()), &pIRegisteredTask);
			if (HRESULT_CODE(hr) == 2)
			{
				pTaskService->Release();
				pTaskFolder->Release();
				return true;
			}
			else if (FAILED(hr))
			{
				log += L"  warning: pTaskFolder->GetTask\n";
				log += L"      result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
				pTaskService->Release();
				pTaskFolder->Release();
				return false;
			}

			hr = pTaskFolder->DeleteTask(_bstr_t(taskPath.c_str()), 0);
			pTaskService->Release();
			pTaskFolder->Release();
			pIRegisteredTask->Release();
			if (FAILED(hr))
			{
				log += L"  error: pTaskFolder->DeleteTask\n";
				log += L"      result: ";
				log += String::toWString(HRESULT_CODE(hr));
				log += L"\n";
				return false;
			}

			return true;
		}
	};
}