#include "stdafx.h"
#include "panel.view.edit.h"

namespace Twins
{
    static void Dmy_EditDone(int, CString const&) {}

    LabelEdit::LabelEdit() 
        : Super()
        , EditDone(Dmy_EditDone)
        , Index(-1)
    {}

    LabelEdit::~LabelEdit() 
    {}

    void LabelEdit::Show(CRect const& rc, int index, CString const& label, bool dontSplitByDot)
    {
        Index = index;

        MoveWindow(rc);
        SetWindowText(label);
        ShowWindow(SW_SHOW);
        SetFocus();

        int dp = (dontSplitByDot ? -1 : label.ReverseFind(_T('.')));
        SetSel(0, dp);
    }

    void LabelEdit::Hide()
    {
        if (IsWindowVisible())
        {
            ShowWindow(SW_HIDE);
            MoveWindow(&rcDefault);
            GetParent().SetFocus();
        }
    }

    void LabelEdit::OnKillFocus(HWND)
    {
        Hide();
        SetMsgHandled(FALSE);
    }

    LabelEdit::DoneHandler& LabelEdit::OnEditDone()
    {
        return EditDone;
    }

    void LabelEdit::OnKeyDown(UINT code, UINT, UINT flags)
    {
        if (VK_ESCAPE == code)
        {
            Hide();
        }
        else if (VK_RETURN == code)
        {
            Hide();

            if (EditDone)
            {
                CString temp;
                GetWindowText(temp);
                EditDone(Index, temp);
            }
        }
        else
        {
            SetMsgHandled(FALSE);
        }
    }
}
