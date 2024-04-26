#include "pch.h"
#include "ClAppMainFrame.h"
#include "ClAppearance.h"
#include "resource.h"

CMainFrame::~CMainFrame()
{
}

CMainFrame::CMainFrame(CClassicAppearance& app)
    : Super()
    , m_App(app)
{
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

    //CRect rcView = rc;
    //ErrorCode error = viewer.Create(m_hWnd, rcView, IdView);
    //if(error)
    //{
    //   WidecharString message = L"Can't create viewer!\r\n"
    //      + std::to_wstring((_Longlong)error.value()) + L": "
    //      + CA2W(error.message().c_str()).m_psz;
    //
    //   ::MessageBoxW(m_hWnd, message.c_str(), Runtime::Info().Executable.Version.ProductName.c_str(), MB_ICONSTOP);
    //   return -1;
    //}
    //
    //viewer.Invalidate();

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
