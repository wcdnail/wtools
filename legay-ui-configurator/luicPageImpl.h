#pragma once

#include <atlwin.h>
#include <atlframe.h>
#include <atlcrack.h>
#include <memory>

struct CPageImpl;

using  CPageImplPtr = std::unique_ptr<CPageImpl>;

struct CPageImpl: ATL::CDialogImpl<CPageImpl, CPageImpl>,
                  WTL::CDialogResize<CPageImpl>
{
    using   Super = ATL::CDialogImpl<CPageImpl, CPageImpl>;
    using Resizer = WTL::CDialogResize<CPageImpl>;

    ~CPageImpl() override;

protected:
    CPageImpl(UINT idd);

    virtual BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

private:
    friend class Super;
    friend class Resizer;

    UINT IDD;

    BEGIN_MSG_MAP_EX(CColorsPage)
        MSG_WM_INITDIALOG(OnInitDialog)
        CHAIN_MSG_MAP(Resizer)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CColorsPage)
        //DLGRESIZE_CONTROL(IDC_TEST_STA6, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    CPageImpl(CPageImpl const&) = delete;
    CPageImpl(CPageImpl&&) = delete;
    CPageImpl& operator = (CPageImpl const&) = delete;
    CPageImpl& operator = (CPageImpl&&) = delete;
};

template <typename Target, typename ...Args>
static inline CPageImplPtr CreatePageDlg(Args&&... args)
{
    auto ptr = std::make_unique<Target>(std::forward<Args>(args)...);
    return ptr;
}
