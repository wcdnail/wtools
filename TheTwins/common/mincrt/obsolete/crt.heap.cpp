#include "stdafx.h"
#include "crt.heap.h"
#include "crt.report.h"

namespace MinCrt
{
#ifdef _WIN32
    static HANDLE CreateOrGetHeap(bool& deleteHeap)
    {
        SYSTEM_INFO sinfo;
        ::GetSystemInfo(&sinfo);

        wchar_t const* source = L"HeapCreate";
        void* heap = ::HeapCreate(HEAP_NO_SERIALIZE | HEAP_GROWABLE, sinfo.dwPageSize, 0);
        deleteHeap = NULL != heap;
        if (!heap)
        {
            ::TraceW(L"MinCrt: Can't create heap (%d)\n", ::GetLastError());

            source = L"GetProcessHeap";
            heap = ::GetProcessHeap();
            if (!heap)
                ::TraceW(L"MinCrt: Can't get process heap (%d)\n", ::GetLastError());
        }

        ::TraceW(L"MinCrt: Heap init %s (%s)\n", (heap ? L"ok" : L"fail"), source);
        return heap;
    }
#endif

    CrtHeap::CrtHeap()
        : Map()
        , NextBlockId(0)
#ifdef _WIN32
        , DeleteHeap(false)
        , Handle(CreateOrGetHeap(DeleteHeap))
#endif
    {}

    CrtHeap::~CrtHeap()
    {
#ifdef _WIN32
        if (DeleteHeap && Handle)
        {
            BOOL hd = ::HeapDestroy(Handle);
            ::TraceW(L"MinCrt: Heap destroy %s\n", (hd ? L"ok" : L"fail"));
            Handle = NULL;
            DeleteHeap = false;
        }
#endif
    }

    void CrtHeap::AddInfo(void* block, size_t size)
    {
        try
        {
            Storage::const_iterator it = Map.find(block);
            if (it != Map.cend())
            {
                // TODO: report already existing.
                return ;
            }

            BlockInfo info;
            info.id = NextBlockId++;
            info.size = size;

            //CONTEXT context;
            //::RtlCaptureContext(&context);
            //info.context = context;

            Map.insert(Pair(block, info));
        }
        catch (...)
        {
            // TODO: report error!
        }
    }

    void CrtHeap::RemoveInfo(void* block)
    {
        try
        {
            Storage::const_iterator it = Map.find(block);
            if (it == Map.cend())
            {
                // TODO: report foreign block.
                return ;
            }

            Map.erase(it);
        }
        catch (...)
        {
            // TODO: report error!
        }
    }

    void* CrtHeap::Alloc(size_t size, bool zeroing)
    {
        size_t nsize = size ? size : 1;
        void* block = NULL;

        if (Handle)
            block = ::HeapAlloc(Handle, zeroing ? HEAP_ZERO_MEMORY : 0, nsize);
        else
            block = (void*)::LocalAlloc(LMEM_FIXED | (zeroing ? LMEM_ZEROINIT : 0), nsize);

        if (block)
            AddInfo(block, size);

        return block;
    }

    void CrtHeap::FreeMem(void* block)
    {
        bool deleted = false;

        if (Handle)
            deleted = (FALSE != ::HeapFree(Handle, 0, block));
        else
            deleted = NULL == ::LocalFree(block);

        if (deleted)
            RemoveInfo(block);
    }

    void CrtHeap::ReportLeaks() const
    {
        if (!Map.empty())
        {
            for (Storage::const_iterator it = Map.cbegin(); it != Map.cend(); ++it)
            {
                void* block = it->first;
                size_t size = it->second.size;
                int id = it->second.id;

                TraceW(L"MinCrt: Memory leak #%3d %p %d bytes long\n", id, block, size);
            }
        }
    }
}
