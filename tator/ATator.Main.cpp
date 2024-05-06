#include "stdafx.h"
#include "ATator.MemDlg.h"
#include "windows.gui.leaks.h"
#include "dh.tracing.h"

CAppModule _Module;

static int Run(LPTSTR /*lpstrCmdLine*/, int nCmdShow)
{
    int nRet = 0;
    //CMessageLoop theLoop;
    //_Module.AddMessageLoop(&theLoop);

    CTatorMainDlg dlg;
    nRet = static_cast<int>(dlg.DoModal());

    //nRet = theLoop.Run();
    //_Module.RemoveMessageLoop();
    return nRet;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpstrCmdLine, int nCmdShow)
{
    HRESULT code = S_OK;
    SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    DefWindowProc(nullptr, 0, 0, 0L);

    code = ::CoInitialize(nullptr);
    ATLASSERT(SUCCEEDED(code));

    DH::InitDebugHelpers(DH::DEBUG_WIN32_OUT);

    AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);

    code = _Module.Init(nullptr, hInstance);
    ATLASSERT(SUCCEEDED(code));

    code = Run(lpstrCmdLine, nCmdShow);

    _Module.Term();
    ::CoUninitialize();

    return static_cast<int>(code);
}
