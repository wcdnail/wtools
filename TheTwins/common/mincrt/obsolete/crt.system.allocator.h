#ifndef _mincrt_system_allocator_h__
#define _mincrt_system_allocator_h__

namespace MinCrt
{
    namespace RawHeap
    {
        void _Reset();
        void* _MemAlloc(size_t size);
        void _MemFree(void* block, size_t size);
        void _ReportLeaks();
    }

    template <typename T>
    class CrtAllocator;

    template <> 
    class CrtAllocator<void>
    {
    public:
        typedef void* pointer;
        typedef const void* const_pointer;
        typedef void value_type;

        template <class U> 
        struct rebind 
        {
            typedef CrtAllocator<U> other; 
        };
    };    

    template <typename T>
    class CrtAllocator
    {
    public:
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T const* const_pointer;
        typedef T& reference;
        typedef T const& const_reference;
        typedef T value_type;

        template <class U> 
        struct rebind 
        { 
            typedef CrtAllocator<U> other; 
        };

        CrtAllocator() {}
        ~CrtAllocator() {}

        template <class U> 
        CrtAllocator(CrtAllocator<U> const&) {}

        pointer address(reference x) const { return &x; }
        const_pointer address(const_reference x) const { return &x; }
        size_type max_size() const { return size_t(-1) / sizeof(value_type); }
        void construct(pointer pointee, T const& val) { new((void*)pointee) T(val); }
        void construct(pointer pointee) { new((void*)pointee) T(); }
        void destroy(pointer pointee) { pointee->~T(); }

        pointer allocate(size_type size)
        {
            pointer block = (pointer)RawHeap::_MemAlloc(size * sizeof(value_type));

            if (!block)
                throw std::bad_alloc();

            return block;
        }

        void deallocate(pointer pointee, size_type size)
        {
            RawHeap::_MemFree((void*)pointee, size * sizeof(value_type));
        }
    };

    template <typename T, typename U>
    inline bool operator == (CrtAllocator<T> const&, CrtAllocator<U> const&)
    {
        return true;
    }

    template <typename T, typename U>
    inline bool operator != (CrtAllocator<T> const&, CrtAllocator<U> const&)
    {
        return false;
    }
}

#endif /* _mincrt_system_allocator_h__ */
