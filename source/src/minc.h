#pragma execution_character_set("utf-8")
#pragma once
#include "ginc.h"

#define WToQString(stdwstring) (QString::fromWCharArray(stdwstring.c_str()))
#define QStringToW(qstring) ((const wchar_t*)(qstring.utf16()))

constexpr int r_exit = 0;
constexpr int r_continue = 1;
constexpr int r_break = 2;
constexpr int key_info = 214;
constexpr int msg_exit = (WM_USER + 0xFF);

#include "../ui/widget/QKeyEdit.h"
#include "../ui/widget/QPopText.h"
#include "../ui/widget/QWindowSelection.h"
#include "type.h"
#include "func.h"
#include "thread.h"
#include "json.h"