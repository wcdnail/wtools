#include <dh.tracing.h>
#include <string.utils.error.code.h>
#include <stdexcept>

namespace Twins
{
    static volatile unsigned long _Counter = 1;
    static const unsigned long _StopFlag = 99999999;

    struct DirectoryWatch::Error: public std::runtime_error
    {
        typedef std::runtime_error Super;

        Error(char const* message, HRESULT hr) : Super(message), Code(hr) {}

        HRESULT Code;
    };

#ifdef _DEBUG2
    wchar_t const* ActionSignString(int action)
    {
        switch (action)
        {
        case FILE_ACTION_ADDED:            return L"+="; // 0x00000001   
        case FILE_ACTION_REMOVED:          return L"-="; // 0x00000002   
        case FILE_ACTION_MODIFIED:         return L"^="; // 0x00000003   
        case FILE_ACTION_RENAMED_OLD_NAME: return L"~="; // 0x00000004   
        case FILE_ACTION_RENAMED_NEW_NAME: return L"!="; // 0x00000005   
        }

        return L"??";
    }
#endif

    static void Initialize(PCWSTR dirPath, DirectoryWatch::Handle& dir, DirectoryWatch::Handle& port, unsigned long& tempId, unsigned long numberOfConcurrentThreads)
    {
        HANDLE rawDirHandle = ::CreateFileW(dirPath
            , FILE_LIST_DIRECTORY | GENERIC_READ
            , FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
            , NULL 
            , OPEN_EXISTING
            , FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED
            , NULL);

        if (INVALID_HANDLE_VALUE == rawDirHandle)	
            throw DirectoryWatch::Error("CreateFile", ::GetLastError());

        DirectoryWatch::Handle tempDir(rawDirHandle, CloseHandle);
        DirectoryWatch::Handle tempPort(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, numberOfConcurrentThreads), CloseHandle);
        if (!tempPort)
            throw DirectoryWatch::Error("CreateIoCompletionPort #1", ::GetLastError());

        tempId = ::InterlockedIncrement(&_Counter);
        HANDLE rawPortHandle = ::CreateIoCompletionPort(rawDirHandle, tempPort.get(), tempId, 0);
        if (!rawPortHandle)
            throw DirectoryWatch::Error("CreateIoCompletionPort #2", ::GetLastError());

        dir.swap(tempDir);
        port.swap(tempPort);
    }

    static inline void Accumulate(DirectoryNotifyMap& notifyMap, PFILE_NOTIFY_INFORMATION ninfo)
    {
        CString filename = DirectoryNotify::GetFilename(ninfo);
        if (filename.IsEmpty())
            return ;

        DirectoryNotifyMap::iterator it = notifyMap.find(filename);
        if (it == notifyMap.end())
        {
            DirectoryNotifyPtr notify(new DirectoryNotify(ninfo, filename));

            if (notify->Valid())
                notifyMap.insert(std::make_pair(filename, notify));
        }

        else
            it->second->UpdateFlags(ninfo);
    }

    template <size_t Size>
    static inline int Parse(BYTE (&buffer)[Size], DirectoryNotifyMap& notifyMap)
    {
        int count = 0;
        DWORD offset = 0;
        for (;;)
        {
            PFILE_NOTIFY_INFORMATION ninfo = (FILE_NOTIFY_INFORMATION*)(buffer + offset);

            Accumulate(notifyMap, ninfo);
            ++count;

            offset += ninfo->NextEntryOffset;
            if (!ninfo->NextEntryOffset)
                break;
        }

        return count;
    }

    static inline void NotifyOwner(void* owner, CString const& dirpath, DirectoryNotifyMap& nmap)
    {
        ::SendMessage((HWND)owner
            , DirectoryWatch::WmChangesDetected
            , (WPARAM)(PCWSTR)dirpath
            , (LPARAM)&nmap
            );

        nmap.clear();
    }
}
