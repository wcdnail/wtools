#pragma once

#include "dialogz.common.h"
#include "file.operation.types.h"
#include <generic.div.h>
#include <large.progress.h>
#include <thread>
#include <mutex>

namespace Twins
{
    class OperationBase;

    class FileProgressDialog: CommonDialog
    {
    private:
        typedef CommonDialog Super;
        typedef std::function<void(void)> WorkerFunction;

    public:
        typedef Cf::LargeProgress<FileSize> FileProgress;
        enum  { WmShowFileReplaceDialog = WM_USER + 1, };

        FileProgressDialog(PCWSTR caption);
        ~FileProgressDialog();

        void Close(int result);
        std::thread& GetThread();
        void Start(OperationBase const& operation, FileList files, SpecFlags flags, OperationResult& error, HWND parent = NULL);
        void SetSource(std::wstring const& filename);
        void SetDest(std::wstring const& filename);
        volatile BOOL* CanceledPtr();
        bool IsCanceled() const;
        void ShowFileReplaceDialog(Fl::Entry const& source, Fl::Entry const& dest, SpecFlags& flags, bool nonguiThread) const;

        FileProgress& Current();
        FileProgress& Total();

        void OnStart(FileSize totalCount, FileSize totalSize);
        void OnStop(FileSize totalCount, FileSize totalSize, FileSize processedCount);
        void OnCurrentProgress(FileSize transfered, FileSize processedSize, FileSize totalSize);
        void OnCurrentProgressDone(FileSize processedCount, FileSize totalCount, FileSize processedSize, FileSize totalSize);

    private:
        CStatic Source;
        CStatic Dest;
        CStatic HCurrent;
        CProgressBarCtrl PCurrent;
        FileProgress FCurrent;
        CStatic HTotal;
        CProgressBarCtrl PTotal;
        FileProgress FTotal;
        CStatic HSize;
        std::thread Worker;
        WorkerFunction WorkerFunctor;
        volatile BOOL Canceled;
        volatile bool Paused;
        CString PauseName;
        mutable std::mutex PauseMx; 
        mutable std::condition_variable PauseCn;

        BOOL OnInitDialog(HWND, LPARAM param);
        void OnCommand(UINT code, int id, HWND);
        LRESULT OnShowFileReplaceDialog(UINT, WPARAM, LPARAM lParam) const;
        void ShowFileReplaceDialog(LPARAM lParam) const;
        bool CancelThread(unsigned msec);
        virtual int OnCommandById(int id);
        void TogglePause();

        BEGIN_MSG_MAP_EX(FileProgressDialog)
            CHAIN_MSG_MAP(Super)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_COMMAND(OnCommand)
            MESSAGE_HANDLER_EX(WmShowFileReplaceDialog, OnShowFileReplaceDialog)
        END_MSG_MAP()
    };

    inline FileProgressDialog::FileProgress& FileProgressDialog::Current() { return FCurrent; }
    inline FileProgressDialog::FileProgress& FileProgressDialog::Total() { return FTotal; }
    inline volatile BOOL* FileProgressDialog::CanceledPtr() { return &Canceled; }
    inline void FileProgressDialog::Close(int result) { ::EndDialog(m_hWnd, result); }
    inline std::thread& FileProgressDialog::GetThread() { return Worker; }
    inline void FileProgressDialog::SetSource(std::wstring const& filename) { Source.SetWindowText(filename.c_str()); }
    inline void FileProgressDialog::SetDest(std::wstring const& filename) { Dest.SetWindowText(filename.c_str()); }
}
