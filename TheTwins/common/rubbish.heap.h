#pragma once

#include <string>
#include <winnt.h>
#include <winbase.h>
#include <atlstr.h>
#include <atltypes.h>

namespace RubbishHeap
{
    typedef WidecharString String;

    inline bool IsCurrentUserAdmin()
    {
        bool rv = false;

        PSID sid = NULL;
        SID_IDENTIFIER_AUTHORITY ident = SECURITY_NT_AUTHORITY;
        if (::AllocateAndInitializeSid(&ident, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &sid))
        {
            BOOL isMember = FALSE;
            rv = (TRUE == ::CheckTokenMembership(0, sid, &isMember));
            ::FreeSid(sid);
        }

        return rv;
    }

    inline String GetHostName()
    {
        DWORD len = 1024;
        WidecharString rv;
        ::GetComputerNameExW(ComputerNamePhysicalDnsFullyQualified, rv.GetBufferSetLength(len), &len);
        rv.ReleaseBufferSetLength(len);
        return String((PCWSTR)rv);
    }

    inline String GetCurrentUserName()
    {
        DWORD len = 2048;
        WidecharString rv;
        ::GetUserNameW(rv.GetBufferSetLength(len), &len);
        rv.ReleaseBufferSetLength(len);
        return String((PCWSTR)rv);
    }
}
