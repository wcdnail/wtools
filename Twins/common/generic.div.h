#pragma once

namespace Cf
{
    template <class T>
    inline T SafeDiv(T value, T divisor)
    {
        return value / (0 == divisor ? (T)1 : divisor);
    }
}
