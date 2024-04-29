#include "stdafx.h"
#include "panel.view.scanner.h"
#include <fs.links.h>
#include <dh.tracing.h>
#include <string.utils.error.code.h>

namespace Twins
{
    size_t   FScanner::infoCacheSize_ = 1024 * 1024 * 12;
    Conf::Section FScanner::settings_(::Settings(), L"FScanner");

    static FUpDirItem UpperDirItem;

    FScanner::~FScanner() 
    {
        if (Thread.joinable()) {
            Cancel();
            SetEvent(Flag[FlagStop]);
            Thread.join();
        }
        for (size_t i=FlagCount; i>0; i--) {
            ::CloseHandle(Flag[i-1]);
        }
    }

    FScanner::FScanner()
        :          Path()
        ,          Mask(L"*.*")
        ,         Items()
        ,   OnFetchDone()
        ,    InProgress()
        ,       IsError()
        ,     ErrorCode(0)
        ,     ErrorText()
        ,  SelectedName()
        , DisplayHidden(false)
        ,          Data()
        ,    AbortFetch()
        ,        Thread()
    {
        FromSettings(settings_, infoCacheSize_);
        auto data = std::make_unique<uint8_t[]>(infoCacheSize_);
        data.swap(Data);
        for (size_t i=0; i<FlagCount; i++) {
            Flag[i] = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
        }
    }

    void FScanner::Spawn()
    {
        std::thread nuThread(&FScanner::HeavyDutyProc, this);
        nuThread.swap(Thread);
    }

    void FScanner::UpdatePath(std::wstring const& up)
    {
        if (!up.empty()) {
            Path.SetPath(up);
        }
        else {
            Path.ResetToCurrent();
        }
    }

    void FScanner::Cancel()
    {
        AbortFetch = true;
        std::this_thread::yield();
    }

    void FScanner::Fetch(PCWSTR path /*= NULL*/)
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
        if (nullptr != path) {
            Path.SetPath(path);
        }
        if (!Thread.joinable()) {
            Spawn();
            std::this_thread::yield();
        }
        ::SetEvent(Flag[FlagFetch]);
    }

    void FScanner::HeavyDutyProc()
    {
        DH::ScopedThreadLog l(__FUNCTIONW__);
        double elapsed = .0;

        for (;;) {
            DWORD awaitingEvent = ::WaitForMultipleObjects(FlagCount, Flag, FALSE, INFINITE);
            if (FlagStop == awaitingEvent) {
                break;
            }
            if (FlagFetch == awaitingEvent) {
                InProgress = true;
                {
                    std::wstring path = Path.FullPath().native();
                    DH::Timer timer;
                    try {
                        InternalError err = ReadDir(path.c_str(), Data.get(), infoCacheSize_);
                        if (IsError = FAILED(err.Hr)) {
                            DH::ThreadPrintf(L" SCANNER: `%s` ERROR - 0x%08x %s\n", path.c_str(), err.Hr, err.Message);
                        }
                        else {
                            ParseBuffer();
                        }
                    }
                    catch (std::exception const& ex) {
                        IsError = true;
                        ErrorText.clear();
                        ErrorCode = ::GetLastError();
                        DH::ThreadPrintf(L" SCANNER: `%s` ERROR - 0x%08x %S\n", path.c_str(), ErrorCode, ex.what());
                    }
                    elapsed = timer.Seconds();
                    DH::ThreadPrintf(L" SCANNER: %s #%d %2.8f sec. %s\n", path.c_str(), Items.size(), elapsed, (AbortFetch ? L"CANCELED" : L""));
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

    FScanner::InternalError FScanner::ReadDir(PCWSTR path, uint8_t buffer[], size_t bsize)
    {
        HANDLE h = ::CreateFileW(path,
            READ_CONTROL | GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_ATTRIBUTE_DIRECTORY,
            nullptr
        );
        HRESULT hr = ::GetLastError();
        if (INVALID_HANDLE_VALUE == h) {
            DWORD attr = ::GetFileAttributesW(path);
            if ((INVALID_FILE_ATTRIBUTES != attr) && (0 != (FILE_ATTRIBUTE_REPARSE_POINT & attr))) {
                DH::ThreadPrintf(L" SCANNER: `%s` ERROR - 0x%08x, try follow link %s\n", path, hr, FileAttributesToString(attr).c_str());
                std::error_code ec;
                std::wstring linkpath = CF::QueryLinkTarget(path, ec);
                if (ec) {
                    return InternalError { ec.value(), L"QueryLinkTarget failed" };
                }
                DH::ThreadPrintf(L" SCANNER: `%s` ==> `%s`\n", path, linkpath.c_str());
                return ReadDir(linkpath.c_str(), buffer, bsize);
            }
            return InternalError { hr, L"CreateFileW return INVALID_HANDLE_VALUE" };
        }
        BOOL br = ::GetFileInformationByHandleEx(h, FileIdBothDirectoryInfo, buffer, (DWORD)bsize);
        hr = ::GetLastError();
        ::CloseHandle(h);
        if (!br || (0 != hr)) {
            return InternalError { hr, L"GetFileInformationByHandleEx return FALSE | GetLastError return !0" };
        }
        return InternalError { S_OK, nullptr };
    }

    void FScanner::ParseBuffer()
    {
        FItemVector temp;

        bool last = false;
        PFILE_ID_BOTH_DIR_INFO current = (PFILE_ID_BOTH_DIR_INFO)Data.get();
        size_t i = 0;
        for (;;) {
            current->FileNameLength /= sizeof(wchar_t);
            FItem *ent = (FItem*)current;

            bool skip = (ent->IsCurrentDir()
                     || (!DisplayHidden ? ent->IsHidden() : false)
                     // TODO: apply this->Mask
                     );
            if (!skip) {
                temp.emplace_back(ent);
            }
            current = (PFILE_ID_BOTH_DIR_INFO)(((BYTE*)current) + current->NextEntryOffset);
            //::InterlockedIncrement(&Progress);
            //if (0 == (Progress % 1000))
            //    View.Invalidate(false);
            if (last || AbortFetch) {
                break;
            }
            if (!current->NextEntryOffset) {
                last = true;
            }
        }
        
        {
            //std::lock_guard lk(Mx);
            Items.swap(temp);
        }
    }
}
