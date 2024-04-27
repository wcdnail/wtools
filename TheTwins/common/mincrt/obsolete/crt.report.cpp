#include "stdafx.h"
#include <strsafe.h>

extern "C" void TraceAV(char const* format, va_list ap)
{
    char traceBufferA[2048];
    if (SUCCEEDED(::StringCbVPrintfA(traceBufferA, _countof(traceBufferA), format, ap)))
        ::OutputDebugStringA(traceBufferA);
}

extern "C" void TraceWV(wchar_t const* format, va_list ap)
{
    wchar_t traceBufferW[2048];
    if (SUCCEEDED(::StringCbVPrintfW(traceBufferW, _countof(traceBufferW), format, ap)))
        ::OutputDebugStringW(traceBufferW);
}

extern "C" void TraceA(char const* format, ...)
{
    va_list ap;
    va_start(ap, format);
    TraceAV(format, ap);
    va_end(ap);
}

extern "C" void TraceW(wchar_t const* format, ...)
{
    va_list ap;
    va_start(ap, format);
    TraceWV(format, ap);
    va_end(ap);
}
