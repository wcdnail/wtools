#pragma once

#include <type_traits>

// warning C4312: 'type cast': conversion from 'unsigned int' to 'void *' of greater size

template <typename TargetType, typename SourceType>
inline TargetType cast_ptr_to_ptr(SourceType from)
{
    return reinterpret_cast<TargetType>(reinterpret_cast<ptrdiff_t>(from));
}

template <typename TargetType, typename SourceType>
inline TargetType cast_to_ptr(SourceType from)
{
    return reinterpret_cast<TargetType>(static_cast<ptrdiff_t>(from));
}

template <typename TargetType, typename SourceType>
inline TargetType cast_from_ptr(SourceType from)
{
    return static_cast<TargetType>(reinterpret_cast<ptrdiff_t>(from));
}
