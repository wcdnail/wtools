#include "stdafx.h"
#include "dh.tracing.h"
#include "dh.timer.h"
#include "string.utils.format.h"
#include "info/runtime.information.h"
#include <fstream>
#include <atlstr.h>
#include <atlconv.h>
#include <boost/thread/mutex.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#ifndef _TRACE_LOG_ENABLED
#  define _TRACE_LOG_ENABLED true
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
            boost::filesystem::path tempath = Runtime::Info().Executable.Filename;
            Name = boost::to_upper_copy(tempath.filename().replace_extension().wstring());
        }
        else
            Name = name;
    }

    TraceCategory::~TraceCategory()
    {}

    namespace
    {
        struct UpTimer: Timer
        {
            UpTimer()
                : Timer()
            {}

            ~UpTimer() {}

            using Timer::Seconds;
        };

        const UpTimer _Uptime;
    }

    // WARN: Don't use virtual functions in this.
    struct TracingContext
    {
        boost::mutex& Mutex() { return MyMx; }

        template <typename C>
        void Puts(C const* text)
        {
            if (IsLogEnabled())
                PutsImpl(text);
        }

        void FlushLog()
        {
            if (IsLogEnabled())
                Log << std::flush;
        }

    private:
        friend TracingContext& TraceContext();

        TracingContext() 
            : MyMx()
            , LogEnabled(_TRACE_LOG_ENABLED)
            , Log() 
        {
            if (LogEnabled)
            {
                std::string name = GenLogName();
                Log.open(name.c_str());

#ifdef _DEBUG_EXTRA_INFO
                std::wostringstream temp;
                Runtime::Info().SimpleReport(temp);
                ThreadPrintfc(Category::Module, "System info:\n%s", temp.str().c_str());
#else
                ThreadPrintfc(Category::Module, L"Launched on %s (%s)\n"
                    , Runtime::Info().Host.Name.c_str()
                    , Runtime::Info().System.Version.DisplayName.c_str()
                    );
#endif
            }
        }

        ~TracingContext() 
        {
            FlushLog();
        }

        static boost::filesystem::path GetLogPath(int n)
        {
            boost::filesystem::path result = Runtime::Info().Executable.Directory;

            result /= Runtime::Info().Executable.Version.ProductName + L".log." 
                    + boost::lexical_cast<std::wstring>(n) + L".txt";

            return result;
        }

        static std::string GenLogName()
        {
            boost::filesystem::path temp;

            for (int i=2; i>=0; i--)
            {
                temp = GetLogPath(i);
                if (boost::filesystem::exists(temp))
                {
                    boost::filesystem::path next = GetLogPath(i + 1);

                    boost::system::error_code ec;
                    boost::filesystem::remove(next, ec);
                    boost::filesystem::rename(temp, next, ec);
                }
            }

            return temp.string();
        }

        bool IsLogEnabled() const { return LogEnabled && Log.is_open(); }
        void PutsImpl(char const* text) { Log << text; }

        void PutsImpl(wchar_t const* text)
        {
            if (text && *text)
            {
                size_t len = ::wcslen(text) + 1;

                CStringA temp;
                int cl = ::WideCharToMultiByte(CP_ACP, 0, text, (int)len, temp.GetBufferSetLength((int)len), (int)len, NULL, NULL);
                temp.ReleaseBufferSetLength(cl);

                Log << (char const*)temp; 
            }
        }

    private:
        boost::mutex MyMx;
        bool LogEnabled;
        std::ofstream Log;
    };

    TracingContext& TraceContext()
    {
        static TracingContext instance;
        return instance;
    }

    static void Puts(char const* text) 
    {
        ::OutputDebugStringA(text); 
        TraceContext().Puts(text);
    }

    static void Puts(wchar_t const* text)
    { 
        ::OutputDebugStringW(text); 
        TraceContext().Puts(text);
    }

    void Printf(PCSTR format, ...)
    {
        va_list ap;
        va_start(ap, format);
        {
            boost::mutex::scoped_lock lk(TraceContext().Mutex());
            Puts(Str::Elipsis<char>::FormatV(format, ap));
            TraceContext().FlushLog();
        }
        va_end(ap);
    }

    void Printf(PCWSTR format, ...)
    {
        va_list ap;
        va_start(ap, format);
        {
            boost::mutex::scoped_lock lk(TraceContext().Mutex());
            Puts(Str::Elipsis<wchar_t>::FormatV(format, ap));
            TraceContext().FlushLog();
        }
        va_end(ap);
    }

    template <typename C>
    static void ThreadPrintfT(C const* format, va_list ap)
    {
        boost::mutex::scoped_lock lk(TraceContext().Mutex());
        Puts(Str::Elipsis<wchar_t>::Format(L"%0*.8f [%06d] ", 20, _Uptime.Seconds(), ::GetCurrentThreadId()));
        Puts(Str::Elipsis<C>::FormatV(format, ap));
        TraceContext().FlushLog();
    }

    template <typename C>
    static void ThreadPrintfT(TraceCategory const& cat, C const* format, va_list ap)
    {
        boost::mutex::scoped_lock lk(TraceContext().Mutex());
        Puts(Str::Elipsis<wchar_t>::Format(L"%0*.8f [%06d] %8s: ", 20, _Uptime.Seconds(), ::GetCurrentThreadId(), cat.GetName()));
        Puts(Str::Elipsis<C>::FormatV(format, ap));
        TraceContext().FlushLog();
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
        ThreadPrintfc(Category::Module, L"%s thread stopped %f seconds (%f hours)\n", message
            , liveTime.Seconds(), liveTime.Seconds() / 3600.); 
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
}
