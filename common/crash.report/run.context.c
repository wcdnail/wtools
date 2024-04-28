#include "run.context.h"
#ifdef _MSC_VER
#  pragma warning(disable: 4820) // 4820: 'RUN_CTX': '12' bytes padding added after data member 'skip'
#  pragma warning(disable: 4191) // 4191: 'type cast': unsafe conversion from 'FARPROC' to 'void (__cdecl *)(PCONTEXT)'
#endif
#ifdef _WIN32
#  define  _CRT_SECURE_NO_WARNINGS
#  define WIN32_LEAN_AND_MEAN
#  define STRICT
#  include <SDKDDKVer.h>
#  include <windows.h>
#endif
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

struct RUN_CTX
{
#ifdef _WIN32
    HANDLE       pid;
    HANDLE       tid;
    CONTEXT  context;
#endif
    int32_t     skip;     /* For stack walker. */
};

typedef void (__stdcall *PRTLCAPTURECONTEXT)(PCONTEXT);

int get_last_error_code(void)
{
    return ((int)GetLastError());
}

HANDLE    run_context_get_pid(PCRUN_CONTEXT pctx) { return pctx ? pctx->pid : INVALID_HANDLE_VALUE; }
HANDLE    run_context_get_tid(PCRUN_CONTEXT pctx) { return pctx ? pctx->tid : INVALID_HANDLE_VALUE; }
CPCONTEXT run_context_get_ctx(PCRUN_CONTEXT pctx) { return pctx ? &pctx->context : NULL; }
int       run_context_get_skp(PCRUN_CONTEXT pctx) { return pctx ? pctx->skip : 0; }

void free_run_context(PRUN_CONTEXT pctx)
{
    if (pctx) {
        free((void*)pctx);
    }
}

void capture_run_context(PRUN_CONTEXT* ppctx)
{
    CONTEXT             context = { 0 };
    DWORD                  lerr = GetLastError();
    PRTLCAPTURECONTEXT rtlpcctx = (PRTLCAPTURECONTEXT)GetProcAddress(GetModuleHandleW(L"kernel32"), "RtlCaptureContext");
    PRUN_CONTEXT             rv = calloc(1, sizeof(struct RUN_CTX));
    if (!rv || !rtlpcctx) {
        return ;
    }
    rtlpcctx(&context);
#ifdef _M_IX86
    {
        _asm    call    x
        _asm x: pop     eax
        _asm    mov     context.Eip, eax
        _asm    mov     context.Ebp, ebp
        _asm    mov     context.Esp, esp
    }
#endif // _M_IX86
    rv->pid      = GetCurrentProcess();
    rv->tid      = GetCurrentThread();
    rv->context  = context;
    rv->skip     = 1;
    SetLastError(lerr);
    (*ppctx) = rv;
}

static void SuspendThreadAndGetContext(HANDLE thread, PCONTEXT context)
{
    assert(NULL != context);
    SuspendThread(thread);
    GetThreadContext(thread, context);
    ResumeThread(thread);
}

void get_context(HANDLE thread, DWORD threadId, PCONTEXT context, DWORD flags)
{
    assert(NULL != context);
    ZeroMemory(context, sizeof(CONTEXT));
    context->ContextFlags = flags;
    if (threadId != GetCurrentThreadId()) {
        SuspendThreadAndGetContext(thread, context);
    }
    else {
        PRUN_CONTEXT rc = NULL;
        capture_run_context(&rc);
        memcpy(context, &rc->context, sizeof(*context));
        free_run_context(rc);
    }
}
