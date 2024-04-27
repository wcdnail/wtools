#include "stdafx.h"
#include "dialogz.file.progress.h"
#include "dialogz.file.replace.h"
#include "file.operation.base.h"
#include "tab.bar.item.h"
#include "res/resource.h"
#include "brute_cast.h"
#include "file.list.h"
#include <twins.langs/twins.lang.strings.h>
#include <string.utils.human.size.h>

namespace Twins
{
    enum { PauseButtonId = -2 };

    FileProgressDialog::FileProgressDialog(PCWSTR caption)
        : Super(IDD_FO_PROGRESS, caption)
        , Source()
        , Dest()
        , HCurrent()
        , PCurrent()
        , FCurrent(PCurrent)
        , HTotal()
        , PTotal()
        , FTotal(PTotal)
        , HSize()
        , Worker()
        , WorkerFunctor()
        , Canceled(FALSE)
        , Paused(false)
        , PauseName()
        , PauseMx() 
        , PauseCn()
    {}

    FileProgressDialog::~FileProgressDialog()
    {
        CancelThread(10);
    }

    bool FileProgressDialog::CancelThread(unsigned msec)
    {
        if (Paused) {
            PauseCn.notify_all();
            Paused = false;
        }
        if (Worker.joinable()) {
            Canceled = TRUE;
            Worker.join();
        }
        return true;
    }

    BOOL FileProgressDialog::OnInitDialog(HWND hwnd, LPARAM param)
    {
        Source.Attach(GetDlgItem(IDC_SHINT));
        Dest.Attach(GetDlgItem(IDC_DHINT));
        HCurrent.Attach(GetDlgItem(IDC_HCURRENT));
        PCurrent.Attach(GetDlgItem(IDC_CPROGRESS));
        PTotal.Attach(GetDlgItem(IDC_TPROGRESS));
        HTotal.Attach(GetDlgItem(IDC_HTOTAL));
        HSize.Attach(GetDlgItem(IDC_HSIZE));

        AddButton(_LS(StrId_Pause), PauseButtonId);
        AddButton(_LS(StrId_Inbackground), ResultStartInIdle);
        AddButton(_LS(StrId_Cancel), ResultCancel);

        HCurrent.SetWindowText(_T(""));
        PCurrent.SetRange(0, 1000);
        PTotal.SetRange(0, 1000);
        HTotal.SetWindowText(_T(""));
        HSize.SetWindowText(_T(""));

        if (WorkerFunctor) {
            Worker = std::thread(WorkerFunctor);
        }
        return TRUE;
    }

    void FileProgressDialog::OnCommand(UINT code, int id, HWND)
    {
        if (IDCANCEL == id)
            CancelThread(200);
    }

    struct REPLACEDLG_PARAMS
    {
        Fl::Entry const& source;
        Fl::Entry const& dest;
        SpecFlags& flags;

        REPLACEDLG_PARAMS(Fl::Entry const& s, Fl::Entry const& d, SpecFlags& f)
            : source(s)
            , dest(d)
            , flags(f)
        {}
    };

    void FileProgressDialog::ShowFileReplaceDialog(Fl::Entry const& source, Fl::Entry const& dest, SpecFlags& flags, bool nonguiThread) const
    {
        REPLACEDLG_PARAMS params(source, dest, flags);

        if (nonguiThread)
        {
            std::unique_lock lk(PauseMx);
            ::PostMessage(m_hWnd, WmShowFileReplaceDialog, 0, (LPARAM)&params);
            PauseCn.wait(lk);
        }
        else
        {
            ShowFileReplaceDialog((LPARAM)&params);
        }
    }

    void FileProgressDialog::ShowFileReplaceDialog(LPARAM lParam) const
    {
        REPLACEDLG_PARAMS* param = brute_cast<REPLACEDLG_PARAMS*>(lParam);
        FileReplaceDialog dlg;
        param->flags = dlg.Ask(param->source, param->dest, param->flags, m_hWnd);
    }

    LRESULT FileProgressDialog::OnShowFileReplaceDialog(UINT, WPARAM, LPARAM lParam) const
    {
        ShowFileReplaceDialog(lParam);
        PauseCn.notify_one();
        return 0;
    }

    int FileProgressDialog::OnCommandById(int id)
    {
        if (ResultCancel == id)
            CancelThread(200);

        else if (PauseButtonId == id)
            TogglePause();

        return 0;
    }

    void FileProgressDialog::TogglePause()
    {
        Paused = !Paused;

        TabBarItem& button = Buttons.GetItemById(PauseButtonId);
        if (Paused)
        {
            PauseName = button.name;
            button.name = _LS(StrId_Start);
        }
        else
        {
            button.name = PauseName;
            PauseCn.notify_one();
        }
    }

    bool FileProgressDialog::IsCanceled() const 
    {
        if (Paused)
        {
            std::unique_lock lk(PauseMx);
            PauseCn.wait(lk);
        }

        return FALSE != Canceled; 
    }

    void FileProgressDialog::Start(OperationBase const& operation, FileList files, SpecFlags flags, OperationResult& error, HWND parent /*= NULL*/)
    {
        OperationBase::Params p(operation, flags, error, *this);
        WorkerFunctor = std::bind(&OperationBase::Process, &operation, std::cref(files), p);
        DoModal(parent);
    }

    static void SetSizeHint(CStatic& hint, FileSize currentSize, FileSize totalSize)
    {
        CString text;
        text.Format(_LS(StrId_SsMb), Str::HumanSize(Str::InMegs(currentSize)).c_str(), Str::HumanSize(Str::InMegs(totalSize)).c_str());
        ::SetWindowText(hint, text);
    }

    static void SetCurrentHint(CStatic& hint, FileSize v)
    {
        CString text;
        text.Format(_LS(StrId_SMb), Str::HumanSize(Str::InMegs(v)).c_str());
        ::SetWindowText(hint, text);
    }

    static void SetTotalHint(CStatic& hint, FileSize current, FileSize total)
    {
        CString text;
        text.Format(L"%I64u/%I64u", current, total);
        ::SetWindowText(hint, text);
    }

    void FileProgressDialog::OnStart(FileSize totalCount, FileSize totalSize)
    {
        SetCurrentHint(HCurrent, 0);
        SetTotalHint(HTotal, 0, totalCount);
        SetSizeHint(HSize, 0, totalSize);

        Total().Range(0, totalSize);
    }

    void FileProgressDialog::OnStop(FileSize totalCount, FileSize totalSize, FileSize processedCount)
    {
        Total() = totalCount;
    }

    void FileProgressDialog::OnCurrentProgress(FileSize transfered, FileSize processedSize, FileSize totalSize)
    {
        SetCurrentHint(HCurrent, transfered);
        SetSizeHint(HSize, processedSize, totalSize);
        Current() = transfered;
        Total() = processedSize;
    }

    void FileProgressDialog::OnCurrentProgressDone(FileSize processedCount, FileSize totalCount, FileSize processedSize, FileSize totalSize)
    {
        SetTotalHint(HTotal, processedCount, totalCount);
        SetSizeHint(HSize, processedSize, totalSize);
        Total() = processedSize;
    }
}
