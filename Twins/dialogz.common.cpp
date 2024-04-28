#include "stdafx.h"
#include "dialogz.common.h"
#include "iconoz.h"
#include <twins.langs/twins.lang.strings.h>
#include <dh.tracing.h>

namespace Twins
{
    void LocalizeControl(HWND hwnd, unsigned strId)
    {
        SetWindowText(hwnd, _LTS(strId));
    }

    void LocalizeDlgControl(HWND owenr, UINT id, unsigned strId)
    {
        LocalizeControl(::GetDlgItem(owenr, id), strId);
    }

    CommonDialog::CommonDialog(UINT idd, PCTSTR caption)
        : Super()
        , IDD(idd)
        , Caption(caption)
        , Buttons(TabBar::AutoWidth | TabBar::ReleaseSelection)
        , DefaultId(ResultOk)
    {}

    CommonDialog::~CommonDialog()
    {}

    int CommonDialog::OnCommandById(int id)
    {
        switch (id)
        {
        case ResultOk:
            EndDialog(DefaultId);
            break;

        case ResultCancel:
        case ResultYes:
        case ResultNo:
        case ResultStart:
        case ResultYesToAll:
        case ResultStartInIdle:
            EndDialog(id);
            break;
        }

        return id;
    }

    void CommonDialog::OnCommand(UINT code, int id, HWND control)
    {
        OnCommandById(id);

        //if (IDOK == id)
        //    EndDialog(DefaultId);

        //else if (IDCANCEL == id)
        //    EndDialog(id);

        //else
        //    setWMHandled(FALSE);
    }

    int CommonDialog::GetButtonsCy() const
    {
        return 22;
    }

    CRect CommonDialog::GetRect() const
    {
        CRect rc;
        ::GetClientRect(m_hWnd, rc);
        rc.DeflateRect(9, 7);
        return rc;
    }

    BOOL CommonDialog::OnInitDialog(HWND, LPARAM params)
    {
        SetIcon(Icons.Main, TRUE);
        SetIcon(Icons.Main, FALSE);

        SetWindowText(Caption);

        CRect rc = GetRect();

        CRect rcButtons = rc;
        rcButtons.top = rcButtons.bottom - GetButtonsCy();
        Buttons.Create(m_hWnd, rcButtons, NULL, WS_CHILD | WS_VISIBLE | SS_SUNKEN | WS_TABSTOP, 0, ID_BUTTONS);
        Buttons.OnClick() = std::bind(&CommonDialog::OnCommandById, this, std::placeholders::_1);
        Buttons.SetIconSize(16, 16);

        CenterWindow(GetParent());

        SetFocus();
        SetMsgHandled(FALSE); 
        return TRUE;
    }

    void CommonDialog::AddButton(PCWSTR caption, int id /*= -1*/, bool defaultId /*= false */, HICON icon /*= NULL*/)
    {
        Buttons.Add(caption, DT_CENTER | DT_VCENTER | DT_SINGLELINE, icon, id);

        if (defaultId && (-1 != id))
            DefaultId = id;
    }

    void CommonDialog::SetCaption(PCWSTR caption)
    {
        Caption = caption;
        SetWindowText(Caption);
    }

    PCWSTR CommonDialog::GetCaption() const
    {
        return Caption;        
    }

    void CommonDialog::OnDestroy()
    {
        GetWindowText(Caption);
        Buttons.DestroyWindow();
        SetMsgHandled(FALSE);
    }
}
