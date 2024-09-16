#pragma once
#include "minc.h"

namespace QiThread
{
	bool PeekExitMsg();

	void StartMacroRun(Macro* pMacro);
	void StartMacroEnd(Macro* pMacro);
	void StartQuickClick();
	void StartWindowState();

	void ExitMacroRun(Macro* pMacro);
	void ExitMacroEnd(Macro* pMacro);
	void ExitQuickClick();
	void ExitWindowState();

	bool MacroRunActive(Macro* pMacro);
	bool MacroEndActive(Macro* pMacro);
	bool QuickClickActive();
	bool WindowStateActive();

	void AddReleaseKey(byte key);
};