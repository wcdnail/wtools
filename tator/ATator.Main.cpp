#include "stdafx.h"
#include "ATator.MemDlg.h"
#include "windows.gui.leaks.h"
#include "string.utils.format.h"
#include "string.utils.error.code.h"
#include "dh.tracing.h"

CAppModule _Module;

static int Run(LPTSTR /*lpstrCmdLine*/, int nCmdShow)
{
    int nRet = 0;
    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    CTatorMainDlg dlg;
    const HWND hWndDlg = dlg.Create(GetActiveWindow());         // если диалог содержит каcтомный контрол,
    if (!hWndDlg) {                                             // класс которого НЕ зарегистрирован, ф-ция вернет NULL
        auto code = static_cast<HRESULT>(GetLastError());       // а GetLastError вернет 0...
        if (ERROR_SUCCESS == code) {
            code = static_cast<HRESULT>(WSAGetLastError());
            if (ERROR_SUCCESS == code) {
                DWORD dwCode = 0;                               // GetExitCodeThread хотя бы вернет значение, отличное от 0
                BOOL  thrRes = GetExitCodeThread(GetCurrentThread(), &dwCode);
                if (!thrRes) {
                    code = static_cast<HRESULT>(GetLastError());
                }
                else {
                    code = static_cast<HRESULT>(dwCode);
                }
            }
        }
        ATL::CString codeMessage = Str::ErrorCode<TCHAR>::SystemMessage(code);
        ATL::CString  strMessage;
        strMessage.Format(_T("Не могу создать диалоговое окно.\r\n[%s]"), codeMessage.GetString());
        MessageBox(GetActiveWindow(), strMessage.GetString(), _T("FATAL"), MB_ICONSTOP);
        return static_cast<int>(code);
    }
    theLoop.AddMessageFilter(&dlg);
    dlg.ShowWindow(SW_SHOW);

    nRet = theLoop.Run();
    theLoop.RemoveMessageFilter(&dlg);
    _Module.RemoveMessageLoop();
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
