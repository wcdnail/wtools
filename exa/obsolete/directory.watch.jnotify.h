#pragma once

#include <string>
#include <map>
#include <queue>
#include <utility>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

namespace DirWatch
{
    typedef void (*ChangeCallback)(int watchID, int action, PCWSTR rootPath, PCWSTR filePath);

    class WatchData: boost::noncopyable
    {
    public:
        WatchData();
        WatchData(PCWSTR path, int mask, bool watchSubtree, HANDLE completionPort);
        virtual ~WatchData();

        char const* getBuffer() const { return _buffer; }
        //FILE_NOTIFY_INFORMATION* getNotifyInfo() { return _notifyInfo; }
        int getBufferSize() const { return sizeof(_buffer); }
        DWORD getBytesReturned() { return _byteReturned; }
        PCWSTR getPath() const {return _path;}
        int getId() const { return _watchId; }
        HANDLE getDirHandle() const { return _hDir; }
        int getMask() const { return _mask; }
        int watchDirectory();
        int unwatchDirectory();

    private:	
        static int _counter;
        WCHAR* _path;
        int _watchId;
        HANDLE _hDir;
        int _mask;
        bool _watchSubtree;
        DWORD _byteReturned;
        OVERLAPPED _overLapped;
        char _buffer[8196];
        HANDLE _completionPort;
    };

    class Event: boost::noncopyable
    {
    public:
        int _watchID;
        int _action;
        WCHAR* _rootPath;
        WCHAR* _filePath;

        Event(int wd, int action, const WCHAR* rootPath, const WCHAR* filePath)
        {
            _watchID = wd;
            _action = action;
            size_t len1 = wcslen(rootPath);
            size_t len2 = wcslen(filePath);
            _rootPath = new WCHAR[len1 + 1];
            _filePath = new WCHAR[len2 + 1];
            wcsncpy(_rootPath, rootPath, len1);
            _rootPath[len1] = 0;
            wcsncpy(_filePath, filePath, len2);
            _filePath[len2] = 0;
        }

        ~Event()
        {
            delete [] _rootPath;
            delete [] _filePath;
        }
    };

    class Win32FSHook: boost::noncopyable
    {
    public:
        static const int ERR_INIT_THREAD = 1;

        Win32FSHook();
        ~Win32FSHook();

        void init(ChangeCallback callback);

        int add_watch(PCWSTR path, long notifyFilter, bool watchSubdirs, DWORD &error);
        void remove_watch(int watchId);
        int watch_count() const;

    private:
        bool _isRunning;
        HANDLE _mainLoopThreadHandle;
        HANDLE _completionPort;
        mutable boost::mutex mx_;
        std::map<int, WatchData*> _wid2WatchData;
        ChangeCallback _callback;
        CRITICAL_SECTION _eventQueueLock;
        HANDLE _eventQueueEvent;
        HANDLE _eventsThread;
        std::queue<Event*> _eventQueue;

        void watchDirectory(WatchData* wd);
        void postEvent(Event *event);
        WatchData* find(int wd);
        void onStopWorker();

        static const DWORD DELETE_WD_SIGNAL = 99999997;
        static const DWORD EXIT_SIGNAL = 99999998;
        static Win32FSHook *instance;
        static DWORD WINAPI mainLoop( LPVOID lpParam );
        static DWORD WINAPI eventLoop( LPVOID lpParam );
    };

    void _log(const char *format, ...);
    void WatchDirMain(CMessageLoop& loop);
}
