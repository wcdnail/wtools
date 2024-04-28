#pragma once

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * STRING_BUFFER.
 * Usefully for storing temporary string results.
 * 
 */
typedef struct STRING_BUFFER_S
{
    char* string;                       /* whole string */
    size_t  next;                       /* next available space */
    size_t  size;                       /* whole string size */
} STRING_BUFFER, *STRING_BUFFER_PTR;

/*
 * Initialize empty STRING_BUFFER.
 * 
 */
void sb_zero(STRING_BUFFER* buffer);

/*
 * Deallocating STRING_BUFFER.
 * 
 */
void sb_free(STRING_BUFFER* buffer);

/*
 * Deallocating string.
 * 
 */
void sb_free_string(char const* string);

/*
 * Write formated string into buffer->string + buffer->next.
 * Increase buffer->next by format size.
 * Returns format size, if success, -1 otherwise.
 * 
 */
int sb_write(STRING_BUFFER* buffer, char const* format, va_list ap);

/*
 * Invoke sb_write(...).
 * Returns format size, if success, -1 otherwise.
 * 
 */
int sb_format(STRING_BUFFER* buffer, char const* format, ...);

#ifdef __cplusplus
}
#endif
