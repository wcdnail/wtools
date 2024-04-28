#pragma once

#ifndef HANDLE
typedef void *HANDLE;
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct _CONTEXT;
typedef struct _CONTEXT* PCONTEXT;

struct RUN_CTX;
typedef struct RUN_CTX* PRUN_CONTEXT;
typedef struct RUN_CTX const* PCRUN_CONTEXT;

int      get_last_error_code(void);

HANDLE   run_context_get_pid(PCRUN_CONTEXT pctx);
HANDLE   run_context_get_tid(PCRUN_CONTEXT pctx);
PCONTEXT run_context_get_ctx(PCRUN_CONTEXT pctx);
int      run_context_get_skp(PCRUN_CONTEXT pctx);

void     free_run_context(PRUN_CONTEXT pctx);
void     capture_run_context(PRUN_CONTEXT* ppctx);

#ifdef __cplusplus
}
#endif
