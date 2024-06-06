#ifndef _assistance_up_time_h__
#define _assistance_up_time_h__

#include <chrono>

namespace Process
{
    class UpTimer;

    UpTimer& UpTime();

    class UpTimer
    {
    public:
        typedef boost::posix_time::ptime TimeType;
        typedef boost::posix_time::time_duration Span;

        static TimeType Current();
        Span Get() const;

        template <class C>
        std::basic_string<C> GetAsString() const;

    private:
        TimeType Start_;

    private:
        friend UpTimer& UpTime();

        UpTimer();
        ~UpTimer();
    };

    inline UpTimer::TimeType UpTimer::Current()
    {
        namespace pt = boost::posix_time;
        return pt::microsec_clock::universal_time();
    }

    inline UpTimer::UpTimer() 
        : Start_(Current())
    {
        namespace pt = boost::posix_time;

#ifdef _WIN32
        FILETIME dummy = {};
        FILETIME creation = {};
        if (::GetProcessTimes(::GetCurrentProcess(), &creation, &dummy, &dummy, &dummy))
            Start_ = pt::from_ftime<TimeType>(creation);
#endif
    }

    inline UpTimer::~UpTimer() 
    {}

    inline UpTimer::Span UpTimer::Get() const
    {
        return Current() - Start_;
    }

    template <class C>
    inline std::basic_string<C> UpTimer::GetAsString() const
    {
        namespace pt = boost::posix_time;
        return pt::to_simple_string_type<C>(Get());
    }

    inline UpTimer& UpTime()
    {
        static UpTimer self;
        return self;
    }
}

#endif // _assistance_up_time_h__
