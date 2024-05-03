#include "stdafx.h"
#include "dialogz.directory.watch.test.h"
#include "directory.watch.notify.h"
#include <string.utils.error.code.h>
#include <dh.tracing.h>
#include <atldlgs.h>
#include <strsafe.h>
#include "res/resource.h"

namespace Ut
{
    static int DialogCounter = 0;
    static int DirectoryIndex = 0;

    static PCWSTR GetDirPath(int& i)
    {
        static const PCWSTR directories[] = 
        {
          L"C:\\TEMP\\0"
        , L"C:\\TEMP\\1"
        , L"C:\\TEMP\\2"
        , L"C:\\TEMP\\3"
        , L"C:\\TEMP\\4"
        , L"C:\\TEMP\\5"
        , L"C:\\TEMP\\6"
        , L"C:\\TEMP\\7"
        , L"C:\\TEMP\\8"
        , L"C:\\TEMP\\9"
        , L"C:\\TEMP\\A"
        , L"C:\\TEMP\\B"
        , L"C:\\TEMP\\C"
        , L"C:\\TEMP\\D"
        , L"C:\\TEMP\\E"
        , L"C:\\TEMP\\F"
        };

        const int lim = (int)_countof(directories)-1;
        
        if (i < 0)
            i = lim;

        else if (i > lim)
            i = 0;

        return directories[i];
    }

    DirWatchDialog::DirWatchDialog()
        : IDD(IDD_DIRWATCH)
        , path_(GetDirPath(DirectoryIndex))
        , watch_()
        , pathEdit_()
        , messages_()
        , mx_()
        , worker_()
    {
        ++DirectoryIndex;
    }

    DirWatchDialog::~DirWatchDialog()
    {
#ifdef _DEBUG
        m_hWnd = NULL; // Нахуй WTL!
#endif
        --DirectoryIndex;

        worker_.join();
    }

    void DirWatchDialog::Init()
    {
        CIcon icon(::LoadIcon(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME)));
        SetIcon(icon, TRUE);
        SetIcon(icon, FALSE);

        CRect dlgRect;
        GetWindowRect(dlgRect);

        static CPoint pos(4, 4);
        SetWindowPos(NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        pos.x += dlgRect.right + 4;
        if ((pos.x + dlgRect.Width()) > GetSystemMetrics(SM_CXFULLSCREEN))
        {
            pos = 4;
            pos.y += dlgRect.bottom + 4;
        }

        pathEdit_.Attach(GetDlgItem(IDC_PATH));
        messages_.Attach(GetDlgItem(IDC_NLIST));

        Restart();

        ++DialogCounter;
    }

    void DirWatchDialog::Restart()
    {
        pathEdit_.SetWindowText(path_);
        watch_.SetOwner(m_hWnd);
        watch_.Cancel();
        worker_ = boost::thread(&DirWatchDialog::Worker, this);
    }

    BOOL DirWatchDialog::OnInitDialog(HWND, LPARAM)
    {
        Init();
        return TRUE;
    }

    void DirWatchDialog::OnCommand(UINT notifyCode, int id, HWND)
    {
        if (IDCANCEL == id)
        {
            --DirectoryIndex;

            watch_.Cancel();

            if (0 == --DialogCounter)
                PostQuitMessage(0);

            else
            {
                m_bModal = TRUE;
                EndDialog(id);
            }
        }

        if (BN_CLICKED == notifyCode)
        {
            if (IDC_BROWSE == id)
            {
                CFolderDialog dlg(m_hWnd, L"Выберите директорию для слежения");
                dlg.SetInitialFolder(path_);

                UINT_PTR rv = dlg.DoModal();
                if (IDOK == rv)
                {
                    path_ = dlg.m_szFolderPath;
                    Restart();
                }
            }
            else if (IDC_WIPELOG == id)
            {
                boost::mutex::scoped_lock lk(mx_);
                messages_.ResetContent();
            }
        }
    }

    void DirWatchDialog::Worker()
    {
        watch_.HandleResult(watch_.Listen(path_));
    }

    void DirWatchDialog::Log(wchar_t const* format, ...)
    {
        va_list ap;
        va_start(ap, format);

        CStringW log;
        log.FormatV(format, ap);
        va_end(ap);

        int n = messages_.AddString(log);
        if (LB_ERR != n)
            messages_.SetCurSel(n);

        Dh::Printf(L"[%04d]: %s\n", ::GetCurrentThreadId(), log);
    }

    LRESULT DirWatchDialog::OnWatchStart(UINT, WPARAM, LPARAM lParam)
    {
        PCWSTR dirPath = (PCWSTR)lParam;
        Log(L"Listening `%s`", dirPath);
        return 0;
    }

    LRESULT DirWatchDialog::OnWatchDone(UINT, WPARAM wParam, LPARAM lParam)
    {
        HRESULT hr = (HRESULT)wParam;
        PCWSTR dirPath = (PCWSTR)lParam;

        Log(L"Listening `%s` done - %d (`%s`)", dirPath, hr, Str::ErrorCode<wchar_t>::SystemMessage(hr));
        return 0;
    }

    #define _RetStr(X) \
        case X: return L#X

    static wchar_t const* ActionString(int action)
    {
        switch (action)
        {
        _RetStr(FILE_ACTION_ADDED           ); // 0x00000001   
        _RetStr(FILE_ACTION_REMOVED         ); // 0x00000002   
        _RetStr(FILE_ACTION_MODIFIED        ); // 0x00000003   
        _RetStr(FILE_ACTION_RENAMED_OLD_NAME); // 0x00000004   
        _RetStr(FILE_ACTION_RENAMED_NEW_NAME); // 0x00000005   
        }

        return L"##";
    }

    LRESULT DirWatchDialog::OnDirChanges(UINT, WPARAM wParam, LPARAM lParam)
    {
        PCWSTR dirpath = (PCWSTR)wParam;
        DirectoryNotifyMap const* nm = (DirectoryNotifyMap const*)lParam;

        boost::mutex::scoped_lock lk(mx_);

        Log(L"-------------------------------------------------------------------------------------");

        for (DirectoryNotifyMap::const_iterator it = nm->begin(); it != nm->end(); ++it)
            Log(L"%s", it->second->ToString());

        return 0;
    }
}
