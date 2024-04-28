#include "stdafx.h"
#include "crt.report.h"

extern "C" 
void __cdecl _invalid_parameter(wchar_t const* expression
                              , wchar_t const* function
                              , wchar_t const* source
                              , unsigned int line
                              , uintptr_t pointee)
{
    ::DTraceW(L"MinCrt: `%s` %s(%d): %s %d\n", expression, source, line, function, pointee);
}
