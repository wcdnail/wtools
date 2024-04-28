#include "stdafx.h"
#include "cdefines.h"
#include <niceday/strings.length.h>
#include <niceday/strings.copy.h>
#include <niceday/strings.compare.h>

#ifdef _MSC_VER 
// Suppress compiler: `intrinsic function, cannot be defined`
#pragma function(memset)
#pragma function(strlen)
#pragma function(strcpy)
#pragma function(strcat)
#pragma function(strcmp)
#pragma function(wcslen)
#pragma function(wcscpy)
#pragma function(wcscat)
#pragma function(wcscmp)
#endif

_extrnc void* memset(void* dest, int x, size_t size) 
{
#if _bits == 64
#undef RtlFillMemory
    __declspec(dllimport) void RtlFillMemory(void *, size_t, char);

    RtlFillMemory(dest, size, (char)x);
#else
    char* p = (char*)dest;
    while (size--) 
        *p++ = (char)x;
#endif

    return dest;
}

_extrnc size_t strlen(char const* source) 
{
    return Strings::Length(source); 
}

_extrnc char* strcpy(char* dest, char const* source) 
{
    return Strings::Copy(dest, Strings::_SizeMax, source, Strings::_SizeMax); 
}

_extrnc errno_t strcpy_s(char* dest, size_t maxd, char const* source) 
{
    errno_t ec;
    Strings::Copy(dest, maxd, source, Strings::_SizeMax, &ec);
    return ec;
}

_extrnc char* strcat(char* dest, char const* source) 
{
    return Strings::Copy(dest + Strings::Length(dest), Strings::_SizeMax, source, Strings::_SizeMax); 
}

_extrnc errno_t strcat_s(char* dest, size_t maxd, char const* source) 
{
    errno_t ec = ENOSPC;

    size_t ld = Strings::Length(dest, maxd);
    if ((ld + 1) < maxd)
        Strings::Copy(dest + ld, maxd - ld, source, Strings::_SizeMax, &ec);

    return ec;
}

_extrnc size_t wcslen(wchar_t const* source) 
{
    return Strings::Length(source); 
}

_extrnc wchar_t* wcscpy(wchar_t* dest, wchar_t const* source) 
{
    return Strings::Copy(dest, Strings::_SizeMax, source, Strings::_SizeMax); 
}

_extrnc errno_t wcscpy_s(wchar_t* dest, size_t maxd, wchar_t const* source) 
{
    errno_t ec;
    Strings::Copy(dest, maxd, source, Strings::_SizeMax, &ec); 
    return ec;
}

_extrnc wchar_t* wcscat(wchar_t* dest, wchar_t const* source) 
{
    return Strings::Copy(dest + Strings::Length(dest), Strings::_SizeMax, source, Strings::_SizeMax); 
}

_extrnc errno_t wcscat_s(wchar_t* dest, size_t maxd, wchar_t const* source) 
{
    errno_t ec = ENOSPC;

    size_t ld = Strings::Length(dest, maxd);
    if ((ld + 1) < maxd)
        Strings::Copy(dest + ld, maxd - ld, source, Strings::_SizeMax, &ec);

    return ec;
}
