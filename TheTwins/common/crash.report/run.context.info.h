#pragma once

#include "run.context.h"
#include "string.buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

char const* exception_code_string(unsigned code);
void describe_operation_code(STRING_BUFFER_PTR buffer, int code);
void describe_system_info(STRING_BUFFER_PTR buffer);
void describe_callstack(STRING_BUFFER_PTR buffer, PCRUN_CONTEXT rc);
void describe_modules(STRING_BUFFER_PTR buffer, void* pid);
void describe_threads(STRING_BUFFER_PTR buffer, void* pid, int inSeparateThread);
void describe_cpu_info(STRING_BUFFER_PTR buffer, PCRUN_CONTEXT rc);

#ifdef __cplusplus
}
#endif
