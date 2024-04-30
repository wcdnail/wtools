#include "stdafx.h"
#include "luicMainFrame.h"
#include "luicMain.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

enum IconIndex: int
{
    IconMain = 0,
};

CMainFrame::~CMainFrame()
{
}

CMainFrame::CMainFrame(CLegacyUIConfigurator& app)
    : m_App(app)
{
}

BOOL CMainFrame::OnInitDlg(HWND, LPARAM)
{
    m_ImList.Create(32, 32, ILC_MASK, 0, 0);

    HICON rawIcon = LoadIconW(m_App.GetModuleInstance(), MAKEINTRESOURCEW(IDI_COMP));
    if (nullptr != rawIcon) {
        SetIcon(rawIcon, FALSE);
        SetIcon(rawIcon, TRUE);
    }
    m_ImList.AddIcon(rawIcon); // 0 = IconMain

    m_Tab.Attach(GetDlgItem(IDC_TAB1));
    m_Tab.ModifyStyle(0, WS_TABSTOP);
    m_Tab.SetImageList(m_ImList);

    m_Tab.InsertItem(0, TCIF_TEXT | TCIF_IMAGE, L"Tab1", IconMain, 0l);

    // debug...
    m_Tab.InsertItem(1, TCIF_TEXT, L"#1 ...", 0, 0l);
    m_Tab.InsertItem(2, TCIF_TEXT, L"#2 ...", 0, 0l);
    m_Tab.InsertItem(3, TCIF_TEXT, L"#3 ...", 0, 0l);

    ModifyStyle(0, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX, SWP_FRAMECHANGED);
    MoveToMonitor{}.Move(m_hWnd, 1);

    DlgResize_Init(true, true);
    return 0;
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
        DebugThreadPrintf(LTH_WM_NOTIFY L"   TCN_SELCHANGE: c:%4d n:%d\n", idCtrl, n);
        break;
    }
    case TCN_SELCHANGING: {
        int n = TabCtrl_GetCurSel(pnmh->hwndFrom);
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
