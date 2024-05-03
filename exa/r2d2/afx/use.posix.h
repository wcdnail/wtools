#ifndef aw_use_posix_h__
#define aw_use_posix_h__

#include <unistd.h>
#include "error.policy.h"

inline std::string to_string(Exception::ErrorCode const& e)
{
    return Exception::ErrnoToString(e.value());
}

#endif // aw_use_posix_h__
