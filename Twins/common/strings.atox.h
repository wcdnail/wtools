#ifndef _nd_strings_atox_h__
#define _nd_strings_atox_h__

#include "strings.length.h"

namespace Strings
{
    /**
     * Преобразует строку в целое (учитывая систему счиления `radix`).
     */
    template <typename T, typename C>
    inline T ConvertFrom(C const* value, size_t len, T radix)
    { 
        if (_is_null_ptr(value))
            return (T)-1;

        if (_SizeMax == len)
            len = Length(value, len);

        C const* start = value;
        C const* stop = value + len;
    
        while ((start < stop) && (*start == (C)' ')) 
            ++start;
    
        T sign = 1;
        if (*start == '-')
        {
            sign = -1;
            ++start;
        }

        T result = 0;

        while (start < stop)
        {
            T digit = (T)*start++;
    
            if (digit >= (T)'a') 
                digit -= ((C)'a' + (C)'9' + 1);

            else if (digit >= (T)'A') 
                digit -= ((C)'A' + (C)'9' + 1);

            digit -= '0';
    
            result *= radix;
            result += (T)digit;
        }
    
        return result * sign;
    }
}

#endif /* _nd_strings_atox_h__ */
