#ifndef _assistance_debug_log_h__
#define _assistance_debug_log_h__

#include "trace.debug.h"
#include "trace.stream.h"

namespace Debug
{
    static const Trace::Stream<Trace::DebugLog> Trace;
}

#define _TRACE_EXPRESSION(expression)               \
    do {                                            \
        Debug::Trace << BOOST_STRINGIZE(expression) \
                     << L" = " << (expression)      \
                     << Trace::Endl;                \
    } while (0)

#endif // _assistance_debug_log_h__
