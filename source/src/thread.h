#pragma once
#include "inc_header.h"
namespace QiThread
{
	bool PeekExitMsg();
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
	bool MacroRunActive(Macro* pMacro);
	bool MacroEndActive(Macro* pMacro);
	bool QuickClickActive();
	bool WindowStateActive();
	// release click
	void AddReleaseKey(byte key);
};