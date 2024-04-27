#ifndef _CR_run_context_h__
#define _CR_run_context_h__

#ifdef __cplusplus
extern "C" {
#endif

struct RUN_CTX;
typedef struct RUN_CTX RUN_CONTEXT;

int get_last_error_code(void);
RUN_CONTEXT capture_context(void);

#ifdef __cplusplus
}
#endif

#endif /* _CR_run_context_h__ */
