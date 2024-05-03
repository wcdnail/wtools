#include "stdafx.h"
#include "luicMainFrameWnd.h"
#include "luicMain.h"
#include "resz/resource.h"
#include <atlwin.h>

CMainFrame::~CMainFrame()
{
}

CMainFrame::CMainFrame()
    : Super()
{
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    if (Super::PreTranslateMessage(pMsg)) {
        return TRUE;
    }
  //if (m_pView && m_pView->PreTranslateMessage(pMsg)) {
  //    return TRUE;
  //}
    return FALSE;
}

BOOL CMainFrame::OnIdle()
{
    return FALSE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT)
{
    auto const* pApp = CLegacyUIConfigurator::App();

    HICON tempIco = pApp->GetIcon(IconMain);
    SetIcon(tempIco, FALSE);
    SetIcon(tempIco, TRUE);

    CreateSimpleStatusBar();
    m_SBar.SubclassWindow(m_hWndStatusBar);
    int arrParts[] =
    {
        ID_DEFAULT_PANE,
      //ID_DEFAULT_PANE + 1,
      //ID_DEFAULT_PANE + 2
    };
    m_SBar.SetPanes(arrParts, _countof(arrParts), false);

    CRect rc;
    GetClientRect(rc);
    rc.InflateRect(1, 1);

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
        CRect rc;
        GetWindowRect(rc);
        // TODO: save rc
    }

    PostQuitMessage(0);
    SetMsgHandled(FALSE);
}
