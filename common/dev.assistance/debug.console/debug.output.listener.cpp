#include "stdafx.h"
#include "debug.output.listener.h"
#include "debug.console.h"
#include <string>

namespace DH
{
    DebugOutputListener::DebugOutputListener(DebugConsole const& owner)
        : owner_(owner)
        , rv_(ERROR_SUCCESS)
        , ackEvent_(nullptr)
        , readyEvent_(nullptr)
        , sharedFile_(nullptr)
        , sharedMem_(nullptr)
        , routine_(nullptr)
        , routineId_(0)
        , threadStarted_(::CreateEvent(nullptr, FALSE, FALSE, nullptr))
        , threadStop_(::CreateEvent(nullptr, FALSE, FALSE, nullptr))
    {
    }

    DebugOutputListener::~DebugOutputListener()
    {
        Stop();
        ::CloseHandle(threadStarted_);
        ::CloseHandle(threadStop_);
        FreeResources(ackEvent_, readyEvent_, sharedFile_, sharedMem_);
    }

    void DebugOutputListener::FreeResources(HANDLE& ackEvent, HANDLE& readyEvent, HANDLE& sharedFile, PVOID& sharedMem)
    {
        if (ackEvent)
        {
            ::CloseHandle(ackEvent);
            ackEvent = nullptr;
        }

        if (readyEvent)
        {
            ::CloseHandle(readyEvent);
            readyEvent = nullptr;
        }

        if (sharedFile)
        {
            ::CloseHandle(sharedFile);
            sharedFile = nullptr;
        }

        if (sharedMem)
        {
            ::UnmapViewOfFile(sharedMem);
            sharedMem = nullptr;
        }
    }

    bool DebugOutputListener::Init()
    {
        HANDLE   ackEvent{nullptr};
        HANDLE readyEvent{nullptr};
        HANDLE sharedFile{nullptr};
        PVOID   sharedMem{nullptr};

        rv_ = ERROR_SUCCESS;
        try {
            SECURITY_DESCRIPTOR sd = {0};
            if (!::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) {
                throw (HRESULT)::GetLastError();
            }
            if (!::SetSecurityDescriptorDacl(&sd, TRUE, nullptr, FALSE)) {
                throw (HRESULT)::GetLastError();
            }

            SECURITY_ATTRIBUTES sa;
            sa.nLength = sizeof(sa);
            sa.lpSecurityDescriptor = &sd;
            sa.bInheritHandle = TRUE;

            ackEvent = ::OpenEvent(SYNCHRONIZE, TRUE, _T("DBWIN_BUFFER_READY"));
            if (!ackEvent)
            {
                ackEvent = ::CreateEvent(&sa, FALSE, FALSE, _T("DBWIN_BUFFER_READY"));
                if (!ackEvent) 
                {
                    throw (HRESULT)::GetLastError();
                }
            }

            readyEvent = ::OpenEvent(EVENT_MODIFY_STATE, TRUE, _T("DBWIN_DATA_READY"));
            if (!readyEvent)
            {
                readyEvent = ::CreateEvent(&sa, FALSE, FALSE, _T("DBWIN_DATA_READY"));
                if (!readyEvent) 
                {
                    throw (HRESULT)::GetLastError();
                }
            }

            sharedFile = ::OpenFileMapping(PAGE_READONLY, TRUE, _T("DBWIN_BUFFER"));
            if (!sharedFile)
            {
                sharedFile = ::CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, 4096, _T("DBWIN_BUFFER"));
                if (!sharedFile) 
                {
                    throw (HRESULT)::GetLastError();
                }
            }

            sharedMem = ::MapViewOfFile(sharedFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
            if (!sharedMem)
            {
                throw (HRESULT)::GetLastError();
            }

            FreeResources(ackEvent_, readyEvent_, sharedFile_, sharedMem_);

            ackEvent_ = ackEvent;
            readyEvent_ = readyEvent;
            sharedFile_ = sharedFile;
            sharedMem_ = sharedMem;
        }
        catch (HRESULT rv)
        {
            FreeResources(ackEvent, readyEvent, sharedFile, sharedMem);
            rv_ = rv;
        }

        return ERROR_SUCCESS == rv_;
    }

    bool DebugOutputListener::Start()
    {
        if (nullptr != routine_) {
            rv_ = ERROR_ALREADY_REGISTERED;
            return false;
        }
        if (!sharedMem_ && !Init()) {
            return false;
        }
        unsigned id = 0;
        auto const proc = reinterpret_cast<HANDLE>(::_beginthreadex(nullptr, 0, CaptureProc, this, 0, &id));
        rv_ = ::GetLastError();
        if (proc) {
            routine_ = proc;
            routineId_ = id;
            ::ResumeThread(routine_);
            ::WaitForSingleObject(threadStarted_, INFINITE);
        }
        return ERROR_SUCCESS == rv_;
    }

    bool DebugOutputListener::Stop()
    {
        if (!routine_) {
            rv_ = ERROR_SERVICE_NOT_FOUND;
            return false;
        }
        ::SetEvent(threadStop_);
        ::WaitForSingleObject(threadStop_, 2000);
        ::CloseHandle(routine_);
        routine_ = nullptr;
        routineId_ = 0;
        return true;
    }

    unsigned __stdcall DebugOutputListener::CaptureProc(void* rawThis) 
    {
        static_cast<DebugOutputListener*>(rawThis)->CaptureProc();
        return 0;
    }

    void DebugOutputListener::CaptureProc() 
    {   
        ::SetEvent(threadStarted_);
        HANDLE events[] = { readyEvent_, threadStop_ };
        DWORD myPid = GetCurrentProcessId();
        while (true) {
            ::SetEvent(ackEvent_);
            DWORD rv = ::WaitForMultipleObjects(_countof(events), events, FALSE, INFINITE);
            if (WAIT_OBJECT_0 == rv) {   
                struct DBUF_D 
                {
                    DWORD pid;
                    char string[4096-sizeof(DWORD)];
                };
                DBUF_D *dbuf = (DBUF_D*)sharedMem_;
                if (myPid == dbuf->pid) {
                    owner_.Puts(dbuf->string);
                }
            }
            else if ((WAIT_OBJECT_0+1) == rv) {
                ::ResetEvent(threadStop_);
                break;
            }
        }
        ::SetEvent(threadStop_);
    }
}
