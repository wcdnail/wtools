#ifndef _crash_report_h__
#define _crash_report_h__

#include "run.context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Call `crash_report` in desperate situation.
 *
 */ 
void crash_report(char const* reason
                , char const* source
                , int line
                , int errcode
                , RUN_CONTEXT const* rc
                , int terminate);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#define CRASH_REPORT_INT(Reason, Code, Context, Terminate)                \
    crash_report(Reason, __FILE__, __LINE__, Code, Context, Terminate)

#define CRASH_REPORT(Reason, Code)                                        \
        CRASH_REPORT_INT(Reason, Code, &capture_context(), 1)

#ifdef __cplusplus

#ifdef _WIN32
#include <atlstr.h>
#include <atlconv.h>
#include "../string.hp.h"

#define CRASH_REPORT_UTF8(Reason, Code)                                   \
    do {                                                                  \
        WidecharString unicode(CA2W((PCSTR)(Reason)));                    \
        CRASH_REPORT(CW2A(unicode.c_str(), CP_UTF8), (Code));             \
    } while (0)
#endif

#endif /* __cplusplus */

#endif /* _crash_report_h__ */
