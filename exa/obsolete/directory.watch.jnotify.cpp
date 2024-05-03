#include "stdafx.h"
#include "directory.watch.jnotify.h"
#include <dh.tracing.h>
#include <locale.helper.h>
#include <string.utils.error.code.h>

PCWSTR FileActionString(DWORD type);

namespace DirWatch
{
    void _log(const char *format, ...)
    {
        char sbuf[2048] = {0};

        va_list args;
        va_start(args, format);
        _vsnprintf(sbuf, _countof(sbuf), format, args);
        va_end(args);

        {
            DWORD tid = GetCurrentThreadId();

            static boost::mutex _logMx;
            boost::mutex::scoped_lock lk(_logMx);

            Dh::Printf("[%04d]: %s\n", (int)tid, sbuf);
        }
    }

    int WatchData::_counter = 0;

    WatchData::WatchData()
    {
    }

    WatchData::~WatchData()
    {
        if (_path != NULL) free(_path);
        _hDir = 0;
    }

    WatchData::WatchData(const WCHAR* path, int mask, bool watchSubtree, HANDLE completionPort)
        : _watchId(++_counter)
        , _mask(mask)
        , _watchSubtree(watchSubtree)
        , _byteReturned(0)
        , _completionPort(completionPort)
    {
        _path = _wcsdup(path); 

        _hDir = CreateFileW(_path, FILE_LIST_DIRECTORY | GENERIC_READ
            , FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
            , NULL //security attributes
            , OPEN_EXISTING
            , FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

        if (_hDir == INVALID_HANDLE_VALUE )	
        {
            throw GetLastError();
        }

        if (NULL == CreateIoCompletionPort(_hDir, _completionPort, (ULONG_PTR)_watchId, 0))
        {
            throw GetLastError();
        }
    }

    int WatchData::unwatchDirectory()
    {
        int c = ::CancelIo(_hDir);

        if (_hDir != INVALID_HANDLE_VALUE) 
            CloseHandle(_hDir);

        if (c == 0)
            return GetLastError();

        return 0;
    }

    int WatchData::watchDirectory()
    {
        memset(_buffer, 0, sizeof(_buffer));
        memset(&_overLapped, 0, sizeof(_overLapped));

        if(!ReadDirectoryChangesW( _hDir
            , _buffer, sizeof(_buffer)
            , _watchSubtree
            , _mask, &_byteReturned, &_overLapped, NULL
           ))
        {
            return GetLastError();
        }

        return 0;
    }

    Win32FSHook *Win32FSHook::instance = 0;

    Win32FSHook::Win32FSHook() 
    {
        _callback = 0;
        _mainLoopThreadHandle = INVALID_HANDLE_VALUE;
        _eventsThread = INVALID_HANDLE_VALUE;
        _isRunning = false;
        InitializeCriticalSection(&_eventQueueLock);
        _eventQueueEvent = CreateEvent(NULL, FALSE,FALSE, NULL);
    }

    void Win32FSHook::init(ChangeCallback callback)
    {
        instance = this;
        _callback = callback;
        if (!_isRunning)
        {
            _completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
            if (_completionPort == NULL)
            {
                throw GetLastError();
            }
            _isRunning = true;

            DWORD dwThreadId;
            LPVOID dwThrdParam = (LPVOID)this; 
            _mainLoopThreadHandle = CreateThread( 
                NULL,                        // default security attributes 
                0,                           // use default stack size  
                Win32FSHook::mainLoop,       // thread function 
                dwThrdParam,                // argument to thread function 
                0,                           // use default creation flags 
                &dwThreadId);                // returns the thread identifier 

            if (_mainLoopThreadHandle == NULL) 
            {
                throw ERR_INIT_THREAD;
            }

            SetThreadPriority(_mainLoopThreadHandle, THREAD_PRIORITY_TIME_CRITICAL);
            _eventsThread = CreateThread(
                NULL,                        // default security attributes
                0,                           // use default stack size
                Win32FSHook::eventLoop,       // thread function
                dwThrdParam,                // argument to thread function
                0,                           // use default creation flags
                &dwThreadId);                // returns the thread identifier

            if (_eventsThread == NULL)
            {
                CloseHandle(_mainLoopThreadHandle);
                throw ERR_INIT_THREAD;
            }
        }
    }

    Win32FSHook::~Win32FSHook()
    {
        _log("+Win32FSHook destructor");
        // terminate thread.
        _isRunning = false;
        SetEvent(_eventQueueEvent);
        PostQueuedCompletionStatus(_completionPort, EXIT_SIGNAL, (ULONG_PTR)NULL, NULL);

        // cleanup
        if (INVALID_HANDLE_VALUE != _mainLoopThreadHandle) CloseHandle(_mainLoopThreadHandle);
        if (INVALID_HANDLE_VALUE != _eventsThread) CloseHandle(_eventsThread);
        CloseHandle(_completionPort);
        //DeleteCriticalSection(&_cSection);
        _log("-Win32FSHook destructor");
    }

    int Win32FSHook::watch_count() const
    {
        boost::mutex::scoped_lock lk(mx_);
        return (int)_wid2WatchData.size();
    }

    void Win32FSHook::remove_watch(int wd)
    {
        _log("+remove_watch(%d)", wd);
        boost::mutex::scoped_lock lk(mx_);
        std::map <int, WatchData*>::const_iterator i = _wid2WatchData.find(wd);
        if (i == _wid2WatchData.end())
        {
            _log("remove_watch: watch id %d not found", wd);
        }
        else
        {
            WatchData *wd = i->second;
            int res = wd->unwatchDirectory();
            if (res != 0)
                _log("Error canceling watch on dir %ls : %d",wd->getPath(), res);
            int res2 = (int)_wid2WatchData.erase(wd->getId());
            if (res2 != 1)
                _log("Error deleting watch %d from map, res=%d",wd->getId(), res2);
            PostQueuedCompletionStatus(_completionPort, DELETE_WD_SIGNAL, (ULONG_PTR)wd, NULL);
        }
        _log("-remove_watch(%d)", wd);
    }

    int Win32FSHook::add_watch(PCWSTR path, long notifyFilter, bool watchSubdirs, DWORD &error)
    {
        error = 0;
        _log("+add_watch(`%ls`)", path);
        
        boost::mutex::scoped_lock lk(mx_);
        try
        {
            WatchData *watchData = new WatchData(path, notifyFilter, watchSubdirs, _completionPort);

            if (watchData == 0) 
                throw (DWORD)ERROR_NOT_ENOUGH_MEMORY;

            int watchId = watchData->getId();
            _wid2WatchData[watchId] = watchData;
            watchDirectory(watchData);

            return watchId;
        }
        catch (DWORD err)
        {
            error = err;
        }

        return 0;	
    }

    WatchData* Win32FSHook::find(int wd)
    {
        WatchData *watchData = 0;
        boost::mutex::scoped_lock lk(mx_);
        std::map <int, WatchData*>::const_iterator it = _wid2WatchData.find(wd);
        if (it != _wid2WatchData.end())
        {
            watchData = it->second;
        }
        return watchData;
    }

    void Win32FSHook::onStopWorker()
    {
        _isRunning = false;
    }

    DWORD WINAPI Win32FSHook::mainLoop( LPVOID lpParam )
    {
        _log("mainLoop starts");
        Win32FSHook* _this = (Win32FSHook*)lpParam;

        HANDLE hPort = _this->_completionPort;
        DWORD dwNoOfBytes = 0;
        ULONG_PTR ulKey = 0;
        OVERLAPPED* pov = NULL;
        WCHAR name[4096];

        while (_this->_isRunning)
        {
            pov = NULL;
            BOOL fSuccess = ::GetQueuedCompletionStatus(
                hPort,         // Completion port handle
                &dwNoOfBytes,  // Bytes transferred
                &ulKey,
                &pov,          // OVERLAPPED structure
                INFINITE       // Notification time-out interval
                );

            int wd = (int)ulKey;
            WatchData *watchData = _this->find(wd);

            if (fSuccess)
            {
                if (dwNoOfBytes == 0)
                    continue; // happens when removing a watch some times.

                if (EXIT_SIGNAL == dwNoOfBytes)
                {
                    _this->onStopWorker();
                    continue;
                }

                if (DELETE_WD_SIGNAL == dwNoOfBytes)
                {
                    WatchData *wd = (WatchData*)ulKey;
                    delete wd;
                    if (_this->watch_count() < 1)
                    {
                        _this->onStopWorker();
                        continue;
                    }
                }

                if (!watchData)
                {
                    _log("mainLoop : ignoring event for watch id %d, no longer in wid2WatchData map", wd);
                    continue;
                }

                const char* buffer = watchData->getBuffer();
                FILE_NOTIFY_INFORMATION *event;
                DWORD offset = 0;

                do
                {
                    event = (FILE_NOTIFY_INFORMATION*)(buffer+offset);
                    int action = event->Action;
                    DWORD len = event->FileNameLength / sizeof(WCHAR);
                    for (DWORD k=0;k<len && k < (sizeof(name)-sizeof(WCHAR))/sizeof(WCHAR);k++)
                    {
                        name[k] = event->FileName[k];
                    }
                    name[len] = 0;

                    _this->postEvent(new Event(watchData->getId(), action, watchData->getPath(), name));
                    offset += event->NextEntryOffset;
                }
                while (event->NextEntryOffset != 0);

                int res = watchData->watchDirectory();
                if (res != 0)
                {
                    _log("Error watching dir %s : %d",watchData->getPath(), res);
                    _this->onStopWorker();
                    break;
                }
            }
            else
            {
                _log("GetQueuedCompletionStatus returned an error");

                if (NULL != watchData)
                    _this->remove_watch(watchData->getId());

                else
                {
                    _this->onStopWorker();
                    break;
                }
            }
        }
        _log("mainLoop exits");
        return 0;
    }


    void Win32FSHook::watchDirectory(WatchData* wd)
    {
        _log("Watching %ls", wd->getPath());
        int res = wd->watchDirectory();
        if (res != 0)
        {
            _log("Error watching dir %ls : %d",wd->getPath(), res);
        }
    }

    void Win32FSHook::postEvent(Event *event)
    {
        EnterCriticalSection(&_eventQueueLock);
        _eventQueue.push(event);
        LeaveCriticalSection(&_eventQueueLock);
        SetEvent(_eventQueueEvent);
    }

    DWORD WINAPI Win32FSHook::eventLoop( LPVOID lpParam )
    {
        _log("eventLoop starts");
        Win32FSHook* _this = (Win32FSHook*)lpParam;
        std::queue<Event*> local;
        while (1)
        {
            // quickly copy to local queue, minimizing the time holding the lock
            EnterCriticalSection(&_this->_eventQueueLock);
            while(_this->_eventQueue.size() > 0)
            {
                Event *e = _this->_eventQueue.front();
                local.push(e);
                _this->_eventQueue.pop();
            }
            LeaveCriticalSection(&_this->_eventQueueLock);

            while(local.size() > 0)
            {
                Event *e = local.front();
                local.pop();
                if (_this->_isRunning)
                {
                    _this->_callback(e->_watchID, e->_action, e->_rootPath, e->_filePath);
                }
                delete e;
            }

            if (_this->_isRunning)
            {
                WaitForSingleObjectEx(_this->_eventQueueEvent, INFINITE, TRUE);
            }
            else
                break;
        }
        _log("eventLoop exits");
        return 0;
    }

    void OnDirChanges(int /*watchID*/, int action, PCWSTR rootPath, PCWSTR filePath)
    {
        DirWatch::_log("X> %ls(%d) `%ls` `%ls`", FileActionString(action), action, rootPath, filePath);
    }

    void WatchDirMain(CMessageLoop& loop)
    {
        try
        {
            DirWatch::Win32FSHook fsHook;
            fsHook.init(OnDirChanges);

            DWORD error = 0;
            int watchId = fsHook.add_watch(L"d:\\TEMP\\2"
                , FILE_NOTIFY_CHANGE_FILE_NAME
                | FILE_NOTIFY_CHANGE_DIR_NAME
                | FILE_NOTIFY_CHANGE_ATTRIBUTES
                | FILE_NOTIFY_CHANGE_SIZE
                | FILE_NOTIFY_CHANGE_LAST_WRITE
                | FILE_NOTIFY_CHANGE_LAST_ACCESS
                | FILE_NOTIFY_CHANGE_CREATION
                | FILE_NOTIFY_CHANGE_SECURITY
                , false, error);

            if (!watchId)
                throw error;

            loop.Run();
        }
        catch (std::exception const& ex)
        {
            Dh::Printf("STDEXP: `%s`\n", ex.what());
        }
        catch (int code)
        {
            Dh::Printf(_T("INTEXP: %d `%s`\n"), code, Str::ErrorCode<>::SystemMessage(code));
        }
        catch (DWORD code)
        {
            Dh::Printf(_T("INTEXP: %d `%s`\n"), code, Str::ErrorCode<>::SystemMessage(code));
        }
    }
}
