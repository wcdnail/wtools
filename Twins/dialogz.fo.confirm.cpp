#include "stdafx.h"
#if 0
#include "dialogz.fo.confirm.h"
#include "file.list.h"
#include <locale.helper.h>
#include "res/resource.h"

namespace Twins
{
    ConfirmDialog::ConfirmDialog(PCTSTR caption, PCTSTR question)
        : Super(IDD_FOCONFIRM, caption)
        , question_(question)
        , list_()
    {
    }

    ConfirmDialog::~ConfirmDialog()
    {
    }

    void ConfirmDialog::GetListFrom(Fl::List const& entries)
    {
        Fl::String temp = L"`";
        temp += entries.ToString(L"`\n", 5);
        list_ = temp.c_str();
    }

    BOOL ConfirmDialog::OnInitDialog(HWND, LPARAM param)
    {
        AddButton(_LS("Да"), ResultYes, true);
        AddButton(_LS("Да, все"), ResultYesToAll);
        AddButton(_LS("Нет"), ResultNo);

        GetDlgItem(IDC_PROMPT).SetWindowText(question_);
        GetDlgItem(IDC_TEXT).SetWindowText(list_);

        CIcon icon(::LoadIcon(NULL, IDI_EXCLAMATION));
        ::SendMessage(GetDlgItem(IDC_SI), STM_SETICON, (WPARAM)icon.m_hIcon, 0);

        return TRUE;
    }
}
#endif
