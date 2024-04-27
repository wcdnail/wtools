#ifndef _cf_using_iconv_h__
#define _cf_using_iconv_h__

#define LIBICONV_PLUG
#define USE_DOS

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4018) /* '<' : signed/unsigned mismatch */
#pragma warning(disable: 4244) /* 'return' : conversion from '__int64' to 'int', possible loss of data */
#endif /* _MSC_VER */

#ifdef __cplusplus
namespace iconv {
#endif /* __cplusplus */

struct loop_funcs
{
    size_t (*loop_convert)(void* icd, const char** inbuf, size_t *inbytesleft, char** outbuf, size_t *outbytesleft);
    size_t (*loop_reset)(void* icd, char** outbuf, size_t *outbytesleft);
};

#include "iconv/converters.h"

#ifdef __cplusplus
} // namespace Iconv
#endif /* __cplusplus */

#ifdef _MSC_VER
#pragma warning(pop)
#endif /* _MSC_VER */

#endif /* _cf_using_iconv_h__ */
