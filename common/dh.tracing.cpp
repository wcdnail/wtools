#include "stdafx.h"
#include "dh.tracing.h"
#include "dh.timer.h"
#include "string.utils.format.h"
#include "info/runtime.information.h"
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
        static const wchar_t   LOG_MODULE_NPH[] = L"_MY_EXE_NAME_PLACEHOLDER";

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
        return isBitSet(LOG_MUTEX_GUARD) ? ScopedGuard(mx_) : ScopedLockFree();
    }

    LogPath LogCtx::GetLogFilepath(int n)
    {
        LogPath result = Runtime::Info().Executable.Directory;

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
#pragma region Category

    namespace Category
    {
        DH::TraceCategory const& WTL()
        {
            static const TraceCategory inst(L"WTL");
            return inst;
        }

        DH::TraceCategory const& Module()
        {
            static const TraceCategory inst(LOG_MODULE_NPH);
            return inst;
        }

        DH::TraceCategory const& Exception()
        {
            static const TraceCategory inst(L"EXCEPTION");
            return inst;
        }
    }

    TraceCategory::~TraceCategory() = default;

    TraceCategory::TraceCategory(PCWSTR pszName)
        : Name{}
    {
        if (0 == ::_wcsnicmp(pszName, LOG_MODULE_NPH, std::size(LOG_MODULE_NPH) / sizeof(LOG_MODULE_NPH[0]))) {
            Runtime::InfoStore::String const* pName{&Runtime::Info().Executable.Version.ProductName};
            if (pName->empty()) {
                pName = &Runtime::Info().Executable.Filename;
            }
            std::filesystem::path const tempath{*pName};
            Name = tempath.filename().replace_extension().wstring();
        }
        else {
            Name = pszName;
        }
    }

    WString const& TraceCategory::GetName() const
    {
        return Name;
    }

#pragma endregion
#pragma region Tracers

    static void printString(ATL::CStringA&& text)
    {
        auto logGuard = LogCtx::instance().ScopedLock();
        if (LogCtx::instance().isBitSet(DEBUG_WIN32_OUT)) {
            OutputDebugStringA(text.GetString());
        }
        LogCtx::instance().puts(std::move(text));
    }

    static void printString(ATL::CStringW&& text)
    {
        auto logGuard = LogCtx::instance().ScopedLock();
        if (LogCtx::instance().isBitSet(DEBUG_WIN32_OUT)) {
            OutputDebugStringW(text.GetString());
        }
        LogCtx::instance().putws(std::move(text));
    }

    void Printf(PCSTR format, ...)
    {
        va_list ap;
        va_start(ap, format);
        printString(Str::Elipsis<char>::FormatV(format, ap));
        va_end(ap);
    }

    void Printf(PCWSTR format, ...)
    {
        va_list ap;
        va_start(ap, format);
        printString(Str::Elipsis<wchar_t>::FormatV(format, ap));
        va_end(ap);
    }

    template <typename CharType>
                struct TTTraits {};
    template <> struct TTTraits<char>    { static inline const  PCSTR Header =  "%0*.8f [%06d] "; static inline const  PCSTR Category =  "%14s| "; };
    template <> struct TTTraits<wchar_t> { static inline const PCWSTR Header = L"%0*.8f [%06d] "; static inline const PCWSTR Category = L"%14s| "; };

    template <typename CharType>
    static void ThreadPrintfT(CharType const* format, va_list ap)
    {
        auto header = Str::Elipsis<CharType>::Format(TTTraits<CharType>::Header, LOG_UPTIME_PRECISS, LOG_Uptime.Seconds(), ::GetCurrentThreadId());
        auto   text = Str::Elipsis<CharType>::FormatV(format, ap);
        printString(header + text);
    }

    template <typename CharType>
    static void ThreadPrintfT(TraceCategory const& cat, CharType const* format, va_list ap)
    {
        auto   header = Str::Elipsis<CharType>::Format(TTTraits<CharType>::Header, LOG_UPTIME_PRECISS, LOG_Uptime.Seconds(), ::GetCurrentThreadId());
        auto category = Str::Elipsis<CharType>::Format(TTTraits<CharType>::Category, cat.GetName().c_str());
        auto     text = Str::Elipsis<CharType>::FormatV(format, ap);
        printString(header + category + text);
    }

    void TPrintf(PCSTR format, ...)
    {
        va_list ap;
        va_start(ap, format);
        ThreadPrintfT<char>(format, ap);
        va_end(ap);
    }

    void TPrintf(PCWSTR format, ...)
    {
        va_list ap;
        va_start(ap, format);
        ThreadPrintfT<wchar_t>(format, ap);
        va_end(ap);
    }

    void TCPrintf(TraceCategory const& cat, PCSTR format, ...)
    {
        va_list ap;
        va_start(ap, format);
        ThreadPrintfT<char>(cat, format, ap);
        va_end(ap);
    }

    void TCPrintf(TraceCategory const& cat, PCWSTR format, ...)
    {
        va_list ap;
        va_start(ap, format);
        ThreadPrintfT<wchar_t>(cat, format, ap);
        va_end(ap);
    }

#pragma endregion
#pragma region Scoped

    static inline void _Start_ScopedThreadLog(PCWSTR message) 
    {
        TCPrintf(Category::Module(), L"%s thread started...\n", message);
    }

    static inline void _Stop_ScopedThreadLog(PCWSTR message, DH::Timer const& liveTime)
    {
        TCPrintf(Category::Module(), L"%s thread stopped %f seconds (%f hours)\n", message,
            liveTime.Seconds(), liveTime.Seconds() / 3600.); 
    }

    ScopedThreadLog::ScopedThreadLog(PCWSTR message)
        : Time()
    {
        ::memset(Message, 0, std::size(Message));
        ::wcsncpy_s(Message, message, std::min<size_t>(::wcslen(message), std::size(Message)));

        _Start_ScopedThreadLog(Message);
    }

    ScopedThreadLog::ScopedThreadLog(int, PCWSTR format, ...)
        : Time()
    {
        ::memset(Message, 0, _countof(Message)-1);
        va_list ap;
        va_start(ap, format);
        ATL::CStringW temp;
        temp.FormatV(format, ap);
        ::wcsncpy_s(Message, temp, std::min<size_t>(static_cast<size_t>(temp.GetLength()), std::size(Message)-1));
        va_end(ap);
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
            std::wstring report = temp.str();
            TCPrintf(Category::Module(), L"System info:\n%s", report.c_str());
        }
        else {
            TCPrintf(Category::Module(), L"Launched on %s (%s)\n",
                Runtime::Info().Host.Name.c_str(),
                Runtime::Info().System.Version.DisplayName.c_str()
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
