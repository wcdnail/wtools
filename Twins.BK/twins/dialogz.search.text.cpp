#include "stdafx.h"
#include "dialogz.search.text.h"
#include <twins.langs/twins.lang.strings.h>
#include <wtl.controls.helpers.h>
#include "res/resource.h"

namespace Twins
{
    HistoryDeque SearchTextDialog::History;

    SearchTextDialog::SearchTextDialog(PCWSTR initialLine, PCWSTR caption)
        : Super(IDD_SEARCHTEXT, caption)
        , Text(initialLine)
        , TextCombo()
        , IsRegexp(FALSE)
        , IsCasesens(TRUE)
        , IsWholewords(FALSE)
        , IsBackward(FALSE)
        , IsInhex(FALSE)
    {}

    SearchTextDialog::~SearchTextDialog()
    {}

    BOOL SearchTextDialog::DoDataExchange(BOOL bSaveAndValidate /*= FALSE*/, UINT nCtlID /*= (UINT)-1*/)
    {
        DDX_CONTROL_HANDLE(IDC_LINEEDIT, TextCombo);
        DDX_CHECK(IDC_CK_REGEXP, IsRegexp);
        DDX_CHECK(IDC_CK_CASESENS, IsCasesens);
        DDX_CHECK(IDC_CK_WHOLEWORDS, IsWholewords);
        DDX_CHECK(IDC_CK_BACKWARD, IsBackward);
        DDX_CHECK(IDC_CK_INHEX, IsInhex);

        return TRUE;
    }

    BOOL SearchTextDialog::OnInitDialog(HWND, LPARAM param)
    {
        DoDataExchange(DDX_LOAD);

        History.LoadTo(TextCombo);
        Helpers::SetDefaultValue(TextCombo, 0);

        AddButton(_LS(StrId_Ok), IDOK);
        AddButton(_LS(StrId_Cancel), IDCANCEL);

        return TRUE;
    }

    void SearchTextDialog::OnDestroy()
    {
        DoDataExchange(DDX_SAVE);
        SetMsgHandled(FALSE);
    }

    int SearchTextDialog::OnCommandById(int id)
    {
        if (IDOK == id)
        {
            CStringW temp;
            TextCombo.GetWindowText(temp);
            Text = temp;

            History.Push(Text);
        }

        return CommonDialog::OnCommandById(id);
    }
}
