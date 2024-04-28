#ifndef _assistance_trace_stream_h__
#define _assistance_trace_stream_h__

#include "trace.flags.h"
#include <boost/noncopyable.hpp>
#include <sstream>

namespace Trace
{
    template <class Logger>
    class Stream: boost::noncopyable
    {
    public:
        Stream();
        ~Stream();

        template <class T>
        Stream const& operator << (T x) const;
        Stream const& operator << (ControlFlag const& flags) const;

    private:
        mutable Logger Logger_;
    };

    template <class Logger>
    inline Stream<Logger>::Stream()
        : Logger_()
    {}

    template <class Logger>
    inline Stream<Logger>::~Stream()
    {}

    template <class Logger>
    template <class T>
    inline Stream<Logger> const& Stream<Logger>::operator << (T x) const
    {
        Logger_.Put(x);
        return *this;
    }

    template <class Logger>
    Stream<Logger> const& Stream<Logger>::operator << (ControlFlag const& flags) const
    {
        if (flags.IsOn(ControlFlag::EndOfLine))
            Logger_.Put('\n');

        if (flags.IsOn(ControlFlag::Purge))
            Logger_.Flush();

        return *this;
    }
}

#endif // _assistance_trace_stream_h__
