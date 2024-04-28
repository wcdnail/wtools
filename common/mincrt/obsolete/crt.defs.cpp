#include "stdafx.h"
#include "crt.defs.h"
#include "crt.report.h"

namespace MinCrt
{
#ifdef _WIN32
    static const DWORD DbgHelperFlags = 0
                                      | SYMOPT_ALLOW_ABSOLUTE_SYMBOLS
                                      | SYMOPT_ALLOW_ZERO_ADDRESS
                                      | SYMOPT_LOAD_ANYTHING
                                      | SYMOPT_LOAD_LINES
                                      | SYMOPT_UNDNAME
                                      ;
#endif

    CrtDef& Crt()
    {
        static CrtDef instance;
        return instance;
    }

    CrtDef::CrtDef()
        : DbgHelper(::GetCurrentProcessId(), DbgHelperFlags)
        , Dtors()
        , Heap()
        , ReturnValue(0)
    {}

    CrtDef::~CrtDef()
    {
        Heap.ReportLeaks();
    }

    int CrtDef::AddGlobal(Dtor const& dtor)
    {
        int retv = -1;

        {
            // TODO: check multi-threading
            Dtors.push_front(dtor);
            retv = (int)Dtors.size();
        }

        return retv;
    }

    void CrtDef::DestructGlobals()
    {
        DtorsStorage dtors;
        {
            // TODO: check multi-threading
            Dtors.swap(dtors);
        }

        if (!dtors.empty())
        {
            for (DtorsStorage::const_iterator it = dtors.cbegin(); it != dtors.cend(); ++it)
                (*it)();
        }
    }
}
