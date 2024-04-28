#include "stdafx.h"
#include "exception.h"
#include "run.context.h"
#include <atlconv.h>

namespace Crash
{
    Exception::~Exception()
    {
        ::free_run_context(RunContext);
    }

    Exception::Exception(CharString const& message, char const* file, int line, int ec)
        : Super(message.c_str())
        , File(file)
        , Line(line)
        , ErrorCode(ec)
        , RunContext(nullptr)
    {
        ::capture_run_context(&RunContext);
    }

    Exception::Exception(WidecharString const& message, char const* file, int line, int ec)
        : Super(CW2A(message.c_str()).m_psz)
        , File(file)
        , Line(line)
        , ErrorCode(ec)
        , RunContext(nullptr)
    {
        ::capture_run_context(&RunContext);
    }

    char const* Exception::GetSource() const { return File; }
    int Exception::GetLine() const { return Line; }

    int Exception::GetErrorCode() const { return ErrorCode; }

    PCRUN_CONTEXT Exception::GetRunContext() const { return RunContext; }
}
