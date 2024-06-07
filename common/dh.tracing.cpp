#include "stdafx.h"
#include "dh.tracing.h"
#include "dh.tracing.defs.h"
#include "dh.timer.h"
#include "string.utils.format.h"
#include "info/runtime.information.h"
#include "dev.assistance/debug.console/debug.console.h"
#include <atlstr.h>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>

namespace DH
{
#pragma region Internals

    namespace
    {
        using        Mutex = std::recursive_mutex;
        using LogStreamPtr = std::unique_ptr<std::ostream>;
        using      LogPath = std::filesystem::path;

        static const Timer           LOG_Uptime;
        static constexpr int LOG_UPTIME_PRECISS = 12;
        static constexpr int    LOG_MAX_ROTATED = 3;
        static constexpr UINT  LOG_DEF_CODEPAGE = CP_UTF8;

#ifdef _DEBUG_XTRA
        class DebugNotifyOnce
        {
            DebugNotifyOnce(PCWSTR message) { OutputDebugStringW(message); }

        public:
            static const DebugNotifyOnce& noLocks()
            {
                static const DebugNotifyOnce inst(L"\t>>>> Logging withoud any MUTEX GUARD's...\n");
                return inst;
            }
        };
#else
        struct DebugNotifyOnce
        {
            DebugNotifyOnce(...) {}
            static const void noLocks() {}
        };
#endif
        struct ScopedLockFree
        {
            ScopedLockFree(ScopedLockFree const&) = default;
            explicit ScopedLockFree();
            virtual ~ScopedLockFree() = default;
        };

        struct ScopedGuard: ScopedLockFree
        {
            Mutex& mx_;
            explicit ScopedGuard(Mutex& mx);
            ~ScopedGuard() override;
            ScopedGuard& operator = (ScopedGuard const&) = delete;
        };

        ScopedLockFree::ScopedLockFree()
        {
            UNREFERENCED_ARG(DebugNotifyOnce::noLocks());
        }

        ScopedGuard::ScopedGuard(Mutex& mx)
            : mx_(mx)
        {
            mx_.lock();
        }

        ScopedGuard::~ScopedGuard()
        {
            mx_.unlock();
        }
    }

#pragma endregion
#pragma region CTX

    class LogCtx
    {
        uint64_t       flags_;
        Mutex             mx_;
        LogStreamPtr     log_;
        std::ostream* stdout_;

        ~LogCtx();
        LogCtx(uint64_t flags);

        bool isLogOpened();
        void FlushLog() const;
        static LogPath GetLogFilepath(int n);
        static std::string GenLogName();

        void putsMBStr(ATL::CStringA&& text) const;
        void putsWDStr(ATL::CStringW&& text) const;

    public:
        DELETE_COPY_MOVE_OF(LogCtx);

        static LogCtx& instance();

        void setBits(uint64_t bits);
        bool isBitSet(uint64_t bits) const;

        ScopedLockFree ScopedLock();

        void puts(ATL::CStringA&& text);
        void putws(ATL::CStringW&& text);
    };

    LogCtx& LogCtx::instance()
    {
        static LogCtx self(LOG_MUTEX_GUARD);
        return self;
    }

    LogCtx::~LogCtx()
    {
        FlushLog();
    }

    LogCtx::LogCtx(uint64_t flags)
        :  flags_(flags)
        ,     mx_()
        ,    log_()
        , stdout_(nullptr)
    {
    }

    void LogCtx::setBits(uint64_t bits)
    {
        flags_ = bits;
    }

    bool LogCtx::isBitSet(uint64_t bits) const
    {
        return (0 != (flags_ & bits));
    }

    ScopedLockFree LogCtx::ScopedLock()
    {
        return isBitSet(LOG_MUTEX_GUARD) ? static_cast<ScopedLockFree>(ScopedGuard(mx_)) : ScopedLockFree();
    }

