#pragma once

#include "dialogz.common.h"
#include <string>
#include <atlddx.h>

namespace Twins
{
    class AboutDialog: CommonDialog
                     , CWinDataExchange<AboutDialog>
    {
    private:
        typedef CommonDialog Super;
        typedef CWinDataExchange<AboutDialog> WinDdx;

    public:
        enum
        {
            ResultReply = ResultStart,
        };

        AboutDialog();
        ~AboutDialog();

        using Super::DoModal;

        static std::wstring AppUrl;

    private:
        friend class WinDdx;

        CBrush Background;
        CFont HeaderFont;
        CFont Header2Font;
        CFont VersionFont;
        CFont AuthorsFont;
        CStatic Header;
        CStatic Header2;
        CEdit Software;
        CEdit License;
        CEdit Authors;

        BOOL DoDataExchange(BOOL bSaveAndValidate = FALSE, UINT nCtlID = (UINT)-1);
        BOOL OnInitDialog(HWND, LPARAM param);
        void OnDestroy();
        BOOL OnEraseBkgnd(CDCHandle dc);
        HBRUSH OnCtlColor(CDCHandle dc, HWND);
        void OnMouseDown(UINT flags, CPoint point);

        BEGIN_MSG_MAP_EX(AboutDialog)
            CHAIN_MSG_MAP(Super)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_CTLCOLORSTATIC(OnCtlColor)
            MSG_WM_CTLCOLOREDIT(OnCtlColor)
            MSG_WM_LBUTTONDOWN(OnMouseDown)
            MSG_WM_RBUTTONDOWN(OnMouseDown)
            MSG_WM_MBUTTONDOWN(OnMouseDown)
        END_MSG_MAP()
    };
}
