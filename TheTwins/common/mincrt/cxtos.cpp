#include "stdafx.h"
#include "cdefines.h"
#include <niceday/strings.xtoa.h>

#define _Implement_XtoS(Name, Ct, It, Rt)                               \
    _extrnc Ct* Name(It x, Ct* dest, Rt radix)                          \
    {                                                                   \
        using namespace Strings;                                        \
        ConvertFrom<It>(x, radix, dest, _SizeMax);                      \
        return dest;                                                    \
    }

#define _Implement_XtoS_s(Name, Ct, It, Rt)                             \
    _extrnc errno_t Name(It x, Ct* dest, size_t maxd, Rt radix)         \
    {                                                                   \
        using namespace Strings;                                        \
                                                                        \
        return ConvertFrom<It>(x, radix, dest, _SizeMax) ? 0 : ERANGE;  \
    }

_Implement_XtoS(itoa, char, int, int);
_Implement_XtoS(_itoa, char, int, int);
_Implement_XtoS(_i64toa, char, int64_t, int);
_Implement_XtoS(_ui64toa, char, uint64_t, int);
_Implement_XtoS(_itow, wchar_t, int, int);
_Implement_XtoS(_i64tow, wchar_t, int64_t, int);
_Implement_XtoS(_ui64tow, wchar_t, uint64_t, int);
_Implement_XtoS_s(_itoa_s, char, int, int);
_Implement_XtoS_s(_i64toa_s, char, int64_t, int);
_Implement_XtoS_s(_ui64toa_s, char, uint64_t, int);
_Implement_XtoS_s(_itow_s, wchar_t, int, int);
_Implement_XtoS_s(_i64tow_s, wchar_t, int64_t, int);
_Implement_XtoS_s(_ui64tow_s, wchar_t, uint64_t, int);