    LogPath LogCtx::GetLogFilepath(int n)
    {
        LogPath result{Runtime::Info().Executable.Directory};
        result /= Runtime::Info().Executable.Version.ProductName + 
                L".log." +
                std::to_wstring(n) +
                L".txt";
        return result;
    }

    std::string LogCtx::GenLogName()
    {
        std::filesystem::path temp;
        int i = LOG_MAX_ROTATED - 1;
        do {
            temp = GetLogFilepath(i);
            if (std::filesystem::exists(temp)) {
                std::filesystem::path next = GetLogFilepath(i + 1);
                std::error_code ec;
                std::filesystem::remove(next, ec);
                std::filesystem::rename(temp, next, ec);
            }
            --i;
        }
        while (i >= 0);
        return temp.string();
    }

    bool LogCtx::isLogOpened()
    {
        if (isBitSet(LOG_TO_STDIO)) {
            if (!stdout_) {
                stdout_ = &std::cout;
            }
        }
        if (isBitSet(LOG_ENABLED)) {
            if (!log_) {
                auto log = std::make_unique<std::ofstream>(GenLogName());
                if (log->is_open()) {
                    log_ = std::move(log);
                }
            }
        }
        return (nullptr != log_.get()) || (nullptr != stdout_);
    }

    void LogCtx::puts(ATL::CStringA&& text)
    {
        if (this->isLogOpened()) {
            this->putsMBStr(std::move(text));
        }
    }

    void LogCtx::putws(ATL::CStringW&& text)
    {
        if (this->isLogOpened()) {
            this->putsWDStr(std::move(text));
        }
    }

    void LogCtx::FlushLog() const
    {
        if (stdout_) {
            (*stdout_) << std::flush;
        }
        if (log_) {
            (*log_) << std::flush;
        }
    }

    void LogCtx::putsMBStr(ATL::CStringA&& text) const
    {
        if (stdout_) {
            (*stdout_) << text;
        }
        if (log_) {
            (*log_) << text;
        }
    }

    void LogCtx::putsWDStr(ATL::CStringW&& text) const
    {
        if (!text || !(*text)) {
            return ;
        }
        ATL::CStringA temp{};
        int  const     len{text.GetLength() + 1};
        PSTR const  buffer{temp.GetBufferSetLength(len)};
        int  const    clen{WideCharToMultiByte(LOG_DEF_CODEPAGE, 0, text, len, buffer, len, nullptr, nullptr)};
        temp.ReleaseBufferSetLength(clen);
        putsMBStr(std::move(temp));
    }

#pragma endregion
#pragma region Tracers

    static inline void printString(unsigned nLevel, ATL::CStringA&& sTS, ATL::CStringA&& sTID, DWORD dwTID, ATL::CStringA&& sLev, ATL::CStringA&& sText)
    {
        ATL::CStringA sLine{};
        if (LogCtx::instance().isBitSet(DEBUG_WIN32_OUT | LOG_ENABLED)) {
            sLine = sTS + sTID + sLev + sText;
        }
        auto logGuard{LogCtx::instance().ScopedLock()};
        if (LogCtx::instance().isBitSet(DEBUG_WIN32_OUT)) {
            OutputDebugStringA(sLine.GetString());
        }
        if (LogCtx::instance().isBitSet(LOG_ENABLED)) {
            LogCtx::instance().puts(std::move(sLine));
        }
        if (LogCtx::instance().isBitSet(DEBUG_DEVCON_OUT)) {
            DH::DebugConsole::Instance().PutsNarrow(nLevel, {sText.GetString(), static_cast<size_t>(sText.GetLength())}, dwTID);
        }
    }

