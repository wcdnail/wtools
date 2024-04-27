#include "stdafx.h"
#include "dialogz.file.prompt.h"
#include "file.list.h"
#include "tab.bar.item.h"
#include <twins.lang.strings.h>
#include <atltypes.h>

namespace Twins
{
    enum
    {
        AdditionalButtonId = CommonDialog::ResultLast + 1,
    };

    PromptDialog::~PromptDialog()
    {
    }

    PromptDialog::PromptDialog(SpecFlags& flags)
        : Super(IDD_FO_PROMPT, NULL)
        , Init()
        , Hint()
        , Path()
        , Icon()
        , List()
        , Additional(false)
        , AdditionalName()
        , RCDialog(flags)
    {
    }

    void PromptDialog::Setup(PCWSTR caption, bool additionButton)
    {
        SetCaption(caption);

        AddButton(_LS(StrId_Start), ResultStart, true);
        AddButton(_LS(StrId_Inbackground), ResultStartInIdle);
        AddButton(_LS(StrId_Cancel), ResultCancel);

        if (additionButton)
            AddButton(_LS(StrId_Additionally), AdditionalButtonId);
    }

    BOOL PromptDialog::OnInitDialog(HWND, LPARAM param)
    {
        Hint.Attach(GetDlgItem(IDC_HINT));
        Path.Attach(GetDlgItem(IDC_PATH));
        List.Attach(GetDlgItem(IDC_LIST));
        Icon.Attach(GetDlgItem(IDC_OP_ICON));

        RCDialog.Create(m_hWnd);

        if (Init)
            Init();

        return TRUE;
    }

    void PromptDialog::OnDestroy()
    {
        SetMsgHandled(FALSE);
    }

    void PromptDialog::OnWindowPosChanged(LPWINDOWPOS pos)
    {
        if (0 != (pos->flags & SWP_SHOWWINDOW))
            AdjustControlPositions();
    }

    void PromptDialog::SetHint(PCWSTR format, ...)
    {
        va_list ap;
        va_start(ap, format);
        CString temp;
        temp.FormatV(format, ap);
        va_end(ap);

        Hint.SetWindowText(temp);
    }

    void PromptDialog::AdjustControlPositions()
    {
        struct Conf
        {
            HWND child;
            int offset;
        };

        Conf conf[] = 
        { 
          { Hint, 2 }
        , { Path, 4 }
        , { Icon, 0 } 
        , { List, 2 }
        , { Buttons, 2 }
        };

        int top = -1;

        for (int i=0; i<_countof(conf); i++)
        {
            if (::IsWindowVisible(conf[i].child))
            {
                CRect rc;
                ::GetWindowRect(conf[i].child, rc);
                ScreenToClient(rc);

                if (-1 == top)
                    top = rc.top;

                int cy = rc.Height();
                ::SetWindowPos(conf[i].child, NULL, rc.left, top, rc.Width(), cy, SWP_NOZORDER);

                if (conf[i].offset)
                    top += (cy + conf[i].offset);
            }
        }

        top += ::GetSystemMetrics(SM_CYCAPTION)
             + ::GetSystemMetrics(SM_CYBORDER)
             + ::GetSystemMetrics(SM_CYDLGFRAME)
             ;

        CRect rc;
        ::GetWindowRect(m_hWnd, rc);
        ::SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.Width(), top + 8, SWP_NOZORDER);
    }

    void PromptDialog::ToggleAdditional()
    {
        Additional = !Additional;

        if (Additional)
        {
            AdditionalName = Buttons.GetItemById(AdditionalButtonId).name;
            Buttons.GetItemById(AdditionalButtonId).name = _LS(StrId_Hide);
        }
        else
            Buttons.GetItemById(AdditionalButtonId).name = AdditionalName;

        Buttons.Invalidate(FALSE);

        CRect rc1;
        RCDialog.GetWindowRect(rc1);
        int cy = rc1.Height();

        CRect rc;
        ::GetWindowRect(m_hWnd, rc);

        rc1 = rc;
        ScreenToClient(rc1);

        RCDialog.SetWindowPos(NULL, rc1.left, rc1.bottom, rc1.Width(), cy, SWP_NOZORDER);
        RCDialog.ShowWindow(Additional ? SW_SHOW : SW_HIDE);

        rc.bottom += Additional ? cy : -cy;
        ::SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER);
    }

    int PromptDialog::OnCommandById(int id)
    {
        if (AdditionalButtonId == id)
        {
            ToggleAdditional();
            return 0;
        }

        return Super::OnCommandById(id);
    }
}
