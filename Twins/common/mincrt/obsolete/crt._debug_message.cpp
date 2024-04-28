#include "stdafx.h"
#include "crt.report.h"

#ifdef _DEBUG
namespace std
{
    void __cdecl _Debug_message(wchar_t const* message, wchar_t const* file, unsigned int line)
    {
        ::DTraceW(L"MinCrt: %s (%s, %d)\n", message, file, line);
    }
}
#endif
