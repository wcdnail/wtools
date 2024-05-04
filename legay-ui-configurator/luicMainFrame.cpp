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

#include "debug.dump.msg.h"

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

void CMainFrame::SetStatus(int status, ATL::CStringW&& message)
{
    DH::ThreadPrintf(LTH_STATUS L" [s:%d] %s\n", status, message.GetString());
    if (!m_SBar.m_hWnd) {
        return;
    }
    m_SBar.SetPaneText(ID_DEFAULT_PANE, message.GetString());
    m_SBar.Invalidate(TRUE);
}

static bool MyIsDialogMessage(HWND hFrameWnd, LPMSG pMsg)
{
    if ((pMsg->message < WM_KEYFIRST   || pMsg->message > WM_KEYLAST) &&
        (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST)) {
        return false;
    }
    // find a direct child of the dialog from the window that has focus
    HWND hWndCtl = ::GetFocus();
    if (::IsChild(hFrameWnd, hWndCtl) && ::GetParent(hWndCtl) != hFrameWnd) {
        do {
            hWndCtl = ::GetParent(hWndCtl);
        }
        while (::GetParent(hWndCtl) != hFrameWnd);
    }
    // give controls a chance to translate this message
    LRESULT lr = SendMessage(hWndCtl, WM_FORWARDMSG, 0, reinterpret_cast<LPARAM>(pMsg));
    DBG_DUMP_WMESSAGE(LTH_CONTROL, pMsg);
    // ##TODO: DO NOT do the Windows default thing
    return false; //CDialogImplBaseT<TBase>::IsDialogMessage(pMsg);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    if (m_View.IsDialogMessageW(pMsg)) {
        DBG_DUMP_WMESSAGE(LTH_CONTROL, pMsg);
        return TRUE;
    }
    else {
        DBG_DUMP_WMESSAGE(LTH_MAINFRAME, pMsg);
    }
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

#ifdef _DEBUG
    {
        wchar_t  _text[256] = {0};
        wchar_t _class[256] = {0};
        int clen = GetClassNameW(m_hWnd, _class, _countof(_class) - 1);
        int tlen = GetWindowTextW(_text, _countof(_text) - 1);
        DebugThreadPrintf(LTH_MAINFRAME L" WM_CREATE w:%p class:'%s' %s\n", m_hWnd, _class, _text);
    }
#endif

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
