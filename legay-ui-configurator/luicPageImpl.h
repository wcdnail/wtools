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

    ~CPageImpl() override;

protected:
    CPageImpl(UINT idd);
    void DlgResizeAdd(int nCtlID, DWORD dwResizeFlags);
    virtual BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

private:
    friend class Super;
    friend class Resizer;

    UINT              IDD;
    ResizeVec m_resiseMap;

    BEGIN_MSG_MAP_EX(CColorsPage)
        MSG_WM_INITDIALOG(OnInitDialog)
        CHAIN_MSG_MAP(Resizer)
    END_MSG_MAP()

    WTL::_AtlDlgResizeMap const* GetDlgResizeMap() const;

    CPageImpl(CPageImpl const&) = delete;
    CPageImpl(CPageImpl&&) = delete;
    CPageImpl& operator = (CPageImpl const&) = delete;
    CPageImpl& operator = (CPageImpl&&) = delete;
};
