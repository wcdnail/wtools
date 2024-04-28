#pragma once

#if 0
#include "dialogz.common.h"
#include <atlddx.h>

namespace Fl
{
    class List;
}

namespace Twins
{
    class ConfirmDialog: CommonDialog
    {
    private:
        typedef CommonDialog Super;

    public:
        ConfirmDialog(PCTSTR caption, PCTSTR question);
        ~ConfirmDialog();

        using Super::DoModal;

        void GetListFrom(Fl::List const& entries);

    private:
        CString question_;
        CString list_;

        BOOL OnInitDialog(HWND, LPARAM param);

        BEGIN_MSG_MAP_EX(ConfirmDialog)
            CHAIN_MSG_MAP(Super)
            MSG_WM_INITDIALOG(OnInitDialog)
        END_MSG_MAP()
    };
}
#endif
