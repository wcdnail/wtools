#include "pch.hxx"
#include "windows.gui.leaks.h"
#include "dh.tracing.h"
#include <gtest/gtest.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "atls.lib")

#if !defined(_CONSOLE)
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, PTSTR lpCmdLine, int nShowCmd)
#endif
int _tmain(int argc, TCHAR* argv[])
{
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
