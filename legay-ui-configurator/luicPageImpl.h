#pragma once

#include <atlwin.h>
#include <atlframe.h>
#include <atlcrack.h>
#include <vector>
#include <memory>

struct CPageImpl;

using  CPageImplPtr = std::unique_ptr<CPageImpl>;

struct CPageImpl: ATL::CDialogImpl<CPageImpl>,
                  WTL::CDialogResize<CPageImpl>
{
    using     Super = ATL::CDialogImpl<CPageImpl>;
    using   Resizer = WTL::CDialogResize<CPageImpl>;
    using ResizeVec = std::vector<_AtlDlgResizeMap>;

    virtual HWND CreateDlg(HWND hWndParent, LPARAM dwInitParam = NULL);

    ~CPageImpl() override;

protected:
    friend class Super;
    friend class Resizer;

    UINT IDD;

    CPageImpl(UINT idd);

    void DlgResizeAdd(int nCtlID, DWORD dwResizeFlags);

    virtual void OnResizeNotify();
    virtual BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    virtual void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);
    virtual LRESULT OnNotify(int idCtrl, LPNMHDR pnmh);
    virtual void OnDestroy();

    HBRUSH OnEraseBkgnd(CDCHandle dc);

private:
    ResizeVec m_resiseMap;

    BEGIN_MSG_MAP_EX(CColorsPage)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_NOTIFY(OnNotify)
        MSG_WM_DESTROY(OnDestroy)
        if (Resizer::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) {
            OnResizeNotify();
            return TRUE;
        }
    END_MSG_MAP()

    WTL::_AtlDlgResizeMap const* GetDlgResizeMap() const;

    CPageImpl(CPageImpl const&) = delete;
    CPageImpl(CPageImpl&&) = delete;
    CPageImpl& operator = (CPageImpl const&) = delete;
    CPageImpl& operator = (CPageImpl&&) = delete;
};
