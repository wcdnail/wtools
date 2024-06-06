#ifndef _assistance_trace_uptime_n_threadid_h__
#define _assistance_trace_uptime_n_threadid_h__

#include "up.time.h"
#include <iomanip>
#include <sstream>
#include <string>

namespace Trace
{
    template <class C>
    inline void _UptimeNThreadId(std::basic_ostringstream<C>& log)
    {
            // put up-time 
        log << Process::UpTime().GetAsString<C>() << (C)' ';

            // put thread id
        log << (C)'[' 
                << std::setw(4) << std::setfill((C)'0') 
#ifndef _WIN32
                << boost::this_thread::get_id() 
#else
                << ::GetCurrentThreadId()
#endif 
            << (C)']' << (C)' ';
    }
}

#endif // _assistance_trace_uptime_n_threadid_h__
