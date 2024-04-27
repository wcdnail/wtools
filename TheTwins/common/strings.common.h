#ifndef _nd_strings_common_h__
#define _nd_strings_common_h__

#include "null.pointee.h"
#include <cstddef>

namespace Strings
{
    typedef int Error;

    static const size_t _SizeMax = (size_t)-1;

    enum ErrorCodes
    {
        NoError = 0,
        StringsAreEqual = 0,
        Truncated,
        SourcePointerIsNull,
        DestinationPointerIsNull,
        NotEnoughSpaceInDestination,
        FindMatch,
        StringTerminated,
    };

    static inline void _SetError(Error* ec, Error code)
    {
        if (!_is_null_ptr(ec))
            *ec = code;
    }
}

#endif /* _nd_strings_common_h__ */
