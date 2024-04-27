#include "pch.hxx"

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, PTSTR lpCmdLine, int nShowCmd)
{
    SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    DefWindowProc(NULL, 0, 0, 0L);

    return 0;
}
