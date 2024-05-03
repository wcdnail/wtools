#ifndef aw_report_bugs_h__
#define aw_report_bugs_h__

#include <cstdio>
#include <boost/noncopyable.hpp>
#include "report.bugs.codes.h"
#include "error.policy.h"

class BugReportBase: boost::noncopyable 
{
public:
    virtual char const* GetMessage() const
    { return "There're no information about this error."; }
};

template <class ExceptionType>
class BugReport: public BugReportBase
{
public:
    BugReport(ExceptionType const& ex): info_(ex) {}
    ~BugReport() {}

    int Show() const
    {
        int rv = ReportDllNotFount;
        try
        {
            DynamicLibrary lib("report.bugs", AutoCompleteExtension);

            int (*DoBugReport)(BugReportBase const&);
            lib.GetFunction(DoBugReport, "DoBugReport");

            rv = DoBugReport(*this);
        }
        catch(...)
        {
            fputs(GetMessage(), stderr);
        }

        return rv;
    }

    char const* GetMessage() const;

private:
    ExceptionType const& info_;
};

template <>
inline char const* BugReport<Exception>::GetMessage() const 
{
    return info_.Dump(); 
}

template <>
inline char const* BugReport<std::exception>::GetMessage() const 
{
    return info_.what(); 
}

template <class ExceptionType>
inline int ShowBugReport(ExceptionType const& ex) 
{
    return BugReport<ExceptionType>(ex).Show();
}

#endif // aw_report_bugs_h__
