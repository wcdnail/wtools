#include "stdafx.h"
#include "crt.defs.h"

#undef malloc
#undef calloc
#undef free

#ifdef _DEBUG
extern "C" void* _MCDECL _malloc_dbg(size_t size, int btype, char const* source, int line)
{
    return MinCrt::Crt().GetHeap().Alloc(size, false);
}

extern "C" void* _MCDECL _calloc_dbg(size_t num, size_t size, int btype, char const* source, int line)
{
    return MinCrt::Crt().GetHeap().Alloc(num * size, true);
}

extern "C" void _MCDECL _free_dbg(void* block, int btype)
{
    MinCrt::Crt().GetHeap().FreeMem(block);
}
#endif

extern "C" void* _MCDECL malloc(size_t size)
{
    return MinCrt::Crt().GetHeap().Alloc(size, false);
}

extern "C" void* _MCDECL calloc(size_t num, size_t size)
{
    return MinCrt::Crt().GetHeap().Alloc(num * size, true);
}

extern "C" void _MCDECL free(void* block)
{
    MinCrt::Crt().GetHeap().FreeMem(block);
}
