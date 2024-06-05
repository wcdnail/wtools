#include "stdafx.h"
#include "wcd.color.picker.dlg.h"
#include <windows.gui.leaks.h>
#include <string.utils.format.h>
#include <string.utils.error.code.h>
#include <dh.tracing.h>
#include <color.stuff.h>
#include <WTL/CAppModuleRef.h>
#include <clipp.h>
#include <iostream>

WTL::CAppModule _Module{};

static int Run(HINSTANCE hInstance)
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
    if (IDOK == nRet) {
        int const nAlpha{dlg.m_ColorPicker.GetAlpha()};
        std::wcout << L"0x" << std::hex << dlg.m_ColorPicker.GetColorRef() << std::endl;
        if (nAlpha < RGB_MAX_INT) {
            std::wcout << L"A: 0x" << std::hex << nAlpha << std::endl;
        }
        std::wcout << std::flush;
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

static bool ParseCmdLine(LPTSTR lpstrCmdLine)
{
    #define _TSTRINGIZE(V)  #V
    #define TSTRINGIZE(V)   _T(_TSTRINGIZE(V))
    #define MIN_SPEC_CX     12
    #define MAX_SPEC_CX     256

    using namespace clipp;
    using CmdLinePtr = std::shared_ptr<void>;

    bool bShowUsage{false};
    tgroup<TCHAR> const cli{
        toption<TCHAR>(_T("-h"), _T("--help")).set(bShowUsage).doc(_T("show this info")),
       (toption<TCHAR>(_T("-u"), _T("--spectrum-size")) & tvalue<TCHAR>(_T("width"), CColorPicker::RasterCX()))
            .doc(_T("set spectrum bitmap's size (min: ") TSTRINGIZE(MIN_SPEC_CX) _T(", max: ") TSTRINGIZE(MAX_SPEC_CX) _T(")")),
    };
    int              argc{0};
    CmdLinePtr const argv{CommandLineToArgvW(lpstrCmdLine, &argc), LocalFree};
    auto const        res{parse<TCHAR>(argc, static_cast<PWSTR*>(argv.get()), cli, 0)};
    if (bShowUsage) {
        auto const    man{clipp::tmake_man_page<TCHAR>(cli).get_string()};
        ATL::CString sMsg{};
        sMsg.Format(_T("[WCD] Color Picker\r\n\r\n%s"), man.c_str());
        MessageBox(GetActiveWindow(), sMsg.GetString(), _T("INFO"), MB_ICONINFORMATION);
        return false;
    }
    if (CColorPicker::RasterCX() < MIN_SPEC_CX) {
        CColorPicker::RasterCX() = MIN_SPEC_CX;
    }
    else if (CColorPicker::RasterCX() > MAX_SPEC_CX) {
        CColorPicker::RasterCX() = MAX_SPEC_CX;
    }
    return true;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpstrCmdLine, int)
{
    CScopedAppRegistrator sAppReg{_Module};
    int                      nRes{0};
    if (!ParseCmdLine(lpstrCmdLine)) {
        return nRes;
    }
    DH::InitDebugHelpers(DH::DEBUG_WIN32_OUT);
    SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    nRes = Run(hInstance);
    _Module.Term();
    ::CoUninitialize();
    return nRes;
}
