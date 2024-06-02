#pragma once

#ifndef _NEED_WMESSAGE_DUMP
#  define _NEED_WMESSAGE_DUMP 0
#endif

#if defined(_DEBUG) && (_NEED_WMESSAGE_DUMP)
#  include <dh.tracing.h>
#  include <dh.tracing.defs.h>
#  include <dev.assistance/dev.assist.h>
#  define DBG_DUMP_WMESSAGE(HDR, CAPT, pMsg)                            \
    do {                                                                 \
        auto msgStr = DH::MessageToStrignW(pMsg);                         \
        DBGTPrint(HDR L" %12s [[ %s ]]\n", CAPT, msgStr.c_str());          \
    }                                                                       \
    while (false)
#  define DBG_DUMP_WMESSAGE_EXT(HDR, CAPT, hWnd, uMsg, wParam, lParam)  \
    do {                                                                 \
        auto msgStr = DH::MessageToStrignW(hWnd, uMsg, wParam, lParam);   \
        DBGTPrint(HDR L" %12s [[ %s ]]\n", CAPT, msgStr.c_str());          \
    }                                                                       \
    while (false)
#else
#  define DBG_DUMP_WMESSAGE(...)      do {} while (false)
#  define DBG_DUMP_WMESSAGE_EXT(...)  do {} while (false)
#endif
