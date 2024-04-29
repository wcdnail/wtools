#pragma once

#include "wcdafx.api.h"

namespace DH
{
    class Timer
    {
    public:
        WCDAFX_API Timer();
        WCDAFX_API ~Timer();

        DECL_COPY_MOVE_OF(Timer);

        WCDAFX_API double Seconds() const;

    private:
        double start_;
    };
}
