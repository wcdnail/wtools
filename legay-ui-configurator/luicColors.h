#pragma once

#include "resz/resource.h"
#include <atlwin.h>
#include <atlframe.h>
#include <atlcrack.h>

struct CColorsPage: ATL::CDialogImpl<CColorsPage>,
                    WTL::CDialogResize<CColorsPage>
{
    enum : int { IDD = IDD_PAGE_COLORS };

    using   Super = ATL::CDialogImpl<CColorsPage>;
    using Resizer = WTL::CDialogResize<CColorsPage>;

    ~CColorsPage() override;
    CColorsPage();

private:
    friend class Super;
    friend class Resizer;

    BEGIN_MSG_MAP_EX(CColorsPage)
        MSG_WM_INITDIALOG(OnInitDlg)
        CHAIN_MSG_MAP(Resizer)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CColorsPage)
        DLGRESIZE_CONTROL(IDC_TEST_STA1, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    int OnInitDlg(HWND, LPARAM);

    CColorsPage(CColorsPage const&) = delete;
    CColorsPage(CColorsPage&&) = delete;
    CColorsPage& operator = (CColorsPage const&) = delete;
    CColorsPage& operator = (CColorsPage&&) = delete;
};
