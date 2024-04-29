#include "stdafx.h"
#include "twins.h"
#include <dh.tracing.h>
#include <windows.gui.leaks.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "atls.lib")

int APIENTRY _tWinMain(HINSTANCE baseAddress, HINSTANCE, LPTSTR, int showCmd)
{
    SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    DefWindowProc(NULL, 0, 0, 0L);

    DH::PrintLogHeader();

    HRESULT rv = Twins::App().Run(baseAddress, showCmd);
    return static_cast<int>(rv);
}
