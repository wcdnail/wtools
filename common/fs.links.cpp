#include "stdafx.h"
#include "fs.links.h"
#include <winioctl.h>

namespace Cf
{
    enum
    {
        DIR_ATTR = (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT),
        REPARSE_MOUNTPOINT_HEADER_SIZE = 8,
    };

    typedef struct 
    {
        DWORD ReparseTag;
        DWORD ReparseDataLength;
        WORD Reserved;
        WORD ReparseTargetLength;
        WORD ReparseTargetMaximumLength;
        WORD Reserved1;
        WCHAR ReparseTarget[1];
    } 
    REPARSE_MOUNTPOINT_DATA_BUFFER, *PREPARSE_MOUNTPOINT_DATA_BUFFER;

    typedef struct _REPARSE_DATA_BUFFER
    {
        DWORD ReparseTag;
        WORD ReparseDataLength;
        WORD Reserved;
        union 
        {
            struct 
            {
                WORD SubstituteNameOffset;
                WORD SubstituteNameLength;
                WORD PrintNameOffset;
                WORD PrintNameLength;
                WCHAR PathBuffer[1];
            } 
            SymbolicLinkReparseBuffer;

            struct 
            {
                WORD SubstituteNameOffset;
                WORD SubstituteNameLength;
                WORD PrintNameOffset;
                WORD PrintNameLength;
                WCHAR PathBuffer[1];
            } 
            MountPointReparseBuffer;

            struct 
            {
                BYTE   DataBuffer[1];
            } 
            GenericReparseBuffer;
        };
    }
    REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;

    #define REPARSE_DATA_BUFFER_HEADER_SIZE \
        FIELD_OFFSET(REPARSE_DATA_BUFFER, GenericReparseBuffer)

    HANDLE OpenDirectory(PCWSTR rawPathName, bool rw) 
    {
        // Obtain backup/restore privilege in case we don't have it
        HANDLE hToken = NULL;
        TOKEN_PRIVILEGES tp = {0};
        ::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
        ::LookupPrivilegeValue(NULL, (rw ? SE_RESTORE_NAME : SE_BACKUP_NAME), &tp.Privileges[0].Luid);
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        ::AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
        ::CloseHandle(hToken);

        HANDLE hDir = ::CreateFileW(rawPathName, rw ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ,
            0, NULL, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, NULL);

        return hDir;
    }

    bool IsDirectoryJunction(PCWSTR rawPathName, HANDLE dir) 
    {
        DWORD dwAttr = ::GetFileAttributesW(rawPathName);
        if (dwAttr == -1) {
            return false;
        }
        if ((dwAttr & DIR_ATTR) != DIR_ATTR) {
            return false;
        }
        if (INVALID_HANDLE_VALUE == dir) {
            return false;
        }
        BYTE buf[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
        REPARSE_MOUNTPOINT_DATA_BUFFER& ReparseBuffer = (REPARSE_MOUNTPOINT_DATA_BUFFER&)buf;
        DWORD dwRet;
        BOOL br = ::DeviceIoControl(dir, FSCTL_GET_REPARSE_POINT, NULL, 0, &ReparseBuffer, MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &dwRet, NULL);
        return br ? (ReparseBuffer.ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) : false;
    }

    std::wstring QueryLinkTarget(std::wstring const& dirpath, std::error_code& ec)
    {
        std::wstring result;
        HANDLE dir = OpenDirectory(dirpath.c_str(), false);

        if (INVALID_HANDLE_VALUE == dir) 
            ec.assign(::GetLastError(), std::system_category());

        else
        {
            std::shared_ptr<void> dirh(dir, CloseHandle);
            DWORD dwRet = 0;
            BYTE buf[MAXIMUM_REPARSE_DATA_BUFFER_SIZE] = {0};
            REPARSE_MOUNTPOINT_DATA_BUFFER& ReparseBuffer = (REPARSE_MOUNTPOINT_DATA_BUFFER&)buf;
            if (!::DeviceIoControl(dir, FSCTL_GET_REPARSE_POINT, NULL, 0, &ReparseBuffer, MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &dwRet, NULL))
                ec.assign(::GetLastError(), std::system_category());

            else
            {
                PCWSTR rawPath = ReparseBuffer.ReparseTarget;
                if (::wcsncmp(rawPath, L"\\??\\", 4) == 0) 
                    rawPath += 4;

                result = rawPath;
            }
        }

        return result;
    }
}
