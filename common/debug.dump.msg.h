#pragma once

#ifndef _NEED_WMESSAGE_DUMP
#  define _NEED_WMESSAGE_DUMP 0
#endif

#if defined(_DEBUG) && (_NEED_WMESSAGE_DUMP)
#  include <dh.tracing.h>
#  include <dev.assistance/dev.assist.h>
#  define DBG_DUMP_WMESSAGE(LEV, CAPT, pMsg)                            \
    do {                                                                 \
        unsigned nLev = (LEV) | TL_NoDCOutput;                            \
        auto msgStr = DH::MessageToStrignW(pMsg);                          \
        DBGTPrint(nLev, L"%12s [[ %s ]]\n", CAPT, msgStr.c_str());          \
    }                                                                        \
    while (false)
#  define DBG_DUMP_WMESSAGE_EXT(LEV, CAPT, hWnd, uMsg, wParam, lParam)  \
    do {                                                                 \
        unsigned nLev = (LEV) | TL_NoDCOutput;                            \
        auto msgStr = DH::MessageToStrignW(hWnd, uMsg, wParam, lParam);    \
        DBGTPrint(nLev, L"%12s [[ %s ]]\n", CAPT, msgStr.c_str());          \
    }                                                                        \
    while (false)
#else
#  define DBG_DUMP_WMESSAGE(...)      do {} while (false)
#  define DBG_DUMP_WMESSAGE_EXT(...)  do {} while (false)
#endif
