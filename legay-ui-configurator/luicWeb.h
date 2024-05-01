#pragma once

#include "luicPageImpl.h"
#include "resz/resource.h"
#include <atlwin.h>
#include <atlframe.h>
#include <atlcrack.h>

struct CPageWeb: ATL::CDialogImpl<CPageWeb, CPageImpl>,
                 WTL::CDialogResize<CPageWeb>
{
    enum : int { IDD = IDD_PAGE_WEB };

    using   Super = ATL::CDialogImpl<CPageWeb, CPageImpl>;
    using Resizer = WTL::CDialogResize<CPageWeb>;

    ~CPageWeb() override;
    CPageWeb();

private:
    friend class Super;
    friend class Resizer;

    BEGIN_MSG_MAP_EX(CColorsPage)
        MSG_WM_INITDIALOG(OnInitDlg)
        CHAIN_MSG_MAP(Resizer)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CColorsPage)
        DLGRESIZE_CONTROL(IDC_TEST_STA5, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    int OnInitDlg(HWND, LPARAM);

    CPageWeb(CPageWeb const&) = delete;
    CPageWeb(CPageWeb&&) = delete;
    CPageWeb& operator = (CPageWeb const&) = delete;
    CPageWeb& operator = (CPageWeb&&) = delete;
};
