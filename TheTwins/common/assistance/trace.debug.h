#ifndef _assistance_trace_debug_h__
#define _assistance_trace_debug_h__

#include "trace.log.h"

namespace Trace
{
    class DebugLog: boost::noncopyable
    {
    public:
        typedef BasicLog::String String;

        DebugLog();
        ~DebugLog();

        template <class U>
        void Put(U const& x);
        void Flush();

        static void Write(String const& string) BOOST_NOEXCEPT;

    private:
        BasicLog Log_;
    };

    inline DebugLog::DebugLog()
        : Log_()
    {}

    inline DebugLog::~DebugLog()
    {
        Flush();
    }

    template <class U>
    inline void DebugLog::Put(U const& x)
    {
        Log_.Put<U>(x);
    }

    inline void DebugLog::Flush()
    {
        Log_.Flush<DebugLog>();
    }

    void DebugLog::Write(String const& string)
    {
#ifdef _WIN32
        ::OutputDebugStringW(string.c_str());
#else
        ::fputws(string.c_str(), stdout);
#endif
    }
}

#endif // _assistance_trace_debug_h__
