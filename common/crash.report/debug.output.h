#ifndef _cr_debug_output_h__
#define _cr_debug_output_h__

#define DEBUGPRINT_LOC()    dbg_printf("%s(%d): ", __FILE__, __LINE__)
#define DEBUGPRINT          DEBUGPRINT_LOC(); dbg_printf
#define DEBUGPRINT2         DEBUGPRINT_LOC(); dbg_printfc

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Outputs formatted message to current debug console.
 *
 */
void dbg_printf(char const* message, ...);

/**
 * Do same that dbg_printf, and output code description.
 *
 */
void dbg_printfc(int code, char const* message, ...);

#ifdef __cplusplus
}
#endif

#endif /* _CR_debug_output_h__ */
