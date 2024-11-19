#pragma execution_character_set("utf-8")
#pragma once
#include "ginc.h"

#define WToQString(stdwstring) (QString::fromWCharArray((stdwstring).c_str()))
#define QStringToW(qstring) ((const wchar_t*)((qstring).utf16()))

enum InterpreterResult
{
	r_exit,
	r_continue,
	r_break,
	r_top
};

constexpr int key_info = 214;
constexpr int msg_exit = (WM_USER + 0xFF);

#include "../ui/widget/QKeyEdit.h"
#include "../ui/widget/QPopText.h"
#include "../ui/widget/QWindowSelection.h"
#include "qitype.h"
#include "type.h"
#include "func.h"
#include "thread.h"
#include "json.h"
#include "interpreter.h"