#pragma once

#include <atlwin.h>
#include <atlddx.h>
#include <atlctrls.h>
#include "res/resource.h"

namespace Twins
{
    namespace ConfigPages
    {
        class PageTemplate: public ATL::CDialogImpl<PageTemplate>
                          , CWinDataExchange<PageTemplate>
        {
        public:
            enum { IDD = IDD_??? };

            PageTemplate() {}
            ~PageTemplate() {}

        private:
            BOOL DoDataExchange(BOOL bSaveAndValidate = FALSE, UINT nCtlID = (UINT)-1)
            {
                return TRUE;
            }

            BOOL OnInitDialog(HWND focused, LPARAM param)
            {
                DoDataExchange(DDX_LOAD);
                return TRUE;
            }

            BEGIN_MSG_MAP_EX(PageTemplate)
                MSG_WM_INITDIALOG(OnInitDialog)
            END_MSG_MAP()
        };
    }
}