    static inline void printString(unsigned nLevel, ATL::CStringW&& sTS, ATL::CStringW&& sTID, DWORD dwTID, ATL::CStringW&& sLev, ATL::CStringW&& sText)
    {
        ATL::CStringW sLine{};
        if (LogCtx::instance().isBitSet(DEBUG_WIN32_OUT | LOG_ENABLED)) {
            sLine = sTS + sTID + sLev + sText;
        }
        auto logGuard{LogCtx::instance().ScopedLock()};
        if (LogCtx::instance().isBitSet(DEBUG_WIN32_OUT)) {
            OutputDebugStringW(sLine.GetString());
        }
        if (LogCtx::instance().isBitSet(LOG_ENABLED)) {
            LogCtx::instance().putws(std::move(sLine));
        }
        if (LogCtx::instance().isBitSet(DEBUG_DEVCON_OUT)) {
            DH::DebugConsole::Instance().PutsWide(nLevel, {sText.GetString(), static_cast<size_t>(sText.GetLength())}, dwTID);
        }
    }

    #define TRACE_LEVEL_LEN         16

    template <typename Char>
    struct TTTraits {};

    template <> struct TTTraits<char>
    {
        static inline const PCSTR  FmtTS{"%0*.8f "};
        static inline const PCSTR FmtTID{"[%06d] "};
        static inline const PCSTR FmtCat{"%-" DH_STRINGIZE(TRACE_LEVEL_LEN) "s "};

        static BOOL StrTruncate(char* szBuffer, UINT nLen, std::string_view svText, DWORD dwFlags)
        {
            return PathCompactPathExA(szBuffer, svText.data(), nLen, dwFlags);
        }
    };

    template <> struct TTTraits<wchar_t>
    {
        static inline const PCWSTR  FmtTS{L"%0*.8f "};
        static inline const PCWSTR FmtTID{L"[%06d] "};
        static inline const PCWSTR FmtCat{L"%-" DH_WSTRINGIZE(TRACE_LEVEL_LEN) L"s "};

        static BOOL StrTruncate(wchar_t* szBuffer, UINT nLen, std::wstring_view svText, DWORD dwFlags)
        {
            return PathCompactPathExW(szBuffer, svText.data(), nLen, dwFlags);
        }
    };

    template <typename Char>
    static std::basic_string<Char> strTruncateLevel(std::basic_string<Char>&& szLevel)
    {
        if (szLevel.empty()) {
            return {};
        }
        if (szLevel.length() < (TRACE_LEVEL_LEN-1)) {
            return std::move(szLevel);
        }
        Char szBuffer[TRACE_LEVEL_LEN]{0};
        if (!TTTraits<Char>::StrTruncate(szBuffer, _countof(szBuffer), szLevel, DT_WORD_ELLIPSIS)) {
            return {szLevel.c_str(), std::min<size_t>(TRACE_LEVEL_LEN-1, szLevel.length())};
        }
        return {szBuffer};
    }

    template <typename Char>
    static std::basic_string<Char> strLevel(unsigned nLevel)
    {
        std::basic_string<Char> szLevel;
        StrTraceLevel(nLevel, szLevel);
        return strTruncateLevel(std::move(szLevel));
    }

    template <typename Char>
    static void printStringT(unsigned nLevel,
       _Printf_format_string_ std::basic_string_view<Char> svFormat,
        va_list ap)
    {
        auto const dwTID{GetCurrentThreadId()};
        auto         sTS{Str::Elipsis<Char>::Format(TTTraits<Char>::FmtTS, LOG_UPTIME_PRECISS, LOG_Uptime.Seconds())};
        auto        sTID{Str::Elipsis<Char>::Format(TTTraits<Char>::FmtTID, dwTID)};
        auto        sLev{Str::Elipsis<Char>::Format(TTTraits<Char>::FmtCat, strLevel<Char>(nLevel).c_str())};
        auto       sText{Str::Elipsis<Char>::FormatV(svFormat.data(), ap)};
        printString(nLevel, std::move(sTS), std::move(sTID), dwTID, std::move(sLev), std::move(sText));
    }

    void TPrintf(unsigned nLevel, _Printf_format_string_ std::string_view svFormat, ...)
    {
        va_list ap;
        va_start(ap, svFormat);
        printStringT<char>(nLevel, svFormat, ap);
        va_end(ap);
    }

