#pragma once

#include <wcdafx.api.h>

namespace Twins
{
    class DirectoryWatch
    {
    public:
        DELETE_COPY_MOVE_OF(DirectoryWatch);

        struct Error;
        typedef std::shared_ptr<void> Handle;

        enum Flags
        {
          ForFileName   = FILE_NOTIFY_CHANGE_FILE_NAME    
        , ForDirName    = FILE_NOTIFY_CHANGE_DIR_NAME     
        , ForAttributes = FILE_NOTIFY_CHANGE_ATTRIBUTES   
        , ForSize       = FILE_NOTIFY_CHANGE_SIZE         
        , ForLastWrite  = FILE_NOTIFY_CHANGE_LAST_WRITE   
        , ForLastAccess = FILE_NOTIFY_CHANGE_LAST_ACCESS  
        , ForCreation   = FILE_NOTIFY_CHANGE_CREATION     
        , ForSecurity   = FILE_NOTIFY_CHANGE_SECURITY  
        , DefaultFlags  = ForFileName | ForDirName | ForAttributes
        };

        enum Messages
        {
            WmWatchStart = 0x4001,
            WmChangesDetected,
            WmWatchDone,
        };

        DirectoryWatch();
        ~DirectoryWatch();

        void Cancel();
        void SetOwner(void* owner);
        void HandleResult(HRESULT hr) const;

        HRESULT Listen(wchar_t const* path
            , unsigned long mask = DefaultFlags
            , unsigned long numberOfConcurrentThreads = 1
            );

    private:
        volatile unsigned long id_;
        Handle port_;
        void* owner_;
        CString dirpath_;
    };
}
