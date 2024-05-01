#include "stdafx.h"
#include "luicMainFrame.h"
#include "luicMain.h"
#include "luicBackground.h"
#include "luicScreenSaver.h"
#include "luicAppearance.h"
#include "luicEffects.h"
#include "luicWeb.h"
#include "luicSettings.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

enum IconIndex: int
{
    IconMain = 0,
};

enum PageIndex: int
{
    PageColors = 0,
};


CMainFrame::~CMainFrame()
{
}

CMainFrame::CMainFrame(CLegacyUIConfigurator& app)
    : m_App(app)
{
    ZeroMemory(&m_Pages, sizeof(m_Pages));
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

    m_ImList.Create(MaxIconWidth, MaxIconHeight, ILC_MASK, _countof(iconsIDs), 0);

    CIconHandle tempIco;
    for (const auto it: iconsIDs) {
        tempIco.LoadIconW(it, MaxIconWidth, MaxIconHeight);
        m_ImList.AddIcon(tempIco.Detach());
    }
}

void CMainFrame::PagesAppend(ATL::CStringW&& str, CPageImplPtr&& pagePtr)
{
    pagePtr->Create(m_hWnd);
    int it = m_Tab.InsertItem(0, TCIF_TEXT /*| TCIF_IMAGE*/, str.GetString(), IconMain, PageColors);

}

void CMainFrame::PagesCreate()
{
    CPageImplPtr   pBackground = CreatePageDlg<CPageBackground>();
    CPageImplPtr  pScreenSaver = CreatePageDlg<CPageScreenSaver>();
    CPageImplPtr   pAppearance = CreatePageDlg<CPageAppearance>();
    CPageImplPtr  pPageEffects = CreatePageDlg<CPageEffects>();
    CPageImplPtr      pPageWeb = CreatePageDlg<CPageWeb>();
    CPageImplPtr pPageSettings = CreatePageDlg<CPageSettings>();

    m_Tab.Attach(GetDlgItem(IDC_TAB1));
    m_Tab.ModifyStyle(0, WS_TABSTOP);
    m_Tab.SetImageList(m_ImList);

    PagesAppend( L"Background", std::move(pBackground));
    PagesAppend(L"ScreenSaver", std::move(pScreenSaver));
    PagesAppend( L"Appearance", std::move(pAppearance));
    PagesAppend(    L"Effects", std::move(pPageEffects));
    PagesAppend(        L"Web", std::move(pPageWeb));
    PagesAppend(   L"Settings", std::move(pPageSettings));
}

BOOL CMainFrame::OnInitDlg(HWND, LPARAM)
{
    ImListCreate();
    PagesCreate();

    HICON tempIco = m_ImList.GetIcon(IconMain);
    SetIcon(tempIco, FALSE);
    SetIcon(tempIco, TRUE);

    //m_Colors.Create(m_hWnd);
    //int it = m_Tab.InsertItem(0, TCIF_TEXT | TCIF_IMAGE, L"Colorz", IconMain, PageColors);
    //
    //CRect rcHead;
    //m_Tab.GetItemRect(it, rcHead);
    //
    //CRect rcTab;
    //m_Tab.GetClientRect(rcTab);
    //LONG hdrCy = rcHead.bottom - rcHead.top;
    ////rcTab.top = rcHead.bottom;
    ////rcTab.bottom -= hdrCy;
    //TabCtrl_AdjustRect(m_Tab.m_hWnd, FALSE, rcTab);
    //rcTab.top += hdrCy + 1;
    //rcTab.right += 2;
    //
    //m_Colors.MoveWindow(rcTab, FALSE);
    //m_Pages[it] = m_Colors.m_hWnd;

    ModifyStyle(0, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX, SWP_FRAMECHANGED);
    //MoveToMonitor{}.Move(m_hWnd, 1);

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
        //if (m_Pages[n]) {
            //::ShowWindow(m_Pages[n], SW_SHOW);
        //}
        DebugThreadPrintf(LTH_WM_NOTIFY L"   TCN_SELCHANGE: c:%4d n:%d\n", idCtrl, n);
        break;
    }
    case TCN_SELCHANGING: {
        int n = TabCtrl_GetCurSel(pnmh->hwndFrom);
        //if (m_Pages[n]) {
            //::ShowWindow(m_Pages[n], SW_HIDE);
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
