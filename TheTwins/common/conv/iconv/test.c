#define WIN32_LEAN_AND_MEAN
#define LIBICONV_PLUG
#define USE_DOS

#pragma warning(push)
#pragma warning(disable: 4018 4244)

#include <stdlib.h>
#include <string.h>
#include <windows.h>

struct loop_funcs
{
    size_t (*loop_convert)(void* icd, const char** inbuf, size_t *inbytesleft, char** outbuf, size_t *outbytesleft);
    size_t (*loop_reset)(void* icd, char** outbuf, size_t *outbytesleft);
};

#include "converters.h"

#pragma warning(pop)
