#pragma once

#include "dialogz.common.h"
#include <atlddx.h>

namespace Twins
{
    class NameDialog: CommonDialog
                    , CWinDataExchange<NameDialog>
    {
    private:
        typedef CommonDialog Super;
        typedef CWinDataExchange<NameDialog> WinDdx;

    public:
        NameDialog(PCWSTR initialLine, PCWSTR description, PCWSTR caption);
        ~NameDialog();

        using Super::DoModal;

        PCWSTR GetText() const;

    private:
        friend WinDdx;

        CString text_;
        CString desc_;
        
        BOOL DoDataExchange(BOOL bSaveAndValidate = FALSE, UINT nCtlID = (UINT)-1);
        BOOL OnInitDialog(HWND, LPARAM param);
        void OnDestroy();

        BEGIN_MSG_MAP_EX(NameDialog)
            CHAIN_MSG_MAP(Super)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()
    };
}
