#include "stdafx.h"
#include "dialogz.file.replace.h"
#include "file.entry.h"
#include <twins.langs/twins.lang.strings.h>
#include <string.utils.human.size.h>
#include <generic.div.h>
#include <atltime.h>
#include <boost/bind.hpp>
#include "res/resource.h"

namespace Twins
{
    enum 
    {
        AdditionalButtonId = -32,
        CompareCommandId,
    };

    FileReplaceDialog::~FileReplaceDialog()
    {}

    FileReplaceDialog::FileReplaceDialog()
        : Super(IDD_FO_REPLACE, _LS(StrId_Replacefiles))
        , TopButtons(TabBar::AutoWidth | TabBar::ReleaseSelection)
        , MidButtons(TabBar::AutoWidth | TabBar::ReleaseSelection)
        , Source()
        , SourceSizeTime()
        , SourceIcon()
        , Dest()
        , DestSizeTime()
        , DestIcon()
        , Result(SpecFlags::None)
    {}

    Cf::PopupMenu FileReplaceDialog::Popup(0);

    BOOL FileReplaceDialog::OnInitDialog(HWND hwnd, LPARAM param)
    {
        ::SetWindowText(GetDlgItem(IDC_SOURCE_NAME), Source);
        ::SetWindowText(GetDlgItem(IDC_SOURCE_SIZE_TIME), SourceSizeTime);
        ::SendMessage(GetDlgItem(IDC_SOURCE_ICO), STM_SETICON, (WPARAM)SourceIcon.m_hIcon, 0);

        ::SetWindowText(GetDlgItem(IDC_DEST_NAME), Dest);
        ::SetWindowText(GetDlgItem(IDC_DEST_SIZE_TIME), DestSizeTime);
        ::SendMessage(GetDlgItem(IDC_DEST_ICO), STM_SETICON, (WPARAM)DestIcon.m_hIcon, 0);

        CRect rc = GetRect();
        int bh = GetButtonsCy();

        rc.top = rc.bottom - (bh * 3 + 4);
        rc.bottom = rc.top + bh;
        TopButtons.Create(m_hWnd, rc, NULL, WS_CHILD | WS_VISIBLE | SS_SUNKEN | WS_TABSTOP | WS_GROUP, 0, ID_BUTTONS2);
        TopButtons.SetWindowPos(GetDlgItem(IDC_DEST_GB), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW);
        TopButtons.OnClick() = boost::bind(&FileReplaceDialog::OnCommandById, this, _1);

        rc.top = rc.bottom + 2;
        rc.bottom = rc.top + bh;
        MidButtons.Create(m_hWnd, rc, NULL, WS_CHILD | WS_VISIBLE | SS_SUNKEN | WS_TABSTOP | WS_GROUP, 0, ID_BUTTONS1);
        MidButtons.SetWindowPos(TopButtons, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW);
        MidButtons.OnClick() = boost::bind(&FileReplaceDialog::OnCommandById, this, _1);

        DefaultId = SpecFlags::Replace;

        TopButtons.Add(SpecFlags::Replace, _LS(StrId_Yes), -1, NULL, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        TopButtons.Add(SpecFlags::ReplaceAll, _LS(StrId_Yesall), -1, NULL, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        TopButtons.Add(SpecFlags::Skip, _LS(StrId_Skip), -1, NULL, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        MidButtons.Add(SpecFlags::Cancel, _LS(StrId_Cancel), -1, NULL, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        MidButtons.Add(SpecFlags::ReplaceOlder, _LS(StrId_Replaceolder), -1, NULL, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        MidButtons.Add(SpecFlags::SkipAll, _LS(StrId_Skipall), -1, NULL, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        AddButton(_LS(StrId_Rename), SpecFlags::Rename);
        AddButton(_LS(StrId_Appendwrite), SpecFlags::Append);
        AddButton(_LS(StrId_Additionally), AdditionalButtonId);
        Buttons.SetWindowPos(MidButtons, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW);

      //LocalizeControl(m_hWnd);
      //LocalizeControl(m_hWnd, IDC_DEST_GB);
      //LocalizeControl(m_hWnd, IDC_SOURCE_GB);
               
        if (!Popup.IsValid())
        {
            Popup.CreatePopup();

            Popup.Add(_LS(StrId_Compare), CompareCommandId);
            Popup.Add(_LS(StrId_Renameexistingfile), SpecFlags::Rename);
            Popup.Add(_LS(StrId_Autorenamesourcefiles), SpecFlags::AutoRenameSource);
            Popup.Add(_LS(StrId_Autorenameexistancefiles), SpecFlags::AutoRenameDest);
            Popup.AddSeparator();
            Popup.Add(_LS(StrId_Renameallwithsamedateandolder), SpecFlags::ReplaceAllOlder);
            Popup.Add(_LS(StrId_Copyallfileswithgreatersize), SpecFlags::ReplaceAllBiggest);
            Popup.Add(_LS(StrId_Copyallfileswithsmallersize), SpecFlags::ReplaceAllSmallest);
        }

        return TRUE;
    }

    static void InitValues(CString& string, CString& string2, CIcon& icon, Fl::Entry const& file)
    {
        string = file.GetPath().c_str();

        string2.Format(_LS(StrId_Sbytess), Str::HumanSize(file.GetSize()).c_str()
            , CTime(CFileTime(file.GetTime())).Format(_T("%x %X")));

        icon.Attach(file.LoadShellIcon(SHGFI_LARGEICON | SHGFI_ADDOVERLAYS));
    }

    unsigned FileReplaceDialog::Ask(Fl::Entry const& source, Fl::Entry const& dest, SpecFlags flags, HWND parent /*= NULL*/)
    {
        InitValues(Source, SourceSizeTime, SourceIcon, source);
        InitValues(Dest, DestSizeTime, DestIcon, dest);

        Result = flags.Value;
        if (flags.Check(SpecFlags::AskUser))
            Result |= (unsigned)DoModal(parent);

        return Result;
    }

    int FileReplaceDialog::OnCommandById(int id)
    {
        if (AdditionalButtonId == id)
        {
            CRect rc;
            Buttons.GetWindowRect(rc);

            CPoint p(rc.right - Cf::SafeDiv(rc.Width(), Buttons.Count()), rc.bottom);
            int mr = Popup.Show(p, m_hWnd, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD);

            if (!mr)
                return -1;

            if (CompareCommandId == mr)
            {
#pragma message(_TODO("Compare files..."))
                return -2;
            }

            id = mr;
        }

        Result |= id;
        return EndDialog(Result);
    }
}
