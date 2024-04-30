#pragma once

#include "resz/resource.h"
#include <atlwin.h>
#include <atlframe.h>
#include <atlcrack.h>

struct CLegacyUIConfigurator;

struct CMainFrame: ATL::CDialogImpl<CMainFrame>,
                   WTL::CDialogResize<CMainFrame>
{
    enum : int { IDD = IDD_LEGACY_UI_CONF_TOOL };

    using   Super = ATL::CDialogImpl<CMainFrame>;
    using Resizer = WTL::CDialogResize<CMainFrame>;

    CMainFrame(CLegacyUIConfigurator& app);
    ~CMainFrame();

private:
    CLegacyUIConfigurator& m_App;

    friend class Super;
    friend class Resizer;

    BEGIN_MSG_MAP_EX(CMainFrame)
        MSG_WM_INITDIALOG(OnInitDlg)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_COMMAND(OnCommand)
        CHAIN_MSG_MAP(Resizer)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CMainFrame)
        //DLGRESIZE_CONTROL(CTRL_ID_VIEW, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    int OnInitDlg(HWND, LPARAM);
    void OnDestroy();
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);

    CMainFrame(CMainFrame const&) = delete;
    CMainFrame(CMainFrame&&) = delete;
    CMainFrame& operator = (CMainFrame const&) = delete;
    CMainFrame& operator = (CMainFrame&&) = delete;
};
