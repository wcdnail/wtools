#include "stdafx.h"
#include "dh.tracing.h"
#include "dh.timer.h"
#include "string.utils.format.h"
#include "info/runtime.information.h"
#include <atlstr.h>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <memory>
#include <atomic>
#include <mutex>

enum TracerFlags: uint64_t
{
    _TRACE_LOG_MULTITHREADED = 0x0000000000000001,
    _TRACE_LOG_ENABLED       = 0x0000000000000002,
    _DEBUG_EXTRA_INFO        = 0x0000000000000004,
};

static constexpr int _TRACE_LOG_MAX_ROTATED_FILES = 3;

namespace Dh
{
    #define _MY_MODULE L"!$$$GETMODULENAME$$#$>>!"

    namespace Category
    {
        const TraceCategory Module(_MY_MODULE);
        const TraceCategory Exception(L"EXCPTION");
    }

    TraceCategory::TraceCategory(wchar_t const* name)
        : Name()
    {
        if (0 == ::_wcsnicmp(name, _MY_MODULE, ::wcslen(_MY_MODULE))) {
            std::filesystem::path tempath = Runtime::Info().Executable.Filename;
            Name = tempath.filename().replace_extension().wstring();
        }
        else {
            Name = name;
        }
    }

    TraceCategory::~TraceCategory()
    {}

    WidecharString const& TraceCategory::GetName() const
    {
        return Name;
    }

    namespace
    {
        struct UpTimer: Timer
        {
            UpTimer(): Timer() {}
            ~UpTimer() {}
            using Timer::Seconds;
        };
        const UpTimer _Uptime;
    }

    struct LogCtx
    {
        using        ABool = std::atomic<bool>;
        using        Mutex = std::recursive_mutex;
        using LogStreamPtr = std::unique_ptr<std::ofstream>;

        LogCtx(LogCtx const&) = delete;
        LogCtx& operator = (LogCtx const&) = delete;

        struct ScopedLockFree
        {
            ScopedLockFree(ScopedLockFree const&) {}
            explicit ScopedLockFree() {}
            virtual ~ScopedLockFree() {}
        };

        struct ScopedGuard: ScopedLockFree
        {
            Mutex& mx_;
            ScopedGuard(Mutex& mx)
                : mx_(mx)
            {
                mx_.lock();
            }
            ~ScopedGuard() override
            {
                mx_.unlock();
            }

            ScopedGuard& operator = (ScopedGuard const&) = delete;
        };

        ScopedLockFree ScopedLock()
        {
            return (0 != (flags_ & _TRACE_LOG_MULTITHREADED)) ? ScopedGuard(mx_) : ScopedLockFree();
        }

        template <typename C>
        void Puts(C const* text)
        {
            if (IsLogEnabled()) {
                PutsImpl(text);
            }
        }

        void FlushLog()
        {
            if (log_) {
                (*log_) << std::flush;
            }
        }

        bool CheckFlag(uint64_t bits) const
        {
            return (0 != (flags_ & bits));
        }

        static LogCtx& instance()
        {
            static LogCtx self(_TRACE_LOG_MULTITHREADED);
            return self;
        }

    private:
        uint64_t   flags_;
        Mutex         mx_;
        LogStreamPtr log_;

        LogCtx(uint64_t flags)
            : flags_(flags)
            ,    mx_()
            ,   log_()
        {
            if (0 != (flags_ & _TRACE_LOG_ENABLED)) {
                log_ = std::make_unique<std::ofstream>(GenLogName());
            }
        }

        ~LogCtx()
        {
            FlushLog();
        }

        static std::filesystem::path GetLogFilepath(int n)
        {
            std::filesystem::path result = Runtime::Info().Executable.Directory;

            result /= Runtime::Info().Executable.Version.ProductName + 
                L".log." +
                std::to_wstring(n) +
                L".txt";

            return result;
        }

        static std::string GenLogName()
        {
            std::filesystem::path temp;
            int i = _TRACE_LOG_MAX_ROTATED_FILES - 1;
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

        bool IsLogEnabled() const { return log_ && log_->is_open(); }
        void PutsImpl(char const* text) { if (log_) (*log_) << text; }

        void PutsImpl(wchar_t const* text)
        {
            if (!log_ || !text || !*text) {
                return ;
            }
            int len = static_cast<int>(::wcslen(text)) + 1;
            CStringA temp;
            PSTR   buffer = temp.GetBufferSetLength(len);
            int cen = ::WideCharToMultiByte(CP_ACP, 0, text, len, buffer, len, nullptr, nullptr);
            temp.ReleaseBufferSetLength(cen);
            (*log_) << temp.GetString(); 
        }
    };

