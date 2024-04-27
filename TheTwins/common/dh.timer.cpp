#include "stdafx.h"
#include "dh.timer.h"

namespace Dh
{
    Timer::Timer()
        : start_(.0)
    {
        LARGE_INTEGER temp;
        ZeroMemory(&temp, sizeof(temp));
        if (::QueryPerformanceCounter(&temp)) {
            start_ = static_cast<double>(temp.QuadPart);
        }
    }

    Timer::~Timer()
    {}

    double Timer::Seconds() const
    {
        double stop = 0.0;
        double freq = 1.0;

        LARGE_INTEGER temp;
        ZeroMemory(&temp, sizeof(temp));
        if (::QueryPerformanceCounter(&temp)) {
            stop = static_cast<double>(temp.QuadPart);

            if (::QueryPerformanceFrequency(&temp)) {
                freq = static_cast<double>(temp.QuadPart);
            }
        }
        return (freq == .0 ? .0 : ((stop - start_) / freq));
    }
}
