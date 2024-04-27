#include "stdafx.h"
#include "dialogz.file.additional.h"
#include <twins.lang.strings.h>
#include <atltypes.h>
#include "res/resource.h"

namespace Twins
{
    FileAdditionalDialog::~FileAdditionalDialog()
    {
    }

    FileAdditionalDialog::FileAdditionalDialog(SpecFlags& flags)
        : Super()
        , IDD(IDD_FO_ADDITIONAL)
        , Flags(flags)
    {
    }

    void LocalizeControl(HWND owenr, UINT id);

    void FileAdditionalDialog::SetCheck(int id, unsigned mask)
    {
        Check[id].SetCheck(Flags.Check(mask) ? TRUE : FALSE);
    }

    void FileAdditionalDialog::GetCheck(int id, unsigned mask)
    {
        Flags.Add(Check[id].GetCheck() ? mask : 0);
    }

    BOOL FileAdditionalDialog::OnInitDialog(HWND, LPARAM param)
    {
        LocalizeControl(m_hWnd, IDC_FO_RS_C10);

        for (int i=0; i<_countof(Check); i++)
        {
            UINT id = IDC_FO_R_C1 + i;
            Check[i].Attach(GetDlgItem(id));
            LocalizeControl(m_hWnd, id);
        }

        SetCheck(0, SpecFlags::SaveNtfsAttributes);
        SetCheck(1, SpecFlags::SkipError);
        SetCheck(2, SpecFlags::ReplaceDeleteHiddenSystem);
        SetCheck(3, SpecFlags::AskUser);
        SetCheck(4, SpecFlags::Replace);
        SetCheck(5, SpecFlags::Skip);
        SetCheck(6, SpecFlags::ReplaceOlder);
        SetCheck(7, SpecFlags::AutoRenameSource);
        SetCheck(8, SpecFlags::AutoRenameDest);

        return TRUE;
    }

    void FileAdditionalDialog::OnDestroy()
    {
        SetMsgHandled(FALSE);

        GetCheck(0, SpecFlags::SaveNtfsAttributes);
        GetCheck(1, SpecFlags::SkipError);
        GetCheck(2, SpecFlags::ReplaceDeleteHiddenSystem);
        GetCheck(3, SpecFlags::AskUser);
        GetCheck(4, SpecFlags::Replace);
        GetCheck(5, SpecFlags::Skip);
        GetCheck(6, SpecFlags::ReplaceOlder);
        GetCheck(7, SpecFlags::AutoRenameSource);
        GetCheck(8, SpecFlags::AutoRenameDest);
    }
}
