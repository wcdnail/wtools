#include "crash.report.h"
#include "string.buffer.h"
#include "Run.Context.Info.h"

#define _USE_MATRIX_REASON_IF_GIVEN_NULL 0

void crash_report_window(STRING_BUFFER_PTR);

void crash_report(char const* reason, 
                  char const* source, int line,
                  int errcode, 
                  RUN_CONTEXT const* rc, 
                  int terminate)
{
    STRING_BUFFER buffer;
    sb_zero(&buffer);

    if (NULL != reason)
        sb_format(&buffer, "%s\r\n\r\n", reason);
#if _USE_MATRIX_REASON_IF_GIVEN_NULL
    else
        sb_format(&buffer, "Fundamental crash of matrix..."
                           "\r\nimproper use of the program or"
                           "\r\nprobably just a simple error"
                           "\r\n\r\n");
#endif

    sb_format(&buffer, "Code: 0x%08x ", errcode);
    describe_last_operation_code(&buffer, errcode);
    sb_format(&buffer, "\r\n\r\n");

    if (NULL != source)
        sb_format(&buffer, "Source:\r\n%s(%d)", source, line);

    sb_format(&buffer, "\r\nStack:");
    describe_callstack(&buffer, rc);

    sb_format(&buffer, "\r\n\r\nCPU:");
    describe_cpu_info(&buffer, rc);

    sb_format(&buffer, "\r\n\r\nThreads:");
    describe_threads(&buffer, rc->pid, 0);

    sb_format(&buffer, "\r\n\r\nModules:");
    describe_modules(&buffer, rc->pid);

    sb_format(&buffer, "\r\n\r\nSystem:");
    describe_system_info(&buffer);

    crash_report_window(&buffer);

    sb_free(&buffer);

    if (terminate)
        exit((int)errcode);
}
