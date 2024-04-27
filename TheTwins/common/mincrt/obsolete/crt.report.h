#ifndef _mincrt_report_h__
#define _mincrt_report_h__

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    void TraceA(char const* format, ...);
    void TraceW(wchar_t const* format, ...);
    void TraceAV(char const* format, va_list ap);
    void TraceWV(wchar_t const* format, va_list ap);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef __cplusplus
#  ifdef _DEBUG
#    define DTraceA TraceA
#    define DTraceW TraceW
#  else
     inline void DTraceA(...) {}
     inline void DTraceW(...) {}
#  endif
#endif /* __cplusplus */

#endif /* _mincrt_report_h__ */