    void TPrintf(unsigned nLevel, _Printf_format_string_ std::wstring_view svFormat, ...)
    {
        va_list ap;
        va_start(ap, svFormat);
        printStringT<wchar_t>(nLevel, svFormat, ap);
        va_end(ap);
    }

    void Printf(unsigned nLevel, _Printf_format_string_ std::string_view svFormat, ...)
    {
        auto const dwTID{GetCurrentThreadId()};
        auto        sLev{Str::Elipsis<char>::Format(TTTraits<char>::FmtCat, strLevel<char>(nLevel).c_str())};
        va_list ap;
        va_start(ap, svFormat);
        printString(nLevel, {}, {}, dwTID, std::move(sLev), Str::Elipsis<char>::FormatV(svFormat.data(), ap));
        va_end(ap);
    }

    void Printf(unsigned nLevel, _Printf_format_string_ std::wstring_view svFormat, ...)
    {
        auto const dwTID{GetCurrentThreadId()};
        auto        sLev{Str::Elipsis<wchar_t>::Format(TTTraits<wchar_t>::FmtCat, strLevel<wchar_t>(nLevel).c_str())};
        va_list ap;
        va_start(ap, svFormat);
        printString(nLevel, {}, {}, dwTID, std::move(sLev), Str::Elipsis<wchar_t>::FormatV(svFormat.data(), ap));
        va_end(ap);
    }

#pragma endregion
#pragma region Scoped

    static inline void _Start_ScopedThreadLog(PCWSTR message) 
    {
        TPrintf(TL_Module, L"%s thread started...\n", message);
    }

    static inline void _Stop_ScopedThreadLog(PCWSTR message, DH::Timer const& liveTime)
    {
        TPrintf(TL_Module, L"%s thread stopped %f seconds (%f hours)\n", message,
            liveTime.Seconds(), liveTime.Seconds() / 3600.); 
    }

    ScopedThreadLog::ScopedThreadLog(std::wstring_view nLevel)
        : Time()
    {
        memset(Message, 0, std::size(Message));
        wcsncpy_s(Message, nLevel.data(), std::min<size_t>(nLevel.length(), std::size(Message)));

        _Start_ScopedThreadLog(Message);
    }

    ScopedThreadLog::ScopedThreadLog(int, _Printf_format_string_ std::wstring_view svFormat, ...)
        : Time()
    {
        memset(Message, 0, _countof(Message)-1);
        va_list ap;
        va_start(ap, svFormat);
        ATL::CStringW temp;
        temp.FormatV(svFormat.data(), ap);
        va_end(ap);
        wcsncpy_s(Message, temp, std::min<size_t>(static_cast<size_t>(temp.GetLength()), std::size(Message)-1));
        _Start_ScopedThreadLog(Message);
    }

    ScopedThreadLog::~ScopedThreadLog()
    {
        _Stop_ScopedThreadLog(Message, Time);
    }

#pragma endregion

    void PrintLogHeader()
    {
        if (0 != (LogCtx::instance().isBitSet(DEBUG_EXTRA_INFO))) {
            std::wostringstream temp;
            Runtime::Info().SimpleReport(temp);
            std::wstring const report{temp.str()};
            TPrintf(TL_Module, L"System info:\n%s", report.c_str());
        }
        else {
            TPrintf(TL_Module, L"Launched on %s [%s @'%s']\n",
                Runtime::Info().Host.Name.c_str(),
                Runtime::Info().System.Version.DisplayName.c_str(),
                ModuleDir().c_str()
            );
        }
    }

    void InitDebugHelpers(uint64_t flags)
    {
        SetConsoleCP(LOG_DEF_CODEPAGE);
        SetConsoleOutputCP(LOG_DEF_CODEPAGE);

        auto& logCtx = LogCtx::instance();
        logCtx.setBits(flags);
        PrintLogHeader();
    }

    double LogUpTime()
    {
        return LOG_Uptime.Seconds();
    }
}
