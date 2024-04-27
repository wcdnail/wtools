#pragma once

#define mbstowcs_s(v, w, x, y, z) mbstowcs(w, y, z)
#define wcstombs_s(v, w, x, y, z) wcstombs(w, y, z)
#define wcsncpy_s wcsncpy
#ifndef _ASSERT
  #define _ASSERT(x)
#endif
#define max(x, y) ((x > y) ? x : y)
#define min(x, y) ((x < y) ? x : y)
#ifdef __STRICT_ANSI__
char *_strdup(const char *str)
{
    int n = strlen(str) + 1;
    char *dup = reinterpret_cast<char *>(malloc(n));
    if (dup)
    { strcpy(dup, str); }
    return dup;
}
#endif

namespace std { namespace tr1 { using namespace std; } }
#include <OleAcc.h>

#ifndef LWS_TRANSPARENT
  #define LWS_TRANSPARENT     0x0001
  #define LWS_IGNORERETURN    0x0002
  //#if _WIN32_WINNT >= 0x0600
  #define LWS_NOPREFIX        0x0004
  #define LWS_USEVISUALSTYLE  0x0008
  #define LWS_USECUSTOMTEXT   0x0010
  #define LWS_RIGHT           0x0020
  //#endif
#endif
#ifndef EM_SETCUEBANNER
  #define EM_SETCUEBANNER       0x1501     // Set the cue banner with the lParm = LPCWSTR
#endif
#ifndef BS_DEFCOMMANDLINK
  #define BS_DEFCOMMANDLINK 0x0000000F
#endif