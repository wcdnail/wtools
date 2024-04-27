#include "stdafx.h"
#include "panel.view.scanner.h"
#include <fs.links.h>
#include <dh.tracing.h>
#include <string.utils.error.code.h>

namespace Twins
{
    size_t FScanner::Ds = 1024 * 1024 * 12;
    Conf::Section FScanner::ClassSettings(::Settings(), L"FScanner");

    static FUpDirItem UpperDirItem;

    FScanner::~FScanner() 
    {
        Cancel();

        ::SetEvent(Flag[FlagStop]);
        ::Sleep(16);

        for (size_t i=FlagCount; i>0; i--)
            ::CloseHandle(Flag[i-1]);
    }

    FScanner::FScanner()
        : Path()
        , Mask(L"*.*")
        , Items()
        , OnFetchDone()
        , InProgress()
        , IsError()
        , ErrorCode(0)
        , ErrorText()
        , SelectedName()
        , DisplayHidden(false)
        , Data()
        , AbortFetch()
        , Thread()
    {
        auto threadRoutine = [this] (){
            this->HeavyDutyProc();
        };

        FromSettings(ClassSettings, Ds);
        Data.reset(new unsigned char[Ds]);
        for (size_t i=0; i<FlagCount; i++) {
            Flag[i] = ::CreateEventW(NULL, FALSE, FALSE, NULL);
        }

        std::thread nuThread(threadRoutine);
        nuThread.swap(Thread);
    }

    void FScanner::UpdatePath(std::wstring const& up)
    {
        if (!up.empty()) {
            Path.SetPath(up);
        }
        else {
            wchar_t drv[MAX_PATH] = {};
            ::GetSystemDirectoryW(drv, _countof(drv));
            drv[3] = 0;
            Path.SetPath(drv);
        }
    }

    void FScanner::Cancel()
    {
        AbortFetch = true;
        ::Sleep(16);
    }

    void FScanner::Fetch(wchar_t const* path /*= NULL*/)
    {
        if (InProgress) {
            Cancel();
        }
        //Progress = 0;
        Items.clear();
        IsError = false;
        AbortFetch = false;
        ErrorCode = 0;
        ErrorText.clear();

        if (NULL != path) {
            Path.SetPath(path);
        }
        ::SetEvent(Flag[FlagFetch]);
    }

    struct InternalError
    {
        HRESULT Hr;
        wchar_t const* Message;
    };

    void FScanner::HeavyDutyProc()
    {
        Dh::ScopedThreadLog l(__FUNCTIONW__);
        double elapsed = .0;

        for (;;) {
            DWORD wr = ::WaitForMultipleObjects(FlagCount, Flag, FALSE, INFINITE);
            InProgress = true;

            if ((WAIT_OBJECT_0 + FlagStop) == wr) {
                break;
            }
            if ((WAIT_OBJECT_0 + FlagFetch) == wr) {
                try {
                    Dh::Timer tm;
                    ReadDir(Path.FullPath().c_str(), Data.get(), Ds);
                    ParseBuffer();
                    elapsed = tm.Seconds();
                    Dh::ThreadPrintf(L" SCANNER: %s #%d %2.8f sec. %s\n", Path.FullPath().c_str(), Items.size(), elapsed, (AbortFetch ? L"CANCELED" : L""));
                }
                catch (InternalError const& ex) {
                    IsError = true;
                    ErrorText = ex.Message;
                    ErrorCode = ex.Hr;

                    Dh::ThreadPrintf(L" SCANNER: `%s` ERROR - 0x%08x %s\n", Path.FullPath().c_str(), ex.Hr, ex.Message);
                }
                catch (std::exception const& ex) {
                    IsError = true;
                    ErrorText.clear();
                    ErrorCode = ::GetLastError();
                    Dh::ThreadPrintf(L" SCANNER: `%s` ERROR - 0x%08x %S\n", Path.FullPath().c_str(), ErrorCode, ex.what());
                }
                InProgress = false;

                // FIXME: upper directory link must be added in error cases
                if (IsError && Items.empty()) {
                    try {
                        FItemVector temp;
                        temp.push_back(&UpperDirItem);
                        Items.swap(temp);
                    }
                    catch (...)
                    {}
                }
                if (OnFetchDone) {
                    OnFetchDone(elapsed);
                }
            }
        }
    }

    void FScanner::ReadDir(wchar_t const* path, unsigned char* buffer, size_t bsize)
    {
        HANDLE h = ::CreateFileW(path
            , READ_CONTROL | GENERIC_READ
            , FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
            , NULL
            , OPEN_EXISTING
            , FILE_FLAG_BACKUP_SEMANTICS | FILE_ATTRIBUTE_DIRECTORY
            , NULL);

        HRESULT hr = ::GetLastError();

        if (INVALID_HANDLE_VALUE == h)
        {
            DWORD attr = ::GetFileAttributesW(path);
            if ((INVALID_FILE_ATTRIBUTES != attr) && (0 != (FILE_ATTRIBUTE_REPARSE_POINT & attr)))
            {
                Dh::ThreadPrintf(L" SCANNER: `%s` ERROR - 0x%08x, try follow link %s\n", path, hr, FileAttributesToString(attr).c_str());

                boost::system::error_code ec;
                std::wstring linkpath = Cf::QueryLinkTarget(path, ec);
                if (ec)
                {
                    InternalError error = { ec.value(), L"QueryLinkTarget failed" };
                    throw error;
                }

                Dh::ThreadPrintf(L" SCANNER: `%s` ==> `%s`\n", path, linkpath.c_str());
                ReadDir(linkpath.c_str(), buffer, bsize);
                return ;
            }

            InternalError error = { hr, L"CreateFileW return INVALID_HANDLE_VALUE" };
            throw error;
        }

        BOOL br = ::GetFileInformationByHandleEx(h, FileIdBothDirectoryInfo, buffer, (DWORD)bsize);
        hr = ::GetLastError();
        ::CloseHandle(h);

        if (!br || (0 != hr))
        {
            InternalError error = { hr, L"GetFileInformationByHandleEx return FALSE | GetLastError return !0" };
            throw error;
        }
    }

    void FScanner::ParseBuffer()
    {
        FItemVector temp;

        bool last = false;
        PFILE_ID_BOTH_DIR_INFO current = (PFILE_ID_BOTH_DIR_INFO)Data.get();
        size_t i = 0;
        for (;;)
        {
            current->FileNameLength /= sizeof(wchar_t);
            FItem *ent = (FItem*)current;

            bool skip = (ent->IsCurrentDir()
                     || (!DisplayHidden ? ent->IsHidden() : false)
                     // TODO: apply this->Mask
                     );

            if (!skip)
                temp.push_back(ent);

            current = (PFILE_ID_BOTH_DIR_INFO)(((BYTE*)current) + current->NextEntryOffset);
            //::InterlockedIncrement(&Progress);

            //if (0 == (Progress % 1000))
            //    View.Invalidate(false);

            if (last || AbortFetch)
                break;

            if (!current->NextEntryOffset)
                last = true;
        }
        
        {
            //std::lock_guard lk(Mx);
            Items.swap(temp);
        }
    }
}
