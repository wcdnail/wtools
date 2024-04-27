#pragma once

#include <type_traits>

template <typename TargetType, typename SourceType>
inline TargetType brute_cast(SourceType&& from)
{
    auto     temp = reinterpret_cast<void**>(from);
    TargetType rv = reinterpret_cast<TargetType>(temp);
    return     rv;
    //return reinterpret_cast<TargetType>((TargetType)from);
}
