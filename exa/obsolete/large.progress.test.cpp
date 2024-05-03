#include "stdafx.h"
#include <large.progress.h>
#include <atlctrls.h>
#include "res/resource.h"

typedef Cf::LargeProgress<unsigned __int64> CProgress64;

class CDialog1: public CDialogImpl<CDialog1>
{
public:
    enum { IDD = IDD_DIALOG1 };

    CDialog1() 
        : ProgressCtl()
        , Progress(ProgressCtl)
    {}

    ~CDialog1() {}

    CEdit Edit1;
    CEdit Edit2;
    CStatic Hint;
    CProgressBarCtrl ProgressCtl;
    CTrackBarCtrl Track;
    CProgress64 Progress;

    void UpdateRange(int Minimum = 0, int Maximum = 1000)
    {
        SetDlgItemInt(IDC_EDIT1, Minimum, TRUE);
        SetDlgItemInt(IDC_EDIT2, Maximum, TRUE);

        ProgressCtl.SetRange(Minimum, Maximum);
        Track.SetRange(Minimum, Maximum);
    }

    BOOL OnInitDialog(HWND, LPARAM)
    {
        Hint.Attach(GetDlgItem(IDC_STA1));
        ProgressCtl.Attach(GetDlgItem(IDC_PROGRESS1));
        Track.Attach(GetDlgItem(IDC_SLIDER1));

        UpdateRange(0, 1000);
        Progress.Range(0, 123);

        CenterWindow(GetParent());
        return TRUE;
    }

    void OnCommand(UINT code, int id, HWND)
    {
        if (IDCANCEL == id)
            EndDialog(id);
    }

    LRESULT OnNotifySlider(LPNMHDR pnmh)
    {
        int p = Track.GetPos();
        Progress = p;

        CString text;
        text.Format(_T("%d - %I64u"), p, Progress.Get());
        Hint.SetWindowText(text);
        return 0;
    }

    BEGIN_MSG_MAP_EX(CDialog1)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_COMMAND(OnCommand)
        NOTIFY_ID_HANDLER_EX(IDC_SLIDER1, OnNotifySlider)
    END_MSG_MAP()
};

void LargeProgressTest()
{
    CDialog1 dlg;
    dlg.DoModal();
}