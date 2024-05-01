#pragma once

#include "luicPageImpl.h"
#include "resz/resource.h"
#include <atlwin.h>
#include <atlframe.h>
#include <atlcrack.h>

struct CPageScreenSaver: ATL::CDialogImpl<CPageScreenSaver, CPageImpl>,
                         WTL::CDialogResize<CPageScreenSaver>
{
    enum : int { IDD = IDD_PAGE_SCREENSAVER };

    using   Super = ATL::CDialogImpl<CPageScreenSaver, CPageImpl>;
    using Resizer = WTL::CDialogResize<CPageScreenSaver>;

    ~CPageScreenSaver() override;
    CPageScreenSaver();

private:
    friend class Super;
    friend class Resizer;

    BEGIN_MSG_MAP_EX(CColorsPage)
        MSG_WM_INITDIALOG(OnInitDlg)
        CHAIN_MSG_MAP(Resizer)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CColorsPage)
        DLGRESIZE_CONTROL(IDC_TEST_STA3, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    int OnInitDlg(HWND, LPARAM);

    CPageScreenSaver(CPageScreenSaver const&) = delete;
    CPageScreenSaver(CPageScreenSaver&&) = delete;
    CPageScreenSaver& operator = (CPageScreenSaver const&) = delete;
    CPageScreenSaver& operator = (CPageScreenSaver&&) = delete;
};
