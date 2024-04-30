#include "pch.hxx"
#include "windows.gui.leaks.h"
#include "err.printer.h"
#include "dh.tracing.h"
#include <gtest/gtest.h>
#include <windows.uses.commoncontrols.h>
#include <windows.uses.richedit.h>
#include <exception>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "atls.lib")
#pragma comment(lib, "windowsapp.lib")

#if 0
static int _ExceptionFilter(PEXCEPTION_POINTERS ep)
{
   DebugThreadPrintf(_T( "SEH [0x%08x]\n" ), ep->ExceptionRecord->ExceptionCode);
   return EXCEPTION_EXECUTE_HANDLER;
}

static void winrtInit()
{
    __try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    }
    __except (_ExceptionFilter(GetExceptionInformation())) {
        HRESULT hr = GetExceptionCode();
        fwprintf_s(stderr, L"WINRT ERROR! %p\n", (void*)hr);
    }
}
#endif

#if !defined(_CONSOLE)
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, PTSTR lpCmdLine, int nShowCmd)
{
#else
int _tmain(int argc, TCHAR* argv[])
{
    HINSTANCE hInstance = GetModuleHandleW(nullptr);
#endif
    try {
        //winrt::init_apartment(winrt::apartment_type::multi_threaded);

        Initialize::CommonControls ccontrols(ICC_COOL_CLASSES | ICC_BAR_CLASSES);
        Initialize::RichEdit richedit;

        CAppModule AtlModule;
        HRESULT rv = AtlModule.Init(nullptr, hInstance);
        if (FAILED(rv)) {
            throw std::runtime_error("CAppModule::Init failed!");
        }

        DH::InitDebugHelpers(DH::LOG_MUTEX_GUARD
                           | DH::DEBUG_EXTRA_INFO
                           | DH::DEBUG_WIN32_OUT
                           | DH::LOG_ENABLED
                           | DH::LOG_TO_STDIO
                           );

        DH::ScopedThreadLog liveTimer(L"UnitTests");
        CF::GUILeaks          uiLeaks;

        SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
        DefWindowProc(NULL, 0, 0, 0L);

        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
    catch (winrt::hresult_error const& ex) {
        PrintException("WINRT ERROR", ex);
    }
    catch (std::exception const& ex) {
        PrintException("ERROR", ex);
    }
    return 2;
}
