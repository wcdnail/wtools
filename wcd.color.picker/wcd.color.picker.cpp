#include "stdafx.h"
#include "wcd.color.picker.dlg.h"
#include <windows.gui.leaks.h>
#include <string.utils.format.h>
#include <string.utils.error.code.h>
#include <dh.tracing.h>
#include <WTL/CAppModuleRef.h>

WTL::CAppModule _Module{};

static int Run(HINSTANCE hInstance, LPTSTR lpstrCmdLine, int nCmdShow)
{
    HRESULT             hCode{ERROR_SUCCESS};
    int                  nRet{0};
    WTL::CMessageLoop theLoop{};
    CWCDColorPicker       dlg{};
    hCode = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hCode)) {
        goto reportError;
    }
    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    DefWindowProc(nullptr, 0, 0, 0L);
    if (!WTL::AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES)) {
        hCode = static_cast<HRESULT>(GetLastError());
        goto reportError;
    }
    hCode = _Module.Init(nullptr, hInstance);
    if (FAILED(hCode)) {
        goto reportError;
    }
    _Module.AddMessageLoop(&theLoop);
    if (ERROR_SUCCESS != (hCode = dlg.Initialize())) {
        goto reportError;
    }
    nRet = static_cast<int>(dlg.DoModal());
    if (-1 == nRet) {
        hCode = static_cast<HRESULT>(GetLastError());
        goto reportError;
    }
    _Module.RemoveMessageLoop();
    return nRet;
reportError:
    ATL::CString const codeMessage{Str::ErrorCode<TCHAR>::SystemMessage(hCode)};
    ATL::CString        strMessage{};
    strMessage.Format(_T("Main dialog creation failed!\r\n[%s]"), codeMessage.GetString());
    MessageBox(GetActiveWindow(), strMessage.GetString(), _T("FATAL"), MB_ICONSTOP);
    _Module.RemoveMessageLoop();
    return static_cast<int>(hCode);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpstrCmdLine, int nCmdShow)
{
    CScopedAppRegistrator sAppReg{_Module};
    int                      nRes{0};
    DH::InitDebugHelpers(DH::DEBUG_WIN32_OUT);
    SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    nRes = Run(hInstance, lpstrCmdLine, nCmdShow);
    _Module.Term();
    ::CoUninitialize();
    return nRes;
}
