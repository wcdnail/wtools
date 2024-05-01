#include "stdafx.h"
#include "luicMainFrame.h"
#include "luicMain.h"
#include "luicBackground.h"
#include "luicScreenSaver.h"
#include "luicAppearance.h"
#include "luicEffects.h"
#include "luicWeb.h"
#include "luicSettings.h"
#include "string.utils.format.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

enum IconIndex: int
{
    IconMain = 0,
};

enum PageIndex: int
{
    PageBegin = -1,
    PageBackground,
    PageScreenSaver,
    PageAppearance,
    PageEffects,
    PageWeb,
    PageSettings,
    PageEnd
};

CMainFrame::~CMainFrame()
{
}

CMainFrame::CMainFrame(CLegacyUIConfigurator& app)
    : m_App(app)
{
}

static void ReportError(ATL::CStringW&& caption, HRESULT code, bool showMessageBox = false, UINT mbType = MB_ICONERROR)
{
    ATL::CStringW msg = Str::ErrorCode<wchar_t>::SystemMessage(code);
    DH::ThreadPrintfc(DH::Category::Module(), L"%s %s\n", caption.GetString(), msg.GetString());

    if (showMessageBox) {
        ATL::CStringW userMsg;
        userMsg.Format(L"%s\r\n%s\r\n", caption.GetString(), msg.GetString());
        MessageBoxW(nullptr, userMsg.GetString(), L"ERROR", mbType);
    }
}

void CMainFrame::ImListCreate()
{
    enum : int
    {
        MaxIconWidth  = 32,
        MaxIconHeight = 32,
    };
    static const int iconsIDs[] = {
        IDI_COMP,
    };
    HRESULT code = S_FALSE;

    m_ImList.Create(MaxIconWidth, MaxIconHeight, ILC_MASK, _countof(iconsIDs), 0);
    if (!m_ImList.m_hImageList) {
        code = static_cast<HRESULT>(GetLastError());
        ReportError(L"Creation of ImageList failed!", code, true, MB_ICONWARNING);
        return ;
    }

    CIconHandle tempIco;
    for (const auto it: iconsIDs) {
        tempIco.LoadIconW(it, MaxIconWidth, MaxIconHeight);
        if (!tempIco.m_hIcon) {
            code = static_cast<HRESULT>(GetLastError());
            ReportError(Str::ElipsisW::Format(L"Load icon (%d) failed!", it), code);
            continue;
        }
        m_ImList.AddIcon(tempIco.Detach());
    }
}

void CMainFrame::PagesGetRect(int tabNum, CRect& rcTab)
{
    CRect rcHead;
    m_Tab.GetItemRect(tabNum, rcHead);

    m_Tab.GetClientRect(rcTab);
    TabCtrl_AdjustRect(m_Tab.m_hWnd, FALSE, rcTab);

    {
        CWindowDC dc(m_hWnd);
        dc.FillSolidRect(rcHead, 0x00ffff00);
        dc.FillSolidRect(rcTab, 0x00ff00ff);
    }
}

void CMainFrame::PagesCreate()
{
    auto   pBackground = std::make_unique<CPageBackground>();
    auto  pScreenSaver = std::make_unique<CPageScreenSaver>();
    auto   pAppearance = std::make_unique<CPageAppearance>();
    auto  pPageEffects = std::make_unique<CPageEffects>();
    auto      pPageWeb = std::make_unique<CPageWeb>();
    auto pPageSettings = std::make_unique<CPageSettings>();

    m_Tab.Attach(GetDlgItem(IDC_TAB1));
    m_Tab.ModifyStyle(0, WS_TABSTOP);
    m_Tab.SetImageList(m_ImList);

    PagesAppend(PageBackground,  L"Background",  std::move(pBackground));
    PagesAppend(PageScreenSaver, L"ScreenSaver", std::move(pScreenSaver));
    PagesAppend(PageAppearance,  L"Appearance",  std::move(pAppearance));
    PagesAppend(PageEffects,     L"Effects",     std::move(pPageEffects));
    PagesAppend(PageWeb,         L"Web",         std::move(pPageWeb));
    PagesAppend(PageSettings,    L"Settings",    std::move(pPageSettings));
}

void CMainFrame::PagesAppend(int desiredIndex, ATL::CStringW&& str, CPageImplPtr&& pagePtr)
{
    HRESULT code = S_FALSE;
    int number = m_Tab.InsertItem(m_Tab.GetItemCount(), TCIF_TEXT /*| TCIF_IMAGE*/, str.GetString(), 0, 0l);
    if (number < 0) {
        code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Append dialog page '%s' failed!", str.GetString()), code, true, MB_ICONERROR);
        return ;
    }
    if (desiredIndex != number) {
        code = ERROR_REVISION_MISMATCH;
        ReportError(Str::ElipsisW::Format(
                        L"Append dialog page '%s' failed!\r\n"
                        L"Desired index %d != %d actual index\r\n",
                        str.GetString(), desiredIndex, number),
                    code, true, MB_ICONSTOP);
        return ;
    }
    if (!pagePtr->Create(m_hWnd)) {
        code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Append dialog page '%s' failed!", str.GetString()), code, true, MB_ICONERROR);
        return ;
    }
    CRect rcTab;
    PagesGetRect(number, rcTab);
    pagePtr->MoveWindow(rcTab, FALSE);
    m_PagesMap[number] = std::move(pagePtr);
}

BOOL CMainFrame::OnInitDlg(HWND, LPARAM)
{
    ModifyStyle(0, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX, SWP_FRAMECHANGED);
    MoveToMonitor{}.Move(m_hWnd, 3);
    ShowWindow(SW_SHOW);

    ImListCreate();
    PagesCreate();

    HICON tempIco = m_ImList.GetIcon(IconMain);
    SetIcon(tempIco, FALSE);
    SetIcon(tempIco, TRUE);

    DlgResize_Init(true, true);
    return TRUE;
}

void CMainFrame::OnDestroy()
{
    PostQuitMessage(0);
}

LRESULT CMainFrame::OnNotify(int idCtrl, LPNMHDR pnmh)
{
    switch (pnmh->code) {
    case TCN_SELCHANGE: {
        int n = TabCtrl_GetCurSel(pnmh->hwndFrom);
        //if (m_PagesMap[n]) {
            //::ShowWindow(m_PagesMap[n], SW_SHOW);
        //}
        DebugThreadPrintf(LTH_WM_NOTIFY L"   TCN_SELCHANGE: c:%4d n:%d\n", idCtrl, n);
        break;
    }
    case TCN_SELCHANGING: {
        int n = TabCtrl_GetCurSel(pnmh->hwndFrom);
        //if (m_PagesMap[n]) {
            //::ShowWindow(m_PagesMap[n], SW_HIDE);
        //}
        DebugThreadPrintf(LTH_WM_NOTIFY L" TCN_SELCHANGING: c:%4d n:%d\n", idCtrl, n);
        break;
    }
    default:
        //DebugThreadPrintf(LTH_WM_NOTIFY L" Unknown: c:%4d n:%04x\n", idCtrl, pnmh->code);
        break;
    }
    return 0;
}

void CMainFrame::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
    switch(nID) {
    case IDCANCEL:
        DestroyWindow();
        break;
    default:
        DebugThreadPrintf(LTH_WM_COMMAND L" Unknown: n:%4d c:%4d w:%08x\n", uNotifyCode, nID, wndCtl);
    }
}
