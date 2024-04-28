#include "string.buffer.h"
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

void sb_free_string(char const* string)
{
    if (NULL != string) {
        free((void*)string);
    }
}

void sb_zero(STRING_BUFFER* buffer)
{
    if (NULL == buffer) {
        return;
    }
    buffer->string  = NULL;
    buffer->next    = 0;
    buffer->size    = 0;
}

/*
 * Allocate buffer->string (buffer->size+1 bytes length).
 * Returns buffer->size+1, if success, 0 otherwise.
 * 
 */
static size_t sb_alloc(STRING_BUFFER* buffer)
{
    void*   mem = NULL;
    size_t size = 0;
    if ((NULL == buffer) || (1 > buffer->size)) {
        return 0;
    }
    size = buffer->size;
    if (0 == size) {
        return 0;
    }
    ++size;
    if (NULL == buffer->string) {
        mem = calloc(size, sizeof(char));
    }
    else {
        mem = realloc((void*)buffer->string, size);
    }
    if (NULL == mem) {
        return 0;
    }
    buffer->string = (char*)mem;
    return size;
}

void sb_free(STRING_BUFFER* buffer)
{
    if (NULL == buffer) {
        return;
    }
    sb_free_string(buffer->string);
    sb_zero(buffer);
}

/*
 * Getting format size by _vscprintf call (or POSIX equivalent).
 * Increase buffer->size by format size.
 * Returns format size, if success, -1 otherwise.
 * 
 */
static int sb_calc(STRING_BUFFER* buffer, char const* format, va_list ap)
{
    int n = 0;
    if (NULL == buffer) {
        return 0;
    }
    n = _vscprintf(format, ap);
    buffer->size += (n > 0 ? n + 1 : 0);
    return n;
}

int sb_write(STRING_BUFFER* buffer, char const* format, va_list ap)
{
    int n = sb_calc(buffer, format, ap);
    if ((NULL == buffer) || (0 > n)) {
        return 0;
    }
    if (!buffer->string || (n > (int)(buffer->size - buffer->next) - 1)) {
        if (!sb_alloc(buffer)) {
            return 0;
        }
    }
    assert(buffer->size >= buffer->next);
    n = vsprintf_s(buffer->string + buffer->next, buffer->size - buffer->next, format, ap);
    buffer->next += (n > 0 ? n : 0);
    return n;
}

int sb_format(STRING_BUFFER* buffer, char const* format, ...)
{
    int rv = -1;
    if (NULL == buffer) {
        return rv;
    }
    va_list ap;
    va_start(ap, format);
    rv = sb_write(buffer, format, ap);
    va_end(ap);
    return rv;
}
