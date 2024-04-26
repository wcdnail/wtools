#include "pch.h"
#include "ClAppMainFrame.h"
#include "ClAppearance.h"
#include "resource.h"
#include <atlwin.h>

struct CMainFrame::CView: ATL::CDialogImpl<CView>
{
    enum { IDD = IDD_MF_VIEW };

    ~CView()
    {
    }

    CView()
    {
    }

    //void DoInitTemplate() 
    //{
    //}
    //
    //void DoInitControls() 
    //{
    //}

    BEGIN_MSG_MAP(CAboutDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    END_MSG_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        //CenterWindow(GetParent());
        return TRUE;
    }

    LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        //EndDialog(wID);
        return 0;
    }
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
    CIcon icon(::LoadIconW(m_App.GetModuleInstance(), MAKEINTRESOURCEW(IDI_MAIN)));
    if (nullptr != icon) {
        HICON raw = icon.Detach();
        SetIcon(raw, FALSE);
        SetIcon(raw, TRUE);
    }

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

    if (m_pView) {
        CRect rcView = rc;
        m_hWndClient = m_pView->Create(m_hWnd, rcView, reinterpret_cast<LPARAM>(this));
      //m_hWndClient = m_pView->Create(m_hWnd, rcView, nullptr,
      //    WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
      //    WS_EX_CLIENTEDGE,
      //    ATL::_U_MENUorID(CTRL_ID_VIEW)
      //);
        if (!m_hWndClient) {
            HRESULT hr = ::GetLastError();
            ATLTRACE2(atlTraceUI, 0, _T("FAILED create VIEW [%08x] <%s>\n"), hr, _T(__FUNCDNAME__));
        }
        else {
            ////m_pView->Invalidate();
            //ATL::_U_MENUorID mnu(CTRL_ID_VIEW);
            //if (!::SetMenu(m_hWndClient, mnu.m_hMenu)) {
            //    HRESULT hr = ::GetLastError();
            //    ATLTRACE2(atlTraceUI, 0, _T("FAILED set VIEW id [%08x] <%s>\n"), hr, _T(__FUNCDNAME__));
            //}
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
    CMessageLoop* pLoop = m_App.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->RemoveIdleHandler(this);
    pLoop->RemoveMessageFilter(this);

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
