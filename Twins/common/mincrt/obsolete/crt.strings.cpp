#include "stdafx.h"

#pragma function(memset)
extern "C" void* memset(void* dst, int val, size_t count)
{
    if (!dst || (count == 0))
        return dst;

    void* start = dst;
    unsigned char* dest = (unsigned char*)dst;

    while (count--) 
        *dest++ = (unsigned char)val;

    return start;
}

#pragma function(strlen)
extern "C" size_t strlen(char const* str)
{
    const char *eos = str;
    while (*eos++) ;
    return eos - str - 1;
}

#pragma function(strcmp)
extern "C" int __cdecl strcmp(char const* src, char const* dst)
{
    int ret = 0;

    while( !(ret = *(unsigned char *)src - *(unsigned char *)dst) && *dst)
        ++src, ++dst;

    if (ret < 0)
        ret = -1;

    else if (ret >0)
        ret = 1 ;

    return ret;
}

extern "C" errno_t strcpy_s(char* dst, size_t size, char const* src)
{
    if (!dst || !src)
        return EINVAL;

    char* p = dst;
    int available = (int)size;

    while ((*p++ = *src++) != 0 && (--available > 0)) 
        ;

    return 0;
}
