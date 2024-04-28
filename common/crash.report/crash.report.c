#include "crash.report.h"
#include "string.buffer.h"
#include "Run.Context.Info.h"
#include <stdlib.h>

void crash_report_window(STRING_BUFFER_PTR);

void crash_report(char const* reason, 
                  char const* source, int line,
                  int errcode, 
                  PCRUN_CONTEXT rc, 
                  int terminate)
{
    STRING_BUFFER buffer = { 0 };
    HANDLE           pid = run_context_get_pid(rc);
    
    sb_zero(&buffer);

    if (NULL != reason) {
        sb_format(&buffer, "%s\r\n\r\n", reason);
    }
    else {
        sb_format(&buffer, "Fundamental crash of matrix..."
                           "\r\nimproper use of the program or"
                           "\r\nprobably just a simple error"
                           "\r\n\r\n");
    }

    sb_format(&buffer, "Code: 0x%08x ", errcode);
    describe_operation_code(&buffer, errcode);
    sb_format(&buffer, "\r\n\r\n");

    if (NULL != source) {
        sb_format(&buffer, "Source:\r\n%s(%d)", source, line);
    }

    sb_format(&buffer, "\r\nStack:");
    describe_callstack(&buffer, rc);

    sb_format(&buffer, "\r\n\r\nCPU:");
    describe_cpu_info(&buffer, rc);

    sb_format(&buffer, "\r\n\r\nThreads:");
    describe_threads(&buffer, pid, 0);

    sb_format(&buffer, "\r\n\r\nModules:");
    describe_modules(&buffer, pid);

    sb_format(&buffer, "\r\n\r\nSystem:");
    describe_system_info(&buffer);

    crash_report_window(&buffer);

    sb_free(&buffer);

    if (terminate) {
        exit((int)errcode);
    }
}
