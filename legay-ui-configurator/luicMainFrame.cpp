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

int CMainFrame::OnCreate(LPCREATESTRUCT)
{
    auto const* pApp = CLegacyUIConfigurator::App();

    FromSettings(m_Settings, m_rcMainFrame);
    MoveWindow(m_rcMainFrame);

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


    CMessageLoop* pLoop = pApp->GetMessageLoop();
    ATLASSERT(pLoop != nullptr);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);

    DlgResize_Init(false, true);
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
