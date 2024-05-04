#include "stdafx.h"
#include "luicMainFrame.h"
#include "luicMain.h"
#include "rect.putinto.h"
#include "dh.tracing.h"
#include "dh.tracing.defs.h"
#include "dev.assistance/dev.assist.h"
#include "debug.dump.msg.h"
#include "resz/resource.h"
#include <atlwin.h>

CMainFrame::~CMainFrame()
{
}

CMainFrame::CMainFrame(Conf::Section const& parentSettings)
    :          Super{}
    ,  m_rcMainFrame{ 0, 0, 600, 800 }
    ,     m_Settings{ parentSettings, L"MainFrame" }
    , m_bInitialized{ false }
{
    Rc::PutInto(Rc::Screen, m_rcMainFrame, Rc::Center);
}

void CMainFrame::SetStatus(int status, ATL::CStringW&& message)
{
    DH::ThreadPrintf(LTH_STATUS L" [s:%d] %s\n", status, message.GetString());
    if (!m_SBar.m_hWnd) {
        return;
    }
    m_SBar.SetPaneText(ID_DEFAULT_PANE, message.GetString());
    m_SBar.Invalidate(TRUE);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    if (Super::PreTranslateMessage(pMsg)) {
        return TRUE;
    }
    if (m_View.PreTranslateMessage(pMsg)) {
        return TRUE;
    }
    DBG_DUMP_WMESSAGE(LTH_MAINFRAME, L"Main", pMsg);
    return FALSE;
}

BOOL CMainFrame::OnIdle()
{
    return FALSE;
}

void CMainFrame::OnResizeNotify()
{
    if (!m_View.m_hWnd) {
        return ;
    }
    CRect rcView;
    CRect rcStatuBar;
    GetClientRect(rcView);
    ::GetClientRect(m_hWndStatusBar, rcStatuBar);
    rcView.bottom -= rcStatuBar.Height();
    rcView.DeflateRect(1, 1);
    m_View.MoveWindow(rcView);
}

int CMainFrame::OnCreate(LPCREATESTRUCT)
{
    DBG_DUMP_WMESSAGE_EXT(LTH_MAINFRAME, L"Main", m_hWnd, WM_CREATE, 0, 0);

    auto const* pApp = CLUIApp::App();

    HICON tempIco = pApp->GetIcon(IconMain);
    SetIcon(tempIco, FALSE);
    SetIcon(tempIco, TRUE);

    FromSettings(m_Settings, m_rcMainFrame);
    MoveWindow(m_rcMainFrame);

    CreateSimpleStatusBar();
    m_SBar.SubclassWindow(m_hWndStatusBar);
    int arrParts[] =
    {
        ID_DEFAULT_PANE,
      //ID_DEFAULT_PANE + 1,
      //ID_DEFAULT_PANE + 2
    };
    m_SBar.SetPanes(arrParts, _countof(arrParts), false);

    if (!m_View.CreateDlg(m_hWnd)) {
        const auto code = static_cast<HRESULT>(GetLastError());
        ReportError(L"Creation of MainView failed!", code, true, MB_ICONSTOP);
        return -1;
    }

    OnResizeNotify();
    m_View.ShowWindow(SW_SHOW);

    CMessageLoop* pLoop = pApp->GetMessageLoop();
    ATLASSERT(pLoop != nullptr);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);

    DlgResize_Init(false, false);

    m_bInitialized = true;
    m_View.SetFocus();
    return 0;
}

void CMainFrame::OnDestroy()
{
    CMessageLoop* pLoop = CLUIApp::App()->GetMessageLoop();
    ATLASSERT(pLoop != nullptr);
    pLoop->RemoveIdleHandler(this);
    pLoop->RemoveMessageFilter(this);

    bool isMaximized = (TRUE == IsZoomed());
    if(!isMaximized) {
        GetWindowRect(m_rcMainFrame);
    }

    PostQuitMessage(0);
    SetMsgHandled(FALSE);
}

void CMainFrame::OnSetFocus(HWND hWndOld)
{
    UNREFERENCED_PARAMETER(hWndOld);
    if (!m_bInitialized) {
        return;
    }
    if (hWndOld != m_View.m_hWnd) {
        m_View.SetFocus();
    }
}

void CMainFrame::OnMenuCommand(int nID)
{
    UNREFERENCED_PARAMETER(nID);
}

void CMainFrame::OnAccelerator(int nID)
{
    switch (nID) {
    case ID_ACC_CTRL_TAB:       m_View.TabShift(1); break;
    case ID_ACC_CTRL_SHIFT_TAB: m_View.TabShift(-1); break;
    case ID_ACC_SELECT_ALL:     m_View.SelectAll(); break;
    }
    //DebugThreadPrintf(LTH_MAINFRAME L" ACCELERATOR: %d %p\n", nID);
}

void CMainFrame::OnCommand(UINT uNotifyCode, int nID, HWND hWndCtl)
{
    //
    // https://learn.microsoft.com/en-us/windows/win32/menurc/wm-command
    //
    //              uNotifyCode        nID         hWndCtrl
    // Menu               0         ID_<menu>         0
    // Accelerator        1         ID_<accel>        0
    // Control           xxx          CtlId        hWndCtrl
    //

    switch (uNotifyCode) {
    case 0: // Menu -------------------------
        OnMenuCommand(nID);
        return ;
    case 1: // Accelerator ------------------
        OnAccelerator(nID);
        return ;
    }

    SetMsgHandled(FALSE);
}
