#ifndef _cf_exceptions_any_h__
#define _cf_exceptions_any_h__

#ifdef _MSC_VER
#pragma once
#endif

#include <dh.tracing.h>
#include <crash.report/exception.h>
#include <crash.report/run.context.h>
#include <crash.report/run.context.info.h>
#include <boost/exception/current_exception_cast.hpp>
#include <boost/exception/all.hpp>
#include <stdexcept>

namespace Cf
{
    namespace AnyException
    {
        template <typename P>
        inline void HandleIt(boost::exception const& ex, P const& pred, char const* codetext, char const* callstack)
        {
            pred("boost::exception", boost::diagnostic_information(ex).c_str(), codetext, callstack);
        }

        template <typename P>
        inline void HandleIt(std::exception const& ex, P const& pred, char const* codetext, char const* callstack)
        {
            pred("std::exception", ex.what(), codetext, callstack);
        }

        template <typename T, typename P>
        inline void HandleItSpec(T const& ex, P const& pred, int lcode, RUN_CONTEXT const& runctx)
        {
            STRING_BUFFER callstack;
            ::sb_zero(&callstack);
            ::describe_callstack(&callstack, &runctx);

            STRING_BUFFER lcodetext;
            ::sb_zero(&lcodetext);
            ::describe_operation_code(&lcodetext, lcode);

            try
            {
                HandleIt(ex, pred, lcodetext.string, callstack.string);
            }
            catch (...)
            {
                // ACHTUNG: implement this...
            }

            ::sb_free(&lcodetext);
            ::sb_free(&callstack);
        }

        template <typename P>
        inline void HandleIt(Crash::Exception const& ex, P const& pred, int, RUN_CONTEXT const&)
        {
            HandleItSpec<Crash::Exception, P>(ex, pred, ex.GetErrorCode(), *ex.GetRunContext());
        }

        template <typename T, typename P>
        inline void HandleIt(T const& ex, P const& pred, int lcode, RUN_CONTEXT const& runctx)
        {
            HandleItSpec<T, P>(ex, pred, lcode, runctx);
        }

        template <typename T, typename P>
        inline bool TryIt(P const& pred, int lcode, RUN_CONTEXT const& runctx)
        {
            T* ex = boost::current_exception_cast<T>();

            if (NULL != ex)
            {
                HandleIt(*ex, pred, lcode, runctx);
                return true;
            }

            return false;
        }
    }

    struct HandleAnyExceptionDefaults
    {
        void operator () (char const* caption, char const* what, char const* codetext, char const* callstack) const
        {
            ::MessageBoxA(NULL, what, caption, MB_ICONEXCLAMATION);
            Dh::ThreadPrintfc(Dh::Category::Exception, "%s\n", what);
        }
    };

    template <typename P>
    inline bool HandleAnyException(P const& pred)
    {
        int lcode = ::get_last_error_code();
        RUN_CONTEXT runctx = {0};

        if (AnyException::TryIt<Crash::Exception>(pred, lcode, runctx))
            return true;
        
        runctx = ::capture_context();

        if (AnyException::TryIt<boost::exception>(pred, lcode, runctx))
            return true;

        if (AnyException::TryIt<std::exception>(pred, lcode, runctx))
            return true;

        return false;
    }
}

#endif // _cf_exceptions_any_h__
