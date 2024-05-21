#include "stdafx.h"
#include "MainDlg.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = nullptr, int nCmdShow = SW_SHOWDEFAULT)
{
    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);
    CMainDlg dlgMain;
    if(!dlgMain.Create(nullptr)) {
        ATLTRACE(_T("Main dialog creation failed!\n"));
        return 0;
    }
    dlgMain.ShowWindow(nCmdShow);
    const int nRet = theLoop.Run();
    _Module.RemoveMessageLoop();
    return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
    HMODULE hInstRichEdit = nullptr; // ::LoadLibrary(_T("Msftedit.dll"));
    HMODULE hRichDll = LoadLibrary(_T("riched20.dll"));
    HRESULT hRes = ::CoInitialize(nullptr);
    ATLASSERT(SUCCEEDED(hRes));
    AtlInitCommonControls(ICC_BAR_CLASSES); // add flags to support other controls
    hRes = _Module.Init(nullptr, hInstance);
    ATLASSERT(SUCCEEDED(hRes));
    int nRet = Run(lpstrCmdLine, nCmdShow);

    _Module.Term();
    ::CoUninitialize();
    if (hInstRichEdit != nullptr) {
        FreeLibrary(hInstRichEdit);
        hInstRichEdit = nullptr;
    }
    if (hRichDll) {
        FreeLibrary(hRichDll);
        hRichDll = nullptr;
    }
    return nRet;
}
