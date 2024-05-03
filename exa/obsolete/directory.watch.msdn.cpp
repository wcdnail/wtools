#include "stdafx.h"
#include <dh.tracing.h>
#include <locale.helper.h>
#include <string.utils.error.code.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#define _RetStr(X) case X: return L#X;

PCWSTR FileActionString(DWORD type)
{
    switch (type)
    {
    _RetStr(FILE_ACTION_ADDED           ); // 0x00000001   
    _RetStr(FILE_ACTION_REMOVED         ); // 0x00000002   
    _RetStr(FILE_ACTION_MODIFIED        ); // 0x00000003   
    _RetStr(FILE_ACTION_RENAMED_OLD_NAME); // 0x00000004   
    _RetStr(FILE_ACTION_RENAMED_NEW_NAME); // 0x00000005   
    }

    return L"NOT_DEFINED";
}

namespace ChangeDirNotify
{
    void RefreshDirectory(PCTSTR);
    void RefreshTree(PCTSTR);
    void WatchDirectory(PCTSTR);

    void Main(PCTSTR path)
    {
        WatchDirectory(path);
    }

    void WatchDirectory(PCTSTR lpDir)
    {
        DWORD dwWaitStatus; 
        HANDLE dwChangeHandles[2]; 
        TCHAR lpDrive[4];
        TCHAR lpFile[_MAX_FNAME];
        TCHAR lpExt[_MAX_EXT];

        _tsplitpath_s(lpDir, lpDrive, 4, NULL, 0, lpFile, _MAX_FNAME, lpExt, _MAX_EXT);

        lpDrive[2] = (TCHAR)'\\';
        lpDrive[3] = (TCHAR)'\0';

        // Watch the directory for file creation and deletion. 

        dwChangeHandles[0] = FindFirstChangeNotification( 
            lpDir,                         // directory to watch 
            FALSE,                         // do not watch subtree 
            FILE_NOTIFY_CHANGE_FILE_NAME); // watch file name changes 

        if (dwChangeHandles[0] == INVALID_HANDLE_VALUE) 
        {
            ATLTRACE("\n ERROR: FindFirstChangeNotification function failed.\n");
            return ;
        }

        // Watch the subtree for directory creation and deletion. 

        dwChangeHandles[1] = FindFirstChangeNotification( 
            lpDrive,                       // directory to watch 
            TRUE,                          // watch the subtree 
            FILE_NOTIFY_CHANGE_DIR_NAME);  // watch dir name changes 

        if (dwChangeHandles[1] == INVALID_HANDLE_VALUE) 
        {
            ATLTRACE("\n ERROR: FindFirstChangeNotification function failed.\n");
            return ;
        }


        // Make a final validation check on our handles.

        if ((dwChangeHandles[0] == NULL) || (dwChangeHandles[1] == NULL))
        {
            ATLTRACE("\n ERROR: Unexpected NULL from FindFirstChangeNotification.\n");
            ExitProcess(GetLastError()); 
        }

        // Change notification is set. Now wait on both notification 
        // handles and refresh accordingly. 

        while (TRUE) 
        { 
            // Wait for notification.

            ATLTRACE("\nWaiting for notification...\n");

            dwWaitStatus = WaitForMultipleObjects(2, dwChangeHandles, 
                FALSE, INFINITE); 

            switch (dwWaitStatus) 
            { 
            case WAIT_OBJECT_0: 

                // A file was created, renamed, or deleted in the directory.
                // Refresh this directory and restart the notification.

                RefreshDirectory(lpDir); 
                if ( FindNextChangeNotification(dwChangeHandles[0]) == FALSE )
                {
                    ATLTRACE("\n ERROR: FindNextChangeNotification function failed.\n");
                    ExitProcess(GetLastError()); 
                }
                break; 

            case WAIT_OBJECT_0 + 1: 

                // A directory was created, renamed, or deleted.
                // Refresh the tree and restart the notification.

                RefreshTree(lpDrive); 
                if (FindNextChangeNotification(dwChangeHandles[1]) == FALSE )
                {
                    ATLTRACE("\n ERROR: FindNextChangeNotification function failed.\n");
                    return ;
                }
                break; 

            case WAIT_TIMEOUT:

                // A timeout occurred, this would happen if some value other 
                // than INFINITE is used in the Wait call and no changes occur.
                // In a single-threaded environment you might not want an
                // INFINITE wait.

                ATLTRACE("\nNo changes in the timeout period.\n");
                break;

            default: 
                ATLTRACE("\n ERROR: Unhandled dwWaitStatus.\n");
                return ;
            }
        }
    }

