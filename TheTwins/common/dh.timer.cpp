#include "stdafx.h"
#include "dh.timer.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#endif

namespace Dh
{
    Timer::Timer()
        : start_(.0)
    {
        LARGE_INTEGER temp = {0};
        if (::QueryPerformanceCounter(&temp))
            start_ = static_cast<double>(temp.QuadPart);
    }

    Timer::~Timer()
    {}

    double Timer::Seconds() const
    {
        double stop = 0.0;
        double freq = 1.0;

        LARGE_INTEGER temp = {0};
        if (::QueryPerformanceCounter(&temp))
        {
            stop = static_cast<double>(temp.QuadPart);

            if (::QueryPerformanceFrequency(&temp))
                freq = static_cast<double>(temp.QuadPart);
        }

        return (freq == .0 ? .0 : ((stop - start_) / freq));
    }
}
