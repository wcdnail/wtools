#include "stdafx.h"
#include "drive.info.h"
#include <dh.tracing.h>
#include <string.utils.human.size.h>
#include <string.utils.error.code.h>
#include <twins.langs/twins.lang.strings.h>
#include <cstdint>

namespace Twins
{
    namespace Drive
    {
        bool IsReady(Enumerator::Item const& info)
        {
            CStringW prev;

            UINT prevMode = SetErrorMode(SEM_FAILCRITICALERRORS);
            DWORD prevLen = GetCurrentDirectoryW(0, NULL);
            ::GetCurrentDirectoryW(prevLen, prev.GetBufferSetLength(prevLen + 1));

            BOOL dirSet = ::SetCurrentDirectory(info.path.c_str());
            info.ready = (FALSE != dirSet);

            HRESULT hr = !dirSet ? GetLastError() : 0;
            if (!hr) {
                DH::TPrintf(0, _T("DRIVERDY: %s\n"), info.path.c_str());
            }
            else {
                DH::TPrintf(TL_Error, _T("DRIVERDY: %s - %d %s\n"), info.path.c_str(), 
                    hr, Str::ErrorCode<>::SystemMessage(hr).GetString());
            }
            ::SetCurrentDirectoryW(prev.GetString());
            ::SetErrorMode(prevMode);
            return FALSE != dirSet;
        }

        CString GetHint(Enumerator::Item const& info)
        {
            CStringW hint;
            if (!info.fs.empty()) {
                hint += info.fs.c_str();
                hint += _T(" ");
            }
            if (!info.label.empty()) {
                hint += _T("[");
                hint += info.label.c_str();
                hint += _T("] ");
            }
            ULARGE_INTEGER totalBytes = { 0 };
            ULARGE_INTEGER  freeBytes = { 0 };
            if (GetDiskFreeSpaceEx(info.path.c_str(), nullptr, &totalBytes, &freeBytes)) {
                CString sizes;

                sizes.Format(_LS(StrId_Sfromsfree),
                    Str::HumanSize(freeBytes.QuadPart).c_str(),
                    Str::HumanSize(totalBytes.QuadPart).c_str()
                );
                hint += sizes;
            }
            return hint;
        }
    }
}
