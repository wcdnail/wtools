//
// Nu tracing stuff
// 

#pragma once

#include "wcdafx.api.h"
#include "dh.timer.h"
#include "string.hp.h"
#include <string>
#include <atlstr.h>

namespace Dh
{
    class TraceCategory
    {
    public:
        WCDAFX_API TraceCategory(wchar_t const* name);
        WCDAFX_API ~TraceCategory();
        wchar_t const* GetName() const { return Name.c_str(); }

    private:
        WidecharString Name;
    };

    struct ScopedThreadLog
    {
        WCDAFX_API ScopedThreadLog(wchar_t const* message);
        WCDAFX_API ScopedThreadLog(int, wchar_t const* format, ...);
        WCDAFX_API ~ScopedThreadLog();

    private:
        Dh::Timer Time;
        wchar_t Message[1024];
    };

    namespace Category
    {
        extern WCDAFX_API const TraceCategory Module;
        extern WCDAFX_API const TraceCategory Exception;
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
            switch (symbol)
            {
            case /* LF */ 0x0a: return (C)'\x89';
            case /* CR */ 0x0d: return (C)'\xac';
            }

            if (symbol < 0x20)
                return (C)'\x95';

            return symbol;
        }

        template <typename C>
        inline void CopyCharsForPrinting(C* dest, size_t dlen, C const* source, size_t slen)
        {
            size_t llen = min(dlen, slen);

            for (size_t i=0; i<llen; i++)
                dest[i] = PrintableChar(source[i]);
        }
    }

    template <typename C, size_t S>
    inline void MakePrintable(C (&dest)[S], C const* source, size_t sl)
    {
        //::memset(dest, 0, S * sizeof(C));
        Impl::CopyCharsForPrinting(dest, S - 1, source, sl);
    }

#ifdef _DEBUG
#   define DebugPrintf          Dh::Printf
#   define DebugThreadPrintf    Dh::ThreadPrintf
#   define DebugThreadPrintfc   Dh::ThreadPrintfc
    static const bool DebugTrue = true;
#else
    inline void DebugPrintf(...) {}
    inline void DebugThreadPrintf(...) {}
    inline void DebugThreadPrintfc(...) {}
    static const bool DebugTrue = false;
#endif
}