    static void Puts(char const* text) 
    {
        ::OutputDebugStringA(text); 
        LogCtx::instance().Puts(text);
    }

    static void Puts(wchar_t const* text)
    { 
        ::OutputDebugStringW(text); 
        LogCtx::instance().Puts(text);
    }

    void Printf(PCSTR format, ...)
    {
        va_list ap;
        va_start(ap, format);
        {
            auto lk = LogCtx::instance().ScopedLock();
            Puts(Str::Elipsis<char>::FormatV(format, ap));
            LogCtx::instance().FlushLog();
        }
        va_end(ap);
    }

    void Printf(PCWSTR format, ...)
    {
        va_list ap;
        va_start(ap, format);
        {
            auto lk = LogCtx::instance().ScopedLock();
            Puts(Str::Elipsis<wchar_t>::FormatV(format, ap));
            LogCtx::instance().FlushLog();
        }
        va_end(ap);
    }

    template <typename C>
    static void ThreadPrintfT(C const* format, va_list ap)
    {
        auto lk = LogCtx::instance().ScopedLock();
        Puts(Str::Elipsis<wchar_t>::Format(L"%0*.8f [%06d] ", 20, _Uptime.Seconds(), ::GetCurrentThreadId()));
        Puts(Str::Elipsis<C>::FormatV(format, ap));
        LogCtx::instance().FlushLog();
    }

    template <typename C>
    static void ThreadPrintfT(TraceCategory const& cat, C const* format, va_list ap)
    {
        auto lk = LogCtx::instance().ScopedLock();
        Puts(Str::Elipsis<wchar_t>::Format(L"%0*.8f [%06d] %8s: ", 20, _Uptime.Seconds(), ::GetCurrentThreadId(), cat.GetName().c_str()));
        Puts(Str::Elipsis<C>::FormatV(format, ap));
        LogCtx::instance().FlushLog();
    }

    void ThreadPrintf(char const* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        ThreadPrintfT<char>(format, ap);
        va_end(ap);
    }

    void ThreadPrintf(wchar_t const* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        ThreadPrintfT<wchar_t>(format, ap);
        va_end(ap);
    }

    void ThreadPrintfc(TraceCategory const& cat, char const* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        ThreadPrintfT<char>(cat, format, ap);
        va_end(ap);
    }

    void ThreadPrintfc(TraceCategory const& cat, wchar_t const* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        ThreadPrintfT<wchar_t>(cat, format, ap);
        va_end(ap);
    }

    static inline void _Start_ScopedThreadLog(PCWSTR message) 
    {
        ThreadPrintfc(Category::Module, L"%s thread started...\n", message);
    }

    static inline void _Stop_ScopedThreadLog(PCWSTR message, Dh::Timer const& liveTime)
    {
        ThreadPrintfc(Category::Module, L"%s thread stopped %f seconds (%f hours)\n", message,
            liveTime.Seconds(), liveTime.Seconds() / 3600.); 
    }

    ScopedThreadLog::ScopedThreadLog(wchar_t const* message)
        : Time()
    {
        ::memset(Message, 0, std::size(Message));
        ::wcsncpy_s(Message, message, std::min<size_t>(::wcslen(message), std::size(Message)));

        _Start_ScopedThreadLog(Message);
    }

    ScopedThreadLog::ScopedThreadLog(int, wchar_t const* format, ...)
        : Time()
    {
        ::memset(Message, 0, _countof(Message)-1);
        va_list ap;
        va_start(ap, format);
        CStringW temp;
        temp.FormatV(format, ap);
        ::wcsncpy_s(Message, temp, std::min<size_t>(static_cast<size_t>(temp.GetLength()), std::size(Message)-1));
        va_end(ap);
        _Start_ScopedThreadLog(Message);
    }

    ScopedThreadLog::~ScopedThreadLog()
    {
        _Stop_ScopedThreadLog(Message, Time);
    }

    void PrintLogHeader()
    {
        if (0 != (LogCtx::instance().CheckFlag(_DEBUG_EXTRA_INFO))) {
            std::wostringstream temp;
            Runtime::Info().SimpleReport(temp);
            std::wstring report = temp.str();
            ThreadPrintfc(Category::Module, L"System info:\n%s", report.c_str());
        }
        else {
            ThreadPrintfc(Category::Module, L"Launched on %s (%s)\n",
                Runtime::Info().Host.Name.c_str(),
                Runtime::Info().System.Version.DisplayName.c_str()
            );
        }
    }
}
