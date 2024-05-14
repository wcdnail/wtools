#include "stdafx.h"
#include "ATator.MemDlg.h"
#include "CDefaultWin32Dlg.h"
#include "windows.gui.leaks.h"
#include "string.utils.format.h"
#include "string.utils.error.code.h"
#include "dh.tracing.h"
#include "3rd-party/inipp.h"
#include <fstream>
#include <iostream>

WTL::CAppModule _Module{};

static HRESULT GetErrorCode(HRESULT code)                   // если диалог содержит каcтомный контрол,
{                                                           // класс которого НЕ зарегистрирован, ф-ция
    if (ERROR_SUCCESS != code) {                            // CDialogImpl::Create вернет NULL
        return code;                                        // а GetLastError вернет 0...
    }                                                       // И как, спрашивается, узнать код ошибки?
    code = static_cast<HRESULT>(CommDlgExtendedError());    //
    if (ERROR_SUCCESS != code) {                            //
        return code;
    }
    code = static_cast<HRESULT>(WSAGetLastError());         //
    if (ERROR_SUCCESS != code) {                            //
        return code;
    }
    DWORD      dwCode{ERROR_SUCCESS};                       // GetExitCodeThread хотя бы вернет значение, отличное от 0
    const BOOL thrRes{GetExitCodeThread(GetCurrentThread(), &dwCode)};
    if (!thrRes) {                                          // но это значение не будет актуальным кодом ошибки...
        return static_cast<HRESULT>(GetLastError());
    }
    return static_cast<HRESULT>(dwCode);
}

static int Run(LPTSTR lpstrCmdLine, int nCmdShow)
{
    HRESULT  code = ERROR_SUCCESS;
    int      nRet = 0;
    bool   bError = false;

    WTL::CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    if constexpr (false) {
        CDefaultWin32Dlg dlg;
        nRet = static_cast<int>(dlg.DoModal());
        if (-1 == nRet) {
            code = static_cast<HRESULT>(GetLastError());
            bError = true;
        }
    }
    else {
        CTatorMainDlg dlg;
        if (ERROR_SUCCESS != (code = dlg.Initialize())) {
            bError = true;
        }
        else {
            if (dlg.Create(GetActiveWindow())) {
                theLoop.AddMessageFilter(&dlg);
                dlg.ShowWindow(nCmdShow);

                nRet = theLoop.Run();
                theLoop.RemoveMessageFilter(&dlg);
            }
            else {
                code = static_cast<HRESULT>(GetLastError());
                bError = true;
            }
        }
    }
    if (bError) {
        code = GetErrorCode(code);
        ATL::CString codeMessage = Str::ErrorCode<TCHAR>::SystemMessage(code);
        ATL::CString  strMessage;
        strMessage.Format(_T("Не могу создать диалоговое окно.\r\n[%s]"), codeMessage.GetString());
        MessageBox(GetActiveWindow(), strMessage.GetString(), _T("FATAL"), MB_ICONSTOP);
        return static_cast<int>(code);
    }
    _Module.RemoveMessageLoop();
    return nRet;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpstrCmdLine, int nCmdShow)
{
    HRESULT code = S_OK;
    SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

    code = ::CoInitialize(nullptr);
    // If you are running on NT 4.0 or higher you can use the following call instead to 
    // make the EXE free threaded. This means that calls come in on a random RPC thread.
    //     HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT(SUCCEEDED(code));
    
    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    DefWindowProc(nullptr, 0, 0, 0L);

    DH::InitDebugHelpers(DH::DEBUG_WIN32_OUT);

    WTL::AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);

    code = _Module.Init(nullptr, hInstance);
    ATLASSERT(SUCCEEDED(code));

    code = Run(lpstrCmdLine, nCmdShow);

    _Module.Term();
    ::CoUninitialize();

    return static_cast<int>(code);
}
