/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Replacement functions for building without linking to the VC CRT
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "mincrt.h"

static
char ttohexdigit(TCHAR c)
{
    if (c < TEXT('0'))
        return -1;

    if (c <= TEXT('9'))
        return (char)(c - TEXT('0'));

    if (c < TEXT('A'))
        return -1;

    if (c <= TEXT('F'))
        return 10 + (char)(c - TEXT('A'));

    if (c < TEXT('a'))
        return -1;

    if (c <= TEXT('f'))
        return 10 + (char)(c - TEXT('a'));

    return -1;
}

unsigned int ttoui(_In_ TCHAR *s, _Out_opt_ TCHAR **endptr)
{
    if (!s)
    {
        if (endptr)
            *endptr = NULL;
        return 0;
    }

    unsigned int n = 0;

    while (*s)
    {
        if (s[0] < TEXT('0') || s[0] > TEXT('9'))
            break;

        n = n * 10 + s[0] - (char)TEXT('0');
        s++;
    }

    if (endptr)
        *endptr = s;
    return n;
}

unsigned long thextoul(_In_ TCHAR *s, _Out_opt_ TCHAR **endptr)
{
    if (!s)
    {
        if (endptr)
            *endptr = NULL;
        return 0;
    }

    unsigned long n = 0;
    char digit;

    while (*s)
    {
        digit = ttohexdigit(s[0]);
        if (digit < 0)
            break;

        n = n * 16 + digit;
        s++;
    }

    if (endptr)
        *endptr = s;
    return n;
}

/* Supports hexadecimal numbers if they start with 0x or 0X. */
unsigned long ttoul(_In_ TCHAR *s, _Out_opt_ TCHAR **endptr)
{
    if (!s)
    {
        if (endptr)
            *endptr = NULL;
        return 0;
    }

    if (s[0] == TEXT('0') &&
        (s[1] == TEXT('x') || s[1] == TEXT('X')))
    {
        return thextoul(&s[2], endptr);
    }

    unsigned long n = 0;

    while (*s)
    {
        if (s[0] < TEXT('0') || s[0] > TEXT('9'))
            break;

        n = n * 10 + s[0] - (char)TEXT('0');
        s++;
    }

    if (endptr)
        *endptr = s;
    return n;
}

_Success_(return != NULL)
TCHAR *tcsdup(_In_ const TCHAR *_Src)
{
    if (!_Src)
        return NULL;

    HANDLE heap = GetProcessHeap();
    if (!heap)
        return NULL;

    size_t len = ((size_t)lstrlen(_Src)) + 1;
    TCHAR *_Dst = (TCHAR *)HeapAlloc(heap, 0, len * sizeof(TCHAR));
    if (!_Dst)
        return NULL;

#if 0
    TCHAR *tmp = _Dst;
    while (*_Src != TEXT('\0'))
        *tmp++ = *_Src++;
    *tmp = TEXT('\0');
#else
    if (!lstrcpy(_Dst, _Src))
        return NULL;
#endif

    return _Dst;
}

/* These functions could be slower than the CRT implementations */

#if defined(_MSC_VER)
#pragma function(memcpy)
#define min_memcpy memcpy
_Post_equal_to_(_Dst)
_At_buffer_(
    (unsigned char *)_Dst,
    _Iter_,
    _Size,
    _Post_satisfies_(((unsigned char *)_Dst)[_Iter_] ==
        ((unsigned char *)_Src)[_Iter_])
)
void *min_memcpy(
#else  /* defined(__GNUC__) */
__attribute__((always_inline))
inline void *memcpy(
#endif
    _Out_writes_bytes_all_(_Size) void *_Dst,
    _In_reads_bytes_(_Size)       const void *_Src,
    _In_                          size_t      _Size
)
{
#if defined(_MSC_VER)
    /* Ignore unitialized memory warning */
#pragma warning(push)
#pragma warning(disable: 6001)
#endif

    if (_Size % sizeof(long) == 0)
    {
        _Size /= sizeof(long);
        unsigned long *_Dst32 = (unsigned long *)_Dst;
        const unsigned long *_Src32 = (unsigned long *)_Src;

        while (_Size--)
            *(_Dst32++) = *(_Src32++);
    }
    else
    {
        unsigned char *_Dst8 = (unsigned char *)_Dst;
        const unsigned char *_Src8 = (unsigned char *)_Src;

        while (_Size--)
            *(_Dst8++) = *(_Src8++);
    }
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

    return _Dst;
}

#if defined(_MSC_VER)
#pragma function(memset)
#define min_memset memset
_Post_equal_to_(_Dst)
_At_buffer_(
    (unsigned char *)_Dst,
    _Iter_,
    _Size,
    _Post_satisfies_(((unsigned char *)_Dst)[_Iter_] == _Val)
)
void *min_memset(
#else  /* defined(__GNUC__) */
__attribute__((optimize(1)))  /* Workaround crash with -O2 */
__attribute__((always_inline))
inline void *memset(
#endif
    _Out_writes_bytes_all_(_Size) void *_Dst,
    _In_                          int    _Val,
    _In_                          size_t _Size
)
{
#if defined(_MSC_VER)
    /* Ignore unitialized memory warning */
#pragma warning(push)
#pragma warning(disable: 6001)
#endif
    unsigned char *_Dst8 = (unsigned char *)_Dst;
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

    while (_Size--)
        *_Dst8++ = (unsigned char)_Val;

    return _Dst;
}

#if defined(_MSC_VER)
#pragma function(memcmp)
#define min_memcmp memcmp
_NODISCARD
#endif
_Check_return_
int __cdecl min_memcmp(
    _In_reads_bytes_(_Size) void const *_Buf1,
    _In_reads_bytes_(_Size) void const *_Buf2,
    _In_                    size_t      _Size
)
{
    if (!_Size)
        return 0;

    while (--_Size && *(unsigned char *)_Buf1 == *(unsigned char *)_Buf2)
    {
        _Buf1 = (unsigned char *)_Buf1 + 1;
        _Buf2 = (unsigned char *)_Buf2 + 1;
    }

    return(*((unsigned char *)_Buf1) - *((unsigned char *)_Buf2));
}
