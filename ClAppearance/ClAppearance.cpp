#include "pch.h"
#include "ClAppearance.h"
#include "CRectPutInto.h"

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

    CClassicAppearance m_App;

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

CClassicAppearance _AtlModule;

extern "C"
int WINAPI _tWinMain(HINSTANCE instHnd, HINSTANCE, LPTSTR, int showCmd)
{
    HRESULT hr = _AtlModule.Run(instHnd, showCmd);
    return static_cast<int>(hr);
}

namespace
{
    struct OleInit
    {
        HRESULT result;
        OleInit()
            : result(::OleInitialize(nullptr))
        {
        }
        ~OleInit()
        {
            if (SUCCEEDED(result)) {
                ::OleUninitialize();
            }
        }
    };

    struct CCtrlInit
    {
        HRESULT result;
        CCtrlInit()
            : result(S_OK)
        {
            ::DefWindowProc(nullptr, 0, 0, 0L);
            if (!WTL::AtlInitCommonControls(ICC_BAR_CLASSES)) {
                result = ::GetLastError();
                if (SUCCEEDED(result)) {
                    result = DISP_E_UNKNOWNINTERFACE;
                }
            }
        }
        ~CCtrlInit()
        {
            /* ##TODO: Deinitialize CommonControls */
        }
    };
}

CClassicAppearance::~CClassicAppearance()
{
}

CClassicAppearance::CClassicAppearance()
    : Super()
    , m_MainFrame(*this)
{
}

HRESULT CClassicAppearance::Run(HINSTANCE instHnd, int showCmd)
{
    static constexpr int MF_Initial_CX = 1000;
    static constexpr int MF_Initial_CY = 600;

    HRESULT          hr;
    HWND           hwnd;
    CMessageLoop   loop;
    OleInit     oleInit;
    CCtrlInit cctrlInit;

    if (FAILED(oleInit.result)) {
        return oleInit.result;
    }
    if (FAILED(cctrlInit.result)) {
        return cctrlInit.result;
    }
    hr = Init(nullptr, instHnd);
    if (FAILED(hr)) {
        return hr;
    }
    if (!AddMessageLoop(&loop)) {
        hr = ::GetLastError();
        return hr;
    }
    CRect rc(0, 0, MF_Initial_CX, MF_Initial_CY);
    Rc::PutInto(Rc::Screen, rc, Rc::Center);
    hwnd = m_MainFrame.Create(nullptr, rc, nullptr);
    if (!hwnd) {
        hr = ::GetLastError();
        return hr;
    }
    m_MainFrame.ShowWindow(showCmd);
    hr = loop.Run();
    RemoveMessageLoop();
    return hr;
}
