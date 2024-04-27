#include "stdafx.h"
#include "crt.system.allocator.h"
#include "crt.report.h"

namespace MinCrt
{
    namespace RawHeap
    {
        static volatile LONGLONG allocationSum = 0;

        void _Reset()
        {
            allocationSum = 0;
        }

        void* _MemAlloc(size_t size)
        {
            void* block = ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, size ? size : 1);

            if (block)
                ::InterlockedExchangeAdd64(&allocationSum, (LONGLONG)size);

            return block;
        }

        void _MemFree(void* block, size_t size)
        {
            HANDLE heap = ::GetProcessHeap();

            if (0 == size)
                size = ::HeapSize(heap, 0, block);

            if (::HeapFree(heap, 0, block))
                ::InterlockedExchangeAdd64(&allocationSum, -((LONGLONG)size));
        }

        void _ReportLeaks()
        {
            if (allocationSum)
                ::TraceW(L"MinCrt: Raw memory leaks %I64u\n", allocationSum);
        }
    }
}
