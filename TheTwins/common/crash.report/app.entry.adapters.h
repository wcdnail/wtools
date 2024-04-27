#ifndef _crash_report_app_entry_adapters_h__
#define _crash_report_app_entry_adapters_h__

#include "crash.report.h"
#include "run.context.info.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif 
#include <windows.h>
#include <winnt.h>
#include <tchar.h>

int _StructExceptionFilter(PEXCEPTION_POINTERS);
int AppEntry(HINSTANCE instance);

#ifdef _CONSOLE
int _tmain(int, TCHAR const*)
{
    HINSTANCE instance = GetModuleHandle(NULL);
#else
int WINAPI _tWinMain(HINSTANCE instance, HINSTANCE, PTSTR, int)
{
#endif
    int result = 0;

    SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    DefWindowProc(NULL, 0, 0, 0L);

    __try
    {
        result = AppEntry(instance);
    }
    __except(_StructExceptionFilter(GetExceptionInformation()))
    {
        result = GetExceptionCode();
    }

    return result;
}

static int _StructExceptionFilter(PEXCEPTION_POINTERS ep)
{
    // HARDCODE: Локализация 
    char message[512] = "Необработанная ошибка: 0x"; 
    char buffer[256] = {0};
    RUN_CONTEXT context = { GetCurrentProcess(), GetCurrentThread(), *ep->ContextRecord, 0 };
    unsigned code = ep->ExceptionRecord->ExceptionCode;

    strcat(message, _ultoa(code, buffer, 16));
    strcat(message, " ");
    strcat(message, exception_code_string(code));
    
    crash_report(message, NULL, 0, GetLastError(), &context, 0);

    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

#endif /* _crash_report_app_entry_adapters_h__ */

