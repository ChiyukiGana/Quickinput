#pragma execution_character_set("utf-8")
#pragma once
#include "ginc.h"

constexpr int msg_exit = (WM_USER + 0xFF);
constexpr int r_exit = 0;
constexpr int r_continue = 1;
constexpr int r_break = 2;

#include "../ui/widget/QKeyEdit.h"
#include "../ui/widget/QPopText.h"
#include "type.h"
#include "func.h"
#include "thread.h"
#include "json.h"