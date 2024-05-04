#include "stdafx.h"
#include "luicMain.h"
#include "icons.dll.h"
#include "dh.tracing.h"
#include "windows.uses.ole.h"
#include "windows.uses.commoncontrols.h"
#include "string.utils.format.h"
#include "windows.uses.gdi+.h"
#include "resz/resource.h"

#if 0
// TODO: GENERATED CODE, reserved for future use, DO NOT EARSE!!!!
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
CAtlApp _AtlModule;
return _AtlModule.WinMain(showCmd);
// TODO: GENERATED CODE, reserved for future use, DO NOT EARSE!!!!
#endif

CTheme CLegacyUIConfigurator::g_ThemeNative{ true };

CLegacyUIConfigurator* CLegacyUIConfigurator::g_pApp{ nullptr };
std::recursive_mutex CLegacyUIConfigurator::m_pAppMx{};

void SetMFStatus(int status, PCWSTR format, ...)
{
    va_list ap;
    va_start(ap, format);
    Str::ElipsisW::String message = Str::ElipsisW::FormatV(format, ap);
    va_end(ap);
    CLegacyUIConfigurator::App()->SetMainFrameStatus(status, std::move(message));
}

void ReportError(ATL::CStringA&& caption, HRESULT code, bool showMBox/* = false*/, UINT mbType/* = MB_ICONERROR*/)
{
    ATL::CStringA msg = Str::ErrorCode<char>::SystemMessage(code);
    DH::ThreadPrintfc(DH::Category::Module(), "%s %s\n", caption.GetString(), msg.GetString());

    if (showMBox) {
        ATL::CStringA userMsg;
        userMsg.Format("%s\r\n%s\r\n", caption.GetString(), msg.GetString());
        MessageBoxA(nullptr, userMsg.GetString(), "ERROR", mbType);
    }
}

void ReportError(ATL::CStringW&& caption, HRESULT code, bool showMBox/* = false*/, UINT mbType/* = MB_ICONERROR*/)
{
    ATL::CStringW msg = Str::ErrorCode<wchar_t>::SystemMessage(code);
    DH::ThreadPrintfc(DH::Category::Module(), L"%s %s\n", caption.GetString(), msg.GetString());

    if (showMBox) {
        ATL::CStringW userMsg;
        userMsg.Format(L"%s\r\n%s\r\n", caption.GetString(), msg.GetString());
        MessageBoxW(nullptr, userMsg.GetString(), L"ERROR", mbType);
    }
}

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
    {
        std::lock_guard<std::recursive_mutex> guard(m_pAppMx);
        g_pApp = nullptr;
    }
}

CLegacyUIConfigurator::CLegacyUIConfigurator()
    :       Super{}
    ,  m_Settings{ L"CLegacyUIConfigurator" }
    , m_MainFrame{ m_Settings }
    ,    m_ImList{}
{
    {
        std::lock_guard<std::recursive_mutex> guard(m_pAppMx);
        g_pApp = this;
    }
}

CTheme& CLegacyUIConfigurator::CurrentTheme() const
{
    return g_ThemeNative;
}

HICON CLegacyUIConfigurator::GetIcon(int icon) const
{
    if (icon < 0 || icon >= IconCount) {
        return nullptr;
    }
    return m_ImList[IL_Own].GetIcon(icon);
}

CMenu const& CLegacyUIConfigurator::GetTestMenu() const
{
    return m_TestMenu;
}

void CLegacyUIConfigurator::SetMainFrameStatus(int status, ATL::CStringW&& message)
{
    m_MainFrame.SetStatus(status, std::move(message));
}

HRESULT CLegacyUIConfigurator::Initialize(ATL::_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID)
{
    HRESULT code = CAppModule::Init(pObjMap, hInstance, pLibID);
    if (FAILED(code)) {
        return code;
    }
    static const ATL::CStringW shell32dll = SHELL32_PATHNAME;
    CIconCollectionFile shellIcons;
    if (!shellIcons.Load(shell32dll.GetString(), true)) {
        code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Load DLL '%s' icon collection failed!", shell32dll.GetString()), code, false, MB_ICONWARNING);
    }
    else {
        const auto shellIconCount = shellIcons.GetArray().size();
        m_ImList[IL_SHELL_32x32].Attach(shellIcons.MakeImageList(32, 32).Detach());
        m_ImList[IL_SHELL_16x16].Attach(shellIcons.MakeImageList(16, 16).Detach());
        ATLASSUME(m_ImList[IL_SHELL_32x32].GetImageCount() == shellIconCount);
    }
    code = ImListCreate();
    m_TestMenu.LoadMenuW(IDR_MENU1);
    return code;
}

CLegacyUIConfigurator* CLegacyUIConfigurator::App()
{
    std::lock_guard<std::recursive_mutex> guard(m_pAppMx);
    if (!g_pApp) {
        throw std::logic_error("CLegacyUIConfigurator::App is NULL");
    }
    return g_pApp;
}

WTL::CImageListManaged const& CLegacyUIConfigurator::GetImageList(int index) const
{
    if (index < 0 || index >= IL_Count) {
        static const WTL::CImageListManaged dmy;
        return dmy;
    }
    return m_ImList[index];
}

HRESULT CLegacyUIConfigurator::ImListCreate()
{
    enum : int
    {
        MaxIconWidth = 16,
        MaxIconHeight = 16,
    };
    static const int iconsIDs[] = {
        IDI_COMP,
        IDI_MON_BRUSH,
        IDI_START_MENU,
        IDI_SAVE,
        IDI_EDIT_NAME,
        IDI_HATCH_CROSS,
        IDI_FOLDER_OPEN,
    };
    HRESULT code = S_FALSE;
    m_ImList[IL_Own].Create(MaxIconWidth, MaxIconHeight, ILC_MASK | ILC_COLOR32, _countof(iconsIDs), 0);
    if (!m_ImList[IL_Own].m_hImageList) {
        code = static_cast<HRESULT>(GetLastError());
        ReportError(L"Creation of ImageList failed!", code, true, MB_ICONWARNING);
        return code;
    }
    CIconHandle tempIco;
    for (const auto it : iconsIDs) {
        tempIco.LoadIconW(it, MaxIconWidth, MaxIconHeight);
        if (!tempIco.m_hIcon) {
            code = static_cast<HRESULT>(GetLastError());
            ReportError(Str::ElipsisW::Format(L"Load icon (%d) failed!", it), code);
            continue;
        }
        m_ImList[IL_Own].AddIcon(tempIco.Detach());
    }
    ATLASSUME(IconCount == m_ImList[IL_Own].GetImageCount());
    return S_OK;
}

HRESULT CLegacyUIConfigurator::Run(HINSTANCE instHnd, int showCmd)
{
    static constexpr int MF_Initial_CX = 1000;
    static constexpr int MF_Initial_CY = 600;

    HRESULT hr = S_FALSE;
    try {
        Initialize::OLE               ole;
        Initialize::CommonControls cctrls;
        Initialize::GdiPlus       gdiPlus;
        CMessageLoop                 loop;

        hr = Initialize(nullptr, instHnd);
        if (FAILED(hr)) {
            ReportError(L"Initialization failure!", hr, true);
            return hr;
        }
        if (!AddMessageLoop(&loop)) {
            hr = static_cast<HRESULT>(GetLastError());
            ReportError(L"MessageLoop append failure!", hr, true);
            return hr;
        }
        if (!m_MainFrame.CreateEx(GetActiveWindow())) {
            hr = static_cast<HRESULT>(GetLastError());
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
