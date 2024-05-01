#pragma once

#include "luicPageImpl.h"
#include "resz/resource.h"
#include <atlwin.h>
#include <atlframe.h>
#include <atlcrack.h>

struct CPageEffects: ATL::CDialogImpl<CPageEffects, CPageImpl>,
                     WTL::CDialogResize<CPageEffects>
{
    enum : int { IDD = IDD_PAGE_EFFECTS };

    using   Super = ATL::CDialogImpl<CPageEffects, CPageImpl>;
    using Resizer = WTL::CDialogResize<CPageEffects>;

    ~CPageEffects() override;
    CPageEffects();

private:
    friend class Super;
    friend class Resizer;

    BEGIN_MSG_MAP_EX(CColorsPage)
        MSG_WM_INITDIALOG(OnInitDlg)
        CHAIN_MSG_MAP(Resizer)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CColorsPage)
        DLGRESIZE_CONTROL(IDC_TEST_STA4, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    int OnInitDlg(HWND, LPARAM);

    CPageEffects(CPageEffects const&) = delete;
    CPageEffects(CPageEffects&&) = delete;
    CPageEffects& operator = (CPageEffects const&) = delete;
    CPageEffects& operator = (CPageEffects&&) = delete;
};
