#include "stdafx.h"
#include "directory.watch.h"
#include "directory.watch.notify.h"
#include "directory.watch.misc.h"

namespace Twins
{
    DirectoryWatch::DirectoryWatch()
        : id_(0)
        , port_()
        , owner_(NULL)
        , dirpath_()
    {}

    DirectoryWatch::~DirectoryWatch()
    {
        Cancel();
    }

    void DirectoryWatch::SetOwner(void* owner)
    {
        owner_ = owner;
    }

    void DirectoryWatch::Cancel()
    {
        if (port_)
            ::PostQueuedCompletionStatus(port_.get(), 0, _StopFlag, NULL);
    }

    void DirectoryWatch::HandleResult(HRESULT hr) const
    {
        ::PostMessage((HWND)owner_, WmWatchDone, (WPARAM)hr, (LPARAM)(PCWSTR)dirpath_);
    }

    HRESULT DirectoryWatch::Listen(PCWSTR dirPath, unsigned long mask /*= DefaultFlags*/, unsigned long numberOfConcurrentThreads /*= 1*/)
    {
        HRESULT hr = 0;
        unsigned long tempId = 0;
        Handle dir;
        Handle port;

        try
        {
            dirpath_ = dirPath;
            Initialize(dirpath_, dir, port, tempId, numberOfConcurrentThreads);

            ::InterlockedExchange(&id_, tempId);
            port_.swap(port);

            ::PostMessage((HWND)owner_, WmWatchStart, 0, (LPARAM)(PCWSTR)dirpath_);

            enum { MinQueryTimeout = 100, MaxQueryTimeout = INFINITE };
            DWORD queryTimeout = MaxQueryTimeout;
            DirectoryNotifyMap notifyMap;

            for (;;)
            {
                ULONG_PTR key = 0;
                BYTE buffer[8192] = {};
                OVERLAPPED ov = {};
                LPOVERLAPPED overlapped = &ov;

                if (!::ReadDirectoryChangesW(dir.get(), buffer, sizeof(buffer), FALSE, mask, NULL, overlapped, NULL))
                    throw Error("ReadDirectoryChangesW", ::GetLastError());

                ::SetLastError(0);
                DWORD transfered = 0;
                BOOL rv = ::GetQueuedCompletionStatus(port_.get(), &transfered, &key, &overlapped, queryTimeout);
                hr = ::GetLastError();
                bool waitTimeout = (WAIT_TIMEOUT == hr);

                if (!rv && !waitTimeout)
                    throw Error("GetQueuedCompletionStatus", hr);

                if (_StopFlag == key)
                {
                    hr = 0;
                    break;
                }

                if (!waitTimeout)
                {
                    queryTimeout = MinQueryTimeout;

                    if (transfered && (key == id_))
                        Parse(buffer, notifyMap);
                }
                else
                {
                    queryTimeout = MaxQueryTimeout;

                    if (!notifyMap.empty())
                        NotifyOwner(owner_, dirpath_, notifyMap);
                }
            }
        }
        catch (Error const& ex)
        {
            hr = ex.Code;
            DH::ThreadPrintf("%s - %d %s\n", ex.what(), hr, Str::ErrorCode<char>::SystemMessage(hr));
        }
        catch (std::exception const& ex)
        {
            hr = E_UNEXPECTED;
            DH::ThreadPrintf("%s - %d %s\n", ex.what(), hr, Str::ErrorCode<char>::SystemMessage(hr));
        }

        ::InterlockedExchange(&id_, 0);
        port.swap(port_);
        return hr;
    }
}
