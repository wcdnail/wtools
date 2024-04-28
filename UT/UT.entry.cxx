#include "pch.hxx"
#include <gtest/gtest.h>

#pragma comment(lib, "atls.lib")

#if !defined(_CONSOLE)
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, PTSTR lpCmdLine, int nShowCmd)
#endif
int _tmain(int argc, TCHAR* argv[])
{
    SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    DefWindowProc(NULL, 0, 0, 0L);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
