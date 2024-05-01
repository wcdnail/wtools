#include "stdafx.h"
#include "luicMain.h"
#include "dh.tracing.h"
#include "string.utils.format.h"
#include "common/windows.uses.ole.h"
#include "common/windows.uses.commoncontrols.h"

void ReportError(ATL::CStringW&& caption, HRESULT code, bool showMessageBox/* = false*/, UINT mbType/* = MB_ICONERROR*/)
{
    ATL::CStringW msg = Str::ErrorCode<wchar_t>::SystemMessage(code);
    DH::ThreadPrintfc(DH::Category::Module(), L"%s %s\n", caption.GetString(), msg.GetString());

    if (showMessageBox) {
        ATL::CStringW userMsg;
        userMsg.Format(L"%s\r\n%s\r\n", caption.GetString(), msg.GetString());
        MessageBoxW(nullptr, userMsg.GetString(), L"ERROR", mbType);
    }
}

#if 0
#include "ClAppearance_i.h"
#include <atlbase.h>

struct CAtlApp: ATL::CAtlExeModuleT<CAtlApp>
{
    using    Super = ATL::CAtlExeModuleT<CAtlApp>;
    using SuperMod = ATL::CAtlModuleT<CAtlApp>;

    ~CAtlApp() override;
    CAtlApp();

private:
    friend Super;
    friend SuperMod;

    CLegacyUIConfigurator m_App;

    DECLARE_LIBID(LIBID_ClAppearanceLib)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_CLAPPEARANCE, "{a189a989-5210-498e-9326-eadb11ad6d14}")

    HRESULT Run(int showCmd);
};

CAtlApp::~CAtlApp()
{
}

CAtlApp::CAtlApp()
    : Super()
    , m_App()
{
}

CAtlApp _AtlModule;
return _AtlModule.WinMain(showCmd);
#endif

extern "C"
int WINAPI _tWinMain(HINSTANCE instHnd, HINSTANCE, LPTSTR, int showCmd)
{
    DH::InitDebugHelpers(DH::DEBUG_WIN32_OUT);
    CLegacyUIConfigurator app;
    HRESULT hr = app.Run(instHnd, showCmd);
    return static_cast<int>(hr);
}

CLegacyUIConfigurator::~CLegacyUIConfigurator()
{
}

CLegacyUIConfigurator::CLegacyUIConfigurator()
    : Super()
    , m_MainFrame(*this)
{
}

HRESULT CLegacyUIConfigurator::Run(HINSTANCE instHnd, int showCmd)
{
    static constexpr int MF_Initial_CX = 1000;
    static constexpr int MF_Initial_CY = 600;

    HRESULT hr = S_FALSE;
    try {
        Initialize::OLE               ole;
        Initialize::CommonControls cctrls;
        HWND                         hwnd;
        CMessageLoop                 loop;

        hr = Init(nullptr, instHnd);
        if (FAILED(hr)) {
            ReportError(L"Initialization failure!", hr, true);
            return hr;
        }
        if (!AddMessageLoop(&loop)) {
            hr = GetLastError();
            ReportError(L"MessageLoop append failure!", hr, true);
            return hr;
        }
        hwnd = m_MainFrame.Create(::GetActiveWindow());
        if (!hwnd) {
            hr = ::GetLastError();
            ReportError(L"MainFrame creation failure!", hr, true);
            return hr;
        }
        ATLTRACE2(atlTraceUI, 0, _T("All OK, launch main window [%08x] <%s>\n"), hr, _T(__FUNCDNAME__));
        m_MainFrame.ShowWindow(showCmd);
        hr = loop.Run();
        RemoveMessageLoop();
    }
    catch(std::exception const& ex) {
        MessageBoxA(nullptr, ex.what(), "epic FAIL", MB_ICONERROR);
    }

    return hr;
}
