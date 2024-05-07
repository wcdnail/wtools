#pragma once

#include "luicMainView.h"
#include "settings.h"
#include "resz/resource.h"
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrlx.h>

struct CMainFrame: WTL::CFrameWindowImpl<CMainFrame, ATL::CWindow>,
                   WTL::CDialogResize<CMainFrame>,
                   WTL::CMessageFilter,
                   WTL::CIdleHandler
{
    using   Super = WTL::CFrameWindowImpl<CMainFrame, ATL::CWindow>;
    using Resizer = WTL::CDialogResize<CMainFrame> ;

    ~CMainFrame() override;
    CMainFrame(Conf::Section const& parentSettings);

    void SetStatus(int status, ATL::CStringW&& message);

private:
    CRect                      m_rcMainFrame;
    Conf::Section                 m_Settings;
    CMainView                         m_View;
    WTL::CMultiPaneStatusBarCtrl m_StatusBar;
    bool                      m_bInitialized;

    friend class Super;
    friend class Resizer;

    DECLARE_FRAME_WND_CLASS_EX(_T("CLegacyUI::MainFrame"),
                               IDR_CLEGACYUI_MAINFRAME,
                               CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
                               COLOR_APPWORKSPACE)

    BOOL PreTranslateMessage(MSG* pMsg) override;
    BOOL OnIdle() override;

    BEGIN_MSG_MAP_EX(CMainFrame)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SETFOCUS(OnSetFocus)
        MSG_WM_COMMAND(OnCommand)
        if(Resizer::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) {
            OnResizeNotify();
            return TRUE;
        }
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CMainFrame)
        DLGRESIZE_CONTROL(ATL_IDW_STATUS_BAR, DLSZ_SIZE_X)
    END_DLGRESIZE_MAP()

    void OnResizeNotify();
    int OnCreate(LPCREATESTRUCT);
    void OnDestroy();
    void OnSetFocus(HWND hWndOld);
    void OnMenuCommand(int nID);
    void OnAccelerator(int nID);
    void OnCommand(UINT uNotifyCode, int nID, HWND hWndCtl);

private:
    CMainFrame(CMainFrame const&) = delete;
    CMainFrame(CMainFrame&&) = delete;
    CMainFrame& operator = (CMainFrame const&) = delete;
    CMainFrame& operator = (CMainFrame&&) = delete;
};
