#ifndef _mincrt_heap_h__
#define _mincrt_heap_h__

#include "crt.system.allocator.h"
#include <map>

namespace MinCrt
{
    class CrtHeap
    {
    public:
        CrtHeap();
        ~CrtHeap();

        void* Alloc(size_t size, bool zeroing);
        void FreeMem(void* block);
        void ReportLeaks() const;

    private:
        struct BlockInfo
        {
            int id;
            size_t size;
        };

        typedef std::pair<void*, BlockInfo> Pair;

        typedef std::map<Pair::first_type
                       , Pair::second_type
                       , std::less<Pair::first_type> 
                       , CrtAllocator<Pair>
                       > Storage;

        Storage Map;
        volatile int NextBlockId;
#ifdef _WIN32
        bool DeleteHeap;
        void* Handle;
#endif

        void AddInfo(void* block, size_t size);
        void RemoveInfo(void* block);

    private:
        CrtHeap(CrtHeap const&);
        CrtHeap& operator = (CrtHeap const&);
    };
}

#endif /* _mincrt_heap_h__ */
