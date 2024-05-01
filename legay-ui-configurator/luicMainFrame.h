#pragma once

#include "luicPageImpl.h"
#include "resz/resource.h"
#include <atlwin.h>
#include <atlframe.h>
#include <atlcrack.h>
#include <map>

struct CLegacyUIConfigurator;

struct CMainFrame: ATL::CDialogImpl<CMainFrame>,
                   WTL::CDialogResize<CMainFrame>
{
    enum : int { IDD = IDD_LEGACY_UI_CONF_TOOL };

    using    Super = ATL::CDialogImpl<CMainFrame>;
    using  Resizer = WTL::CDialogResize<CMainFrame>;
    using PagesMap = std::map<int, CPageImplPtr>;

    ~CMainFrame() override;
    CMainFrame(CLegacyUIConfigurator& app);

private:
    CLegacyUIConfigurator& m_App;
    WTL::CTabCtrl          m_Tab;
    CImageList          m_ImList;
    PagesMap          m_PagesMap;

    void ImListCreate();
    void PagesGetRect(int n, CRect& rcTab);
    void PagesAppend(int desiredIndex, ATL::CStringW&& str, CPageImplPtr&& pagePtr);
    void PagesCreate();

    friend class Super;
    friend class Resizer;

    BEGIN_MSG_MAP_EX(CMainFrame)
        MSG_WM_INITDIALOG(OnInitDlg)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_NOTIFY(OnNotify)
        CHAIN_MSG_MAP(Resizer)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CMainFrame)
        DLGRESIZE_CONTROL(IDC_BN_APPLY, DLSZ_MOVE_X | DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_TAB1, DLSZ_SIZE_X | DLSZ_SIZE_Y)
      //DLGRESIZE_CONTROL(IDD_PAGE_COLORS, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    int OnInitDlg(HWND, LPARAM);
    void OnDestroy();
    LRESULT OnNotify(int idCtrl, LPNMHDR pnmh);
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);

    CMainFrame(CMainFrame const&) = delete;
    CMainFrame(CMainFrame&&) = delete;
    CMainFrame& operator = (CMainFrame const&) = delete;
    CMainFrame& operator = (CMainFrame&&) = delete;
};
