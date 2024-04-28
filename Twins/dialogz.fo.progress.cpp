#include "stdafx.h"
#if 0
#include "dialogz.fo.progress.h"
#include "theme.h"
#include <locale.helper.h>
#include "res/resource.h"

namespace Twins
{
    ProgressDialog::ProgressDialog(PCWSTR caption)
        : Super(IDD_FOPROGRESS, caption)
        , source_()
        , dest_()
        , current_()
        , total_()
        , worker_()
        , function_()
        , error_(ERROR_CALL_NOT_IMPLEMENTED)
        , cpd_(1.)
        , tpd_(1.)
    {
    }

    ProgressDialog::~ProgressDialog()
    {
    }

    BOOL ProgressDialog::OnInitDialog(HWND, LPARAM param)
    {
        source_.Attach(GetDlgItem(IDC_SHINT));
        dest_.Attach(GetDlgItem(IDC_DHINT));
        current_.Attach(GetDlgItem(IDC_CPROGRESS));
        total_.Attach(GetDlgItem(IDC_TPROGRESS));

        AddButton(_LS("Пауза"));
        AddButton(_LS("В фоне"), ResultStartInIdle);
        AddButton(_LS("Отмена"), ResultCancel);

        if (function_)
            worker_ = boost::thread(function_);

        return TRUE;
    }

    void ProgressDialog::Close(int result) 
    {
        ::EndDialog(m_hWnd, result);
    }

    void ProgressDialog::SetErrorCode(HRESULT hr) 
    {
        error_ = hr; 
    }

    HRESULT ProgressDialog::GetErrorCode() const 
    {
        return error_; 
    }

    void ProgressDialog::SetThreadFn(boost::function<void(void)> const& function) 
    {
        function_ = function; 
    }

    boost::thread& ProgressDialog::GetThread() 
    {
        return worker_; 
    }

    void ProgressDialog::SetHints(CString const& source, CString const& dest)
    {
        source_.SetWindowText(source);
        dest_.SetWindowText(dest);
    }

    void ProgressDialog::SetRangeFor(CProgressBarCtrl& ctl, int imin, int imax)
    {
        ::SendMessage(ctl, PBM_SETRANGE32, imin, imax);
    }

    void ProgressDialog::SetPosFor(CProgressBarCtrl& ctl, int pos)
    {
        ::SendMessage(ctl, PBM_SETPOS, pos, 0);
    }

}
#endif // 0
