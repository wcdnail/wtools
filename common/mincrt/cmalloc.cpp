#include "stdafx.h"
#include "cdefines.h"
#include <niceday/null.pointee.h>

#undef calloc
#undef free
#undef malloc
#undef realloc
#undef _recalloc
#undef _aligned_free
#undef _aligned_malloc
#undef _aligned_offset_malloc
#undef _aligned_realloc
#undef _aligned_recalloc
#undef _aligned_offset_realloc
#undef _aligned_offset_recalloc
#undef _aligned_msize
#undef _freea
#undef _expand
#undef _msize

_extrnc void* malloc(size_t size)
{
    return _null_ptr();
}

_extrnc void* calloc(size_t count, size_t size)
{
    return _null_ptr();
}

_extrnc void* realloc(void* block, size_t size)
{
    return _null_ptr();
}

_extrnc size_t _msize(void* block)
{
    return !_is_null_ptr(block) ? 0 : 0;
}

_extrnc void free(void* block)
{
    //if (!_is_null_ptr(block))
        //;
}
