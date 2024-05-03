#pragma once

#define CYGWIN_VERSION_DLL_MAJOR    1007
#define CYGWIN_VERSION_API_MINOR    60

#ifndef __cplusplus
#define inline
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4005) // macro redefinition
#pragma warning(disable: 4028) // formal parameter 1 different from declaration
#pragma warning(disable: 4996) // This function or variable may be unsafe....
#pragma warning(disable: 4129) // unrecognized character escape sequence
#pragma warning(disable: 4244) // 'initializing' : conversion from 'double' to 'int' ...

#endif 

#include <math.h>

#ifdef __cplusplus
#include <atlstr.h>
#endif
