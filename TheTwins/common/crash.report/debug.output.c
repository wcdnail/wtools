#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500

#include "debug.output.h"
#include <windows.h>
#include <stdio.h>
#include <malloc.h>

static void dbg_print_impl_static(char const* message, va_list ap)
{
    char output[4096];
    _vsnprintf_s(output, sizeof(output), _TRUNCATE, message, ap);
    OutputDebugStringA(output);
}

static void dbg_print_impl_calloc(char const* message, va_list ap)
{
    int len = _vscprintf(message, ap);
    if (len > 0)
    {
        char* output = (char*)calloc(len+1, sizeof(char));
        if (NULL != output)
        {
            _vsnprintf_s(output, len, _TRUNCATE, message, ap);
            OutputDebugStringA(output);
            free(output);
        }
    }
}

#define _DBGPRINTIMPL dbg_print_impl_static

void dbg_printf(char const* message, ...)
{
    va_list ap = NULL;
    va_start(ap, message);
    _DBGPRINTIMPL(message, ap);
    va_end(ap);
}

void dbg_printfc(int code, char const* message, ...)
{
    HLOCAL local = NULL;
    DWORD len = 0;
    va_list ap = NULL;

    va_start(ap, message);
    _DBGPRINTIMPL(message, ap);
    va_end(ap);

    len = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL, (DWORD)code, 0, (char*)&local, 0, NULL);

    if (len > 2)
    {
        char* message = (char*)LocalLock(local);
        message[len-2] = '\n';
        message[len-1] = 0;
        LocalUnlock(local);

        OutputDebugStringA(message);
        LocalFree(message);
    }
}

