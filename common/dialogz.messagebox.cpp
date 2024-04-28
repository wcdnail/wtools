#include "stdafx.h"
#include "dialogz.messagebox.h"
#include "dialogz.basic.h"

namespace CF
{
    DialogResult UserDialog::Ask(HWND parent, wchar_t const* what, wchar_t const* title, unsigned flags)
    {
        BasicDialog dlg(what, title, BasicDialog::GetCompatFlags(flags));
        dlg.ShowModal(parent);
        return dlg.Result();
    }
}
