#pragma once

namespace Cf
{
    class Timer
    {
    public:
        Timer();
        ~Timer();

        double Seconds() const;

    private:
        double start_;
    };

    inline Timer::Timer()
        : start_(.0)
    {
        LARGE_INTEGER temp = {0};
        if (::QueryPerformanceCounter(&temp))
            start_ = static_cast<double>(temp.QuadPart);
    }

    inline Timer::~Timer()
    {}

    inline double Timer::Seconds() const
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
