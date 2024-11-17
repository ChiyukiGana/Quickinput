#pragma execution_character_set("utf-8")
#pragma once
#include "minc.h"

namespace PopBox
{
	void Init();
	void Popup(std::wstring text, COLORREF color = RGB(0xCC, 0xEE, 0xFF), UINT time = 1000);
	void Show(std::wstring text, COLORREF color = RGB(0xCC, 0xEE, 0xFF));
	void Hide();
};