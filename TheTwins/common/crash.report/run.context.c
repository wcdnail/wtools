#include "run.context.h"
#include <assert.h>

int get_last_error_code(void)
{
    return ((int)GetLastError());
}

RUN_CONTEXT capture_context(void)
{
    DWORD lerr = GetLastError();

    void (__stdcall *PRtlCaptureContext)(PCONTEXT);

    RUN_CONTEXT rc;
    CONTEXT context;

    PRtlCaptureContext = (void (__stdcall *)(PCONTEXT))
        GetProcAddress(GetModuleHandleW(L"kernel32"), "RtlCaptureContext");

    if (NULL != PRtlCaptureContext)
    {
        ZeroMemory(&context, sizeof(context));
        PRtlCaptureContext(&context);
    }
#ifdef _M_IX86
    else
    {
        _asm    call    x
        _asm x: pop     eax
        _asm    mov     context.Eip, eax
        _asm    mov     context.Ebp, ebp
        _asm    mov     context.Esp, esp
    }
#endif // _M_IX86

    rc.pid      = GetCurrentProcess();
    rc.tid      = GetCurrentThread();
    rc.context  = context;
    rc.skip     = 1;

    SetLastError(lerr);
    return rc;
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

    if (threadId != GetCurrentThreadId())
        SuspendThreadAndGetContext(thread, context);
    else
    {
        RUN_CONTEXT rc = capture_context();
        memcpy(context, &rc.context, sizeof(*context));
    }
}
