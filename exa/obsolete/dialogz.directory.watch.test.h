#pragma once

#include "directory.watch.h"
#include <atlstr.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <boost/thread.hpp>

namespace Ut
{
    using namespace Twins;

    class DirWatchDialog: CDialogImpl<DirWatchDialog>
    {
    private:
        typedef CDialogImpl<DirWatchDialog> Super;

    public:
        DirWatchDialog();
        ~DirWatchDialog();

        using Super::DoModal;
        using Super::Create;
        using Super::ShowWindow;

        void Log(wchar_t const* format, ...);

    private:
        friend Super;

        UINT IDD;
        CString path_;
        DirectoryWatch watch_;
        CEdit pathEdit_;
        CListBox messages_;
        boost::mutex mx_;
        boost::thread worker_;

        BOOL OnInitDialog(HWND, LPARAM);
        void OnDestroy();
        void OnCommand(UINT notifyCode, int id, HWND);
        
        LRESULT OnWatchStart(UINT, WPARAM, LPARAM lParam);
        LRESULT OnWatchDone(UINT, WPARAM, LPARAM lParam);
        LRESULT OnDirChanges(UINT, WPARAM wParam, LPARAM lParam);
        
        void Init();
        void Worker();
        void Restart();

        BEGIN_MSG_MAP_EX(DirWatchDialog)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_COMMAND(OnCommand)
            MESSAGE_HANDLER_EX(DirectoryWatch::WmWatchStart, OnWatchStart)
            MESSAGE_HANDLER_EX(DirectoryWatch::WmWatchDone, OnWatchDone)
            MESSAGE_HANDLER_EX(DirectoryWatch::WmChangesDetected, OnDirChanges)
        END_MSG_MAP()
    };
}
