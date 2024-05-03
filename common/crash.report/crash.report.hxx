#ifndef _crash_report_hxx__
#define _crash_report_hxx__

#include "run.context.h"
#include "crash.report.h"
#include "exception.h"

#ifdef __cplusplus

#include <cerrno>
#include <exception>
#include <boost/exception/all.hpp>

#ifdef _WIN32
#include <comdef.h>
#include <atlconv.h>
#endif

namespace Crash
{
    inline void OnException(Exception const& ex, bool terminateProcess = false)
    {
        crash_report(ex.what(),
            ex.GetSource(),
            ex.GetLine(),
            ex.GetErrorCode(),
            ex.GetRunContext(),
            terminateProcess ? 1 : 0);
    }

    inline void OnException(std::exception const& ex, int ec = errno, RUN_CTX_Ptr rcPtr = cxx_capture_run_context(), bool terminateProcess = false)
    {
        CRASH_REPORT_INT(ex.what(), ec, rcPtr.get(), terminateProcess ? 1 : 0);
    }

    inline void OnException(boost::exception const& ex, int ec = errno, RUN_CTX_Ptr rcPtr = cxx_capture_run_context(), bool terminateProcess = false)
    {
        CRASH_REPORT_INT(boost::diagnostic_information_what(ex), ec, rcPtr.get(), terminateProcess ? 1 : 0);
    }

#ifdef _WIN32
    inline void OnException(_com_error const& ex, RUN_CTX_Ptr rcPtr = cxx_capture_run_context(), bool terminateProcess = false)
    {
        CRASH_REPORT_INT(CT2A(ex.ErrorMessage()).m_psz, (int)ex.Error(), rcPtr.get(), terminateProcess ? 1 : 0);
    }
#endif

    inline void OnException(int ec = errno, RUN_CTX_Ptr rcPtr = cxx_capture_run_context(), bool terminateProcess = false)
    {
        CRASH_REPORT_INT("catch (...)", ec, rcPtr.get(), terminateProcess ? 1 : 0);
    }

    inline void GenericExceptionHandler(int ec = errno, RUN_CTX_Ptr rcPtr = cxx_capture_run_context(), bool terminateProcess = false)
    {
        try
        {
            throw ;
        }
#ifdef _WIN32
        catch (_com_error const& ex)
        {
            OnException(ex, std::move(rcPtr), terminateProcess);
        }
#endif
        catch (Exception const& ex)
        {
            OnException(ex, terminateProcess);
        }
        catch (std::exception const& ex)
        {
            OnException(ex, ec, std::move(rcPtr), terminateProcess);
        }
        catch (boost::exception const& ex)
        {
            OnException(ex, ec, std::move(rcPtr), terminateProcess);
        }
        catch (...)
        {
            OnException(ec, std::move(rcPtr), terminateProcess);
        }
    }
}

#endif /* __cplusplus */

#endif /* _crash_report_hxx__ */
