#include "pch.hxx"

#include <execution>

#include "windows.gui.leaks.h"
#include "dh.tracing.h"
#include <gtest/gtest.h>

#include "windows.uses.commoncontrols.h"
#include "windows.uses.ole.h"
#include "windows.uses.richedit.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "atls.lib")

#if !defined(_CONSOLE)
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, PTSTR lpCmdLine, int nShowCmd)
{
#else
int _tmain(int argc, TCHAR* argv[])
{
    HINSTANCE hInstance = GetModuleHandleW(nullptr);
#endif
    try {
        Initialize::OLE cole;
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
    catch (std::exception const& ex) {
        fprintf_s(stderr, "ERROR! %s\n", ex.what());
    }
    return 2;
}
