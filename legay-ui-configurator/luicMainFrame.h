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

    using     Super = ATL::CDialogImpl<CMainFrame>;
    using   Resizer = WTL::CDialogResize<CMainFrame>;
    using  PagesMap = std::map<int, CPageImplPtr>;
    using ResizeArr = ATL::CSimpleArray<WTL::_AtlDlgResizeMap>;

    ~CMainFrame() override;
    CMainFrame(CLegacyUIConfigurator& app);

private:
    CLegacyUIConfigurator& m_App;
    WTL::CTabCtrl          m_Tab;
    CImageList          m_ImList;
    PagesMap          m_PagesMap;
    CRect          m_rcTabClient;
    ResizeArr        m_ResizeArr;

    void ImListCreate();
    void PagesGetRect();
    void PagesAppend(int desiredIndex, ATL::CStringW&& str, CPageImplPtr&& pagePtr);
    void PagesCreate();
    void PagesShow(int numba, bool show);
    CPageImplPtr const& PagesGetCurrent() const;
    void PagesResizingNotify();

    friend class Super;
    friend class Resizer;

    BEGIN_MSG_MAP_EX(CMainFrame)
        MSG_WM_INITDIALOG(OnInitDlg)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_NOTIFY(OnNotify)
        if (Resizer::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) {
            PagesResizingNotify();
            return TRUE;
        }
    END_MSG_MAP()

    WTL::_AtlDlgResizeMap const* GetDlgResizeMap() const;

    int OnInitDlg(HWND, LPARAM);
    void OnDestroy();
    LRESULT OnNotify(int idCtrl, LPNMHDR pnmh);
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);

    CMainFrame(CMainFrame const&) = delete;
    CMainFrame(CMainFrame&&) = delete;
    CMainFrame& operator = (CMainFrame const&) = delete;
    CMainFrame& operator = (CMainFrame&&) = delete;
};
