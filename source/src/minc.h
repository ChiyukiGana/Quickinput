#pragma execution_character_set("utf-8")
#pragma once
#include "ginc.h"

#define MacroPath L"macro\\"
#define MacroType L".json"

#define msg_exit (WM_USER + 0xFF)

#define r_exit 0
#define r_continue 1
#define r_break 2

#include "type.h"
#include "func.h"
#include "thread.h"
#include "json.h"
#include "PopBox.h"