//
// Nu tracing stuff
// 

#pragma once

#include "wcdafx.api.h"
#include "dh.timer.h"
#include "strint.h"
#include <utility>
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
    WCDAFX_API double LogUpTime();

    class TraceCategory
    {
    public:
        DELETE_COPY_MOVE_OF(TraceCategory);

        WCDAFX_API TraceCategory(PCWSTR name);
        WCDAFX_API ~TraceCategory();
        WString const& GetName() const;

    private:
        WString Name;
    };

    struct ScopedThreadLog
    {
        DELETE_COPY_MOVE_OF(ScopedThreadLog);

        WCDAFX_API ScopedThreadLog(PCWSTR message);
        WCDAFX_API ScopedThreadLog(_Printf_format_string_params_(1) int, PCWSTR format, ...);
        WCDAFX_API ~ScopedThreadLog();

    private:
        DH::Timer  Time;
        wchar_t Message[1024];
    };

    namespace Category
    {
        WCDAFX_API DH::TraceCategory const& Module();
        WCDAFX_API DH::TraceCategory const& Exception();
        WCDAFX_API DH::TraceCategory const& WTL();
    }

    WCDAFX_API void Printf(_In_z_ _Printf_format_string_ PCSTR format, ...);
    WCDAFX_API void Printf(_In_z_ _Printf_format_string_ PCWSTR format, ...);
    
    WCDAFX_API void TPrintf(_In_z_ _Printf_format_string_ PCSTR format, ...);
    WCDAFX_API void TPrintf(_In_z_ _Printf_format_string_ PCWSTR format, ...);
    
    WCDAFX_API void TCPrintf(TraceCategory const& cat, _In_z_ _Printf_format_string_ PCSTR format, ...);
    WCDAFX_API void TCPrintf(TraceCategory const& cat, _In_z_ _Printf_format_string_ PCWSTR format, ...);

    namespace Impl
    {
        template <typename CharType>
        inline CharType PrintableChar(CharType symbol)
        {
            switch (symbol) {
            case /* LF */ 0x0a: return static_cast<CharType>('\x89');
            case /* CR */ 0x0d: return static_cast<CharType>('\xac');
            }
            if (symbol < 0x20) {
                return static_cast<CharType>('\x95');
            }
            return symbol;
        }

        template <typename CharType>
        inline void CopyCharsForPrinting(CharType* dest, size_t dlen, CharType const* source, size_t slen)
        {
            size_t left = std::min<size_t>(dlen, slen);
            while (left > 0) {
                *dest = PrintableChar(*source);
                ++source;
                ++dest;
                --left;
            }
        }
    }

    template <typename CharType, size_t CountValue>
    inline void MakePrintable(CharType (&dest)[CountValue], CharType const* source, size_t sl)
    {
        //::memset(dest, 0, CountValue * sizeof(CharType));
        Impl::CopyCharsForPrinting(dest, CountValue - 1, source, sl);
    }
}

#ifdef _DEBUG
#   define DBGPrint(...)            DH::Printf(__VA_ARGS__)
#   define DBGTPrint(...)           DH::TPrintf(__VA_ARGS__)
#   define DBGCPrint(Category, ...) DH::TCPrintf(Category, __VA_ARGS__)
    static const bool DebugTrue = true;
#else
    inline void DBGPrint(...)       {}
    inline void DBGTPrint(...)      {}
    inline void DBGCPrint( ...)     {}
    static const bool DebugTrue = false;
#endif
