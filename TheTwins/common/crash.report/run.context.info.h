#ifndef _CR_run_context_info_h__
#define _CR_run_context_info_h__

#include "run.context.h"
#include "string.buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

char const* exception_code_string(unsigned code);
void describe_last_operation_code(STRING_BUFFER_PTR buffer, int code);
void describe_system_info(STRING_BUFFER_PTR buffer);
void describe_callstack(STRING_BUFFER_PTR buffer, RUN_CONTEXT const* rc);
void describe_modules(STRING_BUFFER_PTR buffer, void* pid);
void describe_threads(STRING_BUFFER_PTR buffer, void* pid, int inSeparateThread);
void describe_cpu_info(STRING_BUFFER_PTR buffer, RUN_CONTEXT const* rc);

#ifdef __cplusplus
}
#endif

#endif /* _CR_run_context_info_h__ */
