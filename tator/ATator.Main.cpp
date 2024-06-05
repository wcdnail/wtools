#include "stdafx.h"
#include "ATator.MemDlg.h"
#include "CDefaultWin32Dlg.h"
#include "windows.gui.leaks.h"
#include "string.utils.format.h"
#include "string.utils.error.code.h"
#include "dh.tracing.h"
#include <WTL/CAppModuleRef.h>
#include <fstream>

/**
 * https://learn.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features?view=msvc-170
 *
 * libucrt.lib          None                    Statically links the UCRT into your code.               /MT     _MT
 * libucrtd.lib         None                    Debug version of the UCRT for static linking.           /MTd    _DEBUG, _MT
 *                                              Not redistributable.
 * ucrt.lib             ucrtbase.dll            DLL import library for the UCRT.                        /MD     _MT, _DLL
 * ucrtd.lib            ucrtbased.dll           DLL import library for the Debug version of the UCRT.   /MDd    _DEBUG, _MT, _DLL
 *                                              Not redistributable.
 * libvcruntime.lib     None                    Statically linked into your code.                       /MT     _MT
 * libvcruntimed.lib    None                    Debug version for static linking. Not redistributable.  /MTd    _MT, _DEBUG
 * vcruntime.lib        vcruntime<version>.dll  DLL import library for the vcruntime.                   /MD     _MT, _DLL
 * vcruntimed.lib       vcruntime<version>d.dll DLL import library for the Debug vcruntime.             /MDd    _DEBUG, _MT, _DLL
 *                                              Not redistributable.
 * libcmt.lib                                   Statically links the native CRT startup into your code. /MT     _MT
 * libcmtd.lib                                  Statically links the Debug version of the native CRT    /MTd    _DEBUG, _MT
 *                                              startup. Not redistributable.
 * msvcrt.lib                                   Static library for the native CRT startup for use with  /MD     _MT, _DLL
 *                                              DLL UCRT and vcruntime.  
 * msvcrtd.lib                                  Static library for the Debug version of the native CRT  /MDd    _DEBUG, _MT, _DLL
 *                                              startup for use with DLL UCRT and vcruntime.
 *                                              Not redistributable.
 * msvcmrt.lib                                  Static library for the mixed native and managed CRT     /clr
 *                                              startup for use with DLL UCRT and vcruntime.
 * msvcmrtd.lib                                 Static library for the Debug version of the mixed       /clr
 *                                              native and managed CRT startup for use with DLL UCRT 
 *                                              and vcruntime. Not redistributable.
 *
 * msvcurt.lib                                  Deprecated Static library for the pure managed CRT.     /clr:pure
 * msvcurtd.lib                                 Deprecated Static library for the Debug version of      /clr:pure
 *                                              the pure managed CRT. Not redistributable.
 * 
 * C++ standard library (STL) .lib files
 * 
 * libcpmt.lib                                  Multithreaded, static link                              /MT     _MT
 * msvcprt.lib                                  Multithreaded, dynamic link                             /MD     _MT, _DLL
 *                                              (import library for msvcp<version>.dll)
 * libcpmtd.lib                                 Multithreaded, static link                              /MTd    _DEBUG, _MT
 * msvcprtd.lib                                 Multithreaded, dynamic link                             /MDd    _DEBUG, _MT, _DLL
 *                                              (import library for msvcp<version>d.dll)
 *
 */

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

static int Run(LPTSTR /*lpstrCmdLine*/, int nCmdShow)
{
    HRESULT  code = ERROR_SUCCESS;
    int      nRet = 0;
    bool   bError = false;

    WTL::CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    if constexpr (true) {
        CDefaultWin32Dlg dlg;
      //CTatorMainDlg dlg;
        if (ERROR_SUCCESS != (code = dlg.Initialize())) {
            bError = true;
        }
        else {
            nRet = static_cast<int>(dlg.DoModal());
            if (-1 == nRet) {
                code = static_cast<HRESULT>(GetLastError());
                bError = true;
            }
        }
    }
    else {
      //CTatorMainDlg dlg;
        CDefaultWin32Dlg dlg;
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
    CScopedAppRegistrator sAppReg{_Module};

    HRESULT hCode = S_OK;
    SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    DefWindowProc(nullptr, 0, 0, 0L);

    hCode = OleInitialize(nullptr);
    // If you are running on NT 4.0 or higher you can use the following call instead to 
    // make the EXE free threaded. This means that calls come in on a random RPC thread.
    //     HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT(SUCCEEDED(hCode));
    WTL::AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);

    hCode = _Module.Init(nullptr, hInstance);
    ATLASSERT(SUCCEEDED(hCode));

    if (!DH::DebugConsole::Instance().AdjustPrivileges()) {
        ATL::CString strMessage{};
        strMessage.Format(_T("Ошибка утсановки отладочных привилегий DebugConsole:\r\n\r\n%s"),
            DH::DebugConsole::Instance().GetStrings({}).c_str());
        MessageBox(GetActiveWindow(), strMessage.GetString(), _T("WARNING"), MB_ICONWARNING);
    }

    if constexpr (false) {
        if (!DH::DebugConsole::Instance().ReceiveDebugOutput(L"", false)) {
            ATL::CString strMessage{};
            strMessage.Format(_T("Ошибка запуска загрузчика отладочных сообщений DebugConsole:\r\n\r\n%s"),
                DH::DebugConsole::Instance().GetStrings({}).c_str());
            MessageBox(GetActiveWindow(), strMessage.GetString(), _T("WARNING"), MB_ICONWARNING);
        }
    }

    DH::InitDebugHelpers(DH::DEBUG_WIN32_OUT | DH::DEBUG_DEVCON_OUT);
    hCode = Run(lpstrCmdLine, nCmdShow);
    _Module.Term();
    OleUninitialize();
    return static_cast<int>(hCode);
}
