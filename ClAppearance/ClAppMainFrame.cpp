#include "pch.h"
#include "ClAppMainFrame.h"
#include "ClAppearance.h"
#include "resource.h"
#include <atlwin.h>

struct CMainFrame::CView: ATL::CWindowImpl<CView>
{
    ~CView()
    {
    }

    CView()
    {
    }

    DECLARE_WND_CLASS(NULL)

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        pMsg;
        return FALSE;
    }

    BEGIN_MSG_MAP(CView)
    END_MSG_MAP()
};


CMainFrame::~CMainFrame()
{
    delete m_pView;
}

CMainFrame::CMainFrame(CClassicAppearance& app)
    : Super()
    , m_App(app)
    , m_pView(new CView)
{
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    if (Super::PreTranslateMessage(pMsg)) {
        return TRUE;
    }
    if (m_pView && m_pView->PreTranslateMessage(pMsg)) {
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
    CIcon icon(::LoadIconW(m_App.GetModuleInstance(), MAKEINTRESOURCEW(IDI_MAIN)));
    if (nullptr != icon) {
        HICON raw = icon.Detach();
        SetIcon(raw, FALSE);
        SetIcon(raw, TRUE);
    }

    CRect rc;
    GetClientRect(rc);
    rc.InflateRect(1, 1);

    if (m_pView) {
        CRect rcView = rc;
        m_hWndClient = m_pView->Create(m_hWnd, rcView, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
        if (!m_hWndClient) {
            HRESULT hr = ::GetLastError();
            ATLTRACE2(atlTraceUI, 0, _T("FAILED create VIEW [%08x] <%s>\n"), hr, _T(__FUNCDNAME__));
        }
        else {
            m_pView->Invalidate();
        }
    }

    CMessageLoop* pLoop = m_App.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);

    DlgResize_Init(false, false);
    return 0;
}

void CMainFrame::OnDestroy()
{
    bool isMaximized = (TRUE == IsZoomed());
    if(!isMaximized) {
        CRect rc;
        GetWindowRect(rc);
        // TODO: save rc
    }

    ::PostQuitMessage(0);
    SetMsgHandled(FALSE);
}

BOOL CMainFrame::OnEraseBkgnd(CDCHandle dc)
{
    return TRUE;
}

void CMainFrame::OnActivate(UINT, BOOL, HWND)
{
    //viewer.SetFocus();
}

void CMainFrame::OnKeyDown(UINT code, UINT, UINT)
{
    if(VK_ESCAPE == code) {
       //DestroyWindow();
    }
}
