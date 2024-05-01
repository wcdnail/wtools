#pragma once
#if !defined(MINCRT_H)
#define MINCRT_H

#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

unsigned int ttoui(_In_ TCHAR *s, _Out_opt_ TCHAR **endptr);

unsigned long thextoul(_In_ TCHAR *s, _Out_opt_ TCHAR **endptr);

unsigned long ttoul(_In_ TCHAR *s, _Out_opt_ TCHAR **endptr);

_Success_(return != NULL)
TCHAR *tcsdup(_In_ const TCHAR *_Src);

#if !defined(_MSC_VER)

void *min_memcpy(void *_Dst, const void *_Src, size_t _Size);

void *min_memset(void *_Dst, int _Val, size_t _Size);

int __cdecl min_memcmp(void const *_Buf1, void const *_Buf2, size_t _Size);

#define memcmp min_memcmp

#endif  /* !defined(_MSC_VER) */

#if !defined(_countof)
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif

#endif  /* !defined(MINCRT_H) */
