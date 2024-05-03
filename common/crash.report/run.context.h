#pragma once

#include "wcdafx.api.h"

#ifndef HANDLE
typedef void *HANDLE;
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct _CONTEXT;
typedef struct _CONTEXT* PCONTEXT;
typedef struct _CONTEXT const* CPCONTEXT;

struct RUN_CTX;
typedef struct RUN_CTX* PRUN_CONTEXT;
typedef struct RUN_CTX const* PCRUN_CONTEXT;

WCDAFX_API int       get_last_error_code(void);

WCDAFX_API HANDLE    run_context_get_pid(PCRUN_CONTEXT pctx);
WCDAFX_API HANDLE    run_context_get_tid(PCRUN_CONTEXT pctx);
WCDAFX_API CPCONTEXT run_context_get_ctx(PCRUN_CONTEXT pctx);
WCDAFX_API int       run_context_get_skp(PCRUN_CONTEXT pctx);

WCDAFX_API void      free_run_context(PRUN_CONTEXT pctx);
WCDAFX_API void      capture_run_context(PRUN_CONTEXT* ppctx);

#ifdef __cplusplus
}

#include <memory>

template <>
struct std::default_delete<RUN_CTX>
{
    void operator()(RUN_CTX* pctx)
    {
        free_run_context(pctx);
    }
};

using RUN_CTX_Ptr = std::unique_ptr<RUN_CTX>;

inline RUN_CTX_Ptr cxx_capture_run_context()
{
    PRUN_CONTEXT rawPtr = nullptr;
    capture_run_context(&rawPtr);
    RUN_CTX_Ptr res;
    if (rawPtr) {
        res.reset(rawPtr);
    }
    return res;
}

#endif
