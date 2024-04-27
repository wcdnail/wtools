#include "stdafx.h"
#include "dialogz.name.h"
#include <twins.lang.strings.h>
#include "res/resource.h"

namespace Twins
{
    NameDialog::NameDialog(PCWSTR initialLine, PCWSTR description, PCWSTR caption)
        : Super(IDD_NAMEEDIT, caption)
        , text_(initialLine)
        , desc_(description)
    {}

    NameDialog::~NameDialog()
    {}

    BOOL NameDialog::DoDataExchange(BOOL bSaveAndValidate /*= FALSE*/, UINT nCtlID /*= (UINT)-1*/)
    {
        DDX_TEXT(IDC_LINEDESC, desc_);
        DDX_TEXT(IDC_LINEEDIT, text_);
        return TRUE;
    }

    BOOL NameDialog::OnInitDialog(HWND, LPARAM param)
    {
        DoDataExchange(DDX_LOAD);

        AddButton(_LS(StrId_Ok), IDOK);
        AddButton(_LS(StrId_Cancel), IDCANCEL);

        return TRUE;
    }

    void NameDialog::OnDestroy()
    {
        DoDataExchange(DDX_SAVE);
        SetMsgHandled(FALSE);
    }

    PCWSTR NameDialog::GetText() const
    {
        return text_;
    }
}
