#ifndef _crash_report_exception_h__
#define _crash_report_exception_h__

#include "run.context.h"

#ifdef __cplusplus

#include <string.hp.h>
#include <stdexcept>
#include <sstream>
#include <atlstr.h>
#include <atlconv.h>

namespace Crash
{
    class Exception: public std::runtime_error
    {
    private:
        typedef std::runtime_error Super;

    public:
        Exception(CharString const& message
                , char const* file
                , int line
                , int ec = errno
                , RUN_CONTEXT const& rc = ::capture_context()
        )
            : Super(message.c_str())
            , File(file)
            , Line(line)
            , ErrorCode(ec)
            , RunContext(rc)
        {}

        Exception(WidecharString const& message
                , char const* file
                , int line
                , int ec = errno
                , RUN_CONTEXT const& rc = ::capture_context()
        )
            : Super(CW2A(message.c_str()).m_psz)
            , File(file)
            , Line(line)
            , ErrorCode(ec)
            , RunContext(rc)
        {}

        virtual ~Exception() {}

        char const* GetSource() const { return File; }
        int GetLine() const { return Line; }
        int GetErrorCode() const { return ErrorCode; }
        RUN_CONTEXT const* GetRunContext() const { return &RunContext; }

    private:
        char const* File;
        int Line;
        int ErrorCode;
        RUN_CONTEXT RunContext;
    };
}

#define CrashException(M, C) Crash::Exception(M, __FILE__, __LINE__, C)

#endif /* __cplusplus */

#endif /* _crash_report_exception_h__ */
