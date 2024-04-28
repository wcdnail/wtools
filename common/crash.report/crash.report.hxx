#ifndef _crash_report_hxx__
#define _crash_report_hxx__

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
        ::crash_report(ex.what()
            , ex.GetSource()
            , ex.GetLine()
            , ex.GetErrorCode()
            , ex.GetRunContext()
            , terminateProcess ? 1 : 0);
    }

    inline void OnException(std::exception const& ex, int ec = errno, RUN_CONTEXT const& rc = ::capture_context(), bool terminateProcess = false)
    {
        CRASH_REPORT_INT(ex.what(), ec, &rc, terminateProcess ? 1 : 0);
    }

    inline void OnException(boost::exception const& ex, int ec = errno, RUN_CONTEXT const& rc = ::capture_context(), bool terminateProcess = false)
    {
        CRASH_REPORT_INT(boost::diagnostic_information_what(ex), ec, &rc, terminateProcess ? 1 : 0);
    }

#ifdef _WIN32
    inline void OnException(_com_error const& ex, RUN_CONTEXT const& rc = ::capture_context(), bool terminateProcess = false)
    {
        CRASH_REPORT_INT(CT2A(ex.ErrorMessage()).m_psz, (int)ex.Error(), &rc, terminateProcess ? 1 : 0);
    }
#endif

    inline void OnException(int ec = errno, RUN_CONTEXT const& rc = ::capture_context(), bool terminateProcess = false)
    {
        CRASH_REPORT_INT("catch (...)", ec, &rc, terminateProcess ? 1 : 0);
    }

    inline void GenericExceptionHandler(int ec = errno, RUN_CONTEXT const& rc = ::capture_context(), bool terminateProcess = false)
    {
        try
        {
            throw ;
        }
#ifdef _WIN32
        catch (_com_error const& ex)
        {
            OnException(ex, rc, terminateProcess);
        }
#endif
        catch (Exception const& ex)
        {
            OnException(ex, terminateProcess);
        }
        catch (std::exception const& ex)
        {
            OnException(ex, ec, rc, terminateProcess);
        }
        catch (boost::exception const& ex)
        {
            OnException(ex, ec, rc, terminateProcess);
        }
        catch (...)
        {
            OnException(ec, rc, terminateProcess);
        }
    }
}

#endif /* __cplusplus */

#endif /* _crash_report_hxx__ */
