#pragma once

#include <atlframe.h>
#include <atlwin.h>

struct CClassicAppearance;

struct CMainFrame: WTL::CFrameWindowImpl<CMainFrame, ATL::CWindow>,
                   WTL::CDialogResize<CMainFrame>
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
    CClassicAppearance& m_App;

    friend class Super;
    friend class Resizer;

    enum
    {
        IdView = 1000,
    };

    BEGIN_MSG_MAP_EX(CMainFrame)
      MSG_WM_CREATE(OnCreate)
      MSG_WM_DESTROY(OnDestroy)
      MSG_WM_ERASEBKGND(OnEraseBkgnd)
      MSG_WM_ACTIVATE(OnActivate)
      MSG_WM_KEYDOWN(OnKeyDown)
      CHAIN_MSG_MAP(Resizer)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CMainFrame)
      //DLGRESIZE_CONTROL(IdView, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    int OnCreate(LPCREATESTRUCT);
    void OnDestroy();
    BOOL OnEraseBkgnd(CDCHandle dc);
    void OnActivate(UINT, BOOL, HWND);
    void OnKeyDown(UINT code, UINT, UINT);

private:
    CMainFrame(CMainFrame const&) = delete;
    CMainFrame(CMainFrame&&) = delete;
    CMainFrame& operator = (CMainFrame const&) = delete;
    CMainFrame& operator = (CMainFrame&&) = delete;
};
