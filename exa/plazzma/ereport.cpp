#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// 2012
#include <cstdarg>
#include <strsafe.h>
// EO2012

namespace
{
    static FILE* gLogFile = NULL;

    enum Hardcoded
    {
        MaxLogLen = 60,
    };
}

void elogOpen(char const* name) 
{ 
    gLogFile = ::fopen(name, "wt");
}

void elogClose(void) 
{
    if (NULL != gLogFile) 
    {
        fclose(gLogFile); 
        gLogFile = NULL;
    }
}

void elogMessage(char const* what, int x)
{
    char msg[8192];
    ::StringCchCopyA(msg, _countof(msg), what);

    size_t len = 0;
    ::StringCchLengthA(msg, _countof(msg), &len);

    size_t i = 0;

    for (i = len; i<MaxLogLen; i++) 
        msg[i] = '.';

    msg[i] = 0;

    char status[256];
    switch (x) 
    {
    case 0:
        ::StringCchCopyA(status, _countof(status), "[ OK ]");
        break;

    default:
    {
        char sysmsg[1024] = {0};
        DWORD sl = ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, x, 0, sysmsg, sizeof(sysmsg)-1, NULL);
        if (sl > 1)
            sysmsg[sl - 1] = 0;

        ::StringCchPrintfA(status, _countof(status), "[ัมฮษ] 0x%x%s%s"
            , x
            , (sl > 1 ? " - " : "")
            , (sl > 1 ? sysmsg : "")
            );
    }
    }

    ::StringCchCatA(msg, _countof(msg), status);
    ::StringCchCatA(msg, _countof(msg), "\n");

    if (NULL != gLogFile)
        fputs(msg, gLogFile);

    ::OutputDebugStringA(msg); // 2012
}

// 2012
void elogMessage(int x, char const* format, ...)
{
    char buffer[4096] = {0};

    va_list ap;
    va_start(ap, format);
    ::StringCchVPrintfA(buffer, _countof(buffer), format, ap);
    va_end(ap);

    elogMessage(buffer, x);
}
