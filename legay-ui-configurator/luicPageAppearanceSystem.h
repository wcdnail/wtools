#pragma once

#include "luicPageImpl.h"
#include <atlsnap.h>

struct CSysPageAppearance: CPageImpl, ATL::CSnapInPropertyPageImpl<CSysPageAppearance, false>
{
    using Super = ATL::CSnapInPropertyPageImpl<CSysPageAppearance, false>;

    enum : int { IDD_CLASSIC = 200, IDD = IDD_CLASSIC };

    ~CSysPageAppearance() override;
    CSysPageAppearance();

private:
    friend Super;

    HPROPSHEETPAGE m_hPropSheet;

    static UINT PropPageCallback2(HWND hWnd, UINT uMsg, LPPROPSHEETPAGEW ppsp);
    static int PropSheetProc(HWND hWnd, UINT uMsg, LPARAM lParam);

    HWND CreateDlg(HWND hWndParent, LPARAM dwInitParam = NULL) override;
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
