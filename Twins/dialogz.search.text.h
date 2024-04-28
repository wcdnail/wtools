#pragma once

#include "dialogz.common.h"
#include <history.deque.h>
#include <atlddx.h>

namespace Twins
{
    class SearchTextDialog: CommonDialog
                          , CWinDataExchange<SearchTextDialog>
    {
    private:
        typedef CommonDialog Super;
        typedef CWinDataExchange<SearchTextDialog> WinDdx;

    public:
        SearchTextDialog(PCWSTR initialLine, PCWSTR caption);
        ~SearchTextDialog();

        using Super::DoModal;

        PCWSTR GetText() const;

        static HistoryDeque History;

    private:
        friend WinDdx;

        WString Text;
        CComboBox TextCombo;
        BOOL IsRegexp;
        BOOL IsCasesens;
        BOOL IsWholewords;
        BOOL IsBackward;
        BOOL IsInhex;

        virtual int OnCommandById(int id);
        BOOL DoDataExchange(BOOL bSaveAndValidate = FALSE, UINT nCtlID = (UINT)-1);
        BOOL OnInitDialog(HWND, LPARAM param);
        void OnDestroy();

        BEGIN_MSG_MAP_EX(SearchTextDialog)
            CHAIN_MSG_MAP(Super)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()
    };

    inline PCWSTR SearchTextDialog::GetText() const { return Text.c_str(); }
}
