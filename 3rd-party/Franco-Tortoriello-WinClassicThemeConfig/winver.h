#pragma once
#if !defined(WINVER_H)
#define WINVER_H

/* Supported Windows version targets.
 * Building for a particular version removes features from later versions.
 */
#define WINVER_NT4    0x0400  /* _WIN32_WINNT_NT4,   same features as 95 */
#define WINVER_2K     0x0500  /* _WIN32_WINNT_WIN2K, mostly same as 98 / ME */
#define WINVER_XP     0x0501  /* _WIN32_WINNT_WINXP, same features as 2003 */
#define WINVER_VISTA  0x0600  /* _WIN32_WINNT_VISTA, same as 2008 */
#define WINVER_7      0x0601  /* _WIN32_WINNT_WIN7,  same as 2008 R2 */
#define WINVER_8      0x0602  /* _WIN32_WINNT_WIN8,  same features as 8.1 */
#define WINVER_10     0x0A00  /* _WIN32_WINNT_WIN10, same as 11 */

/* Define WINVER and _WIN32_WINNT if necessary */
#if defined(_WIN32_WINNT)
#if !defined(WINVER)
#define WINVER _WIN32_WINNT
#endif
#elif defined(WINVER)
#define _WIN32_WINNT WINVER
#else
/* Default to this version */
#define _WIN32_WINNT WINVER_10
#define WINVER _WIN32_WINNT
#endif

/* If a Unicode build is requested, make sure both _UNICODE and UNICODE are
 * defined.
 */
#if defined(_UNICODE)
#if !defined(UNICODE)
#define UNICODE
#endif

#elif defined(UNICODE)
#if !defined(_UNICODE)
#define _UNICODE
#endif

#else

/* Neither UNICODE nor _UNICODE defined */
#if WINVER >= WINVER_XP
#define UNICODE
#define _UNICODE
#endif

#endif

/* Workarounds to distinguish 9x from NT */

#if !defined(WINVER_IS_95) && WINVER == WINVER_NT4 && !defined(UNICODE)
#define WINVER_IS_95
#endif

#if !defined(WINVER_IS_98) && WINVER == WINVER_2K && !defined(UNICODE)
/* Workaround to distinguish 98 from 2000 */
#define WINVER_IS_98
#endif

#if !defined(_WIN32_IE)
/* Set _WIN32_IE to the lowest supported IE version by the OS.
 * Used by common controls.
 */
#if WINVER >= WINVER_8
#define _WIN32_IE 0x0A00  /* _WIN32_IE_IE100 / IE110 */
#elif WINVER >= WINVER_7
#define _WIN32_IE 0x0800  /* _WIN32_IE_IE80 */
#elif WINVER >= WINVER_VISTA
#define _WIN32_IE 0x0700  /* _WIN32_IE_IE70 */
#elif WINVER >= WINVER_XP
#define _WIN32_IE 0x0600  /* _WIN32_IE_IE60 */
#else
/* Minimum supported version: comctl32.dll v5.80 */
#define _WIN32_IE 0x0500  /* _WIN32_IE_IE50 */
#endif
#endif

#if WINVER < WINVER_VISTA
/* Undefined for older versions */
#define USER_DEFAULT_SCREEN_DPI 96
#endif

#endif  /* !defined(WINVER_H) */
