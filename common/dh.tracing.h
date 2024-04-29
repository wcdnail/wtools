//
// Nu tracing stuff
// 

#pragma once

#include "wcdafx.api.h"
#include "dh.timer.h"
#include "strint.h"
#include <string>

namespace DH
{
    enum InitialFlags: uint64_t
    {
        LOG_MUTEX_GUARD   = 0x0000000000000001,
        LOG_ENABLED       = 0x0000000000000002,
        LOG_TO_STDIO      = 0x0000000000000004,
        DEBUG_EXTRA_INFO  = 0x0000000000000008,
        DEBUG_WIN32_OUT   = 0x0000000000000010,

        DEFAULT_FLAGS     = LOG_ENABLED | DEBUG_EXTRA_INFO | DEBUG_WIN32_OUT,
    };

    WCDAFX_API void InitDebugHelpers(uint64_t flags = DEFAULT_FLAGS);
    WCDAFX_API void PrintLogHeader();

    class TraceCategory
    {
    public:
        DELETE_COPY_MOVE_OF(TraceCategory);

        WCDAFX_API TraceCategory(wchar_t const* name);
        WCDAFX_API ~TraceCategory();
        WString const& GetName() const;

    private:
        WString Name;
    };

    struct ScopedThreadLog
    {
        DELETE_COPY_MOVE_OF(ScopedThreadLog);

        WCDAFX_API ScopedThreadLog(wchar_t const* message);
        WCDAFX_API ScopedThreadLog(int, wchar_t const* format, ...);
        WCDAFX_API ~ScopedThreadLog();

    private:
        DH::Timer  Time;
        wchar_t Message[1024];
    };

    namespace Category
    {
        WCDAFX_API DH::TraceCategory const& Module();
        WCDAFX_API DH::TraceCategory const& Exception();
    }

    WCDAFX_API void Printf(char const* format, ...);
    WCDAFX_API void Printf(wchar_t const* format, ...);
    
    WCDAFX_API void ThreadPrintf(char const* format, ...);
    WCDAFX_API void ThreadPrintf(wchar_t const* format, ...);
    
    WCDAFX_API void ThreadPrintfc(TraceCategory const& cat, char const* format, ...);
    WCDAFX_API void ThreadPrintfc(TraceCategory const& cat, wchar_t const* format, ...);

    namespace Impl
    {
        template <typename C>
        inline C PrintableChar(C symbol)
        {
            switch (symbol) {
            case /* LF */ 0x0a: return (C)'\x89';
            case /* CR */ 0x0d: return (C)'\xac';
            }
            if (symbol < 0x20) {
                return (C)'\x95';
            }
            return symbol;
        }

        template <typename C>
        inline void CopyCharsForPrinting(C* dest, size_t dlen, C const* source, size_t slen)
        {
            size_t left = min(dlen, slen);
            while (left > 0) {
                *dest = PrintableChar(*source);
                ++source;
                ++dest;
                --left;
            }
        }
    }

    template <typename C, size_t S>
    inline void MakePrintable(C (&dest)[S], C const* source, size_t sl)
    {
        //::memset(dest, 0, S * sizeof(C));
        Impl::CopyCharsForPrinting(dest, S - 1, source, sl);
    }

#ifdef _DEBUG
#   define DebugPrintf          DH::Printf
#   define DebugThreadPrintf    DH::ThreadPrintf
#   define DebugThreadPrintfc   DH::ThreadPrintfc
    static const bool DebugTrue = true;
#else
    inline void DebugPrintf(...) {}
    inline void DebugThreadPrintf(...) {}
    inline void DebugThreadPrintfc(...) {}
    static const bool DebugTrue = false;
#endif
}

