#pragma once

#ifdef _DEBUG
#include "dh.tracing.h"
#include "dh.tracing.defs.h"
#include "dev.assistance/dev.assist.h"

#define DBG_DUMP_WMESSAGE(CAPT, pMsg)                         \
    do {                                                       \
        auto msgStr = DH::MessageToStrignW(pMsg);               \
        DebugThreadPrintf(CAPT L" [[ %s ]]\n", msgStr.c_str());  \
    }                                                             \
    while (false)
#else
#define DBG_DUMP_WMESSAGE(CAPT, pMsg)
#endif
