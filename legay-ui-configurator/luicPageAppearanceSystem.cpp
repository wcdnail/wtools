#include "stdafx.h"
#include "luicPageAppearanceSystem.h"
#include "luicMain.h"
#include "dh.tracing.h"
#include "dh.tracing.defs.h"
#include "dev.assistance/dev.assist.h"
#include "resz/resource.h"
#include <prsht.h>

CSysPageAppearance::~CSysPageAppearance()
{
    if (m_hPropSheet) {
        DestroyPropertySheetPage(m_hPropSheet);
    }
}

CSysPageAppearance::CSysPageAppearance()
    : CPageImpl(IDD_CLASSIC)
    , m_hPropSheet(nullptr)
{
}

UINT CALLBACK CSysPageAppearance::PropPageCallback2(HWND hWnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
{
    UNREFERENCED_PARAMETER(hWnd);
    switch (uMsg) {
    case PSPCB_ADDREF:
    case PSPCB_CREATE: {
        ATL::CDialogImplBase* pPage = reinterpret_cast<ATL::CDialogImplBase*>(ppsp->lParam);
        ATL::_AtlWinModule.AddCreateWndData(&pPage->m_thunk.cd, pPage);
        break;
    }
    }
    DebugThreadPrintf(LTH_CONTROL L"PPC: w:%08x m:%d\n", hWnd, uMsg);
    return 0;
}

int CALLBACK CSysPageAppearance::PropSheetProc(HWND hWnd, UINT uMsg, LPARAM lParam)
{
#ifdef _DEBUG
    MSG msg;
    msg.hwnd = hWnd;
    msg.message = uMsg;
    msg.wParam = 0;
    msg.lParam = lParam;
    auto msgStr = DH::MessageToStrignW(&msg);
    DebugThreadPrintf(LTH_CONTROL L" -PPC- [[ %s ]]\n", msgStr.c_str());
#endif

    return static_cast<int>(Super::StartDialogProc(hWnd, uMsg, 0, lParam));
    //switch (uMsg) {
    //case PSCB_INITIALIZED:
    //    g_propSheet.hWnd = hWnd;
    //    SetLargeIcon();
    //}
    return 0;
}

HWND CSysPageAppearance::CreateDlg(HWND hWndParent, LPARAM dwInitParam)
{
    ATLASSUME(m_hPropSheet == nullptr);
    Super::m_psp.dwFlags |= PSP_DEFAULT;
    Super::m_psp.pfnCallback = PropPageCallback2;
    m_hPropSheet = Super::Create();

    PROPSHEETHEADERW psh;
    ZeroMemory(&psh, sizeof(psh));
    psh.dwSize = sizeof(PROPSHEETHEADERW);
    psh.dwFlags = PSH_USECALLBACK | PSH_NOCONTEXTHELP;
    psh.hInstance = ATL::_AtlBaseModule.GetResourceInstance();
    psh.nPages = 0;
    psh.nStartPage = 1;
    psh.phpage = &m_hPropSheet;
    psh.pfnCallback = PropSheetProc;
    PropertySheetW(&psh);

    return Super::m_hWnd;
}

BOOL CSysPageAppearance::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
