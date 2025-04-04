#pragma once
#include "inc_header.h"
namespace QiThread
{
	struct ThreadParam
	{
		bool run;
		Macro* macro;
		std::mutex mutex;
		std::condition_variable load;
	};

	void PrecSleep(clock_t ms);
	bool PeekExitMsg();
	bool PeekSleep(clock_t ms);
	// start
	void StartMacroRun(Macro* pMacro);
	void StartMacroEnd(Macro* pMacro);
	void StartQuickClick();
	void StartWindowState();
	// exit
	void ExitMacroRun(Macro* pMacro);
	void ExitMacroEnd(Macro* pMacro);
	void ExitQuickClick();
	void ExitWindowState();
	// state
	bool MacroRunActive(const Macro* pMacro);
	bool MacroEndActive(const Macro* pMacro);
	bool QuickClickActive();
	bool WindowStateActive();
};