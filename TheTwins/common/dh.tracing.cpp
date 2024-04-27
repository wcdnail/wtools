#include "stdafx.h"
#include "dh.tracing.h"
#include "dh.timer.h"
#include "string.utils.format.h"
#include "windows.gui.leaks.h"
#include "info/runtime.information.h"
#include <fstream>
#include <atlstr.h>
#include <atlconv.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <filesystem>
#include <memory>
#include <mutex>

#ifndef _TRACE_LOG_ENABLED
#  define _TRACE_LOG_ENABLED true
#endif

#ifdef _DEBUG
#  define _DEBUG_EXTRA_INFO
#endif

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
        if (0 == ::_wcsnicmp(name, _MY_MODULE, ::wcslen(_MY_MODULE)))
        {
            std::filesystem::path tempath = Runtime::Info().Executable.Filename;
            Name = boost::to_upper_copy(tempath.filename().replace_extension().wstring());
        }
        else {
            Name = name;
        }
    }

    TraceCategory::~TraceCategory()
    {}

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
        using ABool = std::atomic<bool>;
        using Mutex = std::recursive_mutex;

        struct ScopedLockFree
        {
            ScopedLockFree() {}
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
        };

        ScopedLockFree ScopedLock()
        {
            return multiThreaded_ ? ScopedGuard(mx_) : ScopedLockFree();
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
            if (IsLogEnabled()) {
                log_ << std::flush;
            }
        }

        static LogCtx& instance(bool mt = true)
        {
            static LogCtx self(mt);
            if (!self.isInitialized_) {
                ScopedGuard lk(self.mx_);
                if (self.logEnabled_) {
                    std::string name = GenLogName();
                    self.log_.open(name.c_str());
                }
                self.isInitialized_ = true;
            }
            return self;
        }

    private:
        Mutex              mx_;
        std::ofstream     log_;
        bool       logEnabled_;
        bool    multiThreaded_;
        ABool   isInitialized_;

        LogCtx(bool multiThreaded)
            :            mx_()
            ,           log_()
            ,    logEnabled_(_TRACE_LOG_ENABLED)
            , multiThreaded_(multiThreaded)
            , isInitialized_(false)
        {
        }

        ~LogCtx()
        {
            FlushLog();
        }

        static std::filesystem::path GetLogPath(int n)
        {
            std::filesystem::path result = Runtime::Info().Executable.Directory;
            result /= Runtime::Info().Executable.Version.ProductName + L".log." 
                    + boost::lexical_cast<std::wstring>(n) + L".txt";
            return result;
        }

        static std::string GenLogName()
        {
            std::filesystem::path temp;
            for (int i=2; i>=0; i--) {
                temp = GetLogPath(i);
                if (std::filesystem::exists(temp)) {
                    std::filesystem::path next = GetLogPath(i + 1);
                    std::error_code ec;
                    std::filesystem::remove(next, ec);
                    std::filesystem::rename(temp, next, ec);
                }
            }
            return temp.string();
        }

        bool IsLogEnabled() const { return logEnabled_ && log_.is_open(); }
        void PutsImpl(char const* text) { log_ << text; }

        void PutsImpl(wchar_t const* text)
        {
            if (text && *text) {
                size_t len = ::wcslen(text) + 1;
                CStringA temp;
                int cl = ::WideCharToMultiByte(CP_ACP, 0, text, (int)len, temp.GetBufferSetLength((int)len), (int)len, NULL, NULL);
                temp.ReleaseBufferSetLength(cl);
                log_ << (char const*)temp; 
            }
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
        Puts(Str::Elipsis<wchar_t>::Format(L"%0*.8f [%06d] %8s: ", 20, _Uptime.Seconds(), ::GetCurrentThreadId(), cat.GetName()));
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
        ::memset(Message, 0, _countof(Message)-1);
        ::wcsncpy_s(Message, message, min(::wcslen(message), _countof(Message)-1));

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

        ::wcsncpy_s(Message, temp, min(temp.GetLength(), (int) _countof(Message)-1));

        va_end(ap);

        _Start_ScopedThreadLog(Message);
    }

    ScopedThreadLog::~ScopedThreadLog()
    {
        _Stop_ScopedThreadLog(Message, Time);
    }

    void PrintLogHeader()
    {
        LogCtx::instance();

#ifdef _DEBUG_EXTRA_INFO
        std::wostringstream temp;
        Runtime::Info().SimpleReport(temp);
        std::wstring report = temp.str();
        ThreadPrintfc(Category::Module, L"System info:\n%s", report.c_str());
#else
        ThreadPrintfc(Category::Module, L"Launched on %s (%s)\n",
            Runtime::Info().Host.Name.c_str(),
            Runtime::Info().System.Version.DisplayName.c_str()
        );
#endif
    }
}
