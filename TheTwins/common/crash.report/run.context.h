#ifndef _CR_run_context_h__
#define _CR_run_context_h__

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef _WIN32_WINNT
#    define _WIN32_WINNT 0x0500
#  endif 
#  include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct  
{
#ifdef _WIN32
    HANDLE pid;
    HANDLE tid;
    CONTEXT context;
#endif
    int   skip;     /* For stack walker. */
} 
RUN_CONTEXT, *RUN_CONTEXT_PTR;

int get_last_error_code(void);
RUN_CONTEXT capture_context(void);

#ifdef __cplusplus
}
#endif

#endif /* _CR_run_context_h__ */
