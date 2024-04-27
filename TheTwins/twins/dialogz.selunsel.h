#pragma once

#include "dialogz.common.h"
#include <atlddx.h>
#include <string>
#include <vector>
#include "res/resource.h"

namespace Twins
{
    class SelUnselDialog: CommonDialog
                        , CWinDataExchange<SelUnselDialog>
    {
    private:
        typedef CommonDialog Super;
        typedef CWinDataExchange<SelUnselDialog> WinDdx;
        typedef std::vector<std::wstring> StringVec;

    public:
        SelUnselDialog(PCWSTR caption, PCWSTR rx, StringVec const& history);
        ~SelUnselDialog();

        using Super::DoModal;

        void GetValues(std::wstring& rx, bool& selectFiles, bool& selectDirs, bool& ignoreCase) const;

    private:
        friend Super;
        friend WinDdx;

        CString rx_;
        CComboBox rxBox_;
        CListBox rxList_;
        bool ignoreCase_;
        bool selectFiles_;
        bool selectDirs_;
        BOOL closeAfterSelectTemplate_;
        StringVec const& history_;

        CFont MyFixedFont;

        BOOL DoDataExchange(BOOL bSaveAndValidate = FALSE, UINT nCtlID = (UINT)-1);
        BOOL OnInitDialog(HWND, LPARAM param);
        void OnDestroy();
        void OnTemplate(UINT, int, HWND);

        BEGIN_MSG_MAP_EX(SelUnselDialog)
            COMMAND_HANDLER_EX(IDC_LSTEMPLATES, LBN_DBLCLK, OnTemplate)
            CHAIN_MSG_MAP(Super)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()

        void InitTemplates();
        void InitHistory();
    };
}
