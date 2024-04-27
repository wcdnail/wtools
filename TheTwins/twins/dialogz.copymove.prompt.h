#pragma once

#if 0
#include "dialogz.common.h"
#include <atlddx.h>

namespace Twins
{
    class CopyMovePromptDialog: CommonDialog
                              , CWinDataExchange<CopyMovePromptDialog>
    {
    private:
        typedef CommonDialog Super;
        typedef CWinDataExchange<CopyMovePromptDialog> WinDdx;

    public:
        CopyMovePromptDialog(PCWSTR caption, PCWSTR prompt, PCWSTR targetPath);
        ~CopyMovePromptDialog();

        using Super::DoModal;

    private:
        friend WinDdx;

        CString prompt_;
        CString target_;

        BOOL DoDataExchange(BOOL bSaveAndValidate = FALSE, UINT nCtlID = (UINT)-1);
        BOOL OnInitDialog(HWND, LPARAM param);
        void OnDestroy();

        BEGIN_MSG_MAP_EX(CopyMovePromptDialog)
            CHAIN_MSG_MAP(Super)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()
    };
}
#endif
