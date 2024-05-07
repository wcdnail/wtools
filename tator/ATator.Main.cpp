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

WTL::CAppModule _Module;

static int Run(LPTSTR /*lpstrCmdLine*/, int nCmdShow)
{
    HRESULT  code = ERROR_SUCCESS;
    int      nRet = 0;
    bool bRunLoop = true;
    bool   bError = false;

    WTL::CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    //CTatorMainDlg dlg;
    //HRESULT code = dlg.Initialize();
    CDefaultWin32Dlg dlg;
    if (ERROR_SUCCESS == code) {
        if constexpr (true) {
            bRunLoop = false;
            nRet = static_cast<int>(dlg.DoModal());
            if (-1 == nRet) {
                code = static_cast<HRESULT>(GetLastError()); // ##TODO: Try CommDlgExtendedError 
                bError = true;
            }
        }
        else {
            const HWND hWndDlg = dlg.Create(GetActiveWindow());
            if (hWndDlg) {
                theLoop.AddMessageFilter(&dlg);
                dlg.ShowWindow(SW_SHOW);
            }
            else {
                code = static_cast<HRESULT>(GetLastError());
                bError = true;
            }
        }
        
        if (bError && (ERROR_SUCCESS == code)) {                // если диалог содержит каcтомный контрол,
            code = static_cast<HRESULT>(WSAGetLastError());     // класс которого НЕ зарегистрирован, ф-ция Create вернет NULL
            if (ERROR_SUCCESS == code) {                        // а GetLastError вернет 0...
                DWORD dwCode = ERROR_SUCCESS;                   // GetExitCodeThread хотя бы вернет значение, отличное от 0
                BOOL  thrRes = GetExitCodeThread(GetCurrentThread(), &dwCode);
                if (!thrRes) {
                    code = static_cast<HRESULT>(GetLastError());
                }
                else {
                    code = static_cast<HRESULT>(dwCode);
                }
            }
        }
    }
    if (ERROR_SUCCESS != code) {
        ATL::CString codeMessage = Str::ErrorCode<TCHAR>::SystemMessage(code);
        ATL::CString  strMessage;
        strMessage.Format(_T("Не могу создать диалоговое окно.\r\n[%s]"), codeMessage.GetString());
        MessageBox(GetActiveWindow(), strMessage.GetString(), _T("FATAL"), MB_ICONSTOP);
        return static_cast<int>(code);
    }
    if (bRunLoop) {
        nRet = theLoop.Run();
        theLoop.RemoveMessageFilter(&dlg);
    }
    _Module.RemoveMessageLoop();
    return nRet;
}

void tryIni()
{
    inipp::Ini<char> ini;
    std::ifstream is("C:/_/wtools/legacy-ui-configurator/temes/themes-win98-plus/Inside your Computer (high color).theme");
    ini.parse(is);

    std::ostringstream iniStm;
    ini.generate(iniStm);

    auto const* sectName = "DEFAULT";

    std::ostringstream sectStm;
    ini.default_section(ini.sections[sectName]);
    ini.interpolate();
    ini.generate(sectStm);

    DH::TPrintf("INI:\n", iniStm.str().c_str());
    DH::TPrintf("[%s]:\n", sectName, sectStm.str().c_str());
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

    tryIni();

    WTL::AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);

    code = _Module.Init(nullptr, hInstance);
    ATLASSERT(SUCCEEDED(code));

    code = Run(lpstrCmdLine, nCmdShow);

    _Module.Term();
    ::CoUninitialize();

    return static_cast<int>(code);
}
