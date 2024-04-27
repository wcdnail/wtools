#pragma once

#include "wcdafx.api.h"

namespace Dh
{
    class Timer
    {
    public:
        WCDAFX_API Timer();
        WCDAFX_API ~Timer();

        WCDAFX_API double Seconds() const;

    private:
        double start_;
    };
}
