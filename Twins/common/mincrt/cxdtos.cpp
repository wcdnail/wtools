#include "stdafx.h"
#include "cdefines.h"
#include <niceday/strings.xftoa.h>

// TODO: _loc_update.GetLocaleT()->locinfo)->decimal_point

_extrnc char* _ccdecl gcvt(double x, int count, char* dest)
{
    Strings::ConvertFrom<double>(x, count, '.', dest, Strings::_SizeMax);
    return dest;
}

_extrnc char* _ccdecl _gcvt(double x, int count, char* dest)
{
    Strings::ConvertFrom<double>(x, count, '.', dest, Strings::_SizeMax);
    return dest;
}

_extrnc errno_t _ccdecl _gcvt_s(char* dest, size_t maxd, double x, int count)
{
    return Strings::ConvertFrom<double>(x, count, '.', dest, maxd) ? 0 : ERANGE;
}
