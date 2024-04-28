#include "stdafx.h"
#include "crt.report.h"

int __cdecl _CrtDbgReportW(_In_ int type
                         , _In_opt_z_ const unsigned short* source
                         , _In_ int line
                         , _In_opt_z_ const unsigned short* module
                         , _In_opt_z_ const unsigned short* format
                         , ...)
{
    va_list ap;
    va_start(ap, format);
    TraceW(L"MinCrt: %d %s(%d): %s %d ", type, source, module);
    TraceWV((wchar_t const*)format, ap);
    va_end(ap);
    return 0;
}
