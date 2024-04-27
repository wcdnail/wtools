#ifndef _wcd_afx_string_shared_allocator_h__
#define _wcd_afx_string_shared_allocator_h__

#ifdef _MSC_VER
#pragma once
#endif

namespace Str
{
    /* std::allocator */
    template <typename Y>
    class SharedAllocator
    {
    public:
        typedef typename Y value_type;

        template<typename X>
        struct rebind {	typedef SharedAllocator<X> other; };

        ~SharedAllocator();
        SharedAllocator();
    };

    template <typename Y>
    inline SharedAllocator<Y>::~SharedAllocator()
    {}

    template <typename Y>
    inline SharedAllocator<Y>::SharedAllocator()
    {}
}

#endif /* _wcd_afx_string_hp_h__ */

