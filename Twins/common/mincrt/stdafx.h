#ifndef _mincrt_stdafx_h__
#define _mincrt_stdafx_h__

#define BOOST_DISABLE_ASSERTS
#define STRICT
#define WIN32_LEAN_AND_MEAN

#ifdef _WIN32
#include <sdkddkver.h>
#include <windows.h>
#include <dbghelp.h>
#include <tchar.h>
#include <intrin.h>
#endif 

#include <malloc.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
#include <typeinfo>
#include <exception>
#include <stdexcept>
#endif /* _cplusplus */

#endif /* _mincrt_stdafx_h__ */
