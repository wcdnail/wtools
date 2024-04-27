#ifndef _nd_strings_xftoa_h__
#define _nd_strings_xftoa_h__

#include "strings.common.h"
#include "digimagix.simple.h"

namespace Strings
{
    const int InvaildDotPos = -1;

    /**
     * ѕреобразует вещественное число в строку.
     */
    template <typename T, typename C>
    size_t ConvertFrom(T value
                     , int digitCount
                     , C dotSymbol
                     , C* buffer
                     , size_t bufferLen
                     , Error* error = NULL)
    {
        if (!buffer)
        {
            _SetError(error, DestinationPointerIsNull);
            return 0;
        }

        bool isNegative = value < 0.;
        if (isNegative)
            value = -value;

        T divider;
        int power = DmPowerOf(value, divider);

        int decimals = power >= 0 ? power + 1 : 0;
        digitCount += decimals;
        if ((bufferLen < (size_t)(digitCount + 2)))
        {
            _SetError(error, NotEnoughSpaceInDestination);
            return 0;
        }

        C* converted = buffer;

        if (isNegative)
            *converted++ = (C)'-';

        int dotPos = InvaildDotPos;
        if (value < 1.)
            *converted++ = (C)'0';

        for (int i=0; i<digitCount; ++i, divider /= 10.)
        {
            if ((InvaildDotPos == dotPos) && (value < 1.))
            {
                dotPos = (int)(converted - buffer);
                *converted++ = dotSymbol;
            }

            int n = (int)(value / DmDivider(divider));
            *converted++ = (C)('0' + n);

            value -= divider * n;
        }

        *converted++ = 0;

        _SetError(error, NoError);
        return converted - buffer - 1;
    }
}

#endif /* _nd_strings_xftoa_h__ */
