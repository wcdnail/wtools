#pragma once

#include "resource.h"
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrlx.h>

struct CClassicAppearance;

struct CMainFrame: WTL::CFrameWindowImpl<CMainFrame, ATL::CWindow>,
                   WTL::CDialogResize<CMainFrame>,
                   CMessageFilter,
                   CIdleHandler
{
    using   Super = WTL::CFrameWindowImpl<CMainFrame, ATL::CWindow>;
    using Resizer = WTL::CDialogResize<CMainFrame> ;

    CMainFrame(CClassicAppearance& app);
    ~CMainFrame();

    using Super::Create;
    using Super::DestroyWindow;
    using Super::ShowWindow;
    using Super::SetWindowText;
    using Super::SetIcon;

private:
    enum : int
    {
        CTRL_ID_VIEW = 1000,
    };

    struct CView;

    CClassicAppearance&       m_App;
    CView*                  m_pView;
    CMultiPaneStatusBarCtrl  m_SBar;

    friend class Super;
    friend class Resizer;

    BEGIN_MSG_MAP_EX(CMainFrame)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_ACTIVATE(OnActivate)
        MSG_WM_KEYDOWN(OnKeyDown)
        CHAIN_MSG_MAP(Resizer)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CMainFrame)
        DLGRESIZE_CONTROL(ATL_IDW_STATUS_BAR,   DLSZ_SIZE_X)
        //DLGRESIZE_CONTROL(CTRL_ID_VIEW,         DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    int OnCreate(LPCREATESTRUCT);
    void OnDestroy();
    BOOL OnEraseBkgnd(CDCHandle dc);
    void OnActivate(UINT, BOOL, HWND);
    void OnKeyDown(UINT code, UINT, UINT);

    BOOL PreTranslateMessage(MSG* pMsg) override;
    BOOL OnIdle() override;

private:
    CMainFrame(CMainFrame const&) = delete;
    CMainFrame(CMainFrame&&) = delete;
    CMainFrame& operator = (CMainFrame const&) = delete;
    CMainFrame& operator = (CMainFrame&&) = delete;
};
