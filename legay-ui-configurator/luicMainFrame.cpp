#include "stdafx.h"
#include "luicMainFrame.h"
#include "luicMain.h"
#include "rect.putinto.h"
#include "resz/resource.h"
#include <atlwin.h>

CMainFrame::~CMainFrame()
{
}

CMainFrame::CMainFrame(Conf::Section const& parentSettings)
    :         Super{}
    , m_rcMainFrame{ 0, 0, 600, 800 }
    ,    m_Settings{ parentSettings, L"MainFrame" }
{
    Rc::PutInto(Rc::Screen, m_rcMainFrame, Rc::Center);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    if (Super::PreTranslateMessage(pMsg)) {
        return TRUE;
    }
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
    auto const* pApp = CLegacyUIConfigurator::App();

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
    return 0;
}

void CMainFrame::OnDestroy()
{
    CMessageLoop* pLoop = CLegacyUIConfigurator::App()->GetMessageLoop();
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
