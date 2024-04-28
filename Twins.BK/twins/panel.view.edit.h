#pragma once

#include <atlwin.h>
#include <atlctrls.h>
#include <atlstr.h>

namespace Twins
{
    class LabelEdit: CWindowImpl<LabelEdit, CEdit>
    {
    private:
        typedef CWindowImpl<LabelEdit, CEdit> Super;

    public:
        typedef std::function<void(int, CString const&)> DoneHandler;

        LabelEdit();
        ~LabelEdit();

        using Super::Create;
        using Super::SetFont;
        using Super::IsWindowVisible;

        void Show(CRect const& rc, int index, CString const& label, bool dontSplitByDot = false);
        void Hide();

        DoneHandler& OnEditDone();

    private:
        friend Super;

        DoneHandler EditDone;
        int Index;

        void OnKillFocus(HWND);
        void OnKeyDown(UINT code, UINT, UINT flags);

        BEGIN_MSG_MAP_EX(LabelEdit)
            MSG_WM_KILLFOCUS(OnKillFocus)
            MSG_WM_KEYDOWN(OnKeyDown)
        END_MSG_MAP()
    };
}
