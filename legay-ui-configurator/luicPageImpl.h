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

    virtual BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    HBRUSH OnEraseBkgnd(CDCHandle dc);

private:
    ResizeVec m_resiseMap;

    BEGIN_MSG_MAP_EX(CColorsPage)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        CHAIN_MSG_MAP(Resizer)
    END_MSG_MAP()

    WTL::_AtlDlgResizeMap const* GetDlgResizeMap() const;

    CPageImpl(CPageImpl const&) = delete;
    CPageImpl(CPageImpl&&) = delete;
    CPageImpl& operator = (CPageImpl const&) = delete;
    CPageImpl& operator = (CPageImpl&&) = delete;
};
