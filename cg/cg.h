#pragma once

#include "base.h"
#include "list.h"
#include "string.h"
#include "time.h"
#include "timer.h"

#include "path.h"
#include "file.h"
#include "system.h"
#include "thread.h"
#include "process.h"

#include "input.h"
#include "ihook.h"

#include "color.h"
#include "image.h"
#include "media.h"
#include "auddv.h"

#include "window.h"
#include "msgbox.h"
#include "msgwnd.h"

#include "task.h"

#include "ini.h"

#include "CJsonObject/CJsonObject.hpp"
#include "base64/base64.h"
#include "base64-1/base64.h"

#ifdef DEBUG

#ifdef _M_AMD64 
#pragma comment(lib, "cg/CJsonObject/64d/CJsonObject.lib")
#pragma comment(lib, "cg/base64-1/64d/base64.lib")
#endif // _M_AMD64
#ifdef _M_IX86
#pragma comment(lib, "cg/CJsonObject/32d/CJsonObject.lib")
#pragma comment(lib, "cg/base64-1/32d/base64.lib")
#endif // _M_IX86

#else // DEBUG

#ifdef _M_AMD64
#pragma comment(lib, "cg/CJsonObject/64/CJsonObject.lib")
#pragma comment(lib, "cg/base64-1/64/base64.lib")
#endif // _M_AMD64
#ifdef _M_IX86
#pragma comment(lib, "cg/CJsonObject/32/CJsonObject.lib")
#pragma comment(lib, "cg/base64-1/32/base64.lib")
#endif // _M_IX86

#endif // DEBUG

using namespace CG;