#include "stdafx.h"
#include "dh.tracing.h"
#include "windows.gui.leaks.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "atls.lib")

int APIENTRY _tWinMain(HINSTANCE hinstance, HINSTANCE, LPTSTR, int showCmd)
{
    HRESULT code = S_OK;
    SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    DefWindowProc(nullptr, 0, 0, 0L);

    DH::PrintLogHeader();
    return static_cast<int>(code);
}
