#pragma once

#include "resz/resource.h"
#include <atlwin.h>
#include <atlframe.h>
#include <atlcrack.h>

struct CPageDlgTemplate: ATL::CDialogImpl<CPageDlgTemplate>,
                         WTL::CDialogResize<CPageDlgTemplate>
{
    enum : int { IDD = IDD_PAGE_SCREENSAVER };

    using   Super = ATL::CDialogImpl<CPageDlgTemplate>;
    using Resizer = WTL::CDialogResize<CPageDlgTemplate>;

    ~CPageDlgTemplate() override;
    CPageDlgTemplate();

private:
    friend class Super;
    friend class Resizer;

    BEGIN_MSG_MAP_EX(CColorsPage)
        MSG_WM_INITDIALOG(OnInitDlg)
        CHAIN_MSG_MAP(Resizer)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CColorsPage)
        //DLGRESIZE_CONTROL(IDC_TEST_STA1, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    int OnInitDlg(HWND, LPARAM);

    CPageDlgTemplate(CPageDlgTemplate const&) = delete;
    CPageDlgTemplate(CPageDlgTemplate&&) = delete;
    CPageDlgTemplate& operator = (CPageDlgTemplate const&) = delete;
    CPageDlgTemplate& operator = (CPageDlgTemplate&&) = delete;
};
