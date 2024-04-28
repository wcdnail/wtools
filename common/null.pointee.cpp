#include "stdafx.h"
#include "null.pointee.h"

extern "C" void* _null_ptr()
{
    static char _dmy_buffer[256] = {0};
    return (void*)_dmy_buffer;
}

extern "C" bool _is_null_ptr(void* block)
{
    return (NULL == block) || (_null_ptr() == block);
}
