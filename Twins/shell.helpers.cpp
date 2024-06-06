#include "stdafx.h"
#include "shell.helpers.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>

namespace Twins
{
    CIconHandle LoadShellIcon(std::wstring const& entry, unsigned attrs /*= INVALID_FILE_ATTRIBUTES*/, UINT flags /*= SHGFI_SMALLICON | SHGFI_ADDOVERLAYS*/)
    {
        if (INVALID_FILE_ATTRIBUTES == attrs) {
            attrs = GetFileAttributesW(entry.c_str());
            if (INVALID_FILE_ATTRIBUTES == attrs) {
                HRESULT hr = GetLastError();
                DH::TPrintf(TL_Error, L"SHELLICO: Can't get attrs for `%s` - %d `%s`\n", entry.c_str(), hr, Str::ErrorCode<>::SystemMessage(hr));
                return {};
            }
        }

        SHFILEINFO info{};
        DWORD_PTR    rv{SHGetFileInfoW(entry.c_str(), attrs, &info, sizeof(info), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | flags)};
        if (0 == rv) {
            HRESULT hr = GetLastError();
            DH::TPrintf(TL_Error, L"SHELLICO: Can't load icon for `%s` - %d `%s`\n", entry.c_str(), hr, Str::ErrorCode<>::SystemMessage(hr));
            return {};
        }
        return CIconHandle{info.hIcon};
    }
}
