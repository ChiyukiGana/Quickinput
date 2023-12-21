#pragma once

#include "file.h"
#include "thread.h"
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

        enum Action
        {
            find,
            reg,
            del
        };

    public:
        struct TaskStruct
        {
            bool result = 0;
            int operate = 0;
            std::wstring log;
            std::wstring taskName;
            std::wstring exePath;
            std::wstring runPath;
            std::wstring author;
        };

        // return 0: error, return 1: task registered
        static const TaskStruct Register(std::wstring taskName, std::wstring exePath = Process::exePath(), std::wstring runPath = File::PathLast(Process::exePath().c_str()), std::wstring author = L"unknow")
        {
            TaskStruct ts;
            ts.operate = Action::reg;
            ts.taskName = taskName;
            ts.exePath = exePath;
            ts.runPath = runPath;
            HANDLE th = Thread::Start(TaskOperate, &ts);
            Thread::Wait(th);
            return ts;
        }

        // return 0: no task or error, return 1: task found
        static const TaskStruct Find(std::wstring taskName)
        {
            TaskStruct ts;
            ts.operate = Action::find;
            ts.taskName = taskName;
            HANDLE th = Thread::Start(TaskOperate, &ts);
            Thread::Wait(th);
            return ts;
        }

        // return 0: no task or error, return 1: task deleted
        static const TaskStruct Delete(std::wstring taskName)
        {
            TaskStruct ts;
            ts.operate = Action::del;
            ts.taskName = taskName;
            HANDLE th = Thread::Start(TaskOperate, &ts);
            Thread::Wait(th);
            return ts;
        }

        static DWORD CALLBACK TaskOperate(LPVOID lParam)
        {
            TaskStruct* ts = (TaskStruct*)lParam;

            HRESULT hr = 0;
            ITaskService* pTaskService = 0;
            ITaskFolder* pTaskFolder = 0;

            std::wstring taskPath = String::toWString(L"\\") + ts->taskName;
            std::wstring userPath = System::environmentVariable(L"COMPUTERNAME") + String::toWString(L"\\") + System::environmentVariable(L"USERNAME");

            // Init Com
            {
                ts->log = L"Initialize: \n";

                hr = CoInitializeEx(0, COINIT_MULTITHREADED);
                if (FAILED(hr))
                {
                    if (hr == S_FALSE)
                    {
                        ts->log += L"  warning: CoInitializeEx\n";
                        ts->log += L"    result: ";
                        ts->log += String::toWString(HRESULT_CODE(hr));
                        ts->log += L"\n";
                    }
                    else if (hr == RPC_E_CHANGED_MODE)
                    {
                        ts->log += L"error: CoInitializeEx\n";
                        ts->log += L"  result: ";
                        ts->log += String::toWString(HRESULT_CODE(hr));
                        ts->log += L"\n";
                        ts->result = 0;
                        return 0;
                    }
                }

                hr = CoInitializeSecurity(0, -1, 0, 0, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, 0, 0, 0);
                if (FAILED(hr))
                {
                    if (hr == RPC_E_TOO_LATE)
                    {
                        ts->log += L"  warning: CoInitializeSecurity\n";
                        ts->log += L"    result: ";
                        ts->log += String::toWString(HRESULT_CODE(hr));
                        ts->log += L"\n";
                    }
                    else
                    {
                        ts->log += L"  error: CoInitializeSecurity\n";
                        ts->log += L"    result: ";
                        ts->log += String::toWString(HRESULT_CODE(hr));
                        ts->log += L"\n";
                        ts->result = 0;
                        CoUninitialize();
                        return 0;
                    }
                }

                hr = CoCreateInstance(CLSID_TaskScheduler, 0, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pTaskService);
                if (FAILED(hr))
                {
                    ts->log += L"  error: CoCreateInstance\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    CoUninitialize();
                    return 0;
                }

                hr = pTaskService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
                if (FAILED(hr))
                {
                    ts->log += L"  error: pTaskService->Connect\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    CoUninitialize();
                    return 0;
                }

                hr = pTaskService->GetFolder(_bstr_t(L"\\"), &pTaskFolder);
                if (FAILED(hr))
                {
                    ts->log += L"  error: pTaskService->GetFolder\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    CoUninitialize();
                    return 0;
                }
            }

            if (ts->operate == Action::find)
            {
                ts->log += L"Find: \n";

                IRegisteredTask* pRegisteredTask = 0;
                hr = pTaskFolder->GetTask(_bstr_t(taskPath.c_str()), &pRegisteredTask);
                pTaskService->Release();
                pTaskFolder->Release();
                if (HRESULT_CODE(hr) == 2)
                {
                    ts->result = 0;
                    CoUninitialize();
                    return 0;
                }
                if (FAILED(hr))
                {
                    ts->log += L"  error: pTaskFolder->GetTask\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    CoUninitialize();
                    return 0;
                }

                ts->result = 1;
                pRegisteredTask->Release();
                CoUninitialize();
                return 1;
            }
            else if (ts->operate == Action::del)
            {
                ts->log += L"Delete: \n";

                IRegisteredTask* pIRegisteredTask = 0;
                hr = pTaskFolder->GetTask(_bstr_t(ts->taskName.c_str()), &pIRegisteredTask);
                if (HRESULT_CODE(hr) == 2)
                {
                    ts->result = 1;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    CoUninitialize();
                    return 1;
                }
                else if (FAILED(hr))
                {
                    ts->log += L"  warning: pTaskFolder->GetTask\n";
                    ts->log += L"      result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    CoUninitialize();
                    return 0;
                }

                hr = pTaskFolder->DeleteTask(_bstr_t(ts->taskName.c_str()), 0);
                pTaskService->Release();
                pTaskFolder->Release();
                pIRegisteredTask->Release();
                if (FAILED(hr))
                {
                    ts->log += L"  error: pTaskFolder->DeleteTask\n";
                    ts->log += L"      result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    CoUninitialize();
                    return 0;
                }

                ts->result = 1;
                CoUninitialize();
                return 1;
            }
            else if (ts->operate == Action::reg)
            {
                ts->log += L"Register: \n";

                ITaskDefinition* pTask = 0;
                hr = pTaskService->NewTask(0, &pTask);
                if (FAILED(hr))
                {
                    ts->log += L"  error: pTaskService->NewTask\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskFolder->Release();
                    pTaskService->Release();
                    CoUninitialize();
                    return 0;
                }

                // Get Task information
                IRegistrationInfo* pRegInfo = 0;
                hr = pTask->get_RegistrationInfo(&pRegInfo);
                if (FAILED(hr))
                {
                    ts->log += L"  error: pTask->get_RegistrationInfo\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                // Set Author name
                hr = pRegInfo->put_Author(_bstr_t(ts->author.c_str()));
                pRegInfo->Release();
                if (FAILED(hr))
                {
                    ts->log += L"warning: pRegInfo->put_Author\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                }

                // Get Task principal
                IPrincipal* pPrincipal = 0;
                hr = pTask->get_Principal(&pPrincipal);
                if (FAILED(hr))
                {
                    ts->log += L"  error: get_Principal\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                // Set Principal name
                hr = pPrincipal->put_Id(_bstr_t(L"Principal 1"));
                if (FAILED(hr))
                {
                    ts->log += L"warning: pPrincipal->put_Id\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                }

                // Set Logon type to InterActive
                hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
                if (FAILED(hr))
                {
                    ts->log += L"  error: pPrincipal->put_LogonType\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    pPrincipal->Release();
                    CoUninitialize();
                    return 0;
                }

                // Set Run with high user privileges
                hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
                pPrincipal->Release();
                if (FAILED(hr))
                {
                    ts->log += L"  error: pPrincipal->put_RunLevel\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                // Get Task settings
                ITaskSettings* pSettings = 0;
                hr = pTask->get_Settings(&pSettings);
                if (FAILED(hr))
                {
                    ts->log += L"  error: pTask->get_Settings\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                // Uncheck: Power - Start the task only if the computer is on AC power
                hr = pSettings->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
                if (FAILED(hr))
                {
                    ts->log += L"  error: pSettings->put_DisallowStartIfOnBatteries\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    pSettings->Release();
                    CoUninitialize();
                    return 0;
                }

                // Check: Run task as soon as possible after a scheduled start is missed
                hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
                if (FAILED(hr))
                {
                    ts->log += L"  error: pSettings->put_StartWhenAvailable\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    pSettings->Release();
                    CoUninitialize();
                    return 0;
                }

                // Uncheck: Stop the task if it runs loger than
                hr = pSettings->put_ExecutionTimeLimit(_bstr_t(L"PT0S"));
                pSettings->Release();
                if (FAILED(hr))
                {
                    ts->log += L"  error: pIRepetitionPattern->put_StopAtDurationEnd\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                // Get Trigger collection
                ITriggerCollection* pTriggerCollection = 0;
                hr = pTask->get_Triggers(&pTriggerCollection);
                if (FAILED(hr))
                {
                    ts->log += L"  error: pTask->get_Triggers\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                // Create User logon trigger
                ITrigger* pTrigger = 0;
                hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
                pTriggerCollection->Release();
                if (FAILED(hr))
                {
                    ts->log += L"  error: pTriggerCollection->Create\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                // Create Logon tirgger
                ILogonTrigger* pLogonTrigger = 0;
                hr = pTrigger->QueryInterface(IID_ILogonTrigger, (void**)&pLogonTrigger);
                pTrigger->Release();
                if (FAILED(hr))
                {
                    ts->log += L"  error: pTrigger->QueryInterface\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                // Set Trigger name
                hr = pLogonTrigger->put_Id(_bstr_t(L"Logon trigger"));
                if (FAILED(hr))
                {
                    ts->log += L"warning: pLogonTrigger->put_Id\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                }

                // Set Logon user path
                hr = pLogonTrigger->put_UserId(_bstr_t(userPath.c_str()));
                if (FAILED(hr))
                {
                    ts->log += L"  error: pLogonTrigger->put_UserId\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    pLogonTrigger->Release();
                    CoUninitialize();
                    return 0;
                }

                // Set Run delay
                hr = pLogonTrigger->put_Delay(_bstr_t(L"PT0S"));
                pLogonTrigger->Release();
                if (FAILED(hr))
                {
                    ts->log += L"  error: pLogonTrigger->put_Delay\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                // Get Action collection
                IActionCollection* pActionCollection = 0;
                hr = pTask->get_Actions(&pActionCollection);
                if (FAILED(hr))
                {
                    ts->log += L"  error: pTask->get_Actions\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                // Create Actiton
                IAction* pAction = 0;
                hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
                pActionCollection->Release();
                if (FAILED(hr))
                {
                    ts->log += L"  error: pActionCollection->Create\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                // Create Exec action
                IExecAction* pExecAction = 0;
                hr = pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);
                pAction->Release();
                if (FAILED(hr))
                {
                    ts->log += L"  error: pAction->QueryInterface\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                // Set Process path
                hr = pExecAction->put_Path(_bstr_t(ts->exePath.c_str()));
                if (FAILED(hr))
                {
                    ts->log += L"  error: pExecAction->put_Path\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    pExecAction->Release();
                    CoUninitialize();
                    return 0;
                }

                // Set Process work path
                hr = pExecAction->put_WorkingDirectory(_bstr_t(ts->runPath.c_str()));
                pExecAction->Release();
                if (FAILED(hr))
                {
                    ts->log += L"  error: pExecAction->put_WorkingDirectory\n";
                    ts->log += L"    result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                // Register Task
                IRegisteredTask* pRegisteredTask = 0;
                hr = pTaskFolder->RegisterTaskDefinition(_bstr_t(ts->taskName.c_str()), pTask, TASK_CREATE_OR_UPDATE, _variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(), &pRegisteredTask);
                if (FAILED(hr))
                {
                    ts->log += L"  error: pTaskFolder->RegisterTaskDefinition\n";
                    ts->log += L"      result: ";
                    ts->log += String::toWString(HRESULT_CODE(hr));
                    ts->log += L"\n";
                    ts->result = 0;
                    pTaskService->Release();
                    pTaskFolder->Release();
                    pTask->Release();
                    CoUninitialize();
                    return 0;
                }

                ts->result = 1;
                pTask->Release();
                pTaskFolder->Release();
                pTaskService->Release();
                pRegisteredTask->Release();
                CoUninitialize();
                return 1;
            }
            return 0;
        }
    };
}