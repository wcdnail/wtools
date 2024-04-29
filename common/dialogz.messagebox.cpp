#include "stdafx.h"
#include "dialogz.messagebox.h"
#include "dialogz.basic.h"

namespace CF
{
    DialogResult UserDialog::Ask(HWND parent, WStrView what, WStrView title, unsigned flags)
    {
        BasicDialog dlg(what, title, BasicDialog::GetCompatFlags(flags));
        dlg.ShowModal(parent);
        return dlg.Result();
    }
}
