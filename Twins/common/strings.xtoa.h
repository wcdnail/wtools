#ifndef _nd_strings_xtoa_h__
#define _nd_strings_xtoa_h__

#include "strings.common.h"
#include "digimagix.simple.h"

namespace Strings
{
    template <typename T, typename C>
    inline size_t _ConvertFromn(T value
                              , int radix
                              , C* buffer
                              , size_t bufferLen
                              , bool isNegative
                              , size_t digitCount
                              , bool upperCase
                              , Error* error = NULL
    )
    {
        if (!value)
        {
            *buffer++ = (C)'0';
            *buffer = 0;
            return 1;
        }

        C* converted = buffer + digitCount;
        C firstA = (upperCase ? 'A' : 'a');
        *converted-- = 0;

        while (value) 
        {
            T digit = DmRemainder<T>(value, radix);
            value = DmDiv<T>(value, radix);
            C litera;

            if (digit > 9) 
                litera = (C)((digit - 10) + firstA);
            else
                litera = (C)(digit + (C)'0');

            *converted-- = litera;
        }

        if (isNegative && (10 == radix))
            *converted-- = (C)'-';

        return digitCount;
    }

    template <typename T, typename C>
    inline size_t ConvertFrom(T value
                            , int radix
                            , C* buffer
                            , size_t bufferLen
                            , bool upperCase = false
                            , Error* error = NULL
    )
    {
        if (::_is_null_ptr(buffer))
        {
            _SetError(error, DestinationPointerIsNull);
            return 0;
        }

        if (bufferLen < 2)
        {
            _SetError(error, NotEnoughSpaceInDestination);
            return 0;
        }

        const bool isNegative = value < 0;
        T absv = isNegative ? -value : value;

        size_t digitCount = DmDigitCount(absv, radix, isNegative);
        if (digitCount > bufferLen - 1)
        {
            _SetError(error, NotEnoughSpaceInDestination);
            return 0;
        }

        return _ConvertFromn(absv, radix, buffer, bufferLen, isNegative, digitCount, upperCase, error);
    }
}

#endif /* _nd_strings_xtoa_h__ */
