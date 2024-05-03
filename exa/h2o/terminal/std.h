#ifndef STD_H
#define STD_H

#include <cygwin/version.h>

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <wchar.h>
#include <errno.h>

#if CYGWIN_VERSION_API_MINOR >= 74
#include <wctype.h>
#else
int iswalnum(wint_t);
int iswalpha(wint_t);
int iswspace(wint_t);
#endif

#if CYGWIN_VERSION_API_MINOR < 53
#define strlcpy(dst, src, len) snprintf(dst, len, "%s", src)
#endif

#if CYGWIN_VERSION_API_MINOR < 70
int asprintf(char **, const char *, ...);
int vasprintf(char **, const char *, va_list);
#endif
char *asform(const char *fmt, ...);

#define WINVER 0x500  // Windows 2000
#define _WIN32_WINNT WINVER
#define _WIN32_IE WINVER

#include <windef.h>

#ifdef DMALLOC
#include <dmalloc.h>
#endif

#define always_inline __attribute__((always_inline)) inline
#ifdef _MSC_VER
#define unused(arg)
#else
#define unused(arg) unused_##arg __attribute__((unused))
#endif 
#define no_return __attribute__((noreturn)) void

typedef uint xchar;     // UTF-32
typedef wchar_t wchar;  // UTF-16

typedef signed char schar;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

typedef const char *string;
typedef const wchar *wstring;

typedef void (*void_fn)(void);

#define null NULL

#define __W(s) L##s
#define _W(s) __W(s)

#define lengthof(array) (sizeof(array) / sizeof(*(array)))
#define endof(array)    (&(array)[lengthof(array)])

#define _new(type) ((type *)malloc(sizeof(type)))
#define _newn(type, n) ((type *)calloc((n), sizeof(type)))
#define _renewn(p, n) ((typeof(p)) realloc((p), sizeof(*p) * (n)))
static inline void _delete(const void *p) { free((void *)p); }

void strset(string *sp, string s);

#define when break; case
#define or : case
#define otherwise break; default

#ifdef TRACE
#define trace(xs...) \
    printf("%s:%u:%s:", __FILE__, __LINE__, __func__); \
    printf(" " xs); \
    putchar('\n')
#else
inline void trace(char const*, ...) {}
#endif

template <class T>
inline bool sgn(T x) { T x_ = x; return (x_ > 0) - (x_ < 0); }

template <class T>
inline T sqr(T x) { T x_ = x; return x_ * x_; } 

#include <boost/scoped_array.hpp>

#endif // STD_H
