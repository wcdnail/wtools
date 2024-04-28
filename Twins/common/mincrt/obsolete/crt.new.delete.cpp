#include "stdafx.h"
#include "crt.defs.h"

void* _MCDECL operator new(size_t size)
{
    void* result = MinCrt::Crt().GetHeap().Alloc(size, true);

    if (!result)
        throw std::bad_alloc();

    return result;
}

void* _MCDECL operator new[](size_t size)
{
    void* result = MinCrt::Crt().GetHeap().Alloc(size, true);

    if (!result)
        throw std::bad_alloc();

    return result;
}

void _MCDECL operator delete(void* block)
{
    MinCrt::Crt().GetHeap().FreeMem(block);
}

void _MCDECL operator delete[](void* block)
{
    MinCrt::Crt().GetHeap().FreeMem(block);
}

