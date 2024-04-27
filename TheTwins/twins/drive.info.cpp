#include "stdafx.h"
#include "drive.info.h"
#include <dh.tracing.h>
#include <string.utils.human.size.h>
#include <string.utils.error.code.h>
#include <twins.lang.strings.h>

namespace Twins
{
    namespace Drive
    {
#if 0
        bool IsReady0(Enumerator::Item const& info)
        {
            UINT prevMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
            HANDLE find = INVALID_HANDLE_VALUE;

            try
            {
                WIN32_FIND_DATA fdata = {0};
                find = ::FindFirstFile((info.path + _T("*")).c_str(), &fdata);
                if (INVALID_HANDLE_VALUE != find)
                    ::FindClose(find);
            }
            catch(...)
            {}

            ::SetErrorMode(prevMode);
            return INVALID_HANDLE_VALUE != find;
        }

        bool IsReady1(Enumerator::Item const& info)
        {
            //UINT prevMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
            UINT type = ::GetDriveType(info.path.c_str());
            //if (DRIVE_FIXED != type && DRIVE_REMOTE != type)
            //::SetErrorMode(prevMode);
            return DRIVE_NO_ROOT_DIR != type;
        }
#endif // 0

        bool IsReady(Enumerator::Item const& info)
        {
            CString prev;

            UINT prevMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
            DWORD prevLen = ::GetCurrentDirectory(0, NULL);
            ::GetCurrentDirectory(prevLen, prev.GetBufferSetLength(prevLen + 1));

            BOOL dirSet = ::SetCurrentDirectory(info.path.c_str());
            info.ready = (true == dirSet);

            HRESULT hr = !dirSet ? ::GetLastError() : 0;
            if (!hr)
            {
                Dh::ThreadPrintf(_T("DRIVERDY: %s\n"), info.path.c_str());
            }
            else
            {
                Dh::ThreadPrintf(_T("DRIVERDY: %s - %d %s\n"), info.path.c_str()
                    , hr, Str::ErrorCode<>::SystemMessage(hr));
            }

            ::SetCurrentDirectory(prev);
            ::SetErrorMode(prevMode);

            return FALSE != dirSet;
        }

        CString GetHint(Enumerator::Item const& info)
        {
            CString hint;
            if (!info.fs.empty())
            {
                hint += info.fs.c_str();
                hint += _T(" ");
            }

            if (!info.label.empty())
            {
                hint += _T("[");
                hint += info.label.c_str();
                hint += _T("] ");
            }

            unsigned __int64 totalBytes = 0, freeBytes = 0;
            if (::GetDiskFreeSpaceEx(info.path.c_str(), NULL, (PULARGE_INTEGER)&totalBytes, (PULARGE_INTEGER)&freeBytes))
            {
                CString sizes;

                sizes.Format(_LS(StrId_Sfromsfree), 
                    Str::HumanSize(freeBytes).c_str(), Str::HumanSize(totalBytes).c_str());

                hint += sizes;
            }

            return hint;
        }
    }
}