    void RefreshDirectory(PCTSTR lpDir)
    {
        // This is where you might place code to refresh your
        // directory listing, but not the subtree because it
        // would not be necessary.

        ATLTRACE(TEXT("Directory (%s) changed.\n"), lpDir);
    }

    void RefreshTree(PCTSTR lpDrive)
    {
        // This is where you might place code to refresh your
        // directory listing, including the subtree.

        ATLTRACE(TEXT("Directory tree (%s) changed.\n"), lpDrive);
    }

    boost::shared_ptr<void> exitEvent(::CreateEvent(0, FALSE, FALSE, 0), CloseHandle);

    HRESULT Main2(PCWSTR path)
    {
        HANDLE handle = ::CreateFileW(path
            , FILE_LIST_DIRECTORY 
            //| FILE_READ_EA
            , FILE_SHARE_READ 
            | FILE_SHARE_DELETE 
            | FILE_SHARE_WRITE
            , NULL
            , OPEN_EXISTING
            , FILE_FLAG_BACKUP_SEMANTICS
            | FILE_FLAG_OVERLAPPED
            , NULL);

        if (INVALID_HANDLE_VALUE == handle)
        {
            HRESULT hr = ::GetLastError();
            ATLTRACE(L"%-32s: `%s` failed - %d `%s`\n", L"CreateFile", path, hr, Str::ErrorCode<>::SystemMessage(hr));
            return hr;
        }

        boost::shared_ptr<void> directory(handle, CloseHandle);
        boost::shared_ptr<void> done(::CreateEvent(0, FALSE, FALSE, 0), CloseHandle);

        OVERLAPPED ov = {0};
        ov.hEvent = done.get();

        HANDLE events[] = { exitEvent.get(), ov.hEvent };

        while (TRUE)
        {
            BYTE buffer[5120];
            DWORD bufferSize = sizeof(buffer);
            ZeroMemory(buffer, sizeof(buffer));

            BOOL ResultReadChange = ::ReadDirectoryChangesW(directory.get()
                , buffer, bufferSize, FALSE
                , FILE_NOTIFY_CHANGE_FILE_NAME
                | FILE_NOTIFY_CHANGE_DIR_NAME
                | FILE_NOTIFY_CHANGE_ATTRIBUTES
                | FILE_NOTIFY_CHANGE_SIZE
                | FILE_NOTIFY_CHANGE_LAST_WRITE
                | FILE_NOTIFY_CHANGE_LAST_ACCESS
                | FILE_NOTIFY_CHANGE_CREATION
                //| FILE_NOTIFY_CHANGE_SECURITY
                , &bufferSize
                , &ov, NULL);

            DWORD wr = ::WaitForMultipleObjects(2, events, FALSE, INFINITE);

            if (!::GetOverlappedResult(directory.get(), &ov, &bufferSize, FALSE))
            {
                HRESULT hr = ::GetLastError();
                ATLTRACE(L"%-32s: failed - %d `%s`\n", L"GetOverlappedResult", hr, Str::ErrorCode<>::SystemMessage(hr));
                return hr;
            }

            if (WAIT_OBJECT_0 == wr)
                return ERROR_CANCELLED;

            PFILE_NOTIFY_INFORMATION info = (PFILE_NOTIFY_INFORMATION)buffer;

            while (TRUE)
            {
                CStringW name;
                name.SetString(info->FileName, info->FileNameLength / 2);

                wchar_t buf2[128] = {0};
                ATLTRACE(L"%-32s: %d >> `%s`\n", FileActionString(info->Action), info->Action, name);

                info += info->NextEntryOffset;
                if (!info->NextEntryOffset) 
                    break;
            }
        }             

        return S_OK;
    }

    void Terminate2()
    {
        ::Sleep(2000);
        ::SetEvent(exitEvent.get());
    }
}
