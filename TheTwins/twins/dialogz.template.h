#pragma once

#include "dialogz.common.h"
#include <atlddx.h>

namespace Twins
{
    class __TemplateDialog: CommonDialog
                          , CWinDataExchange<__TemplateDialog>
    {
    private:
        typedef CommonDialog Super;
        typedef CWinDataExchange<__TemplateDialog> WinDdx;

    public:
        __TemplateDialog(PCTSTR caption);
        ~__TemplateDialog();

        using Super::DoModal;

    private:
        friend class WinDdx;

        BOOL DoDataExchange(BOOL bSaveAndValidate = FALSE, UINT nCtlID = (UINT)-1);
        BOOL OnInitDialog(HWND, LPARAM param);
        void OnDestroy();

        BEGIN_MSG_MAP_EX(__TemplateDialog)
            CHAIN_MSG_MAP(Super)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()
    };
}
