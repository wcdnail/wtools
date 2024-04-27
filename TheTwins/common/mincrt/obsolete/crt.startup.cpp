#include "stdafx.h"
#include "crt.defs.h"
#include "excpt.rubbish.heap.h"

namespace App 
{
    int EntryPoint(); 
}

namespace MinCrt
{
    static int EntryPoint()
    {
        RawHeap::_Reset();

        try
        {
            Crt(); // Create MinCrt instance.

            // Run application.
            Crt().SetReturnValue(App::EntryPoint());
        }
        catch(...)
        {
            ::OutputDebugStringA("catch(...)...\n");
        }

        int retv = Crt().GetReturnValue();
        Crt().DestructGlobals();

        RawHeap::_ReportLeaks();
        return retv;
    }

    static int Filter(PEXCEPTION_POINTERS ep)
    {
        return EXCEPTION_EXECUTE_HANDLER;
    }

    static int Win32EntryPoint()
    {
        int result = 72;
        char const* ___t = "Testing string...";

        __try
        {
            result = EntryPoint();
        }
        __except(Filter(GetExceptionInformation()))
        {
            ::OutputDebugStringA("__except\n");
            result = GetExceptionCode();
        }

        return result;
    }
}

extern "C" int WinMainCRTStartup() 
{
    SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    return MinCrt::Win32EntryPoint(); 
}
