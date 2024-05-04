#pragma once

#define _NEED_WMESSAGE_DUMP 0

#if defined(_DEBUG) && (_NEED_WMESSAGE_DUMP)
#  include "dh.tracing.h"
#  include "dh.tracing.defs.h"
#  include "dev.assistance/dev.assist.h"
#  define DBG_DUMP_WMESSAGE(HDR, CAPT, pMsg)                            \
    do {                                                                 \
        auto msgStr = DH::MessageToStrignW(pMsg);                         \
        DebugThreadPrintf(HDR L" %12s [[ %s ]]\n", CAPT, msgStr.c_str());  \
    }                                                                       \
    while (false)
#  define DBG_DUMP_WMESSAGE_EXT(HDR, CAPT, hWnd, uMsg, wParam, lParam)  \
    do {                                                                 \
        auto msgStr = DH::MessageToStrignW(hWnd, uMsg, wParam, lParam);   \
        DebugThreadPrintf(HDR L" %12s [[ %s ]]\n", CAPT, msgStr.c_str());  \
    }                                                                       \
    while (false)
#else
#  define DBG_DUMP_WMESSAGE(...)      do {} while (false)
#  define DBG_DUMP_WMESSAGE_EXT(...)  do {} while (false)
#endif
