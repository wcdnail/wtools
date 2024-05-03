#pragma once

#include <atlwin.h>
#include <atlframe.h>
#include <atlctrlx.h>

struct CMainFrame: WTL::CFrameWindowImpl<CMainFrame, ATL::CWindow>,
                   WTL::CDialogResize<CMainFrame>,
                   CMessageFilter,
                   CIdleHandler
{
    using   Super = WTL::CFrameWindowImpl<CMainFrame, ATL::CWindow>;
    using Resizer = WTL::CDialogResize<CMainFrame> ;

    ~CMainFrame() override;
    CMainFrame();

private:
    CMultiPaneStatusBarCtrl m_SBar;

    friend class Super;
    friend class Resizer;

    BEGIN_MSG_MAP_EX(CMainFrame)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        CHAIN_MSG_MAP(Resizer)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CMainFrame)
        DLGRESIZE_CONTROL(ATL_IDW_STATUS_BAR, DLSZ_SIZE_X)
    END_DLGRESIZE_MAP()

    int OnCreate(LPCREATESTRUCT);
    void OnDestroy();

    BOOL PreTranslateMessage(MSG* pMsg) override;
    BOOL OnIdle() override;

private:
    CMainFrame(CMainFrame const&) = delete;
    CMainFrame(CMainFrame&&) = delete;
    CMainFrame& operator = (CMainFrame const&) = delete;
    CMainFrame& operator = (CMainFrame&&) = delete;
};
