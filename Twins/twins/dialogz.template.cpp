#include "stdafx.h"
#include "dialogz.???.h"
#include "locale.helper.h"
#include "res/resource.h"

namespace Twins
{
    __TemplateDialog::__TemplateDialog(PCTSTR caption)
        : Super(IDD_????, caption)
    {
    }

    __TemplateDialog::~__TemplateDialog()
    {
    }

    BOOL __TemplateDialog::DoDataExchange(BOOL bSaveAndValidate /*= FALSE*/, UINT nCtlID /*= (UINT)-1*/)
    {
        //DDX_TEXT(IDC_LINEDESC, desc_);
        //DDX_TEXT(IDC_LINEEDIT, text_);
        return TRUE;
    }

    BOOL __TemplateDialog::OnInitDialog(HWND, LPARAM param)
    {
        DoDataExchange(DDX_LOAD);

        AddButton(L"Ok", ResultOk);
        AddButton(_LS("Отмена"), ResultCancel);

        return TRUE;
    }

    void __TemplateDialog::OnDestroy()
    {
        DoDataExchange(DDX_SAVE);
        SetMsgHandled(FALSE);
    }
}
